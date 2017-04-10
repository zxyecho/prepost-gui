#include "pointscsvimporter.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStringList>
#include <QTextStream>
#include <QVector3D>

QStringList PointsCsvImporter::fileDialogFilters()
{
	QStringList ret;

	ret << tr("Topography File (*.tpo)");
	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList PointsCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "tpo" << "csv";
	return ret;
}

bool PointsCsvImporter::importData(std::vector<QVector3D*>* points, QPointF* offset, const QString& filename, QWidget* w)
{
	QFile file(filename);
	if (! file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(w, tr("Error"), tr("%1 could not be opened.").arg(QDir::toNativeSeparators(filename)));
		return false;
	}

	QTextStream stream(&file);
	bool first = true;

	do {
		QString str = stream.readLine();
		str.replace(",", " ");

		auto frags = str.split(QRegExp("\\s+"), QString::SkipEmptyParts);
		if (frags.size() < 3) {continue;}

		QTextStream strstream(&str, QIODevice::ReadOnly);
		int index;
		double x, y, z;

		if (frags.size() == 3){
			strstream >> x >> y >> z;
		} else if (frags.size() == 4) {
			strstream >> index >> x >> y >> z;
		} else {
			continue;
		}

		if (first && offset->x() == 0 && offset->y() == 0) {
			offset->setX(x);
			offset->setY(y);
		}

		QVector3D* v = new QVector3D(x - offset->x(), y - offset->y(), z);
		points->push_back(v);

		first = false;
	} while (! stream.atEnd());

	return true;
}