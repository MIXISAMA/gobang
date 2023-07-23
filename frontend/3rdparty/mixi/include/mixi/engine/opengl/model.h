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


class Mesh
{

public:

    VertexArray::Ptr vertex_array_;
    BaseMaterial::Ptr material_;
    Program::Ptr program_;
    // int a = GL_MAX_UNIFORM_BLOCK_BINDINGS;
};

class Model
{

public:

    // Model();

    struct Node
    {
        using Ptr = std::shared_ptr<Node>;
        std::vector<VertexArray::Ptr> vertex_arrays_;
        std::vector<Node::Ptr> children_;
    };

protected:

    std::vector<VertexArray::Ptr> vertex_arrays_;
    std::vector<AssimpMaterial> materials_;
    std::vector<UniformBuffer::Ptr> uniform_buffers_;
    Node root_node_;

};

class ModelCreator
{

public:

    ModelCreator(TexturesManager* textures_manager = nullptr);
    virtual ~ModelCreator();

    Model* new_model(const char* filepath);

    void process_flag(unsigned int p_flag);

protected:

    TexturesManager* textures_manager_;
    bool is_internal_textures_manager_;

    Assimp::Importer importer_;
    unsigned int process_flag_;
    
    const aiScene* scene_;
    Model* model_;

    void process_material_(const aiMaterial* material);
    void process_node_(const aiNode* node);
    void process_mesh_(const aiMesh* mesh);

private:

    void recursively_visit_nodes_(const aiNode* node);

};

} // namespace eng
} // namespace gl
} // namespace mixi
