#ifndef RAWDATANETCDF_H
#define RAWDATANETCDF_H

#include <guicore/pre/rawdata/rawdata.h>
#include <vtkSmartPointer.h>
#include <vtkPolygon.h>
#include <vtkDoubleArray.h>
#include <vtkStructuredGrid.h>
#include <vtkDataSetMapper.h>
#include <vtkThreshold.h>
#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>

#include <netcdf.h>

#include <QList>

class QAction;
class QPolygonF;
class RawDataNetcdfImporter;
class RawDataNetcdfXbandImporter;
class RawDataNetcdfGdalImporter;
class GridRelatedConditionDimensionContainer;

class RawDataNetcdf : public RawData
{
	Q_OBJECT

private:
	class RectRegion {
	public:
		double xMin;
		double xMax;
		double yMin;
		double yMax;
		RectRegion(double xmin, double xmax, double ymin, double ymax)
		{
			xMin = xmin;
			xMax = xmax;
			yMin = ymin;
			yMax = ymax;
		}

		bool pointIsInside(double x, double y) const
		{
			if (x < xMin){return false;}
			if (x > xMax){return false;}
			if (y < yMin){return false;}
			if (y > yMax){return false;}
			return true;
		}
		bool intersect(const QLineF& line) const
		{
			bool isInside1 = pointIsInside(line.x1(), line.y1());
			bool isInside2 = pointIsInside(line.x2(), line.y2());
			return (isInside1 != isInside2);
		}
	};

public:
	static const int MAX_DRAWCELLCOUNT;

	enum CoordinateSystemType {
		LonLat,
		XY
	};

	/// Constructor
	RawDataNetcdf(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition);
	virtual ~RawDataNetcdf();

	const QList<double> lonValues() const {return m_lonValues;}
	const QList<double> latValues() const {return m_latValues;}
	QList<double> lonValues() {return m_lonValues;}
	QList<double> latValues() {return m_latValues;}

	const QList<double> xValues() const {return m_xValues;}
	const QList<double> yValues() const {return m_yValues;}
	QList<double> xValues() {return m_xValues;}
	QList<double> yValues() {return m_yValues;}

	int xSize() const;
	int ySize() const;

	CoordinateSystemType coordinateSystemType() const {return m_coordinateSystemType;}
	QString coordinateSystemName() const {return m_coordinateSystemName;}

	int defineCoords(int ncid, int* xDimId, int* yDimId, int* lonDimId, int* latDimId, int* xVarId, int* yVarId, int* lonVarId, int* latVarId);
	int defineDimensions(int ncid, QList<int>& dimIds, QList<int>& varIds);
	int defineValue(int ncid, int xId, int yId, const QList<int>& dimIds, int *varId);
	int outputCoords(int ncid, int xId, int yId, int lonId, int latId);
	int outputDimensions(int ncid, const QList<int>& varIds);
	bool requestCoordinateSystem(){return true;}
	vtkStructuredGrid* grid() const {return m_grid;}
	void viewOperationEndedGlobal(PreProcessorGraphicsViewInterface* v);
	virtual double thresholdValue() = 0;

public slots:
	void handleDimensionCurrentIndexChange(int oldIndex, int newIndex);
	void handleDimensionValuesChange(const QList<QVariant>& /*before*/, const QList<QVariant>& /*after*/);

private:
	void setupActors();
	void updateShapeData();
	void updateActorSettings();
	void updateSimpifiedGrid(double xmin, double xmax, double ymin, double ymax);
	void updateRegionPolyData();

	static nc_type getNcType(SolverDefinitionGridRelatedConditionDimension* dim);
	static nc_type getNcType(SolverDefinitionGridRelatedCondition* cond);

protected:
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
	void updateFilename();
	void loadExternalData(const QString& filename);
	void saveExternalData(const QString& filename);
	void doApplyOffset(double x, double y);
	int getValueVarId(int ncid, int* varId);

	void getIJIndex(vtkIdType id, unsigned int *i, unsigned int *j) const;
	unsigned int vertexIndex(unsigned int i, unsigned int j) const;

	int lineLimitI(int j, int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const;
	int lineLimitJ(int i, int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const;
	int lineLimitI2(int iIn, int iOut, int dimI, int dimJ, const RectRegion& region) const;
	int lineLimitJ2(int jIn, int jOut, int dimI, int dimJ, const RectRegion& region) const;
	bool lineAtIIntersect(int i, int dimI, int dimJ, const RectRegion& region) const;
	bool lineAtJIntersect(int j, int dimI, int dimJ, const RectRegion& region) const;

	virtual void doHandleDimensionCurrentIndexChange(int oldIndex, int newIndex) = 0;
	virtual void doHandleDimensionValuesChange(GridRelatedConditionDimensionContainer* cont, const QList<QVariant>& before, const QList<QVariant>& after) = 0;

	vtkSmartPointer<vtkStructuredGrid> m_grid;
	vtkSmartPointer<vtkStructuredGrid> m_simplifiedGrid;
	int m_opacityPercent;

	vtkSmartPointer<vtkThreshold> m_threshold;
	vtkSmartPointer<vtkDataSetMapper> m_mapper;
	vtkSmartPointer<vtkActor> m_actor;

	vtkSmartPointer<vtkPolyData> m_regionPolyData;
	vtkSmartPointer<vtkPolyDataMapper> m_regionMapper;
	vtkSmartPointer<vtkActor> m_regionActor;

	QList<double> m_lonValues;
	QList<double> m_latValues;

	QList<double> m_xValues;
	QList<double> m_yValues;

	CoordinateSystemType m_coordinateSystemType;
	QString m_coordinateSystemName;

	bool m_isMasked;

public:
	friend class RawDataNetcdfImporter;
	friend class RawDataNetcdfXbandImporter;
	friend class RawDataNetcdfGdalImporter;
};

#endif // RAWDATANETCDF_H
