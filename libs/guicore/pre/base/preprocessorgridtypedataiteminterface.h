#ifndef PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H
#define PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H

#include "preprocessordataitem.h"
#include <QList>

class QString;
class ScalarsToColorsContainer;
class SolverDefinitionGridType;
class PreProcessorRawDataTopDataItemInterface;
class PreProcessorGridAndGridCreatingConditionDataItemInterface;

class PreProcessorGridTypeDataItemInterface : public PreProcessorDataItem
{

public:
	/// Constructor
	PreProcessorGridTypeDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: PreProcessorDataItem(itemlabel, icon, parent)
	{}
	virtual ScalarsToColorsContainer* scalarsToColors(const QString& attName) const = 0;
	virtual SolverDefinitionGridType* gridType() const = 0;
	virtual const QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>& conditions() const = 0;
	virtual PreProcessorRawDataTopDataItemInterface* rawdataTop() const = 0;
};

#endif // PREPROCESSORGRIDTYPEDATAITEMINTERFACE_H
