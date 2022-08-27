#pragma once

#include <vulkan/vulkan.h>

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Utils.h"

class Shader {
// ------------------------
// Public members
// ------------------------
public:
	// Empty

// ------------------------
// Private members
// ------------------------
private:
	VkShaderModule Vert = VK_NULL_HANDLE;
	VkShaderModule Frag = VK_NULL_HANDLE;

	std::string vertFilename;
	std::string fragFilename;

// ------------------------
// Public methods
// ------------------------
public:
	Shader();
	Shader(const std::string& vertFilename,
		   const std::string& fragFilename);

	VkShaderModule GetVert() const;
	VkShaderModule GetFrag() const;

	void SetVertShaderFilename(const std::string& filename);
	void SetFragShaderFilename(const std::string& filename);

	void CompileShader(const VkDevice& device);

	void DestroyShader(const VkDevice& device);

// ------------------------
// Private methods
// ------------------------
private:
	VkShaderModule CompileShaderModule(const VkDevice& device, const std::vector<char> code);
};