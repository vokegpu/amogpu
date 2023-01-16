#include "amogpu/amogpu.hpp"
#include <iostream>

bool amogpu::log(const std::string &msg) {
    std::cout << msg << '\n';
    return true;
}

void amogpu::init() {
    amogpu::log("AmoGPU initialising...");
}

void amogpu::quit() {

}

amogpu::shader::~shader() {
    this->free_buffers();
}

void amogpu::shader::invoke() {
}

void amogpu::shader::revoke() {

}

void amogpu::shader::free_buffers() {
    if (this->id != 0) glDeleteProgram(this->id);
}

bool amogpu::create(shader *p_shader, const std::vector<amogpu::shading_resource> &shading_resources) {
    if (p_shader == nullptr) {
        return ekg::log("AmoGPU.InvalidShader: nullptr!");
    }

    p_shader.id = glCreateProgram();
    std::string shader_source {};

    for (const amogpu::shading_resource &resource : resources) {
        shader_source.clear();

        if (resource.source) {
            shader_source = resource.path;
        } else if () {}
    }
}