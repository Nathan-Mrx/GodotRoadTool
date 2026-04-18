#include "ProceduralBifurcation.h"
#include "ProceduralRoad.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/curve3d.hpp>

using namespace godot;

void ProceduralBifurcation::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_main_road_path", "path"), &ProceduralBifurcation::SetMainRoadPath);
    ClassDB::bind_method(D_METHOD("get_main_road_path"), &ProceduralBifurcation::GetMainRoadPath);
    ClassDB::bind_method(D_METHOD("set_exit_road_path", "path"), &ProceduralBifurcation::SetExitRoadPath);
    ClassDB::bind_method(D_METHOD("get_exit_road_path"), &ProceduralBifurcation::GetExitRoadPath);
    ClassDB::bind_method(D_METHOD("set_splitting_offset", "offset"), &ProceduralBifurcation::SetSplittingOffset);
    ClassDB::bind_method(D_METHOD("get_splitting_offset"), &ProceduralBifurcation::GetSplittingOffset);
    ClassDB::bind_method(D_METHOD("set_chevron_spacing", "spacing"), &ProceduralBifurcation::SetChevronSpacing);
    ClassDB::bind_method(D_METHOD("get_chevron_spacing"), &ProceduralBifurcation::GetChevronSpacing);
    ClassDB::bind_method(D_METHOD("set_attenuator_scene", "scene"), &ProceduralBifurcation::SetAttenuatorScene);
    ClassDB::bind_method(D_METHOD("get_attenuator_scene"), &ProceduralBifurcation::GetAttenuatorScene);
    ClassDB::bind_method(D_METHOD("rebuild_bifurcation"), &ProceduralBifurcation::RebuildBifurcation);
    ClassDB::bind_method(D_METHOD("auto_detect_offset"), &ProceduralBifurcation::AutoDetectOffset);
    ClassDB::bind_method(D_METHOD("set_bifurcation_side", "side"), &ProceduralBifurcation::SetBifurcationSide);
    ClassDB::bind_method(D_METHOD("get_bifurcation_side"), &ProceduralBifurcation::GetBifurcationSide);
    ClassDB::bind_method(D_METHOD("set_bifurcation_type", "type"), &ProceduralBifurcation::SetBifurcationType);
    ClassDB::bind_method(D_METHOD("get_bifurcation_type"), &ProceduralBifurcation::GetBifurcationType);
    ClassDB::bind_method(D_METHOD("set_transition_length", "len"), &ProceduralBifurcation::SetTransitionLength);
    ClassDB::bind_method(D_METHOD("get_transition_length"), &ProceduralBifurcation::GetTransitionLength);
    ClassDB::bind_method(D_METHOD("set_solid_line_length", "len"), &ProceduralBifurcation::SetSolidLineLength);
    ClassDB::bind_method(D_METHOD("get_solid_line_length"), &ProceduralBifurcation::GetSolidLineLength);
    ClassDB::bind_method(D_METHOD("set_taper_length", "len"), &ProceduralBifurcation::SetTaperLength);
    ClassDB::bind_method(D_METHOD("get_taper_length"), &ProceduralBifurcation::GetTaperLength);

    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "main_road_path"), "set_main_road_path", "get_main_road_path");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "exit_road_path"), "set_exit_road_path", "get_exit_road_path");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "splitting_offset", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "set_splitting_offset", "get_splitting_offset");

    ADD_GROUP("Gore Area", "chevron_");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chevron_spacing"), "set_chevron_spacing", "get_chevron_spacing");

    ADD_GROUP("Props", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "attenuator_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_attenuator_scene", "get_attenuator_scene");

    ADD_GROUP("Routing Logic", "");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bifurcation_side", PROPERTY_HINT_ENUM, "Left:-1,Right:1"), "set_bifurcation_side", "get_bifurcation_side");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "bifurcation_type", PROPERTY_HINT_ENUM, "Exit,Entrance"), "set_bifurcation_type", "get_bifurcation_type");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "transition_length"), "set_transition_length", "get_transition_length");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "solid_line_length"), "set_solid_line_length", "get_solid_line_length");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "taper_length"), "set_taper_length", "get_taper_length");
}

ProceduralBifurcation::ProceduralBifurcation() {}
ProceduralBifurcation::~ProceduralBifurcation() {}

void ProceduralBifurcation::_notification(int p_what) {
    if (p_what == NOTIFICATION_READY) {
        RebuildBifurcation();
    }

    // Nettoyage propre quand on supprime le node
    if (p_what == NOTIFICATION_PREDELETE) {
        ProceduralRoad* main_road = Object::cast_to<ProceduralRoad>(get_node_or_null(MainRoadPath));
        if (main_road) main_road->UnregisterWidthModifier(get_name());
    }
}

void ProceduralBifurcation::AutoDetectOffset() {
    if (!is_inside_tree() || MainRoadPath.is_empty() || ExitRoadPath.is_empty()) return;
    ProceduralRoad* main_road = Object::cast_to<ProceduralRoad>(get_node_or_null(MainRoadPath));
    ProceduralRoad* exit_road = Object::cast_to<ProceduralRoad>(get_node_or_null(ExitRoadPath));
    if (!main_road || !exit_road || main_road->get_curve().is_null() || exit_road->get_curve().is_null()) return;
    Vector3 exit_start_global = exit_road->get_global_transform().xform(exit_road->get_curve()->get_point_position(0));
    Vector3 local_to_main = main_road->get_global_transform().affine_inverse().xform(exit_start_global);
    SplittingOffset = main_road->get_curve()->get_closest_offset(local_to_main);
}

void ProceduralBifurcation::SetMainRoadPath(NodePath p_path) { MainRoadPath = p_path; AutoDetectOffset(); RebuildBifurcation(); }
NodePath ProceduralBifurcation::GetMainRoadPath() const { return MainRoadPath; }
void ProceduralBifurcation::SetExitRoadPath(NodePath p_path) { ExitRoadPath = p_path; AutoDetectOffset(); RebuildBifurcation(); }
NodePath ProceduralBifurcation::GetExitRoadPath() const { return ExitRoadPath; }
void ProceduralBifurcation::SetSplittingOffset(float p_offset) { SplittingOffset = p_offset; RebuildBifurcation(); }
float ProceduralBifurcation::GetSplittingOffset() const { return SplittingOffset; }
void ProceduralBifurcation::SetChevronSpacing(float p_spacing) { ChevronSpacing = p_spacing; RebuildBifurcation(); }
float ProceduralBifurcation::GetChevronSpacing() const { return ChevronSpacing; }
void ProceduralBifurcation::SetAttenuatorScene(const Ref<PackedScene>& p_scene) { AttenuatorScene = p_scene; RebuildBifurcation(); }
Ref<PackedScene> ProceduralBifurcation::GetAttenuatorScene() const { return AttenuatorScene; }
void ProceduralBifurcation::SetBifurcationSide(int p_side) { BifurcationSide = p_side; RebuildBifurcation(); }
int ProceduralBifurcation::GetBifurcationSide() const { return BifurcationSide; }
void ProceduralBifurcation::SetBifurcationType(int p_type) { BifurcationType = p_type; RebuildBifurcation(); }
int ProceduralBifurcation::GetBifurcationType() const { return BifurcationType; }
void ProceduralBifurcation::SetTransitionLength(float p_length) { TransitionLength = p_length; RebuildBifurcation(); }
float ProceduralBifurcation::GetTransitionLength() const { return TransitionLength; }
void ProceduralBifurcation::SetSolidLineLength(float p_length) { SolidLineLength = p_length; RebuildBifurcation(); }
float ProceduralBifurcation::GetSolidLineLength() const { return SolidLineLength; }
void ProceduralBifurcation::SetTaperLength(float p_length) {
    TaperLength = p_length;
    RebuildBifurcation();
}
float ProceduralBifurcation::GetTaperLength() const { return TaperLength; }

void ProceduralBifurcation::RebuildBifurcation() {
    if (!is_inside_tree() || MainRoadPath.is_empty() || ExitRoadPath.is_empty()) return;
    ProceduralRoad* main_road = Object::cast_to<ProceduralRoad>(get_node_or_null(MainRoadPath));
    ProceduralRoad* exit_road = Object::cast_to<ProceduralRoad>(get_node_or_null(ExitRoadPath));
    if (!main_road || !exit_road) return;
    Ref<Curve3D> main_curve = main_road->get_curve();
    Ref<Curve3D> exit_curve = exit_road->get_curve();
    if (main_curve.is_null() || exit_curve.is_null()) return;

    Vector3 up(0, 1, 0);
    float hw_main = main_road->GetTotalRoadWidth() * 0.5f;
    float hw_exit = exit_road->GetTotalRoadWidth() * 0.5f;
    float rt = main_road->GetRoadThickness();
    float side_sign = (float)BifurcationSide;

    // 1. Injection de la voie (Taille exacte sans les épaules)
    float added_width = exit_road->GetTotalRoadWidth() - (2.0f * exit_road->GetShoulderWidth());
    float start_off = (BifurcationType == 0) ? SplittingOffset - TransitionLength : SplittingOffset;
    float end_off = (BifurcationType == 0) ? SplittingOffset : SplittingOffset + TransitionLength;
    float solid_ratio = CLAMP(SolidLineLength / MAX(0.1f, TransitionLength), 0.0f, 1.0f);

    // On passe le TaperLength en dernier paramètre
    main_road->RegisterWidthModifier(get_name(), start_off, end_off, MAX(0.0f, added_width), BifurcationSide, BifurcationType, solid_ratio, TaperLength);

    // 2. Snapping "Line-to-Line" SANS HACK
    Transform3D split_t = main_curve->sample_baked_with_rotation(SplittingOffset, true, true);
    Vector3 fwd_main_local = -split_t.basis.get_column(2).normalized();
    Vector3 right_main_local = fwd_main_local.cross(up).normalized();

    float line_offset_main = (hw_main - main_road->GetShoulderWidth()) * side_sign;
    Vector3 line_pos_local_main = split_t.origin + (right_main_local * line_offset_main);
    Vector3 split_global_line = main_road->get_global_transform().xform(line_pos_local_main);

    float line_offset_exit = -(hw_exit - exit_road->GetShoulderWidth()) * side_sign;
    Vector3 split_global_center = split_global_line - main_road->get_global_transform().basis.xform(right_main_local * line_offset_exit);

    Transform3D bif_t = get_global_transform();
    bif_t.origin = split_global_center;
    set_global_transform(bif_t);

    Vector3 movement_vector = split_global_center - exit_road->get_global_transform().xform(exit_curve->get_point_position(0));
    Transform3D exit_t = exit_road->get_global_transform();
    exit_t.origin += movement_vector;
    exit_road->set_global_transform(exit_t);

    Vector3 main_fwd_global = main_road->get_global_transform().basis.xform(fwd_main_local).normalized();
    Vector3 exit_fwd_local = exit_road->get_global_transform().basis.inverse().xform(main_fwd_global);
    exit_curve->set_point_out(0, exit_fwd_local * 5.0f);
    exit_road->RebuildRoad();

    // 3. Génération du Mesh (Asphalte + Lignes + Zébras)
    if (!GoreMeshInstance) { GoreMeshInstance = memnew(MeshInstance3D); add_child(GoreMeshInstance); }
    Ref<SurfaceTool> st_asphalt; st_asphalt.instantiate(); st_asphalt->begin(Mesh::PRIMITIVE_TRIANGLES);
    Ref<SurfaceTool> st_lines; st_lines.instantiate(); st_lines->begin(Mesh::PRIMITIVE_TRIANGLES);
    Transform3D my_inv = get_global_transform().affine_inverse();
    float line_y = rt * 0.5f + 0.02f;
    float line_w = 0.2f;
    Vector2 uv_scale = main_road->GetUvScale();
    bool flip = (side_sign < 0.0f);

    auto add_thick_line = [&](Vector3 start, Vector3 end, float width) {
        if (start.distance_squared_to(end) < 0.0001f) return;
        Vector3 dir = (end - start).normalized();
        Vector3 r = dir.cross(up).normalized() * (width * 0.5f);
        if (flip) {
            st_lines->set_uv(Vector2(0,0)); st_lines->add_vertex(start + r);
            st_lines->set_uv(Vector2(1,0)); st_lines->add_vertex(start - r);
            st_lines->set_uv(Vector2(1,1)); st_lines->add_vertex(end - r);
            st_lines->set_uv(Vector2(0,0)); st_lines->add_vertex(start + r);
            st_lines->set_uv(Vector2(1,1)); st_lines->add_vertex(end - r);
            st_lines->set_uv(Vector2(0,1)); st_lines->add_vertex(end + r);
        } else {
            st_lines->set_uv(Vector2(0,0)); st_lines->add_vertex(start - r);
            st_lines->set_uv(Vector2(1,0)); st_lines->add_vertex(start + r);
            st_lines->set_uv(Vector2(1,1)); st_lines->add_vertex(end + r);
            st_lines->set_uv(Vector2(0,0)); st_lines->add_vertex(start - r);
            st_lines->set_uv(Vector2(1,1)); st_lines->add_vertex(end + r);
            st_lines->set_uv(Vector2(0,1)); st_lines->add_vertex(end - r);
        }
    };

    int res = 24;
    Vector3 prev_m, prev_e, prev_loc_m, prev_loc_e;
    float prev_v_uv = 0.0f;
    for (int i = 0; i <= res; ++i) {
        float d = ((float)i / res) * GoreLength;
        Transform3D tm = main_curve->sample_baked_with_rotation(SplittingOffset + d, true, true);
        Transform3D te = exit_curve->sample_baked_with_rotation(d, true, true);
        Vector3 rm = (-main_road->get_global_transform().basis.xform(tm.basis.get_column(2)).normalized()).cross(up).normalized();
        Vector3 re = (-exit_road->get_global_transform().basis.xform(te.basis.get_column(2)).normalized()).cross(up).normalized();
        Vector3 p_m = main_road->get_global_transform().xform(tm.origin) + rm * (hw_main - main_road->GetShoulderWidth()) * side_sign;
        Vector3 p_e = exit_road->get_global_transform().xform(te.origin) - re * (hw_exit - exit_road->GetShoulderWidth()) * side_sign;

        Vector3 loc_m = my_inv.xform(p_m); loc_m.y = rt * 0.5f;
        Vector3 loc_e = my_inv.xform(p_e); loc_e.y = rt * 0.5f;
        float v_uv = (SplittingOffset + d) * uv_scale.y;

        if (i > 0) {
            // Surface 0: Asphalte (Le mesh de remplissage)
            if (flip) {
                st_asphalt->set_uv(Vector2(1, prev_v_uv)); st_asphalt->add_vertex(prev_loc_e);
                st_asphalt->set_uv(Vector2(0, prev_v_uv)); st_asphalt->add_vertex(prev_loc_m);
                st_asphalt->set_uv(Vector2(0, v_uv));      st_asphalt->add_vertex(loc_m);
                st_asphalt->set_uv(Vector2(1, prev_v_uv)); st_asphalt->add_vertex(prev_loc_e);
                st_asphalt->set_uv(Vector2(0, v_uv));      st_asphalt->add_vertex(loc_m);
                st_asphalt->set_uv(Vector2(1, v_uv));      st_asphalt->add_vertex(loc_e);
            } else {
                st_asphalt->set_uv(Vector2(0, prev_v_uv)); st_asphalt->add_vertex(prev_loc_m);
                st_asphalt->set_uv(Vector2(1, prev_v_uv)); st_asphalt->add_vertex(prev_loc_e);
                st_asphalt->set_uv(Vector2(1, v_uv));      st_asphalt->add_vertex(loc_e);
                st_asphalt->set_uv(Vector2(0, prev_v_uv)); st_asphalt->add_vertex(prev_loc_m);
                st_asphalt->set_uv(Vector2(1, v_uv));      st_asphalt->add_vertex(loc_e);
                st_asphalt->set_uv(Vector2(0, v_uv));      st_asphalt->add_vertex(loc_m);
            }
            // Surface 1: Marquages
            add_thick_line(prev_m, loc_m + Vector3(0, 0.02f, 0), line_w);
            add_thick_line(prev_e, loc_e + Vector3(0, 0.02f, 0), line_w);
        }
        prev_m = loc_m + Vector3(0, 0.02f, 0); prev_e = loc_e + Vector3(0, 0.02f, 0);
        prev_loc_m = loc_m; prev_loc_e = loc_e;
        prev_v_uv = v_uv;
    }

    // ZÉBRAS
    for (float d = ChevronSpacing; d < GoreLength - ChevronSpacing; d += ChevronSpacing) {
        Transform3D tm = main_curve->sample_baked_with_rotation(SplittingOffset + d, true, true);
        Transform3D te = exit_curve->sample_baked_with_rotation(d, true, true);
        Vector3 rm = (-main_road->get_global_transform().basis.xform(tm.basis.get_column(2)).normalized()).cross(up).normalized();
        Vector3 re = (-exit_road->get_global_transform().basis.xform(te.basis.get_column(2)).normalized()).cross(up).normalized();
        Vector3 p_m = main_road->get_global_transform().xform(tm.origin) + rm * (hw_main - main_road->GetShoulderWidth()) * side_sign;
        Vector3 p_e = exit_road->get_global_transform().xform(te.origin) - re * (hw_exit - exit_road->GetShoulderWidth()) * side_sign;
        Vector3 back_dir = main_road->get_global_transform().basis.xform(tm.basis.get_column(2)).normalized();
        Vector3 tip = ((p_m + p_e) * 0.5f) + back_dir * (ChevronSpacing * 0.8f);
        add_thick_line(my_inv.xform(tip) + Vector3(0, line_y, 0), my_inv.xform(p_m) + Vector3(0, line_y, 0), ChevronWidth);
        add_thick_line(my_inv.xform(tip) + Vector3(0, line_y, 0), my_inv.xform(p_e) + Vector3(0, line_y, 0), ChevronWidth);
    }

    st_asphalt->generate_normals(); st_asphalt->generate_tangents();
    st_lines->generate_normals(); st_lines->generate_tangents();
    Ref<ArrayMesh> final_mesh = st_asphalt->commit();
    st_lines->commit(final_mesh);
    if (main_road->GetRoadMaterial().is_valid()) final_mesh->surface_set_material(0, main_road->GetRoadMaterial());
    if (main_road->GetMarkingsMaterial().is_valid()) final_mesh->surface_set_material(1, main_road->GetMarkingsMaterial());
    GoreMeshInstance->set_mesh(final_mesh);

    // 4. ATTÉNUATEUR (Le cone vert)
    if (AttenuatorInstance) { remove_child(AttenuatorInstance); AttenuatorInstance->queue_free(); AttenuatorInstance = nullptr; }
    if (AttenuatorScene.is_valid()) {
        AttenuatorInstance = Object::cast_to<Node3D>(AttenuatorScene->instantiate());
        if (AttenuatorInstance) {
            add_child(AttenuatorInstance);
            Vector3 att_pos_global = split_global_line;
            att_pos_global.y += rt * 0.5f;
            Transform3D att_t;
            att_t.basis = Basis(main_fwd_global.cross(up).normalized(), up, -main_fwd_global);
            att_t.origin = my_inv.xform(att_pos_global);
            AttenuatorInstance->set_transform(att_t);
        }
    }
}