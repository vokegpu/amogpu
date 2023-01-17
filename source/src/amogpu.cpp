#include "amogpu/amogpu.hpp"
#include <iostream>
#include <fstream>

/* Start of AmoGPU batch segment. */

amogpu::shader *amogpu::batch::p_shader_batch_default {nullptr};

void amogpu::batch::invoke() {
    if (this->buffer_vao == 0) glGenVertexArrays(1, &this->buffer_vao);
    glBindVertexArray(this->buffer_vao);
}

void amogpu::batch::revoke() {
    glBindVertexArray(0);
}

void amogpu::batch::draw() {
    for (amogpu::batch_data &p_batch_data : this->batching_data) {

    }
}

/* End of AmoGPU batch segment. */
/* Start of AmoGPU shader segment. */

amogpu::shader::~shader() {
    this->free_buffers();
}

void amogpu::shader::invoke() {
    glUseProgram(this->id);
}

void amogpu::shader::revoke() {
    glUseProgram(0);
}

void amogpu::shader::free_buffers() {
    if (this->id != 0) glDeleteProgram(this->id);
}

/* End of AmoGPU shader segment. */
/* Stat of AmoGPU utils segment. */

bool amogpu::log(const std::string &msg) {
    std::cout << msg << '\n';
    return true;
}

bool amogpu::create(shader *p_shader, const std::vector<amogpu::shading_resource> &shading_resources) {
    if (p_shader == nullptr || shading_resources.empty()) {
        return amogpu::log("AmoGPU.InvalidShader: Invalid shader resource or shader pointer!");
    }

    p_shader->id = glCreateProgram();
    std::string shader_source {};
    std::vector<uint32_t> compiled_shader_list {};

    for (const amogpu::shading_resource &resources : shading_resources) {
        if (resources.contains_source) {
            shader_source = resources.path;
        } else if (amogpu::read_file(resources.path, shader_source)) {
            shader_source.clear();
        }

        if (shader_source.empty()) {
            break;
        }

        uint32_t shader {glCreateShader(resources.stage)};
        const char* source {shader_source.data()};

        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        int32_t shader_output {};
        glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_output);

        if (shader_output == GL_FALSE) {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &shader_output);
            std::string msg {}; msg.resize(shader_output);
            glGetShaderInfoLog(shader, shader_output, nullptr, msg.data());
            amogpu::log("AmoGPU.InvalidShader: GLSL shader compilation error. \n" + msg);
            break;
        }

        compiled_shader_list.push_back(shader);
    }

    bool attach_shader {shading_resources.size() == compiled_shader_list.size()};
    for (uint32_t &shaders : compiled_shader_list) {
        if (attach_shader) glAttachShader(p_shader->id, shaders);
        glDeleteShader(shaders);
    }

    if (attach_shader) {
        /* Link the shading program if all shaders are compiled and attached. */
        glLinkProgram(p_shader->id);

        int32_t program_output {};
        glGetProgramiv(p_shader->id, GL_LINK_STATUS, &program_output);

        if (program_output == GL_FALSE) {
            glGetProgramiv(p_shader->id, GL_INFO_LOG_LENGTH, &program_output);
            std::string msg {}; msg.resize(program_output);
            glGetProgramInfoLog(p_shader->id, program_output, nullptr, msg.data());
            return amogpu::log("AmoGPU.InvalidShader: GLSL link program error. \n" + msg);
        }

        amogpu::log("AmoGPU.ValidShader: Shader program successfully linked " + std::to_string(compiled_shader_list.size()) + " shaders!");
    }

    return false;
}

bool amogpu::read_file(std::string_view path, std::string &string_builder) {
    std::ifstream ifs {};
    bool error {true};

    ifs.open(path.data());
    if (ifs.is_open()) {
        std::string string_buffer {};
        while (std::getline(ifs, string_buffer)) {
            string_builder = string_buffer;
            string_builder += '\n';
        }

        error = false;
    }

    ifs.close();
    return error;
}

/* End of AmoGPU utils segment. */
/* Start of AmoGPU setup segment. */

const char *amogpu::glversion {"#version 330 core"};
float amogpu::mat4x4_orthographic[16] {};

void amogpu::init() {
    amogpu::log("AmoGPU.Setup: Initialising.");
    amogpu::log("AmoGPU.Setup: Creating environment shaders.");

    std::string vsh_shader_source {amogpu::glversion};
    vsh_shader_source += "\n"
                         "layout (location = 0) in vec2 VertexPosition;\n"
                         "layout (location = 1) in vec2 TexCoordinate;\n"
                         ""
                         "uniform vec4 Rectangle;\n"
                         "uniform mat4 MatrixPerspective;\n"
                         ""
                         "out vec4 Rect;\n"
                         "out vec2 TexCoord;\n"
                         ""
                         "void main() {\n"
                         "  gl_Position = MatrixPerspective * vec4((Rectangle.zw * VertexPosition) + Rectangle.xy, 0.0f, 1.0f);\n"
                         "  Rect = Rectangle;\n"
                         "  TexCoord = TexCoordinate;\n"
                         "}\n";

    std::string fsh_shader_source {amogpu::glversion};
    fsh_shader_source += "\n"
                         "layout (location = 0) out vec4 FragColor;\n"
                         ""
                         "uniform vec4 Color;\n"
                         "uniform sampler2D ActiveTexture;\n"
                         "uniform bool TextureGoingOn;\n"
                         ""
                         "in vec4 Rect;\n"
                         "in vec2 TexCoord;\n"
                         ""
                         "void main() {\n"
                         "  vec4 Sum = Color;\n"
                         "  if (TextureGoingOn) {\n"
                         "    Sum = texture(ActiveTexture, TexCoord);\n"
                         "    Sum = vec4(Sum.rgb, Sum.a * Color.a);\n"
                         "  }\n"
                         "  FragColor = Sum;\n"
                         "}\n";

    amogpu::batch::p_shader_batch_default = new amogpu::shader();
    amogpu::create(amogpu::batch::p_shader_batch_default, {
        {vsh_shader_source, amogpu::shading::vertex, true},
        {fsh_shader_source, amogpu::shading::fragment, true}
    });
}

void amogpu::swap_buffers() {
    float viewport[4] {};
    glGetFloatv(GL_VIEWPORT, viewport);

    /* Calculate orthographic matrix. */

    const float left {0.0f};
    const float right {viewport[0]};
    const float bottom {viewport[1]};
    const float top {0.0f};

    const float depth_near {-1.0f};
    const float depth_far {1.0f};
    const float depth_inv {1.0f / (depth_far - depth_near)};
    const float y_inv {1.0f / (top - bottom)};
    const float x_inv {1.0f / (right - left)};

    amogpu::mat4x4_orthographic[0] = 2.0f * x_inv;
    amogpu::mat4x4_orthographic[1] = 0.0f;
    amogpu::mat4x4_orthographic[2] = 0.0f;
    amogpu::mat4x4_orthographic[3] = 0.0f;

    amogpu::mat4x4_orthographic[4] = 0.0f;
    amogpu::mat4x4_orthographic[5] = 2.0f * y_inv;
    amogpu::mat4x4_orthographic[6] = 0.0f;
    amogpu::mat4x4_orthographic[7] = 0.0f;

    amogpu::mat4x4_orthographic[8] = 0.0f;
    amogpu::mat4x4_orthographic[9] = 0.0f;
    amogpu::mat4x4_orthographic[10] = -2.0f * depth_inv;
    amogpu::mat4x4_orthographic[11] = 0.0f;

    amogpu::mat4x4_orthographic[12] = -(right + left) * x_inv;
    amogpu::mat4x4_orthographic[13] = -(top + bottom) * y_inv;
    amogpu::mat4x4_orthographic[14] = -(depth_far + depth_near) * depth_inv;
    amogpu::mat4x4_orthographic[15] = 1.0f;
}

void amogpu::quit() {

}

/* End of AmoGPU setup segment. */