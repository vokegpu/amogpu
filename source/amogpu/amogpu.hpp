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

    enum class shading {
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
        bool contains_source {};
    };

    struct shader {
    protected:
        uint32_t id {};

        explicit shader() = default;
        ~shader();

        void invoke();
        void revoke();
        void free_buffers();
    };

    struct batch_data {
        amogpu::vec4 color {};
        amogpu::vec4 rectangle {};

        uint32_t active_texture {};
        uint32_t buffer_texture {};
    };

    class batching {
    public:
        uint32_t buffer_vao {};
        uint32_t buffer_vertices {};
        uint32_t buffer_textcoords {};
    public:
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

        amogpu::batching *p_linked_batch {};
        amogpu::batching &*get_linked_batch();
        void invoke_revoke_texture();
    public:
        void link(amogpu::batching *p_batch);
        bool load(std::string_view path, uint8_t size);

        float get_text_width(const std::string &text);
        float get_text_height();

        void blit(const std::string &text, float x, float y, const amogpu::vec4 &color);
    };

    bool create(shader *p_shader, const std::vector<amogpu::shading_resource> &shading_resources);

    bool log(const std::string &msg);
    void init();
    void quit();
}

#endif