#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorarrowgroupdataitem.h"
#include <guibase/structuredgridregion.h>
#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>
#include <vtkMaskPoints.h>
#include <vtkUnstructuredGrid.h>

class Post2dWindowNodeVectorArrowGroupStructuredDataItem : public Post2dWindowNodeVectorArrowGroupDataItem
{
	Q_OBJECT
public:
	Post2dWindowNodeVectorArrowGroupStructuredDataItem(Post2dWindowDataItem* parent);
	virtual ~Post2dWindowNodeVectorArrowGroupStructuredDataItem();
protected:
	virtual void updateActivePoints() override;
	void doLoadFromProjectMainFile(const QDomNode& /*node*/) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) override;
	QDialog* propertyDialog(QWidget* p) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
private:
	int m_iSampleRate;
	int m_jSampleRate;
	StructuredGridRegion::Range2d m_range;
	vtkSmartPointer<vtkMaskPoints> m_arrowMask;
	vtkSmartPointer<vtkExtractGrid> m_arrowExtract;
public:
	friend class Post2dWindowArrowStructuredSetProperty;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_H
