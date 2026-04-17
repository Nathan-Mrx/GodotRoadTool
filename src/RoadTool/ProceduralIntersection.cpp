#include "ProceduralIntersection.h"
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void ProceduralIntersection::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &ProceduralIntersection::SetRadius);
    ClassDB::bind_method(D_METHOD("get_radius"), &ProceduralIntersection::GetRadius);
    ClassDB::bind_method(D_METHOD("get_connection_transform", "road_global_pos"), &ProceduralIntersection::GetConnectionTransform);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
}

ProceduralIntersection::ProceduralIntersection() {}
ProceduralIntersection::~ProceduralIntersection() {}

void ProceduralIntersection::SetRadius(float p_radius) { Radius = p_radius; }
float ProceduralIntersection::GetRadius() const { return Radius; }

Transform3D ProceduralIntersection::GetConnectionTransform(Vector3 p_road_global_pos) const {
    if (!is_inside_tree()) return Transform3D();

    Vector3 center = get_global_position();

    // Direction du centre vers la route (on reste à plat sur XZ)
    Vector3 dir = (p_road_global_pos - center);
    dir.y = 0.0f;

    if (dir.length_squared() < 0.001f) {
        dir = Vector3(0, 0, 1);
    } else {
        dir.normalize();
    }

    // Le point d'aimantation exact sur le cercle
    Vector3 snap_pos = center + dir * Radius;

    // Calcul de la matrice de rotation (Basis)
    // On veut que le "Forward" (-Z) de la transformation pointe vers l'extérieur (dir)
    Vector3 z_axis = -dir;
    Vector3 up_axis = Vector3(0, 1, 0);
    Vector3 x_axis = up_axis.cross(z_axis).normalized();
    Vector3 y_axis = z_axis.cross(x_axis).normalized();

    return Transform3D(Basis(x_axis, y_axis, z_axis), snap_pos);
}