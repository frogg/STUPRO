/*
 * vtkOpaqueOpenGLTexture.cxx
 *
 *  Created on: Feb 16, 2016
 *      Author: bbq
 */

#include <Utils/Graphics/vtkOpaqueOpenGLTexture.h>
#include <vtkObjectFactory.h>
#include <vtkIndent.h>

vtkStandardNewMacro(vtkOpaqueOpenGLTexture);

vtkOpaqueOpenGLTexture::vtkOpaqueOpenGLTexture()
{
}

vtkOpaqueOpenGLTexture::~vtkOpaqueOpenGLTexture()
{
}

void vtkOpaqueOpenGLTexture::PrintSelf(std::ostream& os, vtkIndent indent)
{
}

int vtkOpaqueOpenGLTexture::IsTranslucent()
{
	return 0;
}
