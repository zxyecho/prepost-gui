#ifndef GRIDRELATEDCONDITIONVARIATIONEDITWIDGETCONTAINER_H
#define GRIDRELATEDCONDITIONVARIATIONEDITWIDGETCONTAINER_H

#include "gridrelatedconditionvariationeditwidget.h"
#include <QWidget>
#include <QVBoxLayout>

class GridRelatedConditionVariationEditWidgetContainer : public QWidget
{
public:
	GridRelatedConditionVariationEditWidgetContainer(QWidget* parent)
		: QWidget(parent) {
		m_widget = nullptr;
	}
	void setWidget(GridRelatedConditionVariationEditWidget* w) {
		m_widget = w;
		QVBoxLayout* l = new QVBoxLayout();
		l->setMargin(0);
		l->addWidget(w);
		setLayout(l);
	}
	GridRelatedConditionVariationEditWidget* widget() const {return m_widget;}
	QSize sizeHint() const override {return m_widget->sizeHint();}

private:
	GridRelatedConditionVariationEditWidget* m_widget;
};

#endif // GRIDRELATEDCONDITIONEDITWIDGETCONTAINER_H
