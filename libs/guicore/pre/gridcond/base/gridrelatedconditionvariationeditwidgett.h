#ifndef GRIDRELATEDCONDITIONVARIATIONEDITWIDGETT_H
#define GRIDRELATEDCONDITIONVARIATIONEDITWIDGETT_H

#include "gridrelatedconditionvariationeditwidget.h"
#include "gridrelatedconditioncontainert.h"
#include "../../../solverdef/solverdefinitiongridrelatedconditiont.h"
#include "../../grid/grid.h"
#include "../../base/preprocessorgriddataiteminterface.h"
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <QObject>
#include <QUndoCommand>
#include <vtkDataArray.h>
#include <vtkDataSetAttributes.h>
#include <vtkSmartPointer.h>

/// Widget to Edit Grid related condition value

class GridRelatedConditionEditVariationCommand : public QObject, public QUndoCommand
{
	Q_OBJECT
public:
	GridRelatedConditionEditVariationCommand(const QString& name, vtkDataArray* newValues, vtkDataArray* oldValues, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
		: QUndoCommand(tr("Edit grid attribute value(s)")) {
		m_newValues = newValues;
		m_oldValues = oldValues;
		m_attributes = atts;
		m_name = name;
		m_oldValues->SetName(iRIC::toStr(m_name).c_str());
		m_newValues->SetName(iRIC::toStr(m_name).c_str());
		m_dataItem = dItem;
	}
	void undo() override {
		m_attributes->GetArray(iRIC::toStr(m_name).c_str())->DeepCopy(m_oldValues);
		m_dataItem->updateAttributeActorSettings();
		m_dataItem->informgridRelatedConditionChange(m_name);
		m_dataItem->grid()->setModified();
	}
	void redo() override {
		m_attributes->GetArray(iRIC::toStr(m_name).c_str())->DeepCopy(m_newValues);
		m_dataItem->updateAttributeActorSettings();
		m_dataItem->informgridRelatedConditionChange(m_name);
		m_dataItem->grid()->setModified();
		m_dataItem->grid()->gridRelatedCondition(m_name)->setCustomModified(true);
	}
private:
	vtkSmartPointer<vtkDataArray> m_newValues;
	vtkSmartPointer<vtkDataArray> m_oldValues;
	vtkSmartPointer<vtkDataSetAttributes> m_attributes;
	QString m_name;
	PreProcessorGridDataItemInterface* m_dataItem;
};

template <class V>
class GridRelatedConditionVariationEditWidgetT : public GridRelatedConditionVariationEditWidget
{
public:
	GridRelatedConditionVariationEditWidgetT(QWidget* parent, SolverDefinitionGridRelatedConditionT<V>* cond)
		: GridRelatedConditionVariationEditWidget(parent, cond) {
		m_value = 0;
	}
	void setValue(V value) {
		m_value = value;
		setupWidget();
	}
	V value() {
		getValueFromInnerWidget();
		return m_value;
	}
	void setVariantValue(const QVariant& v) {
		SolverDefinitionGridRelatedConditionT<V>* cond = dynamic_cast<SolverDefinitionGridRelatedConditionT<V>* >(m_gridRelatedCondition);
		V tmpval = cond->fromVariant(v);
		setValue(tmpval);
	}

	QVariant variantValue() override {
		return QVariant(value());
	}

	void applyVariation(GridRelatedConditionContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem) override {
		GridRelatedConditionContainerT<V>* c = dynamic_cast<GridRelatedConditionContainerT<V>* >(container);
		vtkDataArray* oldValues = c->dataArrayCopy();
		V val = value();
		for (auto it = indices.begin(); it != indices.end(); ++it) {
			c->setValue(*it, c->value(*it) + val);
		}
		vtkDataArray* newValues = c->dataArrayCopy();
		iRICUndoStack::instance().push(new GridRelatedConditionEditVariationCommand(c->dataArray()->GetName(), newValues, oldValues, atts, dItem));
		oldValues->Delete();
		newValues->Delete();
	}
protected:
	V m_value;
};

#endif // GRIDRELATEDCONDITIONEDITWIDGETT_H
