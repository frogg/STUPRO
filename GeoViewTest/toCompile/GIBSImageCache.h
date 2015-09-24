#ifndef GIBSImageCache_h
#define GIBSImageCache_h

#include <vtkStdString.h>
#include <vtkImageData.h>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDateTime>
#include <QFile>

class GIBSImageCache {
public:
	// ./cache/gibs/{projection}/{productName}/{yyyy}-{mm}-{dd}/{tileMatrixSet}/{zoomLevel}/{tileRow}/{tileCol}.{fileFormat}
	const vtkStdString CACHE_SCHEME = "./cache/gibs/%s/%s/%04d-%02d-%02d/%s/%d/%d/%d.%s";
	const vtkStdString CACHE_DIRECTORY_SCHEME = "./cache/gibs/%s/%s/%04d-%02d-%02d/%s/%d/%d/";
	const vtkStdString CACHE_FILE_SCHEME = "%d.%s";
	const vtkStdString API_URL_SCHEME = "http://map1.vis.earthdata.nasa.gov/%s/%s/default/%04d-%02d-%02d/%s/%d/%d/%d.%s";

	struct GIBSImageSource {
		vtkStdString projection = "wmts-geo";
		vtkStdString productName = "MODIS_Terra_CorrectedReflectance_TrueColor";
		vtkStdString tileMatrixSet = "EPSG4326_250m";
		vtkStdString fileFormat = "jpg";
		ushort maxZoomLayer = 9;

		GIBSImageSource() { }

		GIBSImageSource(vtkStdString productName,
				vtkStdString tileMatrixSet, ushort maxZoomLayer,
				vtkStdString fileFormat)
			: productName(productName),
			tileMatrixSet(tileMatrixSet),
			maxZoomLayer(maxZoomLayer),
			fileFormat(fileFormat) { }

		GIBSImageSource(vtkStdString projection, vtkStdString productName,
				vtkStdString tileMatrixSet, ushort maxZoomLayer,
				vtkStdString fileFormat)
			: projection(projection),
			productName(productName),
			tileMatrixSet(tileMatrixSet),
			maxZoomLayer(maxZoomLayer),
			fileFormat(fileFormat)  { }
	};

	struct GIBSImageProperties {
		int tileRow = 0;
		int tileCol = 0;
		int zoomLevel = 0;
		int day = 1;
		int month = 9;
		int year = 2015;

		GIBSImageProperties() { }

		GIBSImageProperties(int tileRow, int tileCol, int zoomLevel, int day,
				int month, int year)
			: tileRow(tileRow),
			  tileCol(tileCol),
			  zoomLevel(zoomLevel),
			  day(day),
			  month(month),
			  year(year) { }
	};

	GIBSImageCache();
	GIBSImageCache(GIBSImageSource* imageSource);
	~GIBSImageCache();

	GIBSImageSource* GetImageSource();
	void SetImageSource(GIBSImageSource* imageSource);

	const bool DownloadImage(const GIBSImageProperties& imageProperties);
	const bool CheckCache(const GIBSImageProperties& imageProperties);
	const vtkImageData* GetImage(const GIBSImageProperties& imageProperties);

private:
	GIBSImageSource* imageSource;
	QNetworkAccessManager* networkManager;

	void buildImageUrl(char buffer[], const GIBSImageProperties& imageProperties);
	void buildImageCacheDirectory(char buffer[], const GIBSImageProperties& imageProperties);
	void buildImageCacheImagePath(char buffer[], const GIBSImageProperties& imageProperties);
};

#endif
