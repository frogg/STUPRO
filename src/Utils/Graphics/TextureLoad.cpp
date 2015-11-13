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
	vtkSmartPointer<vtkImageData> vtkimage = vtkImageData::New();

	int width = rgb.width();
	int height = rgb.height();

	if (width != alpha.width() || height != alpha.height())
	{
		throw std::runtime_error((
		        "RGB (" + std::to_string(width) + "x" + std::to_string(height) + ") and Alpha ("
		                + std::to_string(alpha.width()) + "x" + std::to_string(alpha.height())
		                + ") Texture sizes mismatch").c_str());
	}

	vtkimage->SetExtent(0, width - 1, 0, height - 1, 0, 0);
	vtkimage->SetSpacing(1.0, 1.0, 1.0);
	vtkimage->SetOrigin(0.0, 0.0, 0.0);
	vtkimage->AllocateScalars(VTK_UNSIGNED_CHAR, 4);

	for (int i = 0; i < height; i++)
	{
		unsigned char* row;
		row = static_cast<unsigned char*>(vtkimage->GetScalarPointer(0, height - i - 1, 0));

		const QRgb* linePixelsRgb = reinterpret_cast<const QRgb*>(rgb.scanLine(i));
		const QRgb* linePixelsAlpha = reinterpret_cast<const QRgb*>(alpha.scanLine(i));

		for (int j = 0; j < width; j++)
		{
			const QRgb& colRgb = linePixelsRgb[j];
			const QRgb& colAlpha = linePixelsAlpha[j];

			row[j * 4] = qRed(colRgb);
			row[j * 4 + 1] = qGreen(colRgb);
			row[j * 4 + 2] = qBlue(colRgb);
			row[j * 4 + 3] = qRed(colAlpha);
		}
	}

	// Load texture from combined image data.
	vtkSmartPointer<vtkOpenGLTexture> texture = vtkOpenGLTexture::New();
	texture->SetInputData(vtkimage);
	return texture;
}
