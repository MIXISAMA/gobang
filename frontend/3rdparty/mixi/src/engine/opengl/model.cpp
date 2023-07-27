#include "mixi/engine/opengl/model.h"


namespace mixi
{
namespace gl
{
namespace eng
{

Material::Material(const aiMaterial* material) :
    name(material->GetName().C_Str()),
    shininess(0.5)
{
    material->Get(AI_MATKEY_SHININESS, shininess);

    aiString diffuse_tex_path;
    if (material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_tex_path) == AI_SUCCESS) {
        texture_diffuse = TexturesManager::Get(
            std::filesystem::path("resource/model") / diffuse_tex_path.C_Str()
        );
    }

    aiString specular_tex_path;
    if (material->GetTexture(aiTextureType_SPECULAR, 0, &specular_tex_path) == AI_SUCCESS) {
        texture_specular = TexturesManager::Get(
            std::filesystem::path("resource/model") / specular_tex_path.C_Str()
        );
    }

    aiString normals_tex_path;
    if (material->GetTexture(aiTextureType_NORMALS, 0, &normals_tex_path) == AI_SUCCESS) {
        texture_normals = TexturesManager::Get(
            std::filesystem::path("resource/model") / normals_tex_path.C_Str()
        );
    }

    aiString height_tex_path;
    if (material->GetTexture(aiTextureType_HEIGHT, 0, &height_tex_path) == AI_SUCCESS) {
        texture_height = TexturesManager::Get(
            std::filesystem::path("resource/model") / height_tex_path.C_Str()
        );
    }
}

Mesh::Mesh(
    const aiMesh* mesh,
    const std::vector<std::shared_ptr<Material>>& all_materials
) :
    name(mesh->mName.C_Str()),
    material(all_materials[mesh->mMaterialIndex])
{
    if (mesh->mNumVertices == 0) {
        return;
    }

    int stride = 3;
    if (mesh->HasNormals()) {
        stride += 3;
    }
    if (mesh->HasTangentsAndBitangents()) {
        stride += 6;
    }
    for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
        if (!mesh->HasTextureCoords(i)) {
            break;
        }
        stride += 2;
    }

    std::vector<VertexBuffer::Descriptor> descriptor;
    std::vector<float> vertices;
    vertices.resize(mesh->mNumVertices * stride);

    int offset = 0;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices[i * stride + offset + 0] = mesh->mVertices[i].x;
        vertices[i * stride + offset + 1] = mesh->mVertices[i].y;
        vertices[i * stride + offset + 2] = mesh->mVertices[i].z;
    }
    offset += 3;
    descriptor.emplace_back("vertex", 3, GL_FLOAT, GL_FALSE);
    
    if (mesh->HasNormals()) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vertices[i * stride + offset + 0] = mesh->mNormals[i].x;
            vertices[i * stride + offset + 1] = mesh->mNormals[i].y;
            vertices[i * stride + offset + 2] = mesh->mNormals[i].z;
        }
        offset += 3;
        descriptor.emplace_back("normal", 3, GL_FLOAT, GL_FALSE);
    }

    if (mesh->HasTangentsAndBitangents()) {
        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            vertices[i * stride + offset + 0] = mesh->mTangents[i].x;
            vertices[i * stride + offset + 1] = mesh->mTangents[i].y;
            vertices[i * stride + offset + 2] = mesh->mTangents[i].z;
            vertices[i * stride + offset + 3] = mesh->mBitangents[i].x;
            vertices[i * stride + offset + 4] = mesh->mBitangents[i].y;
            vertices[i * stride + offset + 5] = mesh->mBitangents[i].z;
        }
        offset += 6;
        descriptor.emplace_back("tangent",   3, GL_FLOAT, GL_FALSE);
        descriptor.emplace_back("bitangent", 3, GL_FLOAT, GL_FALSE);
    }

    for (int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++) {
        if (!mesh->HasTextureCoords(i)) {
            break;
        }
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            vertices[j * stride + offset + 0] = mesh->mTextureCoords[i][j].x;
            vertices[j * stride + offset + 1] = mesh->mTextureCoords[i][j].y;
        }
        offset += 2;
        std::string texture_name = "texture " + std::to_string(i);
        descriptor.emplace_back(texture_name, 2, GL_FLOAT, GL_FALSE);
    }

    vertex_buffer = std::make_shared<VertexBuffer>(
        vertices.size() * sizeof(float),
        vertices.data(),
        Buffer::Usage::STATIC_READ,
        mesh->mNumVertices,
        descriptor
    );
}

void Mesh::draw() const
{
    if (on_draw) {
        on_draw(*this);
    }
    // vertex_array.draw();
}

void Node::draw() const
{
    if (on_draw) {
        on_draw(*this);
    }
    for (std::shared_ptr<Mesh> mesh : meshes) {
        mesh->draw();
    }
    for (const Node& child : children) {
        child.draw();
    }
}

Model::Model(const char* filepath, unsigned int p_flags)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filepath, p_flags);

    std::vector<std::shared_ptr<Material>> all_materials;
    all_materials.reserve(scene->mNumMaterials);
    for (int i = 0; i < scene->mNumMaterials; i++) {
        all_materials.emplace_back(std::make_shared<Material>(scene->mMaterials[i]));
    }

    std::vector<std::shared_ptr<Mesh>> all_meshes;
    all_meshes.reserve(scene->mNumMeshes);
    for (int i = 0; i < scene->mNumMeshes; i++) {
        all_meshes.emplace_back(std::make_shared<Mesh>(scene->mMeshes[i], all_materials));
    }

    recursively_visit_nodes_(scene->mRootNode, root_node, all_meshes);
}

void Model::draw() const
{
    root_node.draw();
}

void Model::recursively_visit_nodes_(
    const aiNode* ai_node,
    Node& my_node,
    const std::vector<std::shared_ptr<Mesh>>& all_meshes
) {
    my_node.name.assign(ai_node->mName.C_Str());

    my_node.meshes.reserve(ai_node->mNumMeshes);
    for (int i = 0; i < ai_node->mNumMeshes; i++) {
        my_node.meshes.push_back(all_meshes[ai_node->mMeshes[i]]);
    }

    my_node.children.resize(ai_node->mNumChildren);
    for (int i = 0; i < ai_node->mNumChildren; i++) {
        recursively_visit_nodes_(
            ai_node->mChildren[i],
            my_node.children[i],
            all_meshes
        );
    }
}

} // namespace eng
} // namespace gl
} // namespace mixi
