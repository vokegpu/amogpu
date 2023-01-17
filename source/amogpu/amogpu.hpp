#ifndef AMOGPU_H
#define AMOGPU_H

#include <string>
#include <vector>
#include "GL/glew.h"

namespace amogpu {
    typedef struct vec4 {
        union {
            struct {
                float x;
                float y;
                float z;
                float w;
            };

            float data[3];
        };

        float &operator[](uint8_t it) {
            return data[it];
        }
    } vec4;

    enum shading {
        vertex         = GL_VERTEX_SHADER,
        fragment       = GL_FRAGMENT_SHADER,
        geometry       = GL_GEOMETRY_SHADER,
        tesscontrol    = GL_TESS_CONTROL_SHADER,
        tessevaluation = GL_TESS_EVALUATION_SHADER,
        compute        = GL_COMPUTE_SHADER
    };

    struct shading_resource {
        std::string path {};
        amogpu::shading stage {};
        bool contains_source {false};
    };

    struct shader {
        uint32_t id {};

        explicit shader() = default;
        ~shader();

        void invoke();
        void revoke();
        void free_buffers();

        inline void set_uniform_float(const std::string &name, const float v) const {
            glUniform1f(glGetUniformLocation(this->id, name.data()), v);
        }

        inline void set_uniform_int(const std::string &name, const int v) const {
            glUniform1i(glGetUniformLocation(this->id, name.data()), v);
        }

        inline void set_uniform_bool(const std::string &name, const bool v) const {
            glUniform1i(glGetUniformLocation(this->id, name.data()), v);
        }

        inline void set_uniform_vec2(const std::string &name, const int *p_v) const {
            glUniform2iv(glGetUniformLocation(this->id, name.data()), GL_TRUE, p_v);
        }

        inline void set_uniform_vec3(const std::string &name, const int *p_v) const {
            glUniform3iv(glGetUniformLocation(this->id, name.data()), GL_TRUE, p_v);
        }

        inline void set_uniform_vec4(const std::string &name, const int *p_v) const {
            glUniform4iv(glGetUniformLocation(this->id, name.data()), GL_TRUE, p_v);
        }

        inline void set_uniform_vec2(const std::string &name, const float *p_v) const {
            glUniform2fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, p_v);
        }

        inline void set_uniform_vec3(const std::string &name, const float *p_v) const {
            glUniform3fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, p_v);
        }

        inline void set_uniform_vec4(const std::string &name, const float *p_v) const {
            glUniform4fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, p_v);
        }

        inline void set_uniform_mat2(const std::string &name, const float *p_v) const {
            glUniformMatrix2fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, GL_FALSE, p_v);
        }

        inline void set_uniform_mat3(const std::string &name, const float *p_v) const {
            glUniformMatrix3fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, GL_FALSE, p_v);
        }

        inline void set_uniform_mat4(const std::string &name, const float *p_v) const {
            glUniformMatrix4fv(glGetUniformLocation(this->id, name.data()), GL_TRUE, GL_FALSE, p_v);
        }
    };

    struct batch_data {
        amogpu::vec4 color {};
        amogpu::vec4 rectangle {};

        uint32_t active_texture {};
        uint32_t buffer_texture {};
    };

    class batch {
    public:
        uint32_t buffer_vao {};
        uint32_t buffer_vertices {};
        uint32_t buffer_textcoords {};

        std::vector<amogpu::batch_data> batching_data {};
        std::vector<float> mesh_vertex_data {};
    public:
        static amogpu::shader *p_shader_batch_default;

        void invoke();
        void revoke();
        void draw();
    };

    class font_rendering {
    public:
        std::string path {};
        uint32_t buffer_texture {};
        bool font_done {};

        uint8_t font_size {};
        float atlas_width {};
        float atlas_height {};

        amogpu::batch *p_linked_batch {};
        amogpu::batch *&get_linked_batch();
        void invoke_revoke_texture();
    public:
        void link(amogpu::batch *p_batch);
        bool load(std::string_view path, uint8_t size);

        float get_text_width(const std::string &text);
        float get_text_height();

        void blit(const std::string &text, float x, float y, const amogpu::vec4 &color);
    };

    bool create(shader *p_shader, const std::vector<amogpu::shading_resource> &shading_resources);
    bool read_file(std::string_view path, std::string &string_builder);
    bool log(const std::string &msg);

    /* GL version specified in GLSL source code. */
    extern const char *glversion;
    extern float mat4x4_orthographic[16];

    void init();
    void swap_buffers();
    void quit();
}

#endif