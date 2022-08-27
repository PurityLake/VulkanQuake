#include "Shader.h"

// ------------------------
// Public methods
// ------------------------
Shader::Shader() : vertFilename(""), fragFilename("") { }
Shader::Shader(const std::string& vertFilename,
	const std::string& fragFilename)
	: vertFilename(vertFilename),
	  fragFilename(fragFilename) { }

VkShaderModule Shader::GetVert() const {
	return Vert;
}
VkShaderModule Shader::GetFrag() const {
	return Frag;
}

void Shader::SetVertShaderFilename(const std::string& filename) {
	vertFilename = filename;
}
void Shader::SetFragShaderFilename(const std::string& filename) {
	fragFilename = filename;
}

void Shader::CompileShader(const VkDevice& device) {
	Vert = CompileShaderModule(device, utils::readFile(vertFilename));
	Frag = CompileShaderModule(device, utils::readFile(fragFilename));
}

void Shader::DestroyShader(const VkDevice& device) {
	vkDestroyShaderModule(device, Frag, nullptr);
	vkDestroyShaderModule(device, Vert, nullptr);
}

// ------------------------
// Private methods
// ------------------------
VkShaderModule Shader::CompileShaderModule(const VkDevice& device, const std::vector<char> code) {
	VkShaderModuleCreateInfo createInfo{ };
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (utils::FunctionFailed(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule))) {
		throw std::runtime_error("Failed to create shader module!");
	}

	return shaderModule;
}