/*
 * MIT License
 * Copyright (c) 2022 Robert O'Shea
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#include "Shader.h"
#include "Utils.h"

struct QueueFamilyIndicies;
struct SwapChainSupportDetails;

class VulkanQuakeApp {
// ------------------------
// Public members
// ------------------------
public:
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;
	
#ifdef NDEBUG
	const bool EnableValidationLayers = false;
#else
	const bool EnableValidationLayers = true;
#endif

// ------------------------
// Private members
// ------------------------
private:
	SDL_Window* Window;
	VkInstance Instance;
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkPhysicalDeviceFeatures DeviceFeatures{ };
	VkDevice Device;
	VkQueue GraphicsQueue, PresentQueue;
	VkSurfaceKHR Surface;
	VkSwapchainKHR Swapchain;
	std::vector<VkImage> SwapchainImages;
	VkFormat SwapchainImageFormat;
	VkExtent2D SwapchainExtent;
	std::vector<VkImageView> SwapchainImageViews;
	Shader CurrentShader;
	VkRenderPass RenderPass;
	VkPipelineLayout PipelineLayout;
	VkPipeline GraphicsPipeline;

	// --------------------
	// DATA
	// --------------------
	const std::vector<const char*> ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	const std::vector<const char*> DeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::string APP_NAME = "Vulkan Quake";
	const std::string ENGINE_NAME = "Vulkan Quake";

	// --------------------
	// DEBUG
	// --------------------
	VkDebugUtilsMessengerEXT DebugMessenger;

// ------------------------
// Public methods
// ------------------------
public:
	void Run();

// ------------------------
// Private methods
// ------------------------
private:
	// Window
	void InitWindow();
	// Vulkan
	void InitVulkan();
	void CreateInstance();
	void PickPhysicalDevice();
	void CreateLogicialDevice();
	void CreateSurface();
	void CreateSwapchain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	// Game Loop
	void MainLoop();
	// Cleanup
	void Cleanup();

	// ---------------
	// Util methods
	// ---------------
	bool CheckExtensionsAvailable(const std::vector<const char*>& extensionNames) const;
	bool CheckValidaitonLayerSupport() const;

	bool IsDeviceSuitable(const VkPhysicalDevice& device) const;
	QueueFamilyIndicies FindQueueFamilies(const VkPhysicalDevice& device) const;
	bool CheckDeviceExtensionSupport(const VkPhysicalDevice& device) const;

	SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& device) const;

	std::vector<const char*> GetRequiredExtensions() const;

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	VkExtent2D ChooesSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;

	// --------------------
	// Validation Layer
	// --------------------
	void SetUpDebugMessenger();

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instnance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance Instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator);


	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

		return VK_FALSE;
	}
};