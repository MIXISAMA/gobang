#include "gui/imgui/component/component.h"

namespace mixi
{
namespace imgui
{

BaseComponent::BaseComponent(Context& context) :
    context_(context)
{

}

void BaseComponent::render() {
    content();
}

} // namespace imgui
} // namespace mixi
