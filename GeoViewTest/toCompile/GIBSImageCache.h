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

	/**
	 * Creates a new GIBSImageCache instance using the default data source.
	 */
	GIBSImageCache();

	/**
	 * Creates a new GIBSImageCache instance using the given data source.
	 * @param dataSource GIBSDataSource to use for this image cache.
	 */
	GIBSImageCache(GIBSDataSource* dataSource);
	~GIBSImageCache();


	/**
	 * @return the GIBSDataSource used for this image cache
	 */
	GIBSDataSource* GetDataSource();
	/**
	 * @param imageSource the GIBSDataSource to use for this image cache
	 */
	void SetImageSource(GIBSDataSource* imageSource);

	/**
	 * Tries to download the GIBS image with the given properties.
	 * This method is synchronous, i.e. it returns after the image has been
	 * downloaded (which can take a while).
	 * @param imageProperties the properties of the image to download
	 * @return true if the download was successful
	 */
	const bool DownloadImage(const GIBSImageProperties& imageProperties);
	/**
	 * Checks if an image with the given properties is already cached on the
	 * hard drive.
	 * @param imageProperties the properties of the image to download
	 * @return true if the image exists on the hard drive
	 */
	const bool CheckCache(const GIBSImageProperties& imageProperties);
	/**
	 * Returns the GIBS image with the given properties in the VTK format.
	 * Checks if the image is already cached, downloads it if it is not, loads
	 * the image from the disk and returns it.
	 * @param imageProperties the properties of the image to download
	 * @return the loaded VTK image
	 */
	vtkImageData* GetVtkImage(const GIBSImageProperties& imageProperties);
	/**
	 * Returns the GIBS image with the given properties as a QImage.
	 * Checks if the image is already cached, downloads it if it is not, loads
	 * the image from the disk and returns it.
	 * @param imageProperties the properties of the image to download
	 * @return the loaded QImage
	 */
	QImage* GetImage(const GIBSImageProperties& imageProperties);

private:
	/**
	 * The data source to use for all requests on this cache.
	 */
	GIBSDataSource* dataSource;
	/**
	 * The network manager used to download files from the GIBS API
	 */
	QNetworkAccessManager* networkManager;

	/**
	 * Builds the image URL using the given image properties
	 * @param buffer the buffer in which to store the URL
	 * @param imageProperties the image properties with which to build the URL
	 */
	void BuildImageUrl(char buffer[], const GIBSImageProperties& imageProperties);
	/**
	 * Builds the image cache folder path using the given image properties
	 * @param buffer the buffer in which to store the path
	 * @param imageProperties the image properties with which to build the path
	 */
	void BuildImageCacheDirectory(char buffer[], const GIBSImageProperties& imageProperties);
	/**
	 * Builds the image file cache path using the given image properties
	 * @param buffer the buffer in which to store the file's path
	 * @param imageProperties the image properties with which to build the path
	 */
	void BuildImageCacheImagePath(char buffer[], const GIBSImageProperties& imageProperties);
};

#endif
