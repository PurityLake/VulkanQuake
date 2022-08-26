/*
 * Vulkan Quake
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "VulkanQuakeApp.h"

#include <optional>

// structs
struct QueueFamilyIndicies {
	std::optional<uint32_t> GraphicsFamily;

	inline bool isComplete() const {
		return GraphicsFamily.has_value();
	}
};

// Forward decl static functions
static bool isDeviceSuitable(const VkPhysicalDevice& device);
static QueueFamilyIndicies findQueueFamilies(const VkPhysicalDevice& device);

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

	Window = SDL_CreateWindow("Vulkan Quake",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
	if (Window == nullptr) {
		std::cerr << "SDL failed to create window: " << SDL_GetError() << std::endl;
		throw std::runtime_error("Failed to create SDL window");
	}

	SDL_SetWindowResizable(Window, SDL_FALSE);
}

void VulkanQuakeApp::InitVulkan() {
	CreateInstance();
	SetUpDebugMessenger();
	PickPhysicalDevice();
	CreateLogicialDevice();
}

void VulkanQuakeApp::CreateInstance() {
	if (EnableValidationLayers && !CheckValidaitonLayerSupport()) {
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
	if (EnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	if (utils::FunctionFailed(vkCreateInstance(&createInfo, nullptr, &Instance))) {
		throw std::runtime_error("Failed to create instance!");
	}
}

void VulkanQuakeApp::PickPhysicalDevice() {
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(Instance, &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPU(s) with Vulkan support!");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(Instance, &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			PhysicalDevice = device;
			break;
		}
	}

	if (PhysicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

void VulkanQuakeApp::CreateLogicialDevice() {
	QueueFamilyIndicies indicies = findQueueFamilies(PhysicalDevice);

	float queuePriority = 1.0f;

	VkDeviceQueueCreateInfo queueCreateInfo{ };
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indicies.GraphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo createInfo{ };
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &DeviceFeatures;
	createInfo.enabledExtensionCount = 0;
	if (EnableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledExtensionNames = validationLayers.data();
	} else {
		createInfo.enabledLayerCount = 0;
	}

	if (utils::FunctionFailed(vkCreateDevice(PhysicalDevice, &createInfo, nullptr, &Device))) {
		throw std::runtime_error("Failed to create Logical Device!");
	}
	vkGetDeviceQueue(Device, indicies.GraphicsFamily.value(), 0, &GraphicsQueue);
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
	vkDestroyDevice(Device, nullptr);
	if (EnableValidationLayers) {
		DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
	}
	vkDestroyInstance(Instance, nullptr);
	if (Window != nullptr) {
		SDL_DestroyWindow(Window);
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
	SDL_Vulkan_GetInstanceExtensions(Window, &extCount, nullptr);
	std::vector<const char*> extensionNames(extCount);
	SDL_Vulkan_GetInstanceExtensions(Window, &extCount, extensionNames.data());

	if (EnableValidationLayers) {
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
	if (!EnableValidationLayers) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	PopulateDebugMessengerCreateInfo(createInfo);

	if (utils::FunctionFailed(CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugMessenger))) {
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

VkResult VulkanQuakeApp::CreateDebugUtilsMessengerEXT(VkInstance Instance,
	const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	const VkAllocationCallbacks* pAllocator,
	VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		Instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(Instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanQuakeApp::DestroyDebugUtilsMessengerEXT(VkInstance Instance,
	VkDebugUtilsMessengerEXT debugMessenger,
	const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
		Instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(Instance, debugMessenger, pAllocator);
	}
}

// ---------------------------
// Static Functions
// ---------------------------
static bool isDeviceSuitable(const VkPhysicalDevice& device) {
	QueueFamilyIndicies indicies = findQueueFamilies(device);

	return indicies.isComplete();
}

static QueueFamilyIndicies findQueueFamilies(const VkPhysicalDevice& device) {
	QueueFamilyIndicies indicies;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indicies.GraphicsFamily = i;
		}

		if (indicies.isComplete()) {
			break;
		}

		++i;
	}

	return indicies;
}