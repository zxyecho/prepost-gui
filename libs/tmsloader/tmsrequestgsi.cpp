#include "tmsrequestgsi.h"
#include "private/tmsrequestgsi_impl.h"
#include "private/tmsrequestxyz_impl.h"

#include <QString>

#include <unordered_map>

using namespace tmsloader;

TmsRequestGSI::Impl::Impl(const QPointF &centerLonLat, const QSize &size, double scale, TmsRequestGSI::TileType tileType)
{
	std::unordered_map<TmsRequestGSI::TileType, QString> tileTypeUrlMap;
	std::unordered_map<TmsRequestGSI::TileType, QString> tileTypeExtMap;

	tileTypeUrlMap.insert({TmsRequestGSI::TileType::STD, "std"});
	tileTypeUrlMap.insert({TmsRequestGSI::TileType::PALE, "pale"});
	tileTypeUrlMap.insert({TmsRequestGSI::TileType::ENGLISH, "english"});
	tileTypeUrlMap.insert({TmsRequestGSI::TileType::RELIEF, "relief"});
	tileTypeUrlMap.insert({TmsRequestGSI::TileType::ORT, "ort"});

	tileTypeExtMap.insert({TmsRequestGSI::TileType::STD, "png"});
	tileTypeExtMap.insert({TmsRequestGSI::TileType::PALE, "png"});
	tileTypeExtMap.insert({TmsRequestGSI::TileType::ENGLISH, "png"});
	tileTypeExtMap.insert({TmsRequestGSI::TileType::RELIEF, "png"});
	tileTypeExtMap.insert({TmsRequestGSI::TileType::ORT, "jpg"});

	QString url = QString("http://cyberjapandata.gsi.go.jp/xyz/%1/{z}/{x}/{y}.%2").arg(tileTypeUrlMap[tileType]).arg(tileTypeExtMap[tileType]);

	m_requestXYZ = new TmsRequestXYZ(centerLonLat, size, scale, url);
}

TmsRequestGSI::Impl::~Impl()
{
	delete m_requestXYZ;
}

// public interfaces

TmsRequestGSI::TmsRequestGSI(const QPointF& centerLonLat, const QSize& size, double scale, TileType tileType) :
	TmsRequest(centerLonLat, size, scale),
	impl {new Impl {centerLonLat, size, scale, tileType}}
{}

TmsRequestGSI::~TmsRequestGSI()
{
	delete impl;
}

TmsRequestHandler *TmsRequestGSI::buildHandler(int requestId, QNetworkAccessManager* manager, QObject *parent) const
{
	return impl->m_requestXYZ->buildHandler(requestId, manager, parent);
}