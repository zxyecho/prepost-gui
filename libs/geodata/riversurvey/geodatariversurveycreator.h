#ifndef GEODATARIVERSURVEYCREATOR_H
#define GEODATARIVERSURVEYCREATOR_H

#include "gd_riversurvey_global.h"
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/solverdef/solverdefinitiongridattributet.h>

class GD_RIVERSURVEY_EXPORT GeoDataRiverSurveyCreator : public GeoDataCreator
{
	Q_OBJECT

public:
	/// Constructor
	GeoDataRiverSurveyCreator();
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override;
	QString name(unsigned int index) override;
	QString defaultCaption(unsigned int index) override;
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	virtual GeoData* restore(const QDomNode& node, ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATARIVERSURVEYCREATOR_H