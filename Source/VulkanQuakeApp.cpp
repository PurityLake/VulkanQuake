#include "VulkanQuakeApp.h"

void VulkanQuakeApp::Run() {
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}

// ------------------------
// Private methods
// ------------------------
void VulkanQuakeApp::InitWindow() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr << "SDL failed to initialise: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to initialise SDL");
	}

	window = SDL_CreateWindow("Vulkan Quake",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
	if (window == nullptr) {
		std::cerr << "SDL failed to create window: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to create SDL window");
	}

	SDL_SetWindowResizable(window, SDL_FALSE);
}

// ######################
// InitVulkan
// ######################
void VulkanQuakeApp::InitVulkan() {
	CreateInstance();
}

void VulkanQuakeApp::CreateInstance() {
	uint32_t extCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &extCount, nullptr);
	std::vector<const char*> extensionNames(extCount);
	SDL_Vulkan_GetInstanceExtensions(window, &extCount, extensionNames.data());

	if (!CheckExtensionsAvailable(extensionNames)) {
		throw std::runtime_error("Not all required extensions found!");
	}

	std::vector<const char*> layerNames{ };
#ifdef DEBUG
	layerNames.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

	VkApplicationInfo appInfo{ };
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = APP_NAME.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = APP_NAME.c_str();
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{ };

#ifdef __APPLE__
	extensionNames.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
	createInfo.ppEnabledExtensionNames = extensionNames.data();
	createInfo.enabledLayerCount = (uint32_t)layerNames.size();
	createInfo.ppEnabledLayerNames = layerNames.data();

	if (auto res = vkCreateInstance(&createInfo, nullptr, &instance); res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance!");
	}
}

void VulkanQuakeApp::MainLoop() {
	while (true) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}
	}
}

void VulkanQuakeApp::Cleanup() {
	vkDestroyInstance(instance, nullptr);
	if (window != nullptr) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
}

// --------------------------------
// Util Methods
// --------------------------------
struct IsProp {
	const char* name;

	IsProp(const char* name) noexcept : name(name) { }

	bool operator()(VkExtensionProperties& prop) const {
		return !std::strcmp(prop.extensionName, name);
	}
};

bool VulkanQuakeApp::CheckExtensionsAvailable(const std::vector<const char*> extensionNames) const {
	bool foundAll = true;
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	
	for (const auto& name : extensionNames) {
		if (!std::any_of(std::begin(extensions), std::end(extensions), IsProp(name))) {
			std::cerr << "Extension: \"" << name << "\" not found.\n";
			foundAll = false;
		}
	}
	return foundAll;
}