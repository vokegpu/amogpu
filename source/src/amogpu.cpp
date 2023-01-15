#include "amogpu/amogpu.hpp"
#include <iostream>

void foo2() {
    std::cout << "hello :3" << std::endl;
}

void amogpu::foo() {
    return foo2();
}

void amogpu::init() {

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

uint32_t amogpu::shader::get_id() {
    return this->id;
}

void amogpu::shader::free_buffers() {
    if (this->id != 0) glDeleteProgram(this->id);
}
