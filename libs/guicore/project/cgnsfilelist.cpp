#include "cgnsfileentry.h"
#include "cgnsfilelist.h"

#include <QDomElement>
#include <QXmlStreamWriter>

CgnsFileList::CgnsFileList(ProjectDataItem* parent) :
	ProjectDataItem(parent),
	m_current {nullptr}
{}

CgnsFileList::~CgnsFileList()
{}

QList<CgnsFileEntry*> CgnsFileList::cgnsFiles() const
{
	QList<CgnsFileEntry*> ret;
	for (auto it = m_cgnsFiles.begin(); it != m_cgnsFiles.end(); ++it) {
		ret.push_back(*it);
	}
	return ret;
}

CgnsFileEntry* CgnsFileList::current() const
{
	return m_current;
}

CgnsFileEntry* CgnsFileList::setCurrent(const QString& name)
{
	if (m_cgnsFiles.contains(name) && (m_current == nullptr || m_current->filename() != name)) {
		m_current = m_cgnsFiles.value(name);
		emit cgnsFileSwitched(name);
		return m_current;
	}
	return nullptr;
}

bool CgnsFileList::exists(const QString& name) const
{
	return m_cgnsFiles.contains(name);
}

void CgnsFileList::add(const QString& name)
{
	CgnsFileEntry* entry = new CgnsFileEntry(name, this);
	m_cgnsFiles.insert(name, entry);
	emit cgnsFilesUpdated(cgnsFiles());
}

void CgnsFileList::remove(const QString& name)
{
	if (m_cgnsFiles.contains(name)) {
		CgnsFileEntry* entry = m_cgnsFiles.value(name);
		if (entry == m_current) {m_current = nullptr;}
		delete entry;
		m_cgnsFiles.remove(name);
		emit cgnsFilesUpdated(cgnsFiles());
	}
}

QStringList CgnsFileList::containedFiles()
{
	QStringList ret;
	for (auto it = m_cgnsFiles.begin(); it != m_cgnsFiles.end(); ++it) {
		QString fname = (*it)->filename();
		fname.append(".cgn");
		ret << fname;
	}
	return ret;
}

QString CgnsFileList::proposeFilename()
{
	QString name("Case%1");
	int i = 1;
	while (m_cgnsFiles.contains(name.arg(i))) {
		++i;
	}
	return name.arg(i);
}

void CgnsFileList::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_cgnsFiles.clear();
	// All child nodes are CgnsFileEntry nodes.
	QDomNode child = node.firstChild();
	while (! child.isNull()) {
		CgnsFileEntry* entry = new CgnsFileEntry(this);
		entry->loadFromProjectMainFile(child);
		m_cgnsFiles.insert(entry->filename(), entry);
		child = child.nextSibling();
	}
	QDomElement elem = node.toElement();
	QString curr = elem.attribute("current");
	if (m_cgnsFiles.contains(curr)) {
		m_current = m_cgnsFiles.value(curr);
	}
}

void CgnsFileList::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	if (m_current != nullptr) {
		writer.writeAttribute("current", m_current->filename());
	}
	for (auto it = m_cgnsFiles.begin(); it != m_cgnsFiles.end(); ++it) {
		writer.writeStartElement("CgnsFileEntry");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}
