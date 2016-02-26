/*
 * vtkOpaqueOpenGLTexture.h
 *
 *  Created on: Feb 16, 2016
 *      Author: bbq
 */

#ifndef SRC_UTILS_GRAPHICS_VTKOPAQUEOPENGLTEXTURE_H_
#define SRC_UTILS_GRAPHICS_VTKOPAQUEOPENGLTEXTURE_H_

#include <vtkOpenGLTexture.h>
#include <iostream>

class vtkIndent;
class vtkOpenGLTexture;

/**
 * Workaround class that forces a texture to be recognized as opaque to work around a vtk bug.
 */
class vtkOpaqueOpenGLTexture : public vtkOpenGLTexture {
public:
	static vtkOpaqueOpenGLTexture* New();
	vtkTypeMacro(vtkOpaqueOpenGLTexture, vtkOpenGLTexture);
	virtual void PrintSelf(std::ostream& os, vtkIndent indent);

	vtkOpaqueOpenGLTexture();
	virtual ~vtkOpaqueOpenGLTexture();

	virtual int IsTranslucent() override;
};

#endif /* SRC_UTILS_GRAPHICS_VTKOPAQUEOPENGLTEXTURE_H_ */
