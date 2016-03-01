#include <View/vtkSMKronosViewProxy.h>

#include <View/KronosView.h>
#include <Utils/Misc/KronosLogger.hpp>

#include "vtkObjectFactory.h"
#include "vtkClientServerStream.h"
#include "vtkProcessModule.h"
#include "vtkSMInputProperty.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMSessionProxyManager.h"
#include "vtkSMSession.h"
#include "vtkSMRepresentationProxy.h"
#include "vtkSMSourceProxy.h"
#include "vtkClientServerStream.h"

vtkStandardNewMacro(vtkSMKronosViewProxy);

vtkSMKronosViewProxy::vtkSMKronosViewProxy() { }
vtkSMKronosViewProxy::~vtkSMKronosViewProxy() { }

void vtkSMKronosViewProxy::moveCamera(float latitude, float longitude) {
	vtkClientServerStream stream;
	stream << vtkClientServerStream::Invoke
	       << VTKOBJECT(this)
	       << "moveCamera"
	       << latitude
	       << longitude
	       << vtkClientServerStream::End;
	this->ExecuteStream(stream);
}

void vtkSMKronosViewProxy::moveCamera(float latitude, float longitude, float distance) {
	vtkClientServerStream stream;
	stream << vtkClientServerStream::Invoke
	       << VTKOBJECT(this)
	       << "moveCamera"
	       << latitude
	       << longitude
	       << distance
	       << vtkClientServerStream::End;
	this->ExecuteStream(stream);
}
