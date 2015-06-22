#ifndef GEODATARIVERPATHPOINTEXTENSIONADDDIALOG_H
#define GEODATARIVERPATHPOINTEXTENSIONADDDIALOG_H

#include <QDialog>
#include <QVector2D>
class QAbstractButton;
class GeoDataRiverPathPoint;
class GeoDataRiverSurvey;
class QVector2D;
namespace Ui
{
	class GeoDataRiverPathPointExtensionAddDialog;
}

class GeoDataRiverPathPointExtensionAddDialog : public QDialog
{
	Q_OBJECT

public:
	enum LineMode {Left, Right};
	explicit GeoDataRiverPathPointExtensionAddDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, QWidget* parent = nullptr);
	~GeoDataRiverPathPointExtensionAddDialog();
	void setLineMode(LineMode lm) {
		m_lineMode = lm;
	}

public slots:
	void accept() override;
	void reject() override;
	void setPoint(const QVector2D& position);

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	bool m_applyed;
	QVector2D m_position;

private:
	Ui::GeoDataRiverPathPointExtensionAddDialog* ui;
	LineMode m_lineMode;
};

#endif // GEODATARIVERPATHPOINTEXTENSIONADDDIALOG_H
