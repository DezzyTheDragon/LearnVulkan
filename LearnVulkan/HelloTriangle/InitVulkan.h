#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#include <vulkan/vulkan.h>

class InitVulkan
{
public:
	InitVulkan();
	InitVulkan(VkInstance* inst);
	VkInstance* getInstance();
	void cleanup();
private:
	void createInstance();

	//Class members
	//References
	VkInstance* p_instance;		//Hold a reference to the vulkan instance, it will be needed in other classes 
};