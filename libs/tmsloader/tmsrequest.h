#ifndef TMSLOADER_TMSREQUEST_H
#define TMSLOADER_TMSREQUEST_H

#include "tmsloader_api.h"

class QNetworkAccessManager;
class QObject;
class QPointF;
class QSize;

namespace tmsloader {

class TmsRequestHandler;

class TMSLOADER_API TmsRequest
{
public:
	TmsRequest(const QPointF& centerLonLat, const QSize& size, double scale);
	virtual ~TmsRequest();

	virtual TmsRequestHandler* buildHandler(int requestId, QNetworkAccessManager* manager, QObject* parent) const = 0;

protected:
	QPointF center() const;
	QSize size() const;
	double scale() const;

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequest_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUEST_H