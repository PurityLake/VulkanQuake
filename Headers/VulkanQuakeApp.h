/*
 * Vulkan Quake
 *
 * Copyright (C) 2016 Valve Corporation
 * Copyright (C) 2016 LunarG, Inc.
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

#pragma once

#include <vulkan/vulkan.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Utils.h"

class VulkanQuakeApp {
// ------------------------
// Public members
// ------------------------
public:
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;
	const std::string APP_NAME = "Vulkan Quake";
	const std::string ENGINE_NAME = "Vulkan Quake";
	
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

#ifdef DEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

// ------------------------
// Private members
// ------------------------
private:
	SDL_Window* window;
	VkInstance instance;

	VkDebugUtilsMessengerEXT debugMessenger;

// ------------------------
// Public methods
// ------------------------
public:
	void Run();

// ------------------------
// Private methods
// ------------------------
private:
	void InitWindow();
	void InitVulkan();
	void CreateInstance();
	void MainLoop();
	void Cleanup();
// ---------------
// Util methods
// ---------------
	bool CheckExtensionsAvailable(const std::vector<const char*>& extensionNames) const;
	bool CheckValidaitonLayerSupport() const;

	std::vector<const char*> GetRequiredExtensions() const;

// --------------------
// Validation Layer
// --------------------
	void SetUpDebugMessenger();

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instnance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance,
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