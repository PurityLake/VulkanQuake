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

// ------------------------
// Private members
// ------------------------
private:
	SDL_Window* window;
	VkInstance instance;

// ------------------------
// Public methods
// ------------------------
public:
	void Run() {
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}

// ------------------------
// Private methods
// ------------------------
private:
	void InitWindow() {
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
	void InitVulkan() {
		CreateInstance();
	}

	void CreateInstance() {
		uint32_t extCount = 0;
		SDL_Vulkan_GetInstanceExtensions(window, &extCount, nullptr);
		std::vector<const char*> extensionNames(extCount);
		SDL_Vulkan_GetInstanceExtensions(window, &extCount, extensionNames.data());

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
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = (uint32_t)extensionNames.size();
		createInfo.ppEnabledExtensionNames = extensionNames.data();
		createInfo.enabledLayerCount = (uint32_t)layerNames.size();
		createInfo.ppEnabledLayerNames = layerNames.data();

		if (utils::FunctionFailed(vkCreateInstance(&createInfo, nullptr, &instance))) {
			throw std::runtime_error("Failed to create instance!");
		}
	}
	
	void MainLoop() {
		while (true) {
			SDL_Event event;
			if (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					break;
				}
			}
		}
	}
	
	void Cleanup() {
		vkDestroyInstance(instance, nullptr);
		if (window != nullptr) {
			SDL_DestroyWindow(window);
		}
		SDL_Quit();
	}
};