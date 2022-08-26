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
	SetUpDebugMessenger();
}

void VulkanQuakeApp::CreateInstance() {
	if (enableValidationLayers && !CheckValidaitonLayerSupport()) {
		throw std::runtime_error("Validation layers requested, but not available.");
	}

	auto extensions = GetRequiredExtensions();

	VkApplicationInfo appInfo{ };
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = APP_NAME.c_str();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = APP_NAME.c_str();
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{ };
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
#ifdef __APPLE__
	extensionNames.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
	createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (utils::FunctionFailed(vkCreateInstance(&createInfo, nullptr, &instance))) {
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
	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
	if (window != nullptr) {
		SDL_DestroyWindow(window);
	}
	SDL_Quit();
}

// --------------------------------
// Util Methods
// --------------------------------
struct IsExtensionProp {
	const char* name;

	IsExtensionProp(const char* name) noexcept : name(name) { }

	bool operator()(const VkExtensionProperties& prop) const {
		return !std::strcmp(prop.extensionName, name);
	}
};

bool VulkanQuakeApp::CheckExtensionsAvailable(const std::vector<const char*>& extensionNames) const {
	bool foundAll = true;
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	
	for (const char* name : extensionNames) {
		if (!std::any_of(std::begin(extensions), std::end(extensions), IsExtensionProp(name))) {
			std::cerr << "Extension: \"" << name << "\" not found.\n";
			foundAll = false;
		}
	}
	return foundAll;
}

struct IsLayerProp {
	const char* name;

	IsLayerProp(const char* name) : name(name) { }

	bool operator()(const VkLayerProperties& prop) const {
		return !std::strcmp(name, prop.layerName);
	}
};

bool VulkanQuakeApp::CheckValidaitonLayerSupport() const {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* name : validationLayers) {
		if (!std::any_of(std::begin(availableLayers), std::end(availableLayers), IsLayerProp(name))) {
			std::cerr << "Layer: \"" << name << "\" not found\n";
			return false;
		}
	}

	return true;
}

std::vector<const char*> VulkanQuakeApp::GetRequiredExtensions() const {
	uint32_t extCount = 0;
	SDL_Vulkan_GetInstanceExtensions(window, &extCount, nullptr);
	std::vector<const char*> extensionNames(extCount);
	SDL_Vulkan_GetInstanceExtensions(window, &extCount, extensionNames.data());

	if (enableValidationLayers) {
		extensionNames.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	if (!CheckExtensionsAvailable(extensionNames)) {
		throw std::runtime_error("Not all required extensions found!");
	}

	return extensionNames;
}

// --------------------
// Validation Layer
// --------------------
void VulkanQuakeApp::SetUpDebugMessenger() {
	if (!enableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	PopulateDebugMessengerCreateInfo(createInfo);

	if (utils::FunctionFailed(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger))) {
		throw std::runtime_error("Failed to set up debug messenger!");
	}
}

void VulkanQuakeApp::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

VkResult VulkanQuakeApp::CreateDebugUtilsMessengerEXT(VkInstance instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanQuakeApp::DestroyDebugUtilsMessengerEXT(VkInstance instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}