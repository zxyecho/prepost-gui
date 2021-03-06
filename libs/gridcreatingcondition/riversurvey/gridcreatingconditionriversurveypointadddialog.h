#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTADDDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTADDDIALOG_H

#include <QDialog>
#include <geodata/riversurvey/geodatariverpathpoint.h>

namespace Ui
{
	class GridCreatingConditionRiverSurveyPointAddDialog;
}

class GridCreatingConditionRiverSurvey;
class QAbstractButton;

class GridCreatingConditionRiverSurveyPointAddDialog : public QDialog
{
	Q_OBJECT

public:
	GridCreatingConditionRiverSurveyPointAddDialog(GridCreatingConditionRiverSurvey* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurveyPointAddDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	bool m_applied;
	void apply();
	GeoDataRiverPathPoint::CtrlPointsAddMethod buildMethod();
	GridCreatingConditionRiverSurvey* m_condition;
	Ui::GridCreatingConditionRiverSurveyPointAddDialog* ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTADDDIALOG_H
