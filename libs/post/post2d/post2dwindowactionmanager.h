#ifndef POST2DWINDOWACTIONMANAGER_H
#define POST2DWINDOWACTIONMANAGER_H

#include <QObject>
class QAction;
class QMenu;
class QToolBar;
class Post2dWindow;

class Post2dWindowActionManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	Post2dWindowActionManager(Post2dWindow* parent);
	QAction* drawGridAction;
	QAction* drawContourAction;
	QAction* drawArrowAction;
	QAction* drawStreamlineAction;
	QAction* drawParticleAction;
	QAction* drawCellFlagAction;

	QAction* drawTitleAction;
	QAction* drawTimeAction;

	QAction* measuredDataPointAction;
	QAction* measuredDataVectorAction;
	QAction* measuredDataImportAction;

	QMenu* drawMenu(){return m_drawMenu;}
	QMenu* measuredDataMenu(){return m_measuredDataMenu;}
	void connectWithDataModel();
private:
	void init();
	void setupDrawMenu();
	void setupMeasuredDataMenu();
	void setupViewToolBar();
	QMenu* m_drawMenu;
	QMenu* m_measuredDataMenu;
	Post2dWindow* m_post2dWindow;
};

#endif // POST2DWINDOWACTIONMANAGER_H
