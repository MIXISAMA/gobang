#include "geometry/camera.h"

namespace mixi
{
namespace geo
{

Camera::Camera(
    const glm::vec3& position,
    const glm::vec3& world_up
) :
    position_(position),
    world_up_(world_up),
    yaw_(-90.0f),
    pitch_(0.0f)
{
    update_();
}

glm::mat4 Camera::view_matrix() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}

glm::vec3 Camera::position() const
{
    return position_;
}

void Camera::position(const glm::vec3& p)
{
    position_ = p;
}

void Camera::move_forward(float delta)
{
    position_ += front_ * delta;
}

void Camera::move_backward(float delta)
{
    position_ -= front_ * delta;
}

void Camera::move_left(float delta)
{
    position_ -= right_ * delta;
}

void Camera::move_right(float delta)
{
    position_ += right_ * delta;
}

void Camera::move_up(float delta)
{
    position_ += up_ * delta;
}

void Camera::move_down(float delta)
{
    position_ -= up_ * delta;
}

float Camera::yaw() const
{
    return yaw_;
}

void Camera::yaw(float y)
{
    yaw_ = y;
    update_();
}

float Camera::pitch() const
{
    return pitch_;
}

void Camera::pitch(float p)
{
    set_pitch_(p);
    update_();
}

void Camera::yaw_pitch(float y, float p)
{
    yaw_ = y;
    set_pitch_(p);
    update_();
}

void Camera::yaw_left(float delta)
{
    yaw_ -= delta;
    update_();
}

void Camera::yaw_right(float delta)
{
    yaw_ += delta;
    update_();
}

void Camera::pitch_up(float delta)
{
    set_pitch_(pitch_ + delta);
    update_();
}

void Camera::pitch_down(float delta)
{
    set_pitch_(pitch_ - delta);
    update_();
}

void Camera::yaw_right_pitch_up_(float yaw_delta, float pitch_delta)
{
    yaw_ += yaw_delta;
    set_pitch_(pitch_ + pitch_delta);
    update_();
}

void Camera::update_()
{
    glm::vec3 front(
        cos(glm::radians(yaw_)) * cos(glm::radians(pitch_)),
        sin(glm::radians(pitch_)),
        sin(glm::radians(yaw_)) * cos(glm::radians(pitch_))
    );
    front_ = glm::normalize(front);
    right_ = glm::normalize(glm::cross(front_, world_up_));
    up_    = glm::normalize(glm::cross(right_, front_));
}

void Camera::set_pitch_(float p)
{
    if (p > Pitch_Max_) {
        pitch_ = Pitch_Max_;
    }
    else if (p < Pitch_Min_) {
        pitch_ = Pitch_Min_;
    }
    else {
        pitch_ = p;
    }
}

const float Camera::Pitch_Max_ =  89.0f;
const float Camera::Pitch_Min_ = -89.0f;

} // namespace geo
} // namespace mixi
