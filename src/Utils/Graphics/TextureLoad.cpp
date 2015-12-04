/*
 * TextureLoad.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: bbq
 */

#include <qimage.h>
#include <qrgb.h>
#include <vtkImageData.h>
#include <vtkOpenGLTexture.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include "TextureLoad.hpp"
#include <stdexcept>
#include <string>

vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(const QImage& rgb, const QImage& alpha)
{
	// Index for each color channel in an RGBA image.
	static const int CHANNEL_RED = 0;
	static const int CHANNEL_GREEN = 1;
	static const int CHANNEL_BLUE = 2;
	static const int CHANNEL_ALPHA = 3;

	// Number of channels in an RGBA image.
	static const int CHANNEL_COUNT = 4;

	// Create a VTK image to load the RGBA data into.
	vtkSmartPointer<vtkImageData> vtkimage = vtkImageData::New();

	// Store size of RGB input image.
	unsigned int width = rgb.width();
	unsigned int height = rgb.height();

	// Compare sizes of the RGB and alpha input image. If there is a mismatch, the texture cannot be loaded.
	if (width != (unsigned int)alpha.width() || height != (unsigned int)alpha.height())
	{
		throw std::runtime_error(QString("RGB (%1x%2) and Alpha (%3x%4) texture sizes mismatch")
				.arg(width).arg(height).arg(alpha.width()).arg(alpha.height()).toStdString());
	}

	// Initialize parameters for vtkImageData.
	vtkimage->SetExtent(0, width - 1, 0, height - 1, 0, 0);
	vtkimage->SetSpacing(1.0, 1.0, 1.0);
	vtkimage->SetOrigin(0.0, 0.0, 0.0);
	vtkimage->AllocateScalars(VTK_UNSIGNED_CHAR, CHANNEL_COUNT);

	// Iterate over image rows.
	for (unsigned int y = 0; y < height; y++)
	{
		// Get pointer to current target row. Image data will be written to this pointer.
		unsigned char * targetPixels = static_cast<unsigned char*>(vtkimage->GetScalarPointer(0, height - y - 1, 0));

		// Get pointers to current source rows. Image data will be read from these pointers.
		const QRgb* sourcePixelsRgb = reinterpret_cast<const QRgb*>(rgb.scanLine(y));
		const QRgb* sourcePixelsAlpha = reinterpret_cast<const QRgb*>(alpha.scanLine(y));

		// Iterate over image columns.
		for (unsigned int x = 0; x < width; x++)
		{
			// Get current pixel from each of the source rows.
			const QRgb& pxRgb = sourcePixelsRgb[x];
			const QRgb& pxAlpha = sourcePixelsAlpha[x];

			// Combine components from both source rows into target row. Component order is RGBA.
			targetPixels[x * CHANNEL_COUNT + CHANNEL_RED] = qRed(pxRgb);
			targetPixels[x * CHANNEL_COUNT + CHANNEL_GREEN] = qGreen(pxRgb);
			targetPixels[x * CHANNEL_COUNT + CHANNEL_BLUE] = qBlue(pxRgb);
			targetPixels[x * CHANNEL_COUNT + CHANNEL_ALPHA] = qRed(pxAlpha);
		}
	}

	// Load texture from combined image data.
	vtkSmartPointer<vtkOpenGLTexture> texture = vtkOpenGLTexture::New();
	texture->SetInputData(vtkimage);
	return texture;
}
