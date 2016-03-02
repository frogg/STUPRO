#include <View/vtkSMStuproViewProxy.h>

#include <View/vtkPVStuproView.h>
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

vtkStandardNewMacro(vtkSMStuproViewProxy);

vtkSMStuproViewProxy::vtkSMStuproViewProxy() { }
vtkSMStuproViewProxy::~vtkSMStuproViewProxy() { }

void vtkSMStuproViewProxy::moveCamera(float latitude, float longitude) {
	vtkClientServerStream stream;
	stream << vtkClientServerStream::Invoke
	       << VTKOBJECT(this)
	       << "moveCamera"
	       << latitude
	       << longitude
	       << vtkClientServerStream::End;
	this->ExecuteStream(stream);
}

void vtkSMStuproViewProxy::moveCamera(float latitude, float longitude, float distance) {
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
