#pragma once

#include <webgpu/webgpu.h>
#include <vector>

namespace Utilities {
    struct QuadStruct {
        std::vector<float> Vertices;
        std::vector<uint16_t> Indices;
        unsigned int VertexCount;
        unsigned int IndexCount;
    };

    class Quad {
    public:
        QuadStruct CreateNDC() {
            // Full-screen quad vertices in NDC space with UVs
            std::vector<float> vertexData = {
                // Position       // UV
                -1.0f, -1.0f,     0.0f, 0.0f,  // Bottom-left
                 1.0f, -1.0f,     1.0f, 0.0f,  // Bottom-right
                -1.0f,  1.0f,     0.0f, 1.0f,  // Top-left
                 1.0f,  1.0f,     1.0f, 1.0f   // Top-right
            };

            std::vector<uint16_t> indexData = {
                0, 1, 2,  // First triangle
                2, 1, 3   // Second triangle
            };

            return QuadStruct{
                vertexData,
                indexData,
                static_cast<uint32_t>(vertexData.size() / 4), // 4 floats per vertex (position + UV)
                static_cast<uint32_t>(indexData.size())
            };
        }

        QuadStruct CreateCentered() {
            // Centered quad vertices with UVs
            std::vector<float> vertexData = {
                // Position       // UV
                -0.5f, -0.5f,     0.0f, 0.0f,  // Bottom-left
                 0.5f, -0.5f,     1.0f, 0.0f,  // Bottom-right
                -0.5f,  0.5f,     0.0f, 1.0f,  // Top-left
                 0.5f,  0.5f,     1.0f, 1.0f   // Top-right
            };

            std::vector<uint16_t> indexData = {
                0, 1, 2,  // First triangle (bottom-left, bottom-right, top-left)
                2, 1, 3   // Second triangle (top-left, bottom-right, top-right)
            };

            return QuadStruct{
                vertexData,
                indexData,
                static_cast<uint32_t>(vertexData.size() / 4), // 4 floats per vertex (position + UV)
                static_cast<uint32_t>(indexData.size())
            };
        }
    };
}
