#ifndef PROCEDURAL_INTERSECTION_H
#define PROCEDURAL_INTERSECTION_H

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/shape3d.hpp>

namespace godot {

    class ProceduralIntersection : public MeshInstance3D {
        GDCLASS(ProceduralIntersection, MeshInstance3D)

    private:
        float Radius = 10.0f;
        float UvScale = 0.2f;
        bool UseCollision = true;
        StaticBody3D* StaticBody = nullptr;
        CollisionShape3D* CollisionShape = nullptr;

        Vector<Vector2> PerimeterPolygon;
        float CenterY = 0.0f;
        bool TriggerBakeTerrain = false;

    protected:
        static void _bind_methods();

    public:
        ProceduralIntersection();
        ~ProceduralIntersection();

        void SetRadius(float p_radius);
        float GetRadius() const;

        void SetUvScale(float p_scale);
        float GetUvScale() const;

        Transform3D GetConnectionTransform(Vector3 p_road_global_pos) const;

        void SetUseCollision(bool p_use);
        bool GetUseCollision() const;

        void SetTriggerBakeTerrain(bool p_trigger);
        bool GetTriggerBakeTerrain() const;
        void BakeTerrain();

        // L'algorithme de génération de la géométrie
        void RebuildIntersection();
    };
}
#endif