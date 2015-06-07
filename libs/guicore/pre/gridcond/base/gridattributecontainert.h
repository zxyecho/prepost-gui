#ifndef GRIDRELATEDCONDITIONCONTAINERT_H
#define GRIDRELATEDCONDITIONCONTAINERT_H

#include "gridattributecontainer.h"
#include "../../../solverdef/solverdefinitiongridattributet.h"
#include "gridattributedimensionscontainer.h"
#include "gridattributedimensioncontainer.h"
#include "../../../project/projectcgnsfile.h"
#include "../../grid/grid.h"
#include <misc/stringtool.h>

#include <QFile>
#include <QDataStream>

#include <cgnslib.h>
#include <vtkDataArray.h>

#if CGNS_VERSION < 3100
#define cgsize_t int
#endif

template <class V>
class GridAttributeContainerT : public GridAttributeContainer
{
public:
	/// Constructor
	GridAttributeContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond)
		: GridAttributeContainer(grid, cond)
	{}
	/// Destructor
	virtual ~GridAttributeContainerT() {}
	/// Returns the value at the specified index.
	virtual V value(unsigned int index) = 0;
	/// Update the value at the specified index.
	virtual void setValue(unsigned int index, V value) = 0;

	bool loadFromCgnsFile(int fn, int B, int Z) override {
		GridAttributeDimensionsContainer* dims = dimensions();
		bool allok = true;
		if (dims == 0 || dims->containers().size() == 0) {
			allok = loadFromCgnsFileForIndex(fn, B, Z, 0);
		} else {
			for (int index = 0; index <= dims->maxIndex(); ++index) {
				bool ok = loadFromCgnsFileForIndex(fn, B, Z, index);
				if (ok) {
					ok = saveToExternalFile(temporaryExternalFilename(index));
				}
				allok = allok && ok;
			}
		}
		if (allok) {
			setMapped(true);
		}
		return allok;
	}

	bool loadFromCgnsFileForIndex(int fn, int B, int Z, int index) {
		// Goto "GridConditions" node.
		int ier;
		bool found = false;
		cgsize_t count = dataCount();
		ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, iRIC::toStr(name()).c_str(), 0, "end");
		QString aName = arrayNameForIndex(index);
		if (ier == 0) {
			// the corresponding node found.
			// Find "Value" array.
			int narrays;
			cg_narrays(&narrays);
			for (int i = 1; i <= narrays; ++i) {
				char arrayName[ProjectCgnsFile::BUFFERLEN];
				DataType_t dt;
				int dataDimension;
				cgsize_t dimensionVector[3];
				cg_array_info(i, arrayName, &dt, &dataDimension, dimensionVector);
				if (dataDimension != 1 || dimensionVector[0] != count) {return false;}
				if (dt == dataType() && QString(arrayName) == aName) {
					// We've found the array!
					// load data.
					V* data = new V[count];
					ier = cg_array_read(i, data);
					for (cgsize_t j = 0; j < count; ++j) {
						setValue(j, *(data + j));
					}
					delete[] data;
					found = true;
				} else if (dt == RealSingle && dataType() == RealDouble && QString(arrayName) == aName) {
					// We've found the array.
					float* data = new float[count];
					ier = cg_array_read(i, data);
					for (cgsize_t j = 0; j < count; ++j) {
						setValue(j, *(data + j));
					}
					delete[] data;
					found = true;
				}
			}
		}
		if (! found) {
			// not found.
			// use default value.
			SolverDefinitionGridAttributeT<V>* cond = dynamic_cast<SolverDefinitionGridAttributeT<V>*>(condition());

			V defaultVal = cond->defaultValue();
			for (cgsize_t j = 0; j < count; ++j) {
				setValue(j, defaultVal);
			}
		}
		return true;
	}

	bool saveToCgnsFile(int fn, int B, int Z) override {
		int ier;
		// Goto "GridConditions" node.
		ier = cg_goto(fn, B, "Zone_t", Z, "GridConditions", 0, "end");
		if (ier != 0) {return false;}
		// Delete the array if it already exists.
		cg_delete_node(const_cast<char*>(iRIC::toStr(name()).c_str()));
		// Create the user defined data node.
		ier = cg_user_data_write(const_cast<char*>(iRIC::toStr(name()).c_str()));
		if (ier != 0) {return false;}
		// Go to the user defined data node.
		ier = cg_gorel(fn, const_cast<char*>(iRIC::toStr(name()).c_str()), 0, "end");
		if (ier != 0) {return false;}

		GridAttributeDimensionsContainer* dims = dimensions();
		if (dims->containers().size() == 0) {
			return saveToCgnsFileForIndex(0);
		} else {
			bool allok = true;
			for (int index = 0; index <= dims->maxIndex(); ++index) {
				bool ok = loadFromExternalFile(temporaryExternalFilename(index));
				if (ok) {
					ok = saveToCgnsFileForIndex(index);
				}
				allok = allok && ok;
			}
			return allok;
		}
	}

	bool saveToCgnsFileForIndex(int index) {
		int ier;
		// Create the "Value" array
		cgsize_t dimensions[3];
		unsigned int count = dataCount();
		dimensions[0] = count;
		dimensions[1] = dimensions[2] = 0;
		V* data = new V[count];
		for (unsigned int i = 0; i < count; ++i) {
			data[i] = value(i);
		}
		QString aName = arrayNameForIndex(index);
		ier = cg_array_write(iRIC::toStr(aName).c_str() , dataType(), 1, dimensions, data);
		delete[] data;
		if (ier != 0) {return false;}
		return true;
	}

	virtual vtkDataArray* dataArray() const = 0;
	virtual vtkDataArray* dataArrayCopy() const = 0;
	virtual void setModified() override {
		dataArray()->Modified();
	}

	bool loadFromExternalFile(const QString& filename) override {
		QFile f(filename);
		bool ok = f.open(QIODevice::ReadOnly);
		if (!ok) {return false;}
		QDataStream s(&f);

		unsigned int count = dataCount();
		for (unsigned int i = 0; i < count; ++i) {
			V value;
			s >> value;
			setValue(i, value);
		}
		f.close();
		return true;
	}

	bool saveToExternalFile(const QString& filename) override {
		QFile f(filename);
		bool ok = f.open(QIODevice::WriteOnly);
		if (!ok) { return false;}
		QDataStream s(&f);

		unsigned int count = dataCount();
		for (unsigned int i = 0; i < count; ++i) {
			V val = value(i);
			s << val;
		}
		f.close();
		return true;
	}

	bool getValueRange(double* min, double* max) override {
		double range[2];
		dataArray()->GetRange(range);
		*min = range[0];
		*max = range[1];
		return true;
	}

protected:
	QString arrayNameForIndex(int index) {
		if (index == 0) {return "Value";}
		QString name("Value%1");
		return name.arg(index);
	}

	virtual DataType_t dataType() const = 0;
};

#endif // GRIDRELATEDCONDITIONCONTAINERT_H