/*
 * TextureLoad.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: bbq
 */

#include "TextureLoad.hpp"

#include <vtkAlgorithm.h>
#include <vtkImageAppendComponents.h>
#include <vtkImageExtractComponents.h>
#include <vtkJPEGReader.h>
#include <vtkOpenGLTexture.h>
#include <vtkSmartPointer.h>
#include <string>

vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(std::string rgbFile, std::string alphaFile)
{
	// Read RGB data from file.
	vtkSmartPointer<vtkJPEGReader> rgbReader = vtkSmartPointer<vtkJPEGReader>::New();
	rgbReader->SetFileName(rgbFile.c_str());

	// Read alpha data from file.
	vtkSmartPointer<vtkJPEGReader> alphaReader = vtkSmartPointer<vtkJPEGReader>::New();
	alphaReader->SetFileName(alphaFile.c_str());

	// Get red channel from RGB data.
	vtkSmartPointer<vtkImageExtractComponents> extractRedFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractRedFilter->SetInputConnection(rgbReader->GetOutputPort());
	extractRedFilter->SetComponents(0);
	extractRedFilter->Update();

	// Get green channel from RGB data.
	vtkSmartPointer<vtkImageExtractComponents> extractGreenFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractGreenFilter->SetInputConnection(rgbReader->GetOutputPort());
	extractGreenFilter->SetComponents(1);
	extractGreenFilter->Update();

	// Get blue channel from RGB data.
	vtkSmartPointer<vtkImageExtractComponents> extractBlueFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractBlueFilter->SetInputConnection(rgbReader->GetOutputPort());
	extractBlueFilter->SetComponents(2);
	extractBlueFilter->Update();

	// Get any channel from alpha data.
	vtkSmartPointer<vtkImageExtractComponents> extractHeightFilter = vtkSmartPointer<
	        vtkImageExtractComponents>::New();
	extractHeightFilter->SetInputConnection(alphaReader->GetOutputPort());
	extractHeightFilter->SetComponents(0);
	extractHeightFilter->Update();

	// Combine channels.
	vtkSmartPointer<vtkImageAppendComponents> appendFilter = vtkSmartPointer<
	        vtkImageAppendComponents>::New();
	appendFilter->SetInputConnection(0, extractRedFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractGreenFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractBlueFilter->GetOutputPort());
	appendFilter->AddInputConnection(0, extractHeightFilter->GetOutputPort());
	appendFilter->Update();

	// Load texture from combined image data.
	vtkSmartPointer<vtkOpenGLTexture> texture = vtkSmartPointer<vtkOpenGLTexture>::New();
	texture->SetInputConnection(appendFilter->GetOutputPort());
	return texture;

}
