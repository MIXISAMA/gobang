#include "pch.h"

namespace mixi
{
namespace gl
{
namespace eng
{

struct BaseMaterial
{
    using Ptr = std::shared_ptr<BaseMaterial>;
};

class BaseMaterialFactory
{

public:

    enum class Type
    {
        FLOAT,
        VEC3,
    };

    virtual BaseMaterial* create(
        const std::initializer_list<
            std::tuple<std::string, Type, void*>
        >& name_type_data_list
    ) = 0;

};

struct AssimpMaterial : public BaseMaterial
{

};

class AssimpMaterialFactory : public BaseMaterialFactory
{

public:

    AssimpMaterial* create(
        const std::initializer_list<
            std::tuple<std::string, Type, void*>
        >& name_type_data_list
    ) override;

};

} // namespace eng
} // namespace gl
} // namespace mixi
