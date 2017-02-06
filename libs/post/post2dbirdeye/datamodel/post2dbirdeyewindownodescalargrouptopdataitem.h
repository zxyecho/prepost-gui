#ifndef POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H
#define POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H

#include "../post2dbirdeyewindowdataitem.h"
#include <postbase/post2dwindowcontoursetting.h>

#include <QMap>

class Post2dBirdEyeWindowNodeScalarGroupTopDataItem : public Post2dBirdEyeWindowDataItem
{
	Q_OBJECT
public:
	Post2dBirdEyeWindowNodeScalarGroupTopDataItem(Post2dBirdEyeWindowDataItem* parent);
	~Post2dBirdEyeWindowNodeScalarGroupTopDataItem();

	void update();

protected:
	void addCustomMenuItems(QMenu* menu) override;
	QDialog* addDialog(QWidget* parent) override;
	void handleAddDialogAccepted(QDialog* addDialog) override;
	void innerUpdateZScale(double scale) override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	Post2dWindowContourSetting m_setting;
	QMap<std::string, QString> m_colorbarTitleMap;
	double m_zScale;

	class SetSettingCommand;

	friend class Post2dBirdEyeWindowNodeScalarGroupDataItem;

	class CreateCommand;

	QAction* m_addAction;
};

#endif // POST2DBIRDEYEWINDOWNODESCALARGROUPTOPDATAITEM_H