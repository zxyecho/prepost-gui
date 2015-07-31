#ifndef LASTIODIRECTORY_H
#define LASTIODIRECTORY_H

#include "misc_global.h"
#include <QString>

/// Container class thas store the directory that was used for I/O last time in the iRIC GUI.
class MISCDLL_EXPORT LastIODirectory
{

public:
	static QString get();
	static void set(const QString& val);

private:
	/// Constructor
	LastIODirectory();
	/// Destructor
	~LastIODirectory();
	static QString m_value;
};

#endif // LASTIODIRECTORY_H
