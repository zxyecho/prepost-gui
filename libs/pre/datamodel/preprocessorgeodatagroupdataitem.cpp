#include "../factory/geodatafactory.h"
#include "../misc/preprocessorlegendboxeditdialog.h"
#include "../misc/preprocessorscalarbareditdialog.h"
#include "../misc/preprocessorscalarbarlegendboxsettingdialog.h"
#include "../preobjectbrowserview.h"
#include "../preprocessordatamodel.h"
#include "../preprocessorgraphicsview.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridshapedataitem.h"
#include "preprocessorgeodatadataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorstructured2dgridshapedataitem.h"

#include <guibase/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensionselectwidget.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/pre/geodata/geodataimporter.h>
#include <guicore/pre/geodata/geodatamapper.h>
#include <guicore/pre/geodatabackground/geodatabackground.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/scalarstocolors/scalarstocolorseditdialog.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributedimension.h>
#include <misc/iricundostack.h>
#include <misc/lastiodirectory.h>
#include <misc/stringtool.h>
#include <geodata/pointmap/geodatapointmap.h>
#include <geodata/polygon/geodatapolygon.h>
#include <geodata/polygon/geodatapolygonrealcreator.h>
#include <geodata/polygon/geodatapolygonshapeexporter.h>
#include <geodata/riversurvey/geodatariversurveycrosssectionwindow.h>
#include <geodata/riversurvey/geodatariversurveycrosssectionwindowprojectdataitem.h>

#include <QApplication>
#include <QFileDialog>
#include <QInputDialog>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QMenu>
#include <QMessageBox>
#include <QSignalMapper>
#include <QStandardItem>
#include <QTextCodec>
#include <QToolBar>
#include <QXmlStreamWriter>
#include <QtGlobal>

#include <cgnslib.h>

PreProcessorGeoDataGroupDataItem::PreProcessorGeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, PreProcessorDataItem* parent)
	: PreProcessorGeoDataGroupDataItemInterface(cond, parent)
{
	m_condition = cond;
	m_subFolder = cond->name();
	m_standardItemCopy = m_standardItem->clone();

	m_isDeletable = false;
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_addSignalMapper = nullptr;

	m_importAction = new QAction(tr("&Import..."), this);
	m_importAction->setIcon(QIcon(":/libs/guibase/images/iconImport.png"));
	m_importAction->setEnabled(importAvailable());

	m_deleteAllAction = new QAction(tr("Delete &All..."), this);
	m_deleteAllAction->setIcon(QIcon(":/libs/guibase/images/iconDeleteItem.png"));

	m_exportAllPolygonsAction = new QAction(tr("Export All Polygons..."), this);
	m_exportAllPolygonsAction->setIcon(QIcon(":/libs/guibase/images/iconExport.png"));

	m_editColorMapAction = new QAction(tr("&Color Setting..."), this);
	m_editColorMapAction->setIcon(QIcon(":/libs/guibase/images/iconColor.png"));
	m_setupScalarBarAction = new QAction(tr("Set Up Scalarbar..."), this);

	connect(m_importAction, SIGNAL(triggered()), this, SLOT(import()));
	connect(m_deleteAllAction, SIGNAL(triggered()), this, SLOT(deleteAll()));
	connect(m_exportAllPolygonsAction, SIGNAL(triggered()), this, SLOT(exportAllPolygons()));
	connect(this, SIGNAL(selectGeoData(QModelIndex)), dataModel(), SLOT(handleObjectBrowserSelection(QModelIndex)));
	connect(m_editColorMapAction, SIGNAL(triggered()), this, SLOT(editScalarsToColors()));
	connect(m_setupScalarBarAction, SIGNAL(triggered()), dynamic_cast<PreProcessorGeoDataTopDataItem*>(this->parent()), SLOT(setupScalarBar()));

	if (m_condition->isOption()) {
		m_scalarBarSetting.initForLegendBox();
	}
	// for scalar bar / legend box
	m_title = m_condition->englishCaption();

	// add dimensions container
	m_dimensions = new GridAttributeDimensionsContainer(cond, this);

	// add background data item.
	addBackground();

	// dummy filename
	m_filename = "dummy.dat";
}

PreProcessorGeoDataGroupDataItem::~PreProcessorGeoDataGroupDataItem()
{
	delete m_dimensions;
}

void PreProcessorGeoDataGroupDataItem::addCustomMenuItems(QMenu* menu)
{
	GeoDataFactory& factory = GeoDataFactory::instance();
	// create add menu.
	const QList<GeoDataCreator*> creators = factory.compatibleCreators(m_condition);
	m_addMenu = new QMenu(tr("&Add"), menu);

	if (m_addSignalMapper) {delete m_addSignalMapper;}
	m_addSignalMapper = new QSignalMapper(this);

	for (auto it = creators.begin(); it != creators.end(); ++it) {
		if ((*it)->isCreatable()) {
			QString title = (*it)->caption();
			QAction* addAction = m_addMenu->addAction(title.append("..."));
			m_addSignalMapper->setMapping(addAction, *it);
			connect(addAction, SIGNAL(triggered()), m_addSignalMapper, SLOT(map()));
		}
	}
	connect(m_addSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(addGeoData(QObject*)));
	menu->addAction(m_importAction);
	if (m_addMenu->actions().count() != 0) {
		menu->addMenu(m_addMenu);
	}
	menu->addSeparator();

	m_exportAllPolygonsAction->setEnabled(polygonExists());
	menu->addAction(m_exportAllPolygonsAction);
	menu->addSeparator();
	menu->addAction(m_deleteAllAction);

	menu->addSeparator();
	menu->addAction(m_editColorMapAction);
	menu->addAction(m_setupScalarBarAction);
}

void PreProcessorGeoDataGroupDataItem::import()
{
	QStringList filters;
	QList<GeoDataImporter*> importers;

	GeoDataFactory& factory = GeoDataFactory::instance();
	// create add menu.
	const QList<GeoDataCreator*> creators = factory.compatibleCreators(m_condition);

	QStringList availableExtensions;
	QMap<QString, GeoDataImporter*> extMap;
	for (auto it = creators.begin(); it != creators.end(); ++it) {
		const QList<GeoDataImporter*>& imps = (*it)->importers();
		for (auto imp_it = imps.begin(); imp_it != imps.end(); ++imp_it) {
			QStringList fils = (*imp_it)->fileDialogFilters();
			QStringList exts = (*imp_it)->acceptableExtensions();
			for (auto f_it = fils.begin(); f_it != fils.end(); ++f_it) {
				filters.append(*f_it);
				importers.append(*imp_it);
			}
			for (auto f_it = exts.begin(); f_it != exts.end(); ++f_it) {
				availableExtensions << QString("*.").append(*f_it);
				extMap.insert(*f_it, *imp_it);
			}
		}
	}
	filters.push_front(QString(tr("All importable files (%1)")).arg(availableExtensions.join(" ")));

	QString dir = LastIODirectory::get();
	QString selectedFilter;
	// Select the file to import.
	QString filename = QFileDialog::getOpenFileName(preProcessorWindow(), tr("Select file to import"), dir, filters.join(";;"), &selectedFilter);
	if (filename.isNull()) {return;}

	GeoDataImporter* importer = nullptr;
	for (int i = 0; i < filters.count(); ++i) {
		if (filters[i] == selectedFilter) {
			if (i == 0) {
				QFileInfo finfo(filename);
				QString extension = finfo.suffix();
				if (extMap.contains(extension)) {
					importer = extMap.value(extension);
				} else {
					QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("File type can not be recognized from the file extension. : %1").arg(finfo.fileName()));
					return;
				}
			} else {
				importer = importers[i - 1];
			}
		}
	}
	Q_ASSERT(importer != nullptr);

	// execute import.
	int dataCount;
	QWidget* w = preProcessorWindow();
	bool ret = importer->importInit(filename, selectedFilter, &dataCount, m_condition, this, w);
	if (! ret) {
		QMessageBox::warning(preProcessorWindow(), tr("Import failed"), tr("Importing data from %1 failed.").arg(QDir::toNativeSeparators(filename)));
		return;
	}
	PreProcessorGeoDataDataItem* item = nullptr;

	WaitDialog* wDialog = nullptr;
	m_cancelImport = false;
	if (dataCount >= 5) {
		wDialog = new WaitDialog(mainWindow());
		wDialog->setRange(0, dataCount - 1);
		wDialog->setProgress(0);
		wDialog->setMessage(tr("Importing data..."));
		wDialog->showProgressBar();
		connect(wDialog, SIGNAL(canceled()), this, SLOT(cancelImport()));
		wDialog->show();
	}
	// All imports succeeded.
	QFileInfo finfo(filename);
	for (int i = 0; i < dataCount; ++i) {
		item = new PreProcessorGeoDataDataItem(this);
		// first, create an empty geodata.
		GeoData* geodata = importer->creator()->create(item, m_condition);
		item->setGeoData(geodata);
		// set name and caption
		importer->creator()->setNameAndDefaultCaption(this->childItems(), geodata);
		geodata->setupDataItem();
		if (geodata->requestCoordinateSystem()) {
			if (projectData()->mainfile()->coordinateSystem() == nullptr) {
				QMessageBox::information(preProcessorWindow(), tr("Information"), tr("To import the geographic data, specify coodrinate system first."), QMessageBox::Ok);
				int dialogRet = projectData()->mainfile()->showCoordinateSystemDialog();
				if (dialogRet == QDialog::Rejected) {
					delete item;
					item = nullptr;
					goto ERROR;
				}
			}
		}
		// import data from the specified file
		bool ret = importer->importData(geodata, i, w);
		if (! ret) {
			// failed.
			QMessageBox::warning(preProcessorWindow(), tr("Import failed"), tr("Importing data from %1 failed.").arg(QDir::toNativeSeparators(filename)));
			delete item;
			item = nullptr;
			goto ERROR;
		} else {
			QVector2D o = offset();
			geodata->applyOffset(o.x(), o.y());
			// the standarditem is set at the last position, so make it the first.
			QList<QStandardItem*> takenItems = m_standardItem->takeRow(item->standardItem()->row());
			m_standardItem->insertRows(0, takenItems);
			// add the item, in the front.
			m_childItems.push_front(item);
		}
		if (wDialog != nullptr) {
			wDialog->setProgress(i + 1);
		}
		setupConnectionToGeoData(geodata);
	}
	if (wDialog != nullptr) {
		wDialog->hide();
		delete wDialog;
	}
	// All imports succeeded.
	LastIODirectory::set(finfo.absolutePath());

	updateItemMap();
	updateZDepthRange();

	informValueRangeChange();
	informDataChange();

	dataModel()->fit();
	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectGeoData(item->standardItem()->index());
	setModified();

	// import is not undo-able.
	iRICUndoStack::instance().clear();
	return;

ERROR:
	if (wDialog != nullptr) {
		wDialog->hide();
		delete wDialog;
	}
	updateItemMap();
	updateZDepthRange();

	informValueRangeChange();
	informDataChange();

	dataModel()->fit();
	dataModel()->graphicsView()->ResetCameraClippingRange();
	setModified();

	// import is not undo-able.
	iRICUndoStack::instance().clear();
	return;
}

void PreProcessorGeoDataGroupDataItem::doExport()
{
	QStringList dataNames;
	QList<PreProcessorGeoDataDataItem*> datas;

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
		if (item->isExportAvailable()) {
			datas.append(item);
			dataNames.append(item->geoData()->caption());
		}
	}
	if (datas.count() == 0) {
		// no data to export.
		return;
	} else if (datas.count() == 1) {
		// there is only one data to export.
		PreProcessorGeoDataDataItem* item = datas.at(0);
		item->exportGeoData();
	} else {
		// there are multiple data to export.
		// ask user to select which data to export.
		bool ok;
		QString selectedName = QInputDialog::getItem(preProcessorWindow(), tr("Select data to export"), tr("Please select which data to export."), dataNames, 0, false, &ok);
		if (! ok) {
			// User canceled.
			return;
		}
		int index = dataNames.indexOf(selectedName);
		Q_ASSERT(index >= 0 && index < dataNames.count());
		PreProcessorGeoDataDataItem* item = datas.at(index);
		item->exportGeoData();
	}
}

void PreProcessorGeoDataGroupDataItem::addGeoData(QObject* c)
{
	GeoDataCreator* creator = dynamic_cast<GeoDataCreator*>(c);
	// create a new GeoData item.
	PreProcessorGeoDataDataItem* item = new PreProcessorGeoDataDataItem(this);
	// the standarditem is set at the last position, so make it the first.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(item->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	// add the item, in the front.
	m_childItems.push_front(item);
	// create an empty geodata.
	GeoData* geodata = creator->create(item, m_condition);
	item->setGeoData(geodata);
	setupConnectionToGeoData(geodata);
	// set name and caption
	creator->setNameAndDefaultCaption(this->childItems(), geodata);
	geodata->setupDataItem();

	// the background item should be at the last always.
	// update item map.
	updateItemMap();
	// update ZDepthRange
	updateZDepthRange();
	informDataChange();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectGeoData(item->standardItem()->index());
	geodata->showInitialDialog();
	setModified();
}

void PreProcessorGeoDataGroupDataItem::addBackground()
{
	// create an instance or GeoDataBackground, and add it.
	m_backgroundItem = new PreProcessorGeoDataDataItem(this);
	m_childItems.append(m_backgroundItem);
	GeoData* geodata = m_condition->buildBackgroundGeoData(m_backgroundItem);
	setupConnectionToGeoData(geodata);
	m_backgroundItem->setGeoData(geodata);
	geodata->setupDataItem();
}

void PreProcessorGeoDataGroupDataItem::moveBackgroundToLast()
{
	// make m_backgroundItem the last item in m_childItems.
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		if (*it == m_backgroundItem) {
			m_childItems.erase(it);
			break;
		}
	}
	m_childItems.push_back(m_backgroundItem);

	// QStandardItem order should be reordered manually.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(m_backgroundItem->standardItem()->row());
	// in deed, takenItems containes only one item, and that is background item.
	m_standardItem->appendRows(takenItems);
}

void PreProcessorGeoDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	GeoDataFactory& factory = GeoDataFactory::instance();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count() - 1; ++i) {
		PreProcessorGeoDataDataItem* item = new PreProcessorGeoDataDataItem(this);
		QDomNode child = children.at(i);
		// restore
		GeoData* geodata = factory.restore(child, item, m_condition);
		if (geodata != nullptr) {
			item->setGeoData(geodata);
			// load data.
			item->loadFromProjectMainFile(child);
			geodata->setupDataItem();
			setupConnectionToGeoData(geodata);
			item->loadCheckState(child);
			// insert the new item BEFORE the background item.
			int last = m_childItems.count() - 1;
			m_childItems.insert(last, item);
		} else {
			// for some reason, it could not be restored.
			delete item;
		}
	}
	// The last node must be the background item.
	QDomNode child = children.at(children.count() - 1);
	m_backgroundItem->loadFromProjectMainFile(child);

	// the background item should be at the last always.
//	moveBackgroundToLast();
//	informValueRangeChange();
}

void PreProcessorGeoDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_condition->name());
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("GeoData");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

bool PreProcessorGeoDataGroupDataItem::isChildCaptionAvailable(const QString& caption)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		if (dynamic_cast<PreProcessorGeoDataDataItem*>(*it)->geoData()->caption() == caption) {
			return false;
		}
	}
	return true;
}

int PreProcessorGeoDataGroupDataItem::mappingCount() const
{
	int dimCount = 1;
	if (dimensions()->containers().count() > 0) {
		dimCount = dimensions()->maxIndex() + 1;
	}
	int geodataCount = m_childItems.count();
	return dimCount * geodataCount;
}

void PreProcessorGeoDataGroupDataItem::executeMapping(Grid* grid, WaitDialog* dialog)
{
	/*
	if (m_childItems.count() == 1){
		// There is only background raw data.
		// No need to execute mapping.
		return;
	}
	*/
	GridAttributeContainer* container = grid->gridRelatedCondition(m_condition->name());
	bool* boolMap;
	int dataCount = container->dataCount();
	boolMap = new bool[dataCount];
	QList<GeoDataMapperSetting*> settings;
	if (dimensions()->containers().count() == 0) {
		// initialize

		// reset the boolmap.
		for (int i = 0; i < dataCount; ++i) {
			*(boolMap + i) = false;
		}
		for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
			PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
			GeoData* geodata = item->geoData();
			GeoDataMapper* mapper = geodata->mapper();
			mapper->setTarget(grid, container, geodata);
			settings.append(mapper->initialize(boolMap));
		}
		// do mapping

		// reset the boolmap.
		for (int i = 0; i < dataCount; ++i) {
			*(boolMap + i) = false;
		}
		int idx = 0;
		for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
			PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
			GeoData* geodata = item->geoData();
			GeoDataMapper* mapper = geodata->mapper();
			mapper->setTarget(grid, container, geodata);
			mapper->map(boolMap, settings.at(idx));
			++idx;
			if (dialog != nullptr) {
				dialog->setProgress(dialog->progress() + 1);
				qApp->processEvents();
			}
		}
		// terminate
		idx = 0;
		for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
			PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
			GeoData* geodata = item->geoData();
			GeoDataMapper* mapper = geodata->mapper();
			mapper->terminate(settings.at(idx));
			geodata->setMapped();
			++idx;
		}
	} else {
		int currentIndex = dimensions()->currentIndex();
		// initialize

		for (int i = 0; i < dataCount; ++i) {
			*(boolMap + i) = false;
		}
		for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
			PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
			GeoData* geodata = item->geoData();
			GeoDataMapper* mapper = geodata->mapper();
			mapper->setTarget(grid, container, geodata);
			settings.append(mapper->initialize(boolMap));
		}
		// do mapping
		for (int i = 0; i <= dimensions()->maxIndex(); ++i) {
			dimensions()->setCurrentIndex(i, true);
			for (int i = 0; i < dataCount; ++i) {
				*(boolMap + i) = false;
			}
			int idx = 0;
			for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
				PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
				GeoData* geodata = item->geoData();
				GeoDataMapper* mapper = geodata->mapper();
				mapper->setTarget(grid, container, geodata);
				mapper->map(boolMap, settings.at(idx));
				++idx;
				if (dialog != nullptr) {
					dialog->setProgress(dialog->progress() + 1);
					qApp->processEvents();
				}
			}
		}
		// terminate
		int idx = 0;
		for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
			PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
			GeoData* geodata = item->geoData();
			GeoDataMapper* mapper = geodata->mapper();
			mapper->terminate(settings.at(idx));
			geodata->setMapped();
			++idx;
		}
		dimensions()->setCurrentIndex(currentIndex);
	}
	container->setMapped(true);
	delete boolMap;
}

void PreProcessorGeoDataGroupDataItem::setDefaultValue(Grid* grid)
{
	GridAttributeContainer* container = grid->gridRelatedCondition(m_condition->name());
	bool* boolMap;
	int dataCount = container->dataCount();
	boolMap = new bool[dataCount];
	// reset the boolmap.
	// do mapping only by the backgroud item.
	GeoDataMapper* mapper = m_backgroundItem->geoData()->mapper();
	mapper->setTarget(grid, container, m_backgroundItem->geoData());
	for (int i = 0; i < dataCount; ++i) {
		*(boolMap + i) = false;
	}
	GeoDataMapperSetting* s = mapper->initialize(boolMap);
	for (int i = 0; i < dataCount; ++i) {
		*(boolMap + i) = false;
	}
	mapper->map(boolMap, s);
	mapper->terminate(s);
	delete boolMap;
}

void PreProcessorGeoDataGroupDataItem::informValueRangeChange()
{
	PreProcessorGeoDataTopDataItem* topItem = dynamic_cast<PreProcessorGeoDataTopDataItem*>(parent());
	if (topItem != nullptr) {
		topItem->informValueRangeChange(m_condition->name());
	}
}

void PreProcessorGeoDataGroupDataItem::informDataChange()
{
	dynamic_cast<PreProcessorGeoDataTopDataItem*>(parent())->informDataChange();
	updateCrossectionWindows();
}

bool PreProcessorGeoDataGroupDataItem::getValueRange(double* min, double* max)
{
	*min = 0;
	*max = 0;
	bool first = true;
	bool result = false;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
		// background item is not used for this.
		if (m_backgroundItem == item) {continue;}
		double tmpmin;
		double tmpmax;
		if (item->getValueRange(&tmpmin, &tmpmax)) {
			if (first || tmpmin < *min) {*min = tmpmin;}
			if (first || tmpmax > *max) {*max = tmpmax;}
			result = true;
			first = false;
		}
	}
	return result;
}

/*
	 bool PreProcessorGeoDataGroupDataItem::setupImportMenu(QMenu* menu)
	 {
		bool ok = false;
		GeoDataFactory& factory = GeoDataFactory::instance();
		const QList<GeoDataCreator*> creators = factory.compatibleCreators(m_condition);

		if (m_importSignalMapper){delete m_importSignalMapper;}
		m_importSignalMapper = new QSignalMapper(this);
		for (auto it = creators.begin(); it != creators.end(); ++it){
				const QList<GeoDataImporter*>& importers = (*it)->importers();
				for (auto imp_it = importers.begin(); imp_it != importers.end(); ++imp_it){
						QString title = (*imp_it)->caption();
						QAction* importAction = menu->addAction(title.append("..."));
						m_importSignalMapper->setMapping(importAction, *imp_it);
						connect(importAction, SIGNAL(triggered()), m_importSignalMapper, SLOT(map()));
						ok = true;
				}
		}
		connect(m_importSignalMapper, SIGNAL(mapped(QObject*)), this, SLOT(importGeoData(QObject*)));
		return ok;
	 }
 */

const QList<PreProcessorGeoDataDataItemInterface*> PreProcessorGeoDataGroupDataItem::geoDatas() const
{
	QList<PreProcessorGeoDataDataItemInterface*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItemInterface* item = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(*it);
		ret.append(item);
	}
	return ret;
}

void PreProcessorGeoDataGroupDataItem::editScalarsToColors()
{
	PreProcessorGridTypeDataItem* typedi = dynamic_cast<PreProcessorGridTypeDataItem*>(parent()->parent());
	ScalarsToColorsContainer* stc = typedi->scalarsToColors(condition()->name());
	ScalarsToColorsEditDialog* dialog = condition()->createScalarsToColorsEditDialog(preProcessorWindow());
	dialog->setWindowTitle(tr("%1 Color Setting").arg(m_condition->caption()));
	dialog->setContainer(stc);

	// @todo this operation is not made undo-able yet.
	int ret = dialog->exec();
	if (ret != QDialog::Accepted) {return;}
	renderGraphicsView();
}

void PreProcessorGeoDataGroupDataItem::editScalarBarLegendBox(PreProcessorScalarBarLegendBoxSettingDialog* dialog)
{
	if (m_condition->isOption()) {
		// discrete
		PreProcessorLegendBoxEditDialog legendboxDialog(dialog);
		legendboxDialog.setWidth(m_scalarBarSetting.width);
		legendboxDialog.setHeight(m_scalarBarSetting.height);
		legendboxDialog.setPositionX(m_scalarBarSetting.positionX);
		legendboxDialog.setPositionY(m_scalarBarSetting.positionY);
		legendboxDialog.setEntryTextSetting(dialog->labelTextSetting());
		if (legendboxDialog.exec() == QDialog::Accepted) {
			m_scalarBarSetting.width = legendboxDialog.width();
			m_scalarBarSetting.height = legendboxDialog.height();
			m_scalarBarSetting.positionX = legendboxDialog.positionX();
			m_scalarBarSetting.positionY = legendboxDialog.positionY();
			dialog->setLabelTextSetting(legendboxDialog.entryTextSetting());
		}
	} else {
		// continuous
		PreProcessorScalarBarEditDialog scalarbarDialog(dialog);
		scalarbarDialog.hideDisplayCheckBox();
		scalarbarDialog.setOrientation(m_scalarBarSetting.orientation);
		scalarbarDialog.setScalarBarTitle(m_title);
		scalarbarDialog.setNumberOfLabels(m_scalarBarSetting.numberOfLabels);
		scalarbarDialog.setWidth(m_scalarBarSetting.width);
		scalarbarDialog.setHeight(m_scalarBarSetting.height);
		scalarbarDialog.setPositionX(m_scalarBarSetting.positionX);
		scalarbarDialog.setPositionY(m_scalarBarSetting.positionY);
		scalarbarDialog.setTitleTextSetting(dialog->titleTextSetting());
		scalarbarDialog.setLabelTextSetting(dialog->labelTextSetting());
		scalarbarDialog.setLabelFormat(m_scalarBarSetting.labelFormat);

		if (scalarbarDialog.exec() == QDialog::Accepted) {
			m_scalarBarSetting.orientation = scalarbarDialog.orientation();
			m_title = scalarbarDialog.scalarBarTitle();
			m_scalarBarSetting.numberOfLabels = scalarbarDialog.numberOfLabels();
			m_scalarBarSetting.width = scalarbarDialog.width();
			m_scalarBarSetting.height = scalarbarDialog.height();
			m_scalarBarSetting.positionX = scalarbarDialog.positionX();
			m_scalarBarSetting.positionY = scalarbarDialog.positionY();
			dialog->setTitleTextSetting(scalarbarDialog.titleTextSetting());
			dialog->setLabelTextSetting(scalarbarDialog.labelTextSetting());
			m_scalarBarSetting.labelFormat = scalarbarDialog.labelFormat();
		}
	}
	renderGraphicsView();
}

void PreProcessorGeoDataGroupDataItem::addImportAction(QMenu* menu)
{
	QString cap = m_condition->caption();
	cap.append("...");
	QAction* a = new QAction(cap, menu);
	a->setEnabled(importAvailable());
	menu->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(import()));
}

bool PreProcessorGeoDataGroupDataItem::importAvailable()
{
	GeoDataFactory& factory = GeoDataFactory::instance();
	// create add menu.
	const QList<GeoDataCreator*> creators = factory.compatibleCreators(m_condition);
	for (auto it = creators.begin(); it != creators.end(); ++it) {
		const QList<GeoDataImporter*>& imps = (*it)->importers();
		if (imps.count() > 0) {return true;}
	}
	return false;
}

QStringList PreProcessorGeoDataGroupDataItem::getGeoDatasNotMapped()
{
	QStringList ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
		GeoData* geoData = item->geoData();
		if (! geoData->isMapped()) {
			ret.append(geoData->caption());
		}
	}
	return ret;
}

void PreProcessorGeoDataGroupDataItem::addCopyPolygon(GeoDataPolygon* polygon)
{
	GeoDataFactory& factory = GeoDataFactory::instance();
	// create add menu.
	const QList<GeoDataCreator*> creators = factory.compatibleCreators(m_condition);
	GeoDataPolygonCreator* c = nullptr;
	for (int i = 0; i < creators.count(); ++i) {
		c = dynamic_cast<GeoDataPolygonCreator*>(creators[i]);
		if (c != nullptr) {break;}
	}
	if (c == nullptr) {return;}

	// create a new GeoData item.
	PreProcessorGeoDataDataItem* item = new PreProcessorGeoDataDataItem(this);
	// the standarditem is set at the last position, so make it the first.
	QList<QStandardItem*> takenItems = m_standardItem->takeRow(item->standardItem()->row());
	m_standardItem->insertRows(0, takenItems);
	// add the item, in the front.
	m_childItems.push_front(item);
	// create an empty geodata.
	GeoData* geodata = c->create(item, m_condition);
	item->setGeoData(geodata);
	// set name and caption
	c->setNameAndDefaultCaption(this->childItems(), geodata);
	geodata->setupDataItem();

	// now copy the grid shape.
	GeoDataPolygon* newpol = dynamic_cast<GeoDataPolygon*>(geodata);
	newpol->copyShape(polygon);
	setupConnectionToGeoData(newpol);

	// the background item should be at the last always.
	// update item map.
	updateItemMap();
	// update ZDepthRange
	updateZDepthRange();

	dataModel()->objectBrowserView()->select(item->standardItem()->index());
	dataModel()->graphicsView()->ResetCameraClippingRange();
	emit selectGeoData(item->standardItem()->index());
	setModified();

	// this operation is not undo-able.
	iRICUndoStack::instance().clear();
}

void PreProcessorGeoDataGroupDataItem::informSelection(VTKGraphicsView* v)
{
	PreProcessorGeoDataTopDataItem* tItem = dynamic_cast<PreProcessorGeoDataTopDataItem*>(parent());
	tItem->informSelection(v);
}

void PreProcessorGeoDataGroupDataItem::informDeselection(VTKGraphicsView* v)
{
	PreProcessorGeoDataTopDataItem* tItem = dynamic_cast<PreProcessorGeoDataTopDataItem*>(parent());
	tItem->informDeselection(v);
}

void PreProcessorGeoDataGroupDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyPressEvent(event);
}

void PreProcessorGeoDataGroupDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyReleaseEvent(event);
}

void PreProcessorGeoDataGroupDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseDoubleClickEvent(event);
}

void PreProcessorGeoDataGroupDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void PreProcessorGeoDataGroupDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void PreProcessorGeoDataGroupDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void PreProcessorGeoDataGroupDataItem::exportAllPolygons()
{
	QString filename = QFileDialog::getSaveFileName(mainWindow(), tr("Export Polygons"), LastIODirectory::get(), tr("ESRI Shape file (*.shp)"));
	if (filename.isEmpty()) {return;}
	QString dbfFilename;
	GeoDataPolygonRealCreator* c = new GeoDataPolygonRealCreator();
	GeoDataPolygonShapeExporter* exporter = nullptr;
	bool isDouble;
	QTextCodec* codec = QTextCodec::codecForLocale();
	SHPHandle shpHandle;
	DBFHandle dbfHandle;
	bool codecOK = true;
	int index = 0;

	for (int i = 0; i < c->exporters().count(); ++i) {
		exporter = dynamic_cast<GeoDataPolygonShapeExporter*>(c->exporters().at(i));
		if (exporter != nullptr) {break;}
	}
	if (exporter == nullptr) {goto ERROR;}

	dbfFilename = filename;
	dbfFilename.replace(QRegExp(".shp$"), ".dbf");
	shpHandle = exporter->getSHPHandle(filename);
	dbfHandle = exporter->getDBFHandle(dbfFilename, condition(), &isDouble);

	for (int i = 0; i < m_childItems.count(); ++i) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(m_childItems.at(i));
		GeoData* rd = item->geoData();
		if (dynamic_cast<GeoDataPolygon*>(rd) == nullptr) {continue;}
		codecOK = codecOK && codec->canEncode(rd->caption());
	}
	if (! codecOK) {
		codec = QTextCodec::codecForName("UTF-8");
	}

	for (int i = m_childItems.count() - 1; i >= 0; --i) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(m_childItems.at(i));
		GeoData* rd = item->geoData();
		GeoDataPolygon* rdp = dynamic_cast<GeoDataPolygon*>(rd);
		if ((rdp) == nullptr) {continue;}
		SHPObject* obj = exporter->getSHPObject(rdp, shpHandle, index);
		SHPWriteObject(shpHandle, -1, obj);
		SHPDestroyObject(obj);

		exporter->outputAttributes(rdp, dbfHandle, index, isDouble, codec);
		++ index;
	}
	SHPClose(shpHandle);
	DBFClose(dbfHandle);
	return;

ERROR:
	delete c;
}

void PreProcessorGeoDataGroupDataItem::deleteAll()
{
	if (QMessageBox::No == QMessageBox::warning(mainWindow(), tr("Warning"), tr("Are you sure you want to delete all items in %1 group?").arg(standardItem()->text()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No)) {
		return;
	}
	for (int i = m_childItems.count() - 1; i >= 0; --i) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(m_childItems.at(i));
		GeoData* rd = item->geoData();
		if (dynamic_cast<GeoDataBackground*>(rd) != nullptr) {continue;}

		QModelIndex index = item->standardItem()->index();
		dataModel()->deleteItem(index);
	}
}

bool PreProcessorGeoDataGroupDataItem::polygonExists() const
{
	bool ret = false;
	for (int i = 0; i < m_childItems.count(); ++i) {
		PreProcessorGeoDataDataItem* item = dynamic_cast<PreProcessorGeoDataDataItem*>(m_childItems.at(i));
		GeoData* rd = item->geoData();
		ret = ret || (dynamic_cast<GeoDataPolygon*>(rd) != nullptr);
	}
	return ret;
}

void PreProcessorGeoDataGroupDataItem::saveToCgnsFile(const int fn)
{
	int index = 1;
	cg_user_data_write(iRIC::toStr(m_condition->name()).c_str());
	cg_gorel(fn, iRIC::toStr(m_condition->name()).c_str(), 0, NULL);
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataDataItem* ritem = dynamic_cast<PreProcessorGeoDataDataItem*>(*it);
		ritem->setIndex(index);
		ritem->saveToCgnsFile(fn);
		++ index;
	}
	cg_gorel(fn, "..", 0, NULL);
}

void PreProcessorGeoDataGroupDataItem::openCrossSectionWindow(GeoDataRiverSurvey* rs, double crosssection)
{
	GeoDataRiverSurveyCrosssectionWindowProjectDataItem* i = new GeoDataRiverSurveyCrosssectionWindowProjectDataItem(this, preProcessorWindow());
	i->window()->setRiverSurvey(rs);
	i->window()->setCrosssection(crosssection);
	m_crosssectionWindows.append(i);
	QMdiArea* cent = dynamic_cast<QMdiArea*>(iricMainWindow()->centralWidget());
	QMdiSubWindow* container = cent->addSubWindow(i->window());
	container->setWindowIcon(i->window()->icon());
	container->show();
	container->setFocus();
	i->window()->cameraFit();
}

void PreProcessorGeoDataGroupDataItem::updateCrossectionWindows()
{
	for (auto w_it = m_crosssectionWindows.begin(); w_it != m_crosssectionWindows.end(); ++w_it) {
		GeoDataRiverSurveyCrosssectionWindow* w = (*w_it)->window();
		w->updateRiverSurveys();
	}
}

void PreProcessorGeoDataGroupDataItem::toggleCrosssectionWindowsGridCreatingMode(bool gridMode, GeoDataRiverSurvey* rs)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		GeoDataRiverSurveyCrosssectionWindow* w = (*it)->window();
		w->toggleGridCreatingMode(gridMode, rs);
	}
}

void PreProcessorGeoDataGroupDataItem::informCtrlPointUpdateToCrosssectionWindows()
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		GeoDataRiverSurveyCrosssectionWindow* w = (*it)->window();
		w->update();
	}
}

void PreProcessorGeoDataGroupDataItem::requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* item)
{
	for (auto it = m_crosssectionWindows.begin(); it != m_crosssectionWindows.end(); ++it) {
		if (*it == item) {
			m_crosssectionWindows.erase(it);
			delete item;
			return;
		}
	}
}

bool PreProcessorGeoDataGroupDataItem::addToolBarButtons(QToolBar* parent)
{
	if (m_dimensions->selectWidgets().size() == 0) {return false;}
	for (int i = 0; i < m_dimensions->selectWidgets().size(); ++i) {
		GridAttributeDimensionSelectWidget* w = m_dimensions->selectWidgets().at(i);
		QAction* action = parent->addWidget(w);
		action->setVisible(true);
	}
	return true;
}

void PreProcessorGeoDataGroupDataItem::loadExternalData(const QString& /*filename*/)
{
	const QList<GridAttributeDimensionContainer*>& conts = m_dimensions->containers();
	QDir subDir(subPath());
	for (int i = 0; i < conts.size(); ++i) {
		GridAttributeDimensionContainer* cont = conts.at(i);
		QString fileName = QString("Dimension_%1.dat").arg(cont->name());
		cont->loadFromExternalFile(subDir.absoluteFilePath(fileName));
	}
}

void PreProcessorGeoDataGroupDataItem::saveExternalData(const QString& /*filename*/)
{
	const QList<GridAttributeDimensionContainer*>& conts = m_dimensions->containers();
	QDir subDir(subPath());
	for (int i = 0; i < conts.size(); ++i) {
		GridAttributeDimensionContainer* cont = conts.at(i);
		QString fileName = QString("Dimension_%1.dat").arg(cont->name());
		cont->saveToExternalFile(subDir.absoluteFilePath(fileName));
	}
}

QStringList PreProcessorGeoDataGroupDataItem::containedFiles()
{
	QStringList ret = PreProcessorDataItem::containedFiles();
	const QList<GridAttributeDimensionContainer*>& conts = m_dimensions->containers();
	for (int i = 0; i < conts.size(); ++i) {
		GridAttributeDimensionContainer* cont = conts.at(i);
		QString fileName = QString("Dimension_%1.dat").arg(cont->name());
		ret.append(fileName);
	}
	return ret;
}

void PreProcessorGeoDataGroupDataItem::setupConnectionToGeoData(GeoData* geodata)
{
	GridAttributeDimensionsContainer* dims = dimensions();
	connect(dims, SIGNAL(currentIndexChanged(int,int)), geodata, SLOT(handleDimensionCurrentIndexChange(int,int)));
	for (int i = 0; i < dims->containers().size(); ++i) {
		GridAttributeDimensionContainer* cont = dims->containers().at(i);
		connect(cont, SIGNAL(valuesChanged(QList<QVariant>,QList<QVariant>)), geodata, SLOT(handleDimensionValuesChange(QList<QVariant>,QList<QVariant>)));
	}
}

void PreProcessorGeoDataGroupDataItem::setDimensionsToFirst()
{
	m_dimensions->setCurrentIndex(0, true);
}