#include "GIBSImageCache.h"

#include <QDir>
#include <QEventLoop>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <iostream>


GIBSImageCache::GIBSImageCache(GIBSImageSource* imageSource) {
	SetImageSource(imageSource);
	this->networkManager = new QNetworkAccessManager();
}

GIBSImageCache::GIBSImageCache() : GIBSImageCache(new GIBSImageSource()) { }

GIBSImageCache::~GIBSImageCache() {
	delete this->imageSource;
	delete this->networkManager;
}


void GIBSImageCache::SetImageSource(GIBSImageSource* imageSource) {
	this->imageSource = imageSource;
}

GIBSImageCache::GIBSImageSource* GIBSImageCache::GetImageSource() {
	return this->imageSource;
}

const bool GIBSImageCache::DownloadImage(const GIBSImageProperties& imageProperties) {
	/* create and open the file to which the image should be saved */

	// create the folder in which to save the image
	char pathBuffer[200];
	this->buildImageCacheDirectory(pathBuffer, imageProperties);
	if (!QDir().mkpath(pathBuffer)) {
		// error creating the path
		return false;
	}

	// create and open the image file in write only mode
	this->buildImageCacheImagePath(pathBuffer, imageProperties);
	QFile file(pathBuffer);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		// error creating and opening the file
		return false;
	}

	/* initialise and start the file download */
	char urlBuffer[200];
	this->buildImageUrl(urlBuffer, imageProperties);
	QUrl url(urlBuffer);

	QNetworkRequest request(url);
	QNetworkReply* reply = this->networkManager->get(request);

	QEventLoop replyLoop;
	QObject::connect(reply, SIGNAL(finished()), &replyLoop, SLOT(quit()));

	// wait until the download finished
	replyLoop.exec();

	if (reply->error() != QNetworkReply::NoError) {
		// error downloading the file
		return false;
	}

	// write all downloaded data to the image file
	file.write(reply->readAll());

	return true;
}

const bool GIBSImageCache::CheckCache(const GIBSImageProperties& imageProperties) {
	char pathBuffer[200];
	this->buildImageCacheImagePath(pathBuffer, imageProperties);
	return QFile(pathBuffer).exists();
}

const vtkImageData* GIBSImageCache::GetImage(const GIBSImageProperties& imageProperties) {
	if (!this->CheckCache(imageProperties)) {
		this->DownloadImage(imageProperties);
	}

	// return the cached image
	char pathBuffer[200];
	this->buildImageCacheImagePath(pathBuffer, imageProperties);
	QFile file(pathBuffer);
	if (!file.open(QIODevice::Append)) {
		// error opening the file in append mode
		return NULL;
	}

	QTextStream stream(&file);
	stream << "test!";

	return NULL;
}

void GIBSImageCache::buildImageUrl(char buffer[], const GIBSImageProperties& imageProperties) {
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

void GIBSImageCache::buildImageCacheDirectory(
		char buffer[], const GIBSImageProperties& imageProperties) {
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

void GIBSImageCache::buildImageCacheImagePath(
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
