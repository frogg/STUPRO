#include "saveBilAsImage.hpp"

#include <QFile>
#include <QDebug>
#include <QDataStream>

void SaveBilAsImage::saveBilAsImage() {
  int tileWidth = 512;
  int tileHeight = 512;

  QFile bilFile("./heightmap.bil16");
  if (!bilFile.open(QIODevice::ReadOnly)) {
    qDebug() << "Failed to load bilfile";
    return;
  }

  QDataStream in(&bilFile);
  int shortsRead = 0;
  unsigned short heightValues[tileWidth * tileHeight];

  bool abortRead = false;
  for (int x = 0; x < tileWidth && !abortRead; x++) {
    for (int y = 0; y < tileHeight && !abortRead; y++) {
      char height[2];
      int bytesRead = in.readRawData(height, 2);
      if (bytesRead != 2) {
        abortRead = true;
        break;
      }
      heightValues[x + y * tileWidth] = ((height[0] << 8 | height[1]) * 4) % 65536;

      shortsRead++;
    }
  }

  if (abortRead) {
    qDebug() << "couldn't read full byte range";
    return;
  } else {
    qDebug() << "read" << shortsRead << "short values";
  }

  QImage heightmap(tileWidth, tileHeight, QImage::Format_RGB16);

  for (int y = 0; y < tileHeight; y++) {
    for (int x = 0; x < tileWidth; x++) {
      short heightValue = heightValues[x + y * tileWidth];
      QRgb pixelValue = qRgb(heightValue, heightValue, heightValue);
      heightmap.setPixel(x, y, pixelValue);
    }
  }

  heightmap.save("./heightmap.png", "PNG");
}

// QTEST_MAIN(SaveBilAsImage)
int main(int argc, char* argv[]) {
  SaveBilAsImage::saveBilAsImage();

  return 0;
}
