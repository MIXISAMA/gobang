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
    material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_tex_path);
    if (diffuse_tex_path.length != 0) {
        diffuse = TexturesManager::Get(diffuse_tex_path.C_Str());
    }

    aiString specular_tex_path;
    material->GetTexture(aiTextureType_SPECULAR, 0, &specular_tex_path);
    if (specular_tex_path.length != 0) {
        specular = TexturesManager::Get(specular_tex_path.C_Str());
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
    if (!mesh->HasNormals()) {
        throw std::runtime_error("model has no normals");
    }

    int num_tex = 0;
    int columns = 6;
    for (int num_tex = 0; num_tex < AI_MAX_NUMBER_OF_TEXTURECOORDS; num_tex++) {
        if (!mesh->HasTextureCoords(num_tex)) {
            break;
        }
    }
    columns += num_tex * 2;
    std::vector<float> vertices;
    vertices.reserve(mesh->mNumVertices * columns);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        vertices.push_back(mesh->mVertices[i].x);
        vertices.push_back(mesh->mVertices[i].y);
        vertices.push_back(mesh->mVertices[i].z);
        vertices.push_back(mesh->mNormals[i].x);
        vertices.push_back(mesh->mNormals[i].y);
        vertices.push_back(mesh->mNormals[i].z);
        for (int j = 0; j < num_tex; j++) {
            vertices.push_back(mesh->mTextureCoords[j][i].x);
            vertices.push_back(mesh->mTextureCoords[j][i].y);
        }
    }
    std::vector<VertexBuffer::Descriptor> descriptor;
    descriptor.reserve(num_tex + 2);
    descriptor.emplace_back("vertex", 3, GL_FLOAT, GL_FALSE);
    descriptor.emplace_back("normal", 3, GL_FLOAT, GL_FALSE);
    for (int i = 0; i < num_tex; i++) {
        std::string texture_name = "texture ";
        if (mesh->HasTextureCoordsName(i)) {
            texture_name += mesh->GetTextureCoordsName(i)->C_Str();
        } else {
            texture_name += std::to_string(i);
        }
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
