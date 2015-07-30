#ifndef SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitiongridattributet.h"

class GUICOREDLL_EXPORT SolverDefinitionGridComplexAttribute : public SolverDefinitionGridAttributeInteger
{

public:
	/// Constructor
	SolverDefinitionGridComplexAttribute(QDomElement node, const SolverDefinitionTranslator& translator);
	/// Destructor
	~SolverDefinitionGridComplexAttribute();

	const QDomElement& element() const;

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	QString undefinedString() const;
	QString undefinedEnglishString() const;

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;

	class Impl;
	Impl* m_impl;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
