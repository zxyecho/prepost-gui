#include "project.h"
#include "../arbitraryhwm/arbitraryhwm.h"
#include "../crosssection/crosssection.h"
#include "../leftbankhwm/leftbankhwm.h"
#include "../rightbankhwm/rightbankhwm.h"
#include "../../misc/mathutil.h"

#include "private/project_impl.h"

#include <QVector3D>

#include <map>
#include <vector>

namespace {

void addToVector(const Points& points, const BaseLine& line, std::multimap<double, double>* vals)
{
	for (QVector3D* p : points.points()) {
		double pos = line.calcPosition(p->x(), p->y());
		vals->insert(std::make_pair(pos, p->z()));
	}
}

void addToElevMap(std::map<CrossSection*, std::vector<double> >* elevVals, CrossSection* cs, double val)
{
	auto it = elevVals->find(cs);
	if (it == elevVals->end()) {
		std::vector<double> emptyVec;
		elevVals->insert(std::make_pair(cs, emptyVec));
		it = elevVals->find(cs);
	}
	it->second.push_back(val);
}

} // namespace

Project::Impl::Impl(Project *project) :
	m_rootDataItem {project},
	m_elevationPoints {&m_rootDataItem},
	m_waterSurfaceElevationPoints {&m_rootDataItem},
	m_crossSections {&m_rootDataItem},
	m_baseLine {&m_rootDataItem}
{}

Project::Impl::~Impl()
{}

// public interfraces

Project::Project() :
	impl {new Impl {this}}
{}

Project::~Project()
{
	delete impl;
}

RootDataItem* Project::rootDataItem() const
{
	return &(impl->m_rootDataItem);
}

const ElevationPoints& Project::elevationPoints() const
{
	return impl->m_elevationPoints;
}

ElevationPoints& Project::elevationPoints()
{
	return impl->m_elevationPoints;
}

const WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints() const
{
	return impl->m_waterSurfaceElevationPoints;
}

WaterSurfaceElevationPoints& Project::waterSurfaceElevationPoints()
{
	return impl->m_waterSurfaceElevationPoints;
}

const BaseLine& Project::baseLine() const
{
	return impl->m_baseLine;
}

BaseLine& Project::baseLine()
{
	return impl->m_baseLine;
}

const CrossSections& Project::crossSections() const
{
	return impl->m_crossSections;
}

CrossSections& Project::crossSections()
{
	return impl->m_crossSections;
}

const QPointF& Project::offset() const
{
	return impl->m_offset;
}

void Project::setOffset(const QPointF& offset)
{
	impl->m_offset = offset;
}

void Project::calcCrossSectionElevations()
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	if (bl.polyLine().size() < 2) {return;}

	std::multimap<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (crosses) {
			posMap.insert(std::make_pair(pos, cs));
		}
	}

	std::multimap<double, double> vals;

	auto& wse = waterSurfaceElevationPoints();

	addToVector(wse.leftBankHWM(), bl, &vals);
	addToVector(wse.rightBankHWM(), bl, &vals);
	addToVector(wse.arbitraryHWM(), bl, &vals);

	std::map<CrossSection*, std::vector<double> > elevVals;

	for (auto it = posMap.begin(); it != posMap.end(); ++it) {
		auto it2 = it;
		++ it2;
		if (it2 == posMap.end()) {break;}

		double min = it->first;
		double max = it2->first;

		std::vector<double> xvec, yvec;

		auto it_b = vals.lower_bound(min);
		auto it_e = vals.upper_bound(max);
		for (auto tmp_it = it_b; tmp_it != it_e; ++ tmp_it) {
			xvec.push_back(tmp_it->first);
			yvec.push_back(tmp_it->second);
		}
		double a, b;
		MathUtil::leastSquares(xvec, yvec, &a, &b);

		addToElevMap(&elevVals, it->second,  a * min + b);
		addToElevMap(&elevVals, it2->second, a * max + b);
	}

	for (auto pair : elevVals) {
		if (pair.second.size() == 0) {continue;}

		double sum = 0;
		for (double v : pair.second) {
			sum += v;
		}
		double avg = sum / pair.second.size();
		pair.first->setWaterElevation(avg);
	}
}

void Project::mapPointsToCrossSections()
{
	if (crossSections().crossSectionVector().size() == 0) {return;}

	const auto& csVec = crossSections().crossSectionVector();
	for (auto cs : csVec) {
		cs->clearMappedPoints();
	}
	for (QVector3D* p : elevationPoints().points()) {
		std::multimap<double, CrossSection*> distanceMap;
		QPointF nearestPoint;
		double distance;
		for (auto cs : csVec) {
			cs->getNearestPoint(p->x(), p->y(), &nearestPoint, &distance);
			distanceMap.insert(std::make_pair(distance, cs));
		}
		auto it = distanceMap.begin();
		it->second->addMappedPoint(p);
	}
}

bool Project::sortCrossSectionsIfPossible()
{
	auto& bl = baseLine();
	auto csVec = crossSections().crossSectionVector();

	if (bl.polyLine().size() < 2) {return false;}

	std::multimap<double, CrossSection*> posMap;
	bool crosses;
	double x, y, pos;

	for (CrossSection* cs : csVec) {
		bl.getCrossingPoint(cs, &crosses, &x, &y, &pos);
		if (! crosses) {return false;}

		posMap.insert(std::make_pair(pos, cs));
	}

	auto& childItems = crossSections().childItems();
	childItems.clear();
	for (auto pair : posMap) {
		childItems.push_back(pair.second);
	}
	return true;
}

void Project::emitUpdated()
{
	emit updated();
}