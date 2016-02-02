#include <Utils/ParaView/CustomFilterLoader.hpp>
#include <QDir>
#include <vtkSmartPointer.h>
#include <vtkSMProxyManager.h>
#include <vtkSMSessionProxyManager.h>
#include <string>

const char* CustomFilterLoader::customFilterPath = "res/filters";

void CustomFilterLoader::loadCustomFilters() {
	QDir filterDir(customFilterPath);

	auto fileList = filterDir.entryList();

	vtkSmartPointer<vtkSMSessionProxyManager> proxyMgr = vtkSMProxyManager::GetProxyManager()->GetActiveSessionProxyManager();

	for(auto it = fileList.begin(); it != fileList.end(); ++it) {
		QString curFileName = *it;

		if(curFileName.length() > 2 && (curFileName.endsWith(".xml") || curFileName.endsWith(".cpd"))) {
			QString curPath(customFilterPath);
			curPath.append("/" + curFileName);

			proxyMgr->LoadCustomProxyDefinitions(curPath.toStdString().c_str());
		}
	}
}
