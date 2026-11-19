#include <Eigen/Geometry>  // Vector3::cross
#include <algorithm>
#include <cmath>
#include <tonyMesh/app/camera.hpp>
#include <tonyMesh/mesh.hpp>

namespace tonyMesh::app {
namespace {
constexpr float kOrbitSpeed = 0.01f;  // radians per pixel
constexpr float kPanSpeed = 0.0015f;  // fraction of the distance per pixel
constexpr float kZoomFactor = 0.9f;   // distance multiplier per wheel step
constexpr float kMinDistance = 1e-3f;
// Just under 90 degrees: avoids the flip when looking straight up or down.
constexpr float kMaxPitch = 1.55f;

struct Basis {
    Eigen::Vector3f right, up, forward;
};

Basis makeBasis(const Eigen::Vector3f& eye, const Eigen::Vector3f& target) {
    Basis b;
    b.forward = (target - eye).normalized();
    b.right = b.forward.cross(Eigen::Vector3f::UnitY()).normalized();
    b.up = b.right.cross(b.forward);
    return b;
}
}  // namespace

Eigen::Vector3f Camera::eye() const {
    return m_target +
           m_distance * Eigen::Vector3f(std::cos(m_pitch) * std::sin(m_yaw),
                                        std::sin(m_pitch),
                                        std::cos(m_pitch) * std::cos(m_yaw));
}

void Camera::orbit(float dx, float dy) {
    m_yaw -= dx * kOrbitSpeed;
    m_pitch = std::clamp(m_pitch + dy * kOrbitSpeed, -kMaxPitch, kMaxPitch);
}

void Camera::pan(float dx, float dy) {
    const Basis b = makeBasis(eye(), m_target);
    const float scale = kPanSpeed * m_distance;  // farther away = bigger steps
    m_target += (-dx * b.right + dy * b.up) * scale;
}

void Camera::zoom(float scroll) {
    m_distance =
        std::max(kMinDistance, m_distance * std::pow(kZoomFactor, scroll));
}

void Camera::frame(const Mesh& mesh, float margin) {
    if (mesh.numVertices() == 0) return;

    const Eigen::Vector3f lo = mesh.vertices().colwise().minCoeff().transpose();
    const Eigen::Vector3f hi = mesh.vertices().colwise().maxCoeff().transpose();

    m_target = 0.5f * (lo + hi);
    const float radius =
        std::max(0.5f * (hi - lo).norm() * margin, kMinDistance);
    m_distance = radius / std::sin(m_fov * 0.5f);
    m_near = radius * 0.01f;
    m_far = (m_distance + radius) * 10.f;
}

Eigen::Matrix4f Camera::view() const {
    const Eigen::Vector3f e = eye();
    const Basis b = makeBasis(e, m_target);

    Eigen::Matrix4f v = Eigen::Matrix4f::Identity();
    v.block<1, 3>(0, 0) = b.right.transpose();
    v.block<1, 3>(1, 0) = b.up.transpose();
    v.block<1, 3>(2, 0) = -b.forward.transpose();
    v(0, 3) = -b.right.dot(e);
    v(1, 3) = -b.up.dot(e);
    v(2, 3) = b.forward.dot(e);
    return v;
}

Eigen::Matrix4f Camera::projection(float aspect) const {
    const float f = 1.f / std::tan(m_fov * 0.5f);

    Eigen::Matrix4f p = Eigen::Matrix4f::Zero();
    p(0, 0) = f / aspect;
    p(1, 1) = f;
    p(2, 2) = (m_far + m_near) / (m_near - m_far);
    p(2, 3) = 2.f * m_far * m_near / (m_near - m_far);
    p(3, 2) = -1.f;
    return p;
}

}  // namespace tonyMesh::app
