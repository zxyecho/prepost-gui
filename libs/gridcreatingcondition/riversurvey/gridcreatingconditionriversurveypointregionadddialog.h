#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTREGIONADDDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTREGIONADDDIALOG_H

#include <QDialog>
#include <QList>

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class GridCreatingConditionRiverSurvey;

namespace Ui
{
	class GridCreatingConditionRiverSurveyPointRegionAddDialog;
}

class GridCreatingConditionRiverSurveyPointRegionAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionRiverSurveyPointRegionAddDialog(GridCreatingConditionRiverSurvey* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurveyPointRegionAddDialog();
	void setData(RawDataRiverSurvey* rs);
	void setStartPoint(RawDataRiverPathPoint* p);
	void setEndPoint(RawDataRiverPathPoint* p);
	RawDataRiverPathPoint* startPoint();
	RawDataRiverPathPoint* endPoint();

public slots:
	void accept() override;

private slots:
	void handleStartUpdate();
	void handleEndUpdate();

protected:
	void changeEvent(QEvent* e) override;

private:
	Ui::GridCreatingConditionRiverSurveyPointRegionAddDialog* ui;

	QList<RawDataRiverPathPoint*> m_points;
	GridCreatingConditionRiverSurvey* m_condition;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTREGIONADDDIALOG_H
