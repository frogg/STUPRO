#include "SaveBilAsImage.hpp"

#include <qdatastream.h>
#include <qdebug.h>
#include <qfile.h>
#include <qimage.h>
#include <qiodevice.h>
#include <qrgb.h>
#include <qstring.h>

QRgb SaveBilAsImage::colorScale(int min, int max, int value) {
	if (value > max / 2) {
		// green 255 + red scale
		return qRgb(((value - (max / 2)) * 0xff) / (max / 2), 0xff, 0x00);
	} else if (value > 0) {
		// green scale
		return qRgb(0x00, (value * 2 * 0xff) / max, 0x00);
	} else if (value == 0) {
		return qRgb(0x00, 0x00, 0x00);
	} else if (value >= min / 2) {
		// blue scale
		return qRgb(0x00, 0x00, (value * 2 * 0xff) / min);
	} else {
		// blue 255 + red scale
		return qRgb(((value - (min / 2)) * 0xff) / (min / 2), 0x00, 0xff);
	}
}

QString SaveBilAsImage::toBinaryString(short number) {
	QString ret("");
	for (int i = 0; i < 16; i++) {
		ret = (number & 0x0001 ? "1" : "0") + ret;
		number >>= 1;
	}
	return ret;
}

QString SaveBilAsImage::toBinaryString(char number) {
	QString ret("");
	for (int i = 0; i < 8; i++) {
		ret = (number & 0x0001 ? "1" : "0") + ret;
		number >>= 1;
	}
	return ret;
}

void SaveBilAsImage::saveBilAsImage() {
	int tileWidth = 2048;
	int tileHeight = 2048;

	QFile bilFile("./heightmap-uk.bil16");
	if (!bilFile.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to load bilfile";
		return;
	}

	QDataStream in(&bilFile);
	short *heightValues = new short[tileWidth * tileHeight];

	short minValue = 0;
	short maxValue = 0;

	bool abortRead = false;
	for (int y = 0; y < tileHeight && !abortRead; y++) {
		short previousValue = 0;
		char previousBytes[2] = { 0 };
		for (int x = 0; x < tileWidth && !abortRead; x++) {
			char height[2];
			int bytesRead = in.readRawData(height, 2);
			if (bytesRead != 2) {
				abortRead = true;
				break;
			}
			short heightValue = (height[1] << 8) | (unsigned char)height[0];

			if (heightValue < minValue) {
				minValue = heightValue;
			}
			if (heightValue > maxValue) {
				maxValue = heightValue;
			}

			heightValues[x + y * tileWidth] = heightValue;
		}
	}

	if (abortRead) {
		qDebug() << "couldn't read full byte range";
		return;
	}

	QImage heightmap(tileWidth, tileHeight, QImage::Format_RGB32);

	for (int y = 0; y < tileHeight; y++) {
		for (int x = 0; x < tileWidth; x++) {
			int heightValue = heightValues[x + y * tileWidth];
			QRgb pixelValue = SaveBilAsImage::colorScale(minValue, maxValue, heightValue);
			heightmap.setPixel(x, y, pixelValue);
		}
	}

	heightmap.save("./heightmap-uk.png", "PNG");
}

int main(int argc, char *argv[]) {
	SaveBilAsImage::saveBilAsImage();

	return 0;
}
