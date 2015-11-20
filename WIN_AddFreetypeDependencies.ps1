$ParaViewDir = (Get-Content bin/CMakeCache.txt) | Select-String -pattern "ParaView_DIR"
$ParaViewDir = $ParaViewDir.ToString().Trim("ParaView_DIR:PATH=").Replace("/", "\")
$ParaViewDir = "$ParaViewDir\lib\Debug"
$FreeTypeDirs = "$ParaViewDir\vtkRenderingFreeTypeOpenGLCS-pv4.3.lib;$ParaViewDir\vtkRenderingFreeTypeCS-pv4.3.lib;$ParaViewDir\vtkRenderingFreeTypeOpenGL-pv4.3.lib;$ParaViewDir\vtkRenderingFreeType-pv4.3.lib;"
(Get-Content bin/src/paraview_stupro.vcxproj) | ForEach-Object { $_ -replace "<AdditionalDependencies>", "<AdditionalDependencies>$FreeTypeDirs" } | Set-Content bin/src/paraview_stupro.vcxproj
