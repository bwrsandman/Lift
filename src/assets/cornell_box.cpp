#include "cornell_box.h"
using namespace glm;

namespace assets {

void addTriangle(std::vector<uint32_t>& indices,
                 const uint32_t offset,
                 const uint32_t i0,
                 const uint32_t i1,
                 const uint32_t i2) {
    indices.push_back(offset + i0);
    indices.push_back(offset + i1);
    indices.push_back(offset + i2);
}

void CornellBox::create(const float scale,
                        std::vector<Vertex>& vertices,
                        std::vector<uint32_t>& indices,
                        std::vector<Material>& materials) {
    materials.push_back(Material::lambertian(vec3(0.05f, 0.80f, 0.05f)));  // green
    materials.push_back(Material::lambertian(vec3(0.80f, 0.05f, 0.05f)));  // red
    materials.push_back(Material::lambertian(vec3(0.80f, 0.80f, 0.80f)));  // white
    materials.push_back(Material::emissive(vec3(1.0f, 1.0f, 1.0f), vec3(15.0f), -1));                  // light

    const float s = scale;

    const vec3 l0(0, 0, 0);
    const vec3 l1(0, 0, -s);
    const vec3 l2(0, s, -s);
    const vec3 l3(0, s, 0);

    const vec3 r0(s, 0, 0);
    const vec3 r1(s, 0, -s);
    const vec3 r2(s, s, -s);
    const vec3 r3(s, s, 0);

    // Left green panel
    auto i = static_cast<uint32_t>(vertices.size());
    vertices.push_back(Vertex {l0, vec3(1, 0, 0), vec2(0, 1), 1});
    vertices.push_back(Vertex {l1, vec3(1, 0, 0), vec2(1, 1), 1});
    vertices.push_back(Vertex {l2, vec3(1, 0, 0), vec2(1, 0), 1});
    vertices.push_back(Vertex {l3, vec3(1, 0, 0), vec2(0, 0), 1});

    addTriangle(indices, i, 0, 1, 2);
    addTriangle(indices, i, 0, 2, 3);

    // Right red panel
    i = static_cast<uint32_t>(vertices.size());
    vertices.push_back(Vertex {r0, vec3(-1, 0, 0), vec2(0, 1), 0});
    vertices.push_back(Vertex {r1, vec3(-1, 0, 0), vec2(1, 1), 0});
    vertices.push_back(Vertex {r2, vec3(-1, 0, 0), vec2(1, 0), 0});
    vertices.push_back(Vertex {r3, vec3(-1, 0, 0), vec2(0, 0), 0});

    addTriangle(indices, i, 2, 1, 0);
    addTriangle(indices, i, 3, 2, 0);

    // Back white panel
    i = static_cast<uint32_t>(vertices.size());
    vertices.push_back(Vertex {l1, vec3(0, 0, 1), vec2(0, 1), 2});
    vertices.push_back(Vertex {r1, vec3(0, 0, 1), vec2(1, 1), 2});
    vertices.push_back(Vertex {r2, vec3(0, 0, 1), vec2(1, 0), 2});
    vertices.push_back(Vertex {l2, vec3(0, 0, 1), vec2(0, 0), 2});

    addTriangle(indices, i, 0, 1, 2);
    addTriangle(indices, i, 0, 2, 3);

    // Bottom white panel
    i = static_cast<uint32_t>(vertices.size());
    vertices.push_back(Vertex {l0, vec3(0, 1, 0), vec2(0, 1), 2});
    vertices.push_back(Vertex {r0, vec3(0, 1, 0), vec2(1, 1), 2});
    vertices.push_back(Vertex {r1, vec3(0, 1, 0), vec2(1, 0), 2});
    vertices.push_back(Vertex {l1, vec3(0, 1, 0), vec2(0, 0), 2});

    addTriangle(indices, i, 0, 1, 2);
    addTriangle(indices, i, 0, 2, 3);

    // Top white panel
    i = static_cast<uint32_t>(vertices.size());
    vertices.push_back(Vertex {l2, vec3(0, -1, 0), vec2(0, 1), 2});
    vertices.push_back(Vertex {r2, vec3(0, -1, 0), vec2(1, 1), 2});
    vertices.push_back(Vertex {r3, vec3(0, -1, 0), vec2(1, 0), 2});
    vertices.push_back(Vertex {l3, vec3(0, -1, 0), vec2(0, 0), 2});

    addTriangle(indices, i, 0, 1, 2);
    addTriangle(indices, i, 0, 2, 3);

    // Light
    i = static_cast<uint32_t>(vertices.size());

    const float x0 = s * (213.0f / 555.0f);
    const float x1 = s * (343.0f / 555.0f);
    const float z0 = s * (-555.0f + 332.0f) / 555.0f;
    const float z1 = s * (-555.0f + 227.0f) / 555.0f;
    const float y1 = s * 0.998f;

    vertices.push_back(Vertex {vec3(x0, y1, z1), vec3(0, -1, 0), vec2(0, 1), 3});
    vertices.push_back(Vertex {vec3(x1, y1, z1), vec3(0, -1, 0), vec2(1, 1), 3});
    vertices.push_back(Vertex {vec3(x1, y1, z0), vec3(0, -1, 0), vec2(1, 0), 3});
    vertices.push_back(Vertex {vec3(x0, y1, z0), vec3(0, -1, 0), vec2(0, 0), 3});

    addTriangle(indices, i, 0, 1, 2);
    addTriangle(indices, i, 0, 2, 3);
}

}  // namespace assets
