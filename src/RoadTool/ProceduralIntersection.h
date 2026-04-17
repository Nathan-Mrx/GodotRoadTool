#ifndef PROCEDURAL_INTERSECTION_H
#define PROCEDURAL_INTERSECTION_H

#include <godot_cpp/classes/node3d.hpp>

namespace godot {

    class ProceduralIntersection : public Node3D {
        GDCLASS(ProceduralIntersection, Node3D)

    private:
        float Radius = 10.0f; // La taille du carrefour

    protected:
        static void _bind_methods();

    public:
        ProceduralIntersection();
        ~ProceduralIntersection();

        void SetRadius(float p_radius);
        float GetRadius() const;

        // LA MAGIE : Calcule le point d'accroche optimal et l'angle sur le bord du carrefour
        Transform3D GetConnectionTransform(Vector3 p_road_global_pos) const;
    };
}
#endif