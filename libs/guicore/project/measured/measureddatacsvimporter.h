#ifndef MEASUREDDATACSVIMPORTER_H
#define MEASUREDDATACSVIMPORTER_H

#include "measureddataimporteri.h"

class MeasuredDataCsvImporter : public MeasuredDataImporterI
{
public:
	MeasuredData* importData(const QString &filename) const override;
};

#endif // MEASUREDDATACSVIMPORTER_H