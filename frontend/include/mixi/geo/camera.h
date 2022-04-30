#pragma once
#include "pch.h"

namespace mixi
{
namespace geo
{

class Camera
{

public:

    Camera(
        const glm::vec3& position = glm::vec3(0.0f, 0.0f, 0.0f),
        const glm::vec3& world_up = glm::vec3(0.0f, 1.0f, 0.0f)
    );

    glm::mat4 view_matrix() const;
    
    glm::vec3 position() const;
    void position(const glm::vec3& p);

    void move_forward (float delta);
    void move_backward(float delta);
    void move_left    (float delta);
    void move_right   (float delta);
    void move_up      (float delta);
    void move_down    (float delta);

    float yaw() const;
    void yaw(float y);
    float pitch() const;
    void pitch(float p);
    void yaw_pitch(float y, float p);

    void yaw_left  (float delta);
    void yaw_right (float delta);
    void pitch_up  (float delta);
    void pitch_down(float delta);
    void yaw_right_pitch_up_(float yaw_delta, float pitch_delta);

protected:

    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    glm::vec3 right_;

    float yaw_;
    float pitch_;

    const glm::vec3 world_up_;

    static const float Pitch_Max_;
    static const float Pitch_Min_;

    void update_();

    void set_pitch_(float p);

};

} // namespace geo
} // namespace mixi
