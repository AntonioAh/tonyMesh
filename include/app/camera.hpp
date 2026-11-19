#pragma once

#include <Eigen/Core>
#include <numbers>

namespace tonyMesh {
class Mesh;  // forward declaration: the header does not need the full type
}

namespace tonyMesh::app {

// Orbit camera: circles around a target point at a given distance.
class Camera {
public:
    void orbit(float dx, float dy);  // mouse delta in pixels
    void pan(float dx, float dy);    // mouse delta in pixels
    void zoom(float scroll);         // mouse wheel steps
    // Center on the mesh and fit it in view; margin > 1 leaves extra room.
    void frame(const Mesh& mesh, float margin = 1.f);

    [[nodiscard]] Eigen::Vector3f eye() const;
    [[nodiscard]] Eigen::Matrix4f view() const;
    [[nodiscard]] Eigen::Matrix4f projection(float aspect) const;

private:
    Eigen::Vector3f m_target{Eigen::Vector3f::Zero()};
    float m_distance{3.f};
    float m_yaw{0.f};     // radians, around the Y axis
    float m_pitch{0.3f};  // radians, up/down
    float m_fov{std::numbers::pi_v<float> / 4.f};
    float m_near{0.01f};
    float m_far{1000.f};
};

}  // namespace tonyMesh::app
