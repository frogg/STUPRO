<#
This script can be used on Windows to automatically add the FreeType-library-dependencies
to the generated Visual Studio solution.
#>

<# Load the Visual Basic namespace for the use of InputBoxes. #>
[System.Reflection.Assembly]::LoadWithPartialName('Microsoft.VisualBasic') | Out-Null

$binaryPath = [Microsoft.VisualBasic.Interaction]::InputBox("Enter the binary folder (in form of '../bin/') below:", "Kronos build-folder", "../bin/")

if($binaryPath -eq "") {
	"Probably aborted or entered empty path. Aborting!"
	break
}

if(!(Test-Path $binaryPath* -include CMakeCache.txt)) {
	"Could not find CMakeCache.txt in $binaryPath!"
	break
}

$buildMode = [Microsoft.VisualBasic.Interaction]::InputBox("Enter the used build mode below (either 'Debug' or 'Release'): ", "Kronos build-mode", "Debug")

$cmakeCachePath = $binaryPath + "CMakeCache.txt"
<# Search for ParaView_DIR in the CMakeCache to get the path of ParaView automatically. #>
$paraviewDir = ((Get-Content $cmakeCachePath) | Select-String -pattern "ParaView_DIR").ToString().Replace("ParaView_DIR:PATH=", "").Replace("/", "\") + "\lib\$buildMode"

$paraviewVersion = [Microsoft.VisualBasic.Interaction]::InputBox("Enter the used ParaView version ('4.3' or '4.4' are supported):", "Kronos ParaView-version", "4.3")

if($paraviewVersion -eq "4.3")
{
	$freetypeLibs = "$paraviewDir\vtkRenderingFreeTypeOpenGLCS-pv4.3.lib;$paraviewDir\vtkRenderingFreeTypeCS-pv4.3.lib;$paraviewDir\vtkRenderingFreeTypeOpenGL-pv4.3.lib;$paraviewDir\vtkRenderingFreeType-pv4.3.lib;"
}
elseif($paraviewVersion -eq "4.4")
{
	$freetypeLibs = "$paraviewDir\vtkRenderingFreeTypeCS-pv4.4.lib;$paraviewDir\vtkRenderingFreeType-pv4.4.lib;"
}
else
{
	"ParaView version has to be either 4.3 or 4.4 (was $paraviewVersion)!"
	break;
}

$projectPath = $binaryPath + "src/paraview_stupro.vcxproj"
<# Use the paths and other info to replace the beginning of the additional dependencies of the default project file with the added libs. #>
(Get-Content $projectPath) | ForEach-Object { $_ -replace "<AdditionalDependencies>", "<AdditionalDependencies>$freetypeLibs" } | Set-Content $projectPath
