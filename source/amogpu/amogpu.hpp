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

    bool create(shader *&p_shader, const std::vector<amogpu::shading_resource> &shading_resources);

    class batching {
    public:
        void invoke();
        void revoke();
    };

    void init();
    void quit();
}

#endif