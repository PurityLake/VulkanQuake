/*
 * Vulkan Program
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

#include <vulkan/vulkan.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>

class VulkanQuakeApp {
// ------------------------
// Public members
// ------------------------
public:
	const uint32_t WIDTH = 1280;
	const uint32_t HEIGHT = 720;

// ------------------------
// Private members
// ------------------------
private:
	SDL_Window* window;


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
			throw std::exception("Failed to initialise SDL");
		}
		
		window = SDL_CreateWindow("Vulkan Quake",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
		if (window == nullptr) {
			std::cerr << "SDL failed to create window: " << SDL_GetError() << std::endl;
			throw std::exception("Failed to create SDL window");
		}

		SDL_SetWindowResizable(window, SDL_FALSE);
	}

	// ######################
	// InitVulakn
	void InitVulkan() {
	
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
		if (window != nullptr) {
			SDL_DestroyWindow(window);
		}
		SDL_Quit();
	}
};