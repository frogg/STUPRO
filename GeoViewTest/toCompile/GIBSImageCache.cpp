#include "GIBSImageCache.h"

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <vtkJPEGReader.h>
#include <vtkSmartPointer.h>
#include <iostream>


GIBSImageCache::GIBSImageCache(GIBSDataSource* imageSource) {
	SetImageSource(imageSource);
	this->networkManager = new QNetworkAccessManager();
}

GIBSImageCache::GIBSImageCache() : GIBSImageCache(new GIBSDataSource()) { }

GIBSImageCache::~GIBSImageCache() {
//	delete this->imageSource;
	delete this->networkManager;
}


void GIBSImageCache::SetImageSource(GIBSDataSource* imageSource) {
	this->imageSource = imageSource;
}

GIBSDataSource* GIBSImageCache::GetImageSource() {
	return this->imageSource;
}

const bool GIBSImageCache::DownloadImage(const GIBSImageProperties& imageProperties) {
	/* initialise and start the file download */
	char urlBuffer[PATH_BUFFER_SIZE];
	this->BuildImageUrl(urlBuffer, imageProperties);
	QUrl url(urlBuffer);

	QNetworkRequest request(url);
	QNetworkReply* reply = this->networkManager->get(request);

	QEventLoop replyLoop;
	QObject::connect(reply, SIGNAL(finished()), &replyLoop, SLOT(quit()));

	// wait until the download finished
	replyLoop.exec();

	if (reply->error() != QNetworkReply::NoError) {
		std::cout << "Error downloading image from " << urlBuffer
				<< ": error code " << reply->error() << std::endl;
		return false;
	}

	// create the folder in which to save the image
	char pathBuffer[PATH_BUFFER_SIZE];
	this->BuildImageCacheDirectory(pathBuffer, imageProperties);
	if (!QDir().mkpath(pathBuffer)) {
		// error creating the path
		return false;
	}

	// create and open the image file in write only mode
	this->BuildImageCacheImagePath(pathBuffer, imageProperties);
	QFile file(pathBuffer);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		// error creating and opening the file
		return false;
	}

	// write all downloaded data to the image file
	file.write(reply->readAll());

	return true;
}

const bool GIBSImageCache::CheckCache(const GIBSImageProperties& imageProperties) {
	char pathBuffer[PATH_BUFFER_SIZE];
	this->BuildImageCacheImagePath(pathBuffer, imageProperties);
	return QFile(pathBuffer).exists();
}

vtkImageData* GIBSImageCache::GetVtkImage(const GIBSImageProperties& imageProperties) {
	if (!this->CheckCache(imageProperties)) {
		if (!this->DownloadImage(imageProperties)) {
			// error downloading and saving the image
			return NULL;
		}
	}

	// return the cached image
	char imagePathBuffer[PATH_BUFFER_SIZE];
	this->BuildImageCacheImagePath(imagePathBuffer, imageProperties);

	vtkSmartPointer<vtkJPEGReader> reader = vtkSmartPointer<vtkJPEGReader>::New();
	reader->SetFileName(imagePathBuffer);
	reader->Update();

	vtkImageData* test = vtkImageData::New();
	test->ShallowCopy(reader->GetOutput());

	return test;
}

QImage* GIBSImageCache::GetImage(const GIBSImageProperties& imageProperties) {
	if (!this->CheckCache(imageProperties)) {
		if (!this->DownloadImage(imageProperties)) {
			// error downloading and saving the image
			return NULL;
		}
	}

	// return the cached image
	char imagePathBuffer[PATH_BUFFER_SIZE];
	this->BuildImageCacheImagePath(imagePathBuffer, imageProperties);

	QImage* image = new QImage(imagePathBuffer);
	return image;
}

void GIBSImageCache::BuildImageUrl(char buffer[], const GIBSImageProperties& imageProperties) {
	sprintf(buffer, API_URL_SCHEME.c_str(),
			this->imageSource->projection.c_str(),
			this->imageSource->productName.c_str(),
			imageProperties.year,
			imageProperties.month,
			imageProperties.day,
			this->imageSource->tileMatrixSet.c_str(),
			imageProperties.zoomLevel,
			imageProperties.tileRow,
			imageProperties.tileCol,
			this->imageSource->fileFormat.c_str());
}

void GIBSImageCache::BuildImageCacheDirectory(
		char buffer[], const GIBSImageProperties& imageProperties) {
	std::cout << this->imageSource->projection.c_str() << std::endl;
	sprintf(buffer, CACHE_DIRECTORY_SCHEME.c_str(),
			this->imageSource->projection.c_str(),
			this->imageSource->productName.c_str(),
			imageProperties.year,
			imageProperties.month,
			imageProperties.day,
			this->imageSource->tileMatrixSet.c_str(),
			imageProperties.zoomLevel,
			imageProperties.tileRow);
}

void GIBSImageCache::BuildImageCacheImagePath(
		char buffer[], const GIBSImageProperties& imageProperties) {
	sprintf(buffer, (CACHE_DIRECTORY_SCHEME + CACHE_FILE_SCHEME).c_str(),
			this->imageSource->projection.c_str(),
			this->imageSource->productName.c_str(),
			imageProperties.year,
			imageProperties.month,
			imageProperties.day,
			this->imageSource->tileMatrixSet.c_str(),
			imageProperties.zoomLevel,
			imageProperties.tileRow,
			imageProperties.tileCol,
			this->imageSource->fileFormat.c_str());
}
