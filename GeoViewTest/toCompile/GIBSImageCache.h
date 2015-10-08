#ifndef GIBSImageCache_h
#define GIBSImageCache_h

#include <vtkStdString.h>
#include <vtkImageData.h>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>

struct GIBSDataSource {
	// see https://wiki.earthdata.nasa.gov/display/GIBS/GIBS+Available+Imagery+Products
	const vtkStdString projection = "wmts-geo";
	const vtkStdString productName = "BlueMarble_NextGeneration";//"MODIS_Terra_CorrectedReflectance_TrueColor";
	const vtkStdString tileMatrixSet = "EPSG4326_500m";//"EPSG4326_250m";
	const vtkStdString fileFormat = "jpg";
	const ushort maxZoomLayer = 9;

	GIBSDataSource() { }

	GIBSDataSource(vtkStdString productName, vtkStdString tileMatrixSet,
			ushort maxZoomLayer, vtkStdString fileFormat)
		: productName(productName),
		tileMatrixSet(tileMatrixSet),
		maxZoomLayer(maxZoomLayer),
		fileFormat(fileFormat) { }

	GIBSDataSource(vtkStdString projection, vtkStdString productName,
			vtkStdString tileMatrixSet, ushort maxZoomLayer,
			vtkStdString fileFormat)
		: projection(projection),
		productName(productName),
		tileMatrixSet(tileMatrixSet),
		maxZoomLayer(maxZoomLayer),
		fileFormat(fileFormat)  { }
};

struct GIBSImageProperties {
	int zoomLevel = 0;
	int tileRow = 0;
	int tileCol = 0;
	int day = 1;
	int month = 9;
	int year = 2015;

	GIBSImageProperties() { }

	GIBSImageProperties(int zoomLevel, int tileRow, int tileCol)
		: zoomLevel(zoomLevel),
		  tileRow(tileRow),
		  tileCol(tileCol) { }

	GIBSImageProperties(int zoomLevel, int tileRow, int tileCol, int day,
			int month, int year)
		: zoomLevel(zoomLevel),
		  tileRow(tileRow),
		  tileCol(tileCol),
		  day(day),
		  month(month),
		  year(year) { }
};

class GIBSImageCache {
public:
	// ./cache/gibs/{projection}/{productName}/{yyyy}-{mm}-{dd}/{tileMatrixSet}/{zoomLevel}/{tileRow}/{tileCol}.{fileFormat}
	const vtkStdString CACHE_SCHEME = "./cache/gibs/%s/%s/%04d-%02d-%02d/%s/%d/%d/%d.%s";
	const vtkStdString CACHE_DIRECTORY_SCHEME = "./cache/gibs/%s/%s/%04d-%02d-%02d/%s/%d/%d/";
	const vtkStdString CACHE_FILE_SCHEME = "%d.%s";
	const vtkStdString API_URL_SCHEME = "http://map1.vis.earthdata.nasa.gov/%s/%s/default/%04d-%02d-%02d/%s/%d/%d/%d.%s";

	const int PATH_BUFFER_SIZE = 512;

	GIBSImageCache();
	GIBSImageCache(GIBSDataSource* imageSource);
	~GIBSImageCache();

	GIBSDataSource* GetImageSource();
	void SetImageSource(GIBSDataSource* imageSource);

	const bool DownloadImage(const GIBSImageProperties& imageProperties);
	const bool CheckCache(const GIBSImageProperties& imageProperties);
	vtkImageData* GetVtkImage(const GIBSImageProperties& imageProperties);
	QImage* GetImage(const GIBSImageProperties& imageProperties);

private:
	GIBSDataSource* imageSource;
	QNetworkAccessManager* networkManager;

	void BuildImageUrl(char buffer[], const GIBSImageProperties& imageProperties);
	void BuildImageCacheDirectory(char buffer[], const GIBSImageProperties& imageProperties);
	void BuildImageCacheImagePath(char buffer[], const GIBSImageProperties& imageProperties);
};

#endif
