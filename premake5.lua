workspace "LearnVulkan"
	architecture "x64"
	configurations { "Debug", "Release" }
	
project "LearnVulkan"
	location "LearnVulkan"
	kind "ConsoleApp"
	language "C++"
	targetdir "bin/%{cfg.buildcfg}"
	
	files { "%{prj.name}/**.h", "%{prj.name}/**.cpp" }
	
	includedirs 
	{
		"C:/VulkanSDK/1.2.189.2/Include",
		"$(SolutionDir)Dependencies/glm-0.9.9.7",
		"$(SolutionDir)Dependencies/glfw-3.3.5/include"
	}
	
	libdirs
	{
		"C:/VulkanSDK/1.2.189.2/Lib",
		"$(SolutionDir)Dependencies/glfw-3.3.5/lib-vc2019"
	}
	
	links
	{
		"vulkan-1.lib",
		"glfw3.lib"
	}
	
	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"
		
	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"