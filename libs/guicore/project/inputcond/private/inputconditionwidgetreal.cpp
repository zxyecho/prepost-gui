#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerreal.h"
#include "inputconditionwidgetreal.h"
#include "inputconditionwidgettooltip.h"

#include <guibase/widget/realnumbereditwidget.h>

#include <QHBoxLayout>
#include <QMessageBox>

InputConditionWidgetReal::InputConditionWidgetReal(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerReal* cont) :
	InputConditionWidget(defnode),
	m_container {cont},
	m_lineEdit {new RealNumberEditWidget(this)}
{
	m_lineEdit->setMinimumWidth(100);
	m_lineEdit->setAlignment(Qt::AlignRight);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	layout->addWidget(m_lineEdit, 1);
	layout->setMargin(InputConditionWidget::margin);
	setLayout(layout);

	setValue(cont->value());
	connect(m_lineEdit, SIGNAL(valueChanged(double)), this, SLOT(getWidgetValue(double)));
	connect(m_container, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
}

void InputConditionWidgetReal::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

void InputConditionWidgetReal::setValue(double v)
{
	m_lineEdit->setValue(v);
}

void InputConditionWidgetReal::getWidgetValue(double v)
{
	m_container->setValue(v);
}

void InputConditionWidgetReal::setDisabled(bool disable)
{
	m_lineEdit->setDisabled(disable);
}
