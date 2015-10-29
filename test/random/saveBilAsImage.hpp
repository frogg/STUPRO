#ifndef KRONOS_TEST_SAVEBILASIMAGE_HPP
#define KRONOS_TEST_SAVEBILASIMAGE_HPP

#include <QtTest/QtTest>

class SaveBilAsImage
{
public:
	static void saveBilAsImage();
private:
	static QRgb colorScale(int min, int max, int value);
	static QString toBinaryString(short number);
	static QString toBinaryString(char number);
};

#endif
