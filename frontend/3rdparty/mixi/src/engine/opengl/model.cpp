#include "engine/opengl/model.h"


namespace mixi
{
namespace gl
{
namespace eng
{

ModelCreator::ModelCreator(TexturesManager* textures_manager) :
    process_flag_(aiProcess_Triangulate | aiProcess_FlipUVs)
{
    if (textures_manager == nullptr) {
        is_internal_textures_manager_ = false;
        textures_manager_ = textures_manager;
    } else {
        is_internal_textures_manager_ = true;
        textures_manager_ = new TexturesManager;
    }
};

ModelCreator::~ModelCreator()
{
    if (is_internal_textures_manager_) {
        delete textures_manager_;
    }
}

void ModelCreator::process_flag(unsigned int p_flag)
{
    process_flag_ = p_flag;
}

Model* ModelCreator::new_model(const char* filepath)
{
    model_ = new Model;
    scene_ = importer_.ReadFile(filepath, process_flag_);

    for (int i = 0; i < scene_->mNumMaterials; i++) {
        process_material_(scene_->mMaterials[i]);
    }
    recursively_visit_nodes_(scene_->mRootNode);

    return model_;
}

void ModelCreator::process_material_(const aiMaterial* material)
{

}

void ModelCreator::process_node_(const aiNode* node)
{
    std::cout << "node: " << node->mName.C_Str() << std::endl;
}

void ModelCreator::process_mesh_(const aiMesh* mesh)
{
    std::cout << "mesh: " << mesh->mName.C_Str() << std::endl;

    bool has_normals = mesh->HasNormals();
    int num_uv_channels = mesh->GetNumUVChannels();

    int stride = (3 + (has_normals ? 3 : 0) + num_uv_channels * 2) * sizeof(float);

    std::byte data[mesh->mNumVertices * stride];
    for (int i = 0; i < mesh->mNumVertices; i++)

    if (mesh->HasNormals()) {
        mesh->mNormals; // ...todo
    }
    
    
    for (int j = 0; j < num_uv_channels; j++) {
        const aiVector3D* coords = mesh->mTextureCoords[j];
        // const aiString* name = mesh->mTextureCoordsNames[j];

        
    }


    const aiMaterial* material = scene_->mMaterials[mesh->mMaterialIndex];
    int num_textures_diffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
    for (int i = 0; i < num_textures_diffuse; i++) {
        aiString path;
        material->GetTexture(aiTextureType_DIFFUSE, i, &path);
        // ...todo
    }

    // mesh->mVertices;
    // mesh->mNumVertices;
    
    // mesh->mNormals;

    // mesh->mNumUVComponents;

    // const aiMaterial* material = scene_->mMaterials[mesh->mMaterialIndex];
    // material->GetTextureCount();

    

    // mesh->mNumFaces;
    



    // gl::VertexBuffer::Ptr vertex_buffer = std::make_shared<gl::VertexBuffer>(
    //     mesh->mNumVertices * sizeof(aiVector3D),
    //     Buffer::Usage::STATIC_DRAW,
    //     mesh->mNumVertices,
    //     std::vector<gl::VertexBuffer::Descriptor>{{
    //         .size = 3,
    //         .type = GL_FLOAT,
    //         .normalized = GL_FALSE,
    //         .stride     = sizeof(glm::vec3),
    //         .pointer    = (void*)0
    //     }}
    // );

}

void ModelCreator::recursively_visit_nodes_(const aiNode* node)
{
    for (int i = 0; i < node->mNumMeshes; i++) {
        process_mesh_(scene_->mMeshes[node->mMeshes[i]]);
    }

    for (int i = 0; i < node->mNumChildren; i++) {
        recursively_visit_nodes_(node->mChildren[i]);
    }
}

} // namespace eng
} // namespace gl
} // namespace mixi
