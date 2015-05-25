#ifndef LOOKUPTABLECONTAINER_H
#define LOOKUPTABLECONTAINER_H

#include "../guicore_global.h"
#include "scalarstocolorscontainer.h"
#include <guibase/colormapsettingwidget.h>

class GUICOREDLL_EXPORT LookupTableContainer : public ScalarsToColorsContainer
{
public:
	LookupTableContainer();
	LookupTableContainer(ProjectDataItem* d);
	LookupTableContainer(const LookupTableContainer& c);
	void update() override;
	virtual void setValueRange(double min, double max) override;
	void getValueRange(double* min, double* max) override;
	ColorMapSettingWidget::ColorMap colorMap() const {return m_colorMap;}
	void setColorMap(ColorMapSettingWidget::ColorMap cm) {m_colorMap = cm;}
	const ColorMapSettingWidget::CustomSetting& customSetting() const {return m_customSetting;}
	void setCustomSetting(ColorMapSettingWidget::CustomSetting cs) {m_customSetting = cs;}
	bool autoRange() const {return m_autoRange;}
	void setAutoRange(bool ar) {m_autoRange = ar;}
	double autoMin() const {return m_autoMin;}
	double autoMax() const {return m_autoMax;}
	double manualMin() const {return m_manualMin;}
	void setManualMin(double min) {m_manualMin = min;}
	double manualMax() const {return m_manualMax;}
	void setManualMax(double max) {m_manualMax = max;}
	LookupTableContainer& operator=(const LookupTableContainer& c);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	bool m_autoRange;
	double m_autoMin;
	double m_autoMax;
	double m_manualMin;
	double m_manualMax;
	ColorMapSettingWidget::ColorMap m_colorMap;
	ColorMapSettingWidget::CustomSetting m_customSetting;
};

#endif // LOOKUPTABLECONTAINER_H
