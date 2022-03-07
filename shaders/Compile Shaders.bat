@echo off
set /p name="File name: "
C:\VulkanSDK\1.2.189.2\Bin\glslc.exe %name%.vert -o v_%name%.spv
C:\VulkanSDK\1.2.189.2\Bin\glslc.exe %name%.frag -o f_%name%.spv
pause