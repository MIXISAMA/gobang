#pragma once
#include "mixi/pch.h"
#include "mixi/render/opengl/vertex_array.h"
#include "mixi/render/opengl/texture.h"
#include "mixi/render/opengl/program.h"
#include "mixi/engine/opengl/textures_manager.h"
#include "mixi/engine/opengl/material.h"

namespace mixi
{
namespace gl
{
namespace eng
{


struct Material
{
    std::string name;
    std::shared_ptr<Texture2D> texture_diffuse;
    std::shared_ptr<Texture2D> texture_specular;
    std::shared_ptr<Texture2D> texture_normals;
    std::shared_ptr<Texture2D> texture_height;
    float shininess;
    Material(const aiMaterial* material);
};

struct Mesh
{
    std::string name;
    std::shared_ptr<Material> material;
    std::shared_ptr<VertexBuffer> vertex_buffer;
    std::function<void(const Mesh&)> on_draw;
    Mesh(
        const aiMesh* mesh,
        const std::vector<std::shared_ptr<Material>>& all_materials
    );
    void draw() const;
};

struct Node
{
    std::string name;
    std::vector<Node> children;
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::function<void(const Node&)> on_draw;
    void draw() const;
};

class Model
{

public:

    Node root_node;
    std::function<void(const Model&)> on_draw;
    Model(
        const char* filepath,
        unsigned int p_flags = aiProcess_Triangulate | aiProcess_FlipUVs
    );
    void draw() const;

protected:

    void recursively_visit_nodes_(
        const aiNode* ai_node,
        Node& my_node,
        const std::vector<std::shared_ptr<Mesh>>& all_meshes
    );

};

} // namespace eng
} // namespace gl
} // namespace mixi
