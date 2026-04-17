#include "ProceduralRoadGizmos.h"
#include "ProceduralRoad.h"
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_selection.hpp>
#include <godot_cpp/classes/label3d.hpp>
#include <godot_cpp/classes/base_material3d.hpp>

using namespace godot;

void ProceduralRoadGizmosPlugin::_bind_methods() {}

ProceduralRoadGizmosPlugin::ProceduralRoadGizmosPlugin() {
    create_material("road_mat", Color(1.0, 0.6, 0.0), false, true, false);
    create_material("falloff_mat", Color(1.0, 0.2, 0.2, 0.5), false, true, false);

    create_material("tilt_line_active", Color(0.0, 1.0, 1.0, 1.0), false, true, false);
    create_material("tilt_line_dim", Color(0.0, 0.5, 0.5, 0.15), false, true, false);
    create_handle_material("tilt_handle_mat");
}

ProceduralRoadGizmosPlugin::~ProceduralRoadGizmosPlugin() {}

bool ProceduralRoadGizmosPlugin::_has_gizmo(Node3D *p_spatial) const {
    return Object::cast_to<ProceduralRoad>(p_spatial) != nullptr;
}

String ProceduralRoadGizmosPlugin::_get_gizmo_name() const {
    return "ProceduralRoad";
}

void ProceduralRoadGizmosPlugin::_redraw(const Ref<EditorNode3DGizmo> &p_gizmo) {
    p_gizmo->clear();

    ProceduralRoad *road = Object::cast_to<ProceduralRoad>(p_gizmo->get_node_3d());
    if (!road) return;

    bool is_selected = false;
    EditorSelection *selection = EditorInterface::get_singleton()->get_selection();
    if (selection) {
        TypedArray<Node> selected_nodes = selection->get_selected_nodes();
        for (int i = 0; i < selected_nodes.size(); ++i) {
            if (Object::cast_to<Node>(selected_nodes[i]) == road) {
                is_selected = true;
                break;
            }
        }
    }

    if (!is_selected) return;

    Ref<Curve3D> curve = road->get_curve();
    if (curve.is_null() || curve->get_point_count() < 2) return;

    PackedVector3Array road_lines;
    PackedVector3Array falloff_lines;

    float half_width = road->GetTotalRoadWidth() * 0.5f;
    float falloff = road->GetTerrainFalloff();
    float half_falloff = half_width + falloff;

    int sample_count = MAX(10, Math::ceil(curve->get_baked_length() / 2.0f));
    float length = curve->get_baked_length();
    Vector3 prev_road_l, prev_road_r, prev_fall_l, prev_fall_r;

    for (int i = 0; i <= sample_count; ++i) {
        float offset = (float)i / sample_count * length;
        Transform3D t = curve->sample_baked_with_rotation(offset, true, true);

        Vector3 p0 = t.origin;
        Vector3 up = t.basis.get_column(1).normalized();
        Vector3 right = -t.basis.get_column(2).cross(up).normalized();

        Vector3 road_l = p0 - right * half_width;
        Vector3 road_r = p0 + right * half_width;
        Vector3 fall_l = p0 - right * half_falloff;
        Vector3 fall_r = p0 + right * half_falloff;

        if (i > 0) {
            road_lines.push_back(prev_road_l); road_lines.push_back(road_l);
            road_lines.push_back(prev_road_r); road_lines.push_back(road_r);
            falloff_lines.push_back(prev_fall_l); falloff_lines.push_back(fall_l);
            falloff_lines.push_back(prev_fall_r); falloff_lines.push_back(fall_r);
        }
        prev_road_l = road_l; prev_road_r = road_r;
        prev_fall_l = fall_l; prev_fall_r = fall_r;
    }

    p_gizmo->add_lines(road_lines, get_material("road_mat", p_gizmo));
    p_gizmo->add_lines(falloff_lines, get_material("falloff_mat", p_gizmo));

    PackedVector3Array active_tilt_lines;
    PackedVector3Array dim_tilt_lines;
    PackedVector3Array tilt_handles;
    PackedInt32Array handle_ids;

    for (int i = 0; i < curve->get_point_count(); ++i) {
        Vector3 p_local = curve->get_point_position(i);
        float offset = curve->get_closest_offset(p_local);
        Transform3D t = curve->sample_baked_with_rotation(offset, true, true);

        Vector3 up = t.basis.get_column(1).normalized();
        Vector3 handle_pos = p_local + up * 2.5f;

        if (road == active_road && i == active_handle) {
            active_tilt_lines.push_back(p_local);
            active_tilt_lines.push_back(handle_pos);
        } else {
            dim_tilt_lines.push_back(p_local);
            dim_tilt_lines.push_back(handle_pos);
        }

        tilt_handles.push_back(handle_pos);
        handle_ids.push_back(i);
    }

    if (active_tilt_lines.size() > 0) p_gizmo->add_lines(active_tilt_lines, get_material("tilt_line_active", p_gizmo));
    if (dim_tilt_lines.size() > 0) p_gizmo->add_lines(dim_tilt_lines, get_material("tilt_line_dim", p_gizmo));
    p_gizmo->add_handles(tilt_handles, get_material("tilt_handle_mat", p_gizmo), handle_ids, false, false);
}

String ProceduralRoadGizmosPlugin::_get_handle_name(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary) const {
    return "Roll (Tilt) Point " + String::num_int64(p_handle_id);
}

Variant ProceduralRoadGizmosPlugin::_get_handle_value(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary) const {
    ProceduralRoad *road = Object::cast_to<ProceduralRoad>(p_gizmo->get_node_3d());
    if (!road || road->get_curve().is_null()) return Variant(0.0f);
    return road->get_curve()->get_point_tilt(p_handle_id);
}

void ProceduralRoadGizmosPlugin::_set_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary, Camera3D *p_camera, const Vector2 &p_screen_pos) {
    ProceduralRoad *road = Object::cast_to<ProceduralRoad>(p_gizmo->get_node_3d());
    if (!road || road->get_curve().is_null()) return;

    active_road = road;
    active_handle = p_handle_id;

    Ref<Curve3D> curve = road->get_curve();
    Vector3 p_local = curve->get_point_position(p_handle_id);
    Vector3 p_global = road->get_global_transform().xform(p_local);

    float offset = curve->get_closest_offset(p_local);
    Transform3D t = curve->sample_baked_with_rotation(offset, true, true);
    Vector3 tangent_local = -t.basis.get_column(2).normalized();
    Vector3 tangent_global = road->get_global_transform().basis.xform(tangent_local).normalized();

    Vector3 ray_from = p_camera->project_ray_origin(p_screen_pos);
    Vector3 ray_dir = p_camera->project_ray_normal(p_screen_pos);
    Plane plane(tangent_global, p_global);

    Vector3 hit;
    if (plane.intersects_ray(ray_from, ray_dir, &hit)) {
        Vector3 dir_global = (hit - p_global).normalized();

        Vector3 world_up(0, 1, 0);
        if (Math::abs(tangent_global.dot(world_up)) > 0.99f) world_up = Vector3(1, 0, 0);

        Vector3 ref_right = tangent_global.cross(world_up).normalized();
        Vector3 ref_up = ref_right.cross(tangent_global).normalized();

        float x = dir_global.dot(ref_right);
        float y = dir_global.dot(ref_up);
        float angle = Math::atan2(x, y);

        curve->set_point_tilt(p_handle_id, angle);
        road->RebuildRoad();

        // ==========================================================
        // --- LA MAGIE : Le Label 3D Holographique ---
        // ==========================================================
        Label3D *label = Object::cast_to<Label3D>(road->get_node_or_null(NodePath("_GizmoTiltLabel")));
        if (!label) {
            label = memnew(Label3D);
            label->set_name("_GizmoTiltLabel");
            label->set_billboard_mode(BaseMaterial3D::BILLBOARD_ENABLED); // Fait face à la caméra
            label->set_draw_flag(Label3D::FLAG_DISABLE_DEPTH_TEST, true); // Passe au travers de la montagne/route
            label->set_modulate(Color(0.0, 1.0, 1.0)); // Cyan vif
            label->set_font_size(72);
            label->set_outline_size(12);
            road->add_child(label);
        }

        // Calcul et affichage au dixième de degré près
        float deg = angle * 180.0f / Math_PI;
        label->set_text(String::num(deg, 1) + " deg");

        // Le texte suit la position de ta souris projetée dans l'espace 3D !
        label->set_global_position(hit + Vector3(0, 0.5, 0));
    }
}

void ProceduralRoadGizmosPlugin::_commit_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
    ProceduralRoad *road = Object::cast_to<ProceduralRoad>(p_gizmo->get_node_3d());
    if (!road || road->get_curve().is_null()) return;

    active_road = nullptr;
    active_handle = -1;

    // Nettoyage immédiat du texte 3D à la fin du clic
    Node *label = road->get_node_or_null(NodePath("_GizmoTiltLabel"));
    if (label) {
        label->queue_free();
    }

    if (p_cancel) {
        road->get_curve()->set_point_tilt(p_handle_id, p_restore);
        road->RebuildRoad();
    } else {
        road->update_gizmos();
    }
}