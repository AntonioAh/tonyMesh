#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>
#include <tonyMesh/mesh.hpp>

namespace tonyMesh::app {
// One appearance of a mesh in the scene. The mesh itself is owned elsewhere
// (ResourceManager); many Objects can point to the same Mesh.
class Object {
public:
    explicit Object(const Mesh& mesh) : m_mesh(&mesh) {}

    const Mesh& mesh() const { return *m_mesh; }

    Eigen::Affine3f model() const {
        return Eigen::Translation3f(m_position) * m_rotation *
               Eigen::Scaling(m_scale);
    }

    Eigen::Vector3f& position() { return m_position; }
    const Eigen::Vector3f& position() const { return m_position; }

    float& scale() { return m_scale; }
    float scale() const { return m_scale; }

    Eigen::Quaternionf& rotation() { return m_rotation; }
    const Eigen::Quaternionf& rotation() const { return m_rotation; }

    Eigen::Vector3f& color() { return m_color; }
    const Eigen::Vector3f& color() const { return m_color; }

private:
    const Mesh* m_mesh;  // non-owning
    Eigen::Vector3f m_position{Eigen::Vector3f::Zero()};
    float m_scale{1.0f};
    Eigen::Quaternionf m_rotation{Eigen::Quaternionf::Identity()};
    Eigen::Vector3f m_color{0.8f, 0.8f, 0.8f};
};
}  // namespace tonyMesh::app
