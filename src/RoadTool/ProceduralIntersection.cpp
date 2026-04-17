#include "ProceduralIntersection.h"
#include "ProceduralRoad.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <vector>
#include <algorithm>

using namespace godot;

struct RoadPort {
    Vector3 BasePos;
    Vector3 LeftPt;
    Vector3 RightPt;
    Vector3 Dir;
    float Angle;

    float ShoulderWidth;
    float LineOffset;
    Color EdgeColor;
    float RoadThickness;
};

void ProceduralIntersection::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_radius", "radius"), &ProceduralIntersection::SetRadius);
    ClassDB::bind_method(D_METHOD("get_radius"), &ProceduralIntersection::GetRadius);
    ClassDB::bind_method(D_METHOD("set_uv_scale", "scale"), &ProceduralIntersection::SetUvScale);
    ClassDB::bind_method(D_METHOD("get_uv_scale"), &ProceduralIntersection::GetUvScale);
    ClassDB::bind_method(D_METHOD("set_use_collision", "use"), &ProceduralIntersection::SetUseCollision);
    ClassDB::bind_method(D_METHOD("get_use_collision"), &ProceduralIntersection::GetUseCollision);
    ClassDB::bind_method(D_METHOD("get_connection_transform", "road_global_pos"), &ProceduralIntersection::GetConnectionTransform);
    ClassDB::bind_method(D_METHOD("rebuild_intersection"), &ProceduralIntersection::RebuildIntersection);
    ClassDB::bind_method(D_METHOD("set_trigger_bake_terrain", "trigger"), &ProceduralIntersection::SetTriggerBakeTerrain);
    ClassDB::bind_method(D_METHOD("get_trigger_bake_terrain"), &ProceduralIntersection::GetTriggerBakeTerrain);
    ClassDB::bind_method(D_METHOD("bake_terrain"), &ProceduralIntersection::BakeTerrain);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "uv_scale"), "set_uv_scale", "get_uv_scale");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_collision"), "set_use_collision", "get_use_collision");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "bake_terrain_button"), "set_trigger_bake_terrain", "get_trigger_bake_terrain");

}

ProceduralIntersection::ProceduralIntersection() {}
ProceduralIntersection::~ProceduralIntersection() {}

void ProceduralIntersection::SetRadius(float p_radius) {
    Radius = p_radius;
    RebuildIntersection();
}
float ProceduralIntersection::GetRadius() const { return Radius; }

void ProceduralIntersection::SetUvScale(float p_scale) {
    UvScale = p_scale;
    RebuildIntersection();
}
float ProceduralIntersection::GetUvScale() const { return UvScale; }

void ProceduralIntersection::SetUseCollision(bool p_use) {
    UseCollision = p_use;
    RebuildIntersection();
}
bool ProceduralIntersection::GetUseCollision() const { return UseCollision; }

Transform3D ProceduralIntersection::GetConnectionTransform(Vector3 p_road_global_pos) const {
    if (!is_inside_tree()) return Transform3D();

    Vector3 center = get_global_position();
    Vector3 dir = (p_road_global_pos - center);
    dir.y = 0.0f;

    if (dir.length_squared() < 0.001f) {
        dir = Vector3(0, 0, 1);
    } else {
        dir.normalize();
    }

    Vector3 snap_pos = center + dir * Radius;
    Vector3 z_axis = -dir;
    Vector3 up_axis = Vector3(0, 1, 0);
    Vector3 x_axis = up_axis.cross(z_axis).normalized();
    Vector3 y_axis = z_axis.cross(x_axis).normalized();

    return Transform3D(Basis(x_axis, y_axis, z_axis), snap_pos);
}

void ProceduralIntersection::SetTriggerBakeTerrain(bool p_trigger) {
    TriggerBakeTerrain = p_trigger;
    if (p_trigger) {
        BakeTerrain();
        TriggerBakeTerrain = false; // Agit comme un bouton, se remet à false tout seul
    }
}
bool ProceduralIntersection::GetTriggerBakeTerrain() const { return TriggerBakeTerrain; }

void ProceduralIntersection::RebuildIntersection() {
    if (!is_inside_tree()) return;
    Node* parent = get_parent();
    if (!parent) return;

    std::vector<RoadPort> ports;
    Vector3 center_avg = Vector3(0, 0, 0);

    Ref<Material> road_mat;
    Ref<Material> line_mat;
    Vector2 road_uv_scale = Vector2(1.0f, 1.0f);
    float line_width = 0.15f;
    int profile_res = 8;
    bool global_style_fetched = false;

    TypedArray<Node> children = parent->get_children();
    for (int i = 0; i < children.size(); ++i) {
        ProceduralRoad* road = Object::cast_to<ProceduralRoad>(children[i]);
        if (!road) continue;

        if (road->get_node_or_null(road->GetConnectedStart()) != this &&
            road->get_node_or_null(road->GetConnectedEnd()) != this) {
            continue;
        }

        if (!global_style_fetched) {
            road_mat = road->GetRoadMaterial();
            line_mat = road->GetMarkingsMaterial();
            road_uv_scale = road->GetUvScale();
            profile_res = road->GetProfileResolution();
            global_style_fetched = true;
        }

        Ref<Curve3D> curve = road->get_curve();
        if (curve.is_null() || curve->get_point_count() < 2) continue;

        Transform3D road_trans = road->get_global_transform();

        float total_half_width = road->GetTotalRoadWidth() * 0.5f;
        float skirt_radius = road->GetRoadThickness() * 0.5f;
        float hinge_half_width = total_half_width - skirt_radius;

        auto process_port = [&](bool is_start) {
            Vector3 p0, p1;
            if (is_start) {
                p0 = road_trans.xform(curve->sample_baked_with_rotation(0.0f, true, true).origin);
                p1 = road_trans.xform(curve->sample_baked_with_rotation(MIN(1.0f, curve->get_baked_length()), true, true).origin);
            } else {
                float len = curve->get_baked_length();
                p0 = road_trans.xform(curve->sample_baked_with_rotation(len, true, true).origin);
                p1 = road_trans.xform(curve->sample_baked_with_rotation(MAX(0.0f, len - 1.0f), true, true).origin);
            }

            Vector3 dir = (p1 - p0);
            dir.y = 0.0f;
            if (dir.length_squared() < 0.001f) return;
            dir.normalize();

            Vector3 right = Vector3(0, 1, 0).cross(dir).normalized();

            RoadPort port;
            port.BasePos = p0;
            port.Dir = dir;
            port.LeftPt = p0 - right * hinge_half_width;
            port.RightPt = p0 + right * hinge_half_width;
            port.Angle = Math::atan2(dir.x, dir.z);

            port.ShoulderWidth = road->GetShoulderWidth();
            port.LineOffset = road->GetLineOffset();
            port.EdgeColor = road->GetEdgeLineColor();
            port.RoadThickness = road->GetRoadThickness();

            ports.push_back(port);
            center_avg += p0;
        };

        if (road->get_node_or_null(road->GetConnectedStart()) == this) process_port(true);
        if (road->get_node_or_null(road->GetConnectedEnd()) == this) process_port(false);
    }

    if (ports.size() < 2) {
        set_mesh(Ref<Mesh>());
        if (CollisionShape) {
            CollisionShape->set_shape(Ref<Shape3D>());
        }
        return;
    }

    center_avg /= ports.size();

    CenterY = center_avg.y;
    PerimeterPolygon.clear(); // On vide l'ancienne empreinte

    Vector3 my_pos = get_global_position();
    center_avg.x = my_pos.x;
    center_avg.z = my_pos.z;

    std::sort(ports.begin(), ports.end(), [](const RoadPort& a, const RoadPort& b) {
        return a.Angle < b.Angle;
    });

    Ref<SurfaceTool> st_road;
    st_road.instantiate();
    st_road->begin(Mesh::PRIMITIVE_TRIANGLES);

    Ref<SurfaceTool> st_lines;
    st_lines.instantiate();
    st_lines->begin(Mesh::PRIMITIVE_TRIANGLES);

    Transform3D my_global = get_global_transform();
    Transform3D my_inv = my_global.affine_inverse();
    Vector3 Up(0, 1, 0);

    auto set_uv_world = [&](Ref<SurfaceTool> st, Vector3 glob_pos) {
        st->set_uv(Vector2(glob_pos.x, glob_pos.z) * road_uv_scale);
    };

    int curve_resolution = 16;

    for (size_t i = 0; i < ports.size(); ++i) {
        size_t next = (i + 1) % ports.size();

        float t_y = ports[i].RoadThickness * 0.5f;

        Vector3 p_center = my_inv.xform(center_avg); p_center.y = t_y;
        Vector3 p_left = my_inv.xform(ports[i].LeftPt); p_left.y = t_y;
        Vector3 p_right = my_inv.xform(ports[i].RightPt); p_right.y = t_y;

        st_road->set_uv(Vector2(0, 0)); // Placeholder UV for flat surfaces before explicit mapping
        set_uv_world(st_road, center_avg); st_road->add_vertex(p_center);
        set_uv_world(st_road, ports[i].RightPt); st_road->add_vertex(p_right);
        set_uv_world(st_road, ports[i].LeftPt); st_road->add_vertex(p_left);

        Vector3 P0 = ports[i].RightPt;
        Vector3 P3 = ports[next].LeftPt;
        Vector3 Dir0 = ports[i].Dir;
        Vector3 Dir3 = ports[next].Dir;

        float corner_dist = P0.distance_to(P3) * 0.45f;
        Vector3 CP1 = P0 - Dir0 * corner_dist;
        Vector3 CP2 = P3 - Dir3 * corner_dist;

        Vector3 loc_P0 = my_inv.xform(P0);
        Vector3 glob_P0 = my_global.xform(loc_P0);
        PerimeterPolygon.push_back(Vector2(glob_P0.x, glob_P0.z));
        Vector3 loc_CP1 = my_inv.xform(CP1);
        Vector3 loc_CP2 = my_inv.xform(CP2);
        Vector3 loc_P3 = my_inv.xform(P3);

        Vector3 prev_arc_pt = loc_P0;
        prev_arc_pt.y = ports[i].RoadThickness * 0.5f;

        Vector3 dP0 = 3.0f * (loc_CP1 - loc_P0);
        Vector3 prev_tangent = dP0.length_squared() > 0.001f ? dP0.normalized() : Vector3(0,0,1);
        Vector3 prev_outward = prev_tangent.cross(Up).normalized();
        Vector3 prev_inward = -prev_outward;

        float current_line_len = 0.0f;

        for (int k = 1; k <= curve_resolution; ++k) {
            float t = (float)k / curve_resolution;
            float u = 1.0f - t;

            float current_thickness = Math::lerp(ports[i].RoadThickness, ports[next].RoadThickness, t);
            float current_shoulder = Math::lerp(ports[i].ShoulderWidth, ports[next].ShoulderWidth, t);
            float current_line_offset = Math::lerp(ports[i].LineOffset, ports[next].LineOffset, t);
            Color current_edge_color = ports[i].EdgeColor.lerp(ports[next].EdgeColor, t);

            float current_top_y = current_thickness * 0.5f;
            float current_target_line_y = current_top_y + current_line_offset;
            float current_radius = current_thickness * 0.5f;

            Vector3 loc_arc_pt = (u*u*u)*loc_P0 + 3*(u*u)*t*loc_CP1 + 3*u*(t*t)*loc_CP2 + (t*t*t)*loc_P3;
            loc_arc_pt.y = current_top_y;
            Vector3 glob_arc_pt = my_global.xform(loc_arc_pt);
            PerimeterPolygon.push_back(Vector2(glob_arc_pt.x, glob_arc_pt.z));

            Vector3 dP = 3*(u*u)*(loc_CP1 - loc_P0) + 6*u*t*(loc_CP2 - loc_CP1) + 3*(t*t)*(loc_P3 - loc_CP2);
            Vector3 curr_tangent = dP.length_squared() > 0.001f ? dP.normalized() : prev_tangent;
            Vector3 curr_outward = curr_tangent.cross(Up).normalized();
            Vector3 curr_inward = -curr_outward;

            Vector3 dynamic_p_center = p_center; dynamic_p_center.y = current_top_y;
            set_uv_world(st_road, center_avg); st_road->add_vertex(dynamic_p_center);
            set_uv_world(st_road, glob_arc_pt); st_road->add_vertex(loc_arc_pt);
            set_uv_world(st_road, my_global.xform(prev_arc_pt)); st_road->add_vertex(prev_arc_pt);

            Vector3 prev_center_edge = prev_arc_pt + Vector3(0, -current_radius, 0);
            Vector3 curr_center_edge = loc_arc_pt + Vector3(0, -current_radius, 0);

            for (int p = 0; p < profile_res; ++p) {
                float angle_t0 = (float)p / profile_res;
                float angle_t1 = (float)(p + 1) / profile_res;

                float ang0 = Math_PI * 0.5f - angle_t0 * Math_PI;
                float ang1 = Math_PI * 0.5f - angle_t1 * Math_PI;

                Vector3 off0_prev = prev_outward * (current_radius * Math::cos(ang0)) + Vector3(0, current_radius * Math::sin(ang0), 0);
                Vector3 off1_prev = prev_outward * (current_radius * Math::cos(ang1)) + Vector3(0, current_radius * Math::sin(ang1), 0);
                Vector3 off0_curr = curr_outward * (current_radius * Math::cos(ang0)) + Vector3(0, current_radius * Math::sin(ang0), 0);
                Vector3 off1_curr = curr_outward * (current_radius * Math::cos(ang1)) + Vector3(0, current_radius * Math::sin(ang1), 0);

                Vector3 p00 = prev_center_edge + off0_prev;
                Vector3 p01 = prev_center_edge + off1_prev;
                Vector3 p10 = curr_center_edge + off0_curr;
                Vector3 p11 = curr_center_edge + off1_curr;

                st_road->set_uv(Vector2(0, 1.0f - angle_t0)); st_road->add_vertex(p00);
                st_road->set_uv(Vector2(1, 1.0f - angle_t1)); st_road->add_vertex(p11);
                st_road->set_uv(Vector2(0, 1.0f - angle_t1)); st_road->add_vertex(p01);

                st_road->set_uv(Vector2(0, 1.0f - angle_t0)); st_road->add_vertex(p00);
                st_road->set_uv(Vector2(1, 1.0f - angle_t0)); st_road->add_vertex(p10);
                st_road->set_uv(Vector2(1, 1.0f - angle_t1)); st_road->add_vertex(p11);
            }

            float flat_shoulder_dist = current_shoulder - current_radius;
            float line_inset = flat_shoulder_dist - (line_width * 0.5f);

            Vector3 l_out_prev = prev_arc_pt + prev_inward * line_inset;
            l_out_prev.y = current_target_line_y;
            Vector3 l_in_prev = l_out_prev + prev_inward * line_width;

            Vector3 l_out_curr = loc_arc_pt + curr_inward * line_inset;
            l_out_curr.y = current_target_line_y;
            Vector3 l_in_curr = l_out_curr + curr_inward * line_width;

            float step_len = prev_arc_pt.distance_to(loc_arc_pt);
            st_lines->set_color(current_edge_color);

            st_lines->set_uv(Vector2(0, current_line_len)); st_lines->add_vertex(l_out_prev);
            st_lines->set_uv(Vector2(1, current_line_len + step_len)); st_lines->add_vertex(l_in_curr);
            st_lines->set_uv(Vector2(0, current_line_len + step_len)); st_lines->add_vertex(l_out_curr);

            st_lines->set_uv(Vector2(0, current_line_len)); st_lines->add_vertex(l_out_prev);
            st_lines->set_uv(Vector2(1, current_line_len)); st_lines->add_vertex(l_in_prev);
            st_lines->set_uv(Vector2(1, current_line_len + step_len)); st_lines->add_vertex(l_in_curr);

            current_line_len += step_len;
            prev_arc_pt = loc_arc_pt;
            prev_tangent = curr_tangent;
            prev_outward = curr_outward;
            prev_inward = curr_inward;
        }
    }

    st_road->generate_normals();
    st_road->generate_tangents();
    st_lines->generate_normals();
    st_lines->generate_tangents();

    Ref<ArrayMesh> final_mesh = st_road->commit();
    if (road_mat.is_valid()) final_mesh->surface_set_material(0, road_mat);

    if (!StaticBody) {
        StaticBody = memnew(StaticBody3D);
        add_child(StaticBody);
    }
    if (!CollisionShape) {
        CollisionShape = memnew(CollisionShape3D);
        StaticBody->add_child(CollisionShape);
    }

    if (UseCollision && final_mesh.is_valid()) {
        Ref<Shape3D> trimesh = final_mesh->create_trimesh_shape();
        CollisionShape->set_shape(trimesh);
    } else {
        CollisionShape->set_shape(Ref<Shape3D>());
    }

    st_lines->commit(final_mesh);
    if (line_mat.is_valid()) final_mesh->surface_set_material(1, line_mat);

    set_mesh(final_mesh);
}

void ProceduralIntersection::BakeTerrain() {
    if (PerimeterPolygon.size() < 3) return;

    Node* parent = get_parent();
    if (!parent) return;

    NodePath terrain_path;
    float falloff = 2.0f;
    float depth_offset = 0.0f;
    float road_thickness = 0.5f;
    bool found_terrain = false;

    // 1. Héritage intelligent : on vole les paramètres de la première route connectée
    TypedArray<Node> children = parent->get_children();
    for (int i = 0; i < children.size(); ++i) {
        ProceduralRoad* road = Object::cast_to<ProceduralRoad>(children[i]);
        if (road && (road->get_node_or_null(road->GetConnectedStart()) == this || road->get_node_or_null(road->GetConnectedEnd()) == this)) {
            terrain_path = road->GetTerrainPath();
            falloff = road->GetTerrainFalloff();
            depth_offset = road->GetTerrainDepthOffset();
            road_thickness = road->GetRoadThickness();
            found_terrain = true;
            break;
        }
    }

    if (!found_terrain || terrain_path.is_empty()) {
        UtilityFunctions::printerr("ProceduralIntersection: Aucune route connectée avec un TerrainPath valide.");
        return;
    }

    Node* terrain_node = get_node_or_null(terrain_path);
    if (!terrain_node) return;

    Object* storage = nullptr;
    if (terrain_node->has_method("get_data")) storage = terrain_node->call("get_data").get_validated_object();
    else if (terrain_node->has_method("get_storage")) storage = terrain_node->call("get_storage").get_validated_object();

    if (!storage) return;

    // 2. Calcul de la Bounding Box du carrefour
    float min_x = 1e9, max_x = -1e9, min_z = 1e9, max_z = -1e9;
    for (int i = 0; i < PerimeterPolygon.size(); ++i) {
        Vector2 p = PerimeterPolygon[i];
        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.y < min_z) min_z = p.y;
        if (p.y > max_z) max_z = p.y;
    }

    float search_radius = falloff + 1.5f;
    min_x -= search_radius; max_x += search_radius;
    min_z -= search_radius; max_z += search_radius;

    bool modified = false;
    float target_y = CenterY + (road_thickness * 0.5f) - depth_offset;

    // 3. Pénétration du terrain pixel par pixel
    for (float x = Math::floor(min_x); x <= Math::ceil(max_x); x += 1.0f) {
        for (float z = Math::floor(min_z); z <= Math::ceil(max_z); z += 1.0f) {
            Vector2 p(x, z);
            Vector3 pixel_pos(x, 0, z);

            float min_dist_sq = 1e9;
            bool inside = false;

            // Mathématiques : Algorithme de Ray-Casting & Distance au Segment
            for (int i = 0, j = PerimeterPolygon.size() - 1; i < PerimeterPolygon.size(); j = i++) {
                Vector2 a = PerimeterPolygon[j];
                Vector2 b = PerimeterPolygon[i];

                // Point-in-Polygon check (Crossing Number)
                if (((b.y > p.y) != (a.y > p.y)) && (p.x < (a.x - b.x) * (p.y - b.y) / (a.y - b.y) + b.x)) {
                    inside = !inside;
                }

                // Calcul de la distance exacte avec le bord le plus proche
                Vector2 ab = b - a;
                float len_sq = ab.length_squared();
                float t = len_sq > 0.0001f ? CLAMP((p - a).dot(ab) / len_sq, 0.0f, 1.0f) : 0.0f;
                Vector2 proj = a + ab * t;
                float d_sq = p.distance_squared_to(proj);
                if (d_sq < min_dist_sq) min_dist_sq = d_sq;
            }

            float dist = inside ? 0.0f : Math::sqrt(min_dist_sq);

            if (dist <= falloff) {
                float current_height = 0.0f;
                if (storage->has_method("get_height")) current_height = storage->call("get_height", pixel_pos);
                else continue;

                float final_height = current_height;

                if (dist <= 0.01f) {
                    final_height = target_y; // Totalement sous le bitume
                } else {
                    float falloff_t = dist / falloff;
                    float smooth_t = falloff_t * falloff_t * (3.0f - 2.0f * falloff_t);
                    final_height = Math::lerp(target_y, current_height, smooth_t); // Talus lissé
                }

                if (Math::abs(final_height - current_height) > 0.01f) {
                    if (storage->has_method("set_height")) storage->call("set_height", pixel_pos, final_height);
                    else if (storage->has_method("set_pixel")) storage->call("set_pixel", 0, pixel_pos, Color(final_height, 0, 0, 1));
                    modified = true;
                }
            }
        }
    }

    if (modified) {
        if (storage->has_method("update_maps")) storage->call("update_maps", 0);
        else if (storage->has_method("force_update_maps")) storage->call("force_update_maps", 0);
    }
}