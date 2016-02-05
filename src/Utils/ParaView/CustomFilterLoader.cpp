#include <Utils/ParaView/CustomFilterLoader.hpp>
#include <QDir>
#include <vtkSmartPointer.h>
#include <vtkSMProxyManager.h>
#include <vtkSMSessionProxyManager.h>
#include <string>

const QString CustomFilterLoader::customFilterPath = "res/filters";

void CustomFilterLoader::loadCustomFilters() {
	// Get all files in the custom filter directory.
	QStringList fileList = QDir(customFilterPath).entryList();

	// Get the active proxy manager which will load the filters.
	vtkSmartPointer<vtkSMSessionProxyManager> proxyMgr = vtkSMProxyManager::GetProxyManager()->GetActiveSessionProxyManager();

	// Declare a pattern-izeable QString beforehand.
	QString fileNamePath(customFilterPath + "/%1");

	for(auto curFileName : fileList) {

		if(curFileName.length() > 2 // Exclude "." and ".."
		   && (curFileName.endsWith(".xml") || curFileName.endsWith(".cpd"))) { // Only allow xml and cpd files.

			// Use the proxy manager to load the custom filters using c-strings.
			proxyMgr->LoadCustomProxyDefinitions((fileNamePath.arg(curFileName)).toStdString().c_str());
		}
	}
}
