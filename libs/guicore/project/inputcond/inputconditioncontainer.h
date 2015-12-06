#ifndef INPUTCONDITIONCONTAINER_H
#define INPUTCONDITIONCONTAINER_H

#include "../../guicore_global.h"

#include <QString>
#include <QObject>
#include <QVariant>

#include <string>

class QDir;
class QTextStream;

namespace YAML {
	class Node;
} // namespace YAML

class GUICOREDLL_EXPORT InputConditionContainer : public QObject
{
	Q_OBJECT

public:
	InputConditionContainer();
	InputConditionContainer(const QString& name, const QString& caption);
	InputConditionContainer(const InputConditionContainer& c);
	virtual ~InputConditionContainer();

	void setName(const QString& name);
	void setBCProperty(const QString& bcname, int bcindex);
	void setComplexProperty(const QString& compname, int compindex);

	void copyValues(const InputConditionContainer& c);

	virtual int load() = 0;
	virtual int save() = 0;
	virtual void clear() = 0;

	virtual QVariant variantValue() const = 0;

	virtual void importFromYaml(const YAML::Node& doc, const QDir& dir) = 0;
	virtual void exportToYaml(QTextStream* stream, const QDir& dir) = 0;

signals:
	void valueChanged();

protected:
	const std::string& name() const;
	const QString& caption() const;

	bool isBoundaryCondition() const;
	const std::string& bcName() const;
	int bcIndex() const;

	bool isComplexCondition() const;
	const std::string& complexName() const;
	int complexIndex() const;

	static char* toC(const std::string& str);

private:
	std::string m_name;
	QString m_caption;

	bool m_isBoundaryCondition;
	std::string m_bcName;
	int m_bcIndex;

	bool m_isComplexCondition;
	std::string m_complexName;
	int m_complexIndex;
};
#endif // INPUTCONDITIONCONTAINER_H
