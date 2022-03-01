#define GLFW_INCLUDE_VULKAN
#define _CRTDGB_MAP_ALLOC
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <stdlib.h>
#include <crtdbg.h>

//#include "HelloTriangle.h"
#include "HelloTriangle/HelloTriangleMain.h"

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);


	//HelloTriangle ht;
	HelloTriangleMain ht;

	try
	{
		ht.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}


	_CrtDumpMemoryLeaks();
	return EXIT_SUCCESS;
}