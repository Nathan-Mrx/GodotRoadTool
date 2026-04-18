#include "ProceduralRoad.h"
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include "ProceduralIntersection.h"

using namespace godot;

struct RoadLineDef {
    float OffsetX;
    Color ColorValue;
};

void ProceduralRoad::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_road_thickness", "thickness"), &ProceduralRoad::SetRoadThickness);
    ClassDB::bind_method(D_METHOD("get_road_thickness"), &ProceduralRoad::GetRoadThickness);
    ClassDB::bind_method(D_METHOD("set_profile_resolution", "resolution"), &ProceduralRoad::SetProfileResolution);
    ClassDB::bind_method(D_METHOD("get_profile_resolution"), &ProceduralRoad::GetProfileResolution);
    ClassDB::bind_method(D_METHOD("set_uv_scale", "scale"), &ProceduralRoad::SetUvScale);
    ClassDB::bind_method(D_METHOD("get_uv_scale"), &ProceduralRoad::GetUvScale);

    ClassDB::bind_method(D_METHOD("set_lane_count", "count"), &ProceduralRoad::SetLaneCount);
    ClassDB::bind_method(D_METHOD("get_lane_count"), &ProceduralRoad::GetLaneCount);
    ClassDB::bind_method(D_METHOD("set_lane_width", "width"), &ProceduralRoad::SetLaneWidth);
    ClassDB::bind_method(D_METHOD("get_lane_width"), &ProceduralRoad::GetLaneWidth);
    ClassDB::bind_method(D_METHOD("set_shoulder_width", "width"), &ProceduralRoad::SetShoulderWidth);
    ClassDB::bind_method(D_METHOD("get_shoulder_width"), &ProceduralRoad::GetShoulderWidth);
    ClassDB::bind_method(D_METHOD("set_is_one_way", "is_one_way"), &ProceduralRoad::SetIsOneWay);
    ClassDB::bind_method(D_METHOD("get_is_one_way"), &ProceduralRoad::GetIsOneWay);
    ClassDB::bind_method(D_METHOD("set_center_line_index", "index"), &ProceduralRoad::SetCenterLineIndex);
    ClassDB::bind_method(D_METHOD("get_center_line_index"), &ProceduralRoad::GetCenterLineIndex);

    ClassDB::bind_method(D_METHOD("set_edge_line_color", "color"), &ProceduralRoad::SetEdgeLineColor);
    ClassDB::bind_method(D_METHOD("get_edge_line_color"), &ProceduralRoad::GetEdgeLineColor);
    ClassDB::bind_method(D_METHOD("set_lane_line_color", "color"), &ProceduralRoad::SetLaneLineColor);
    ClassDB::bind_method(D_METHOD("get_lane_line_color"), &ProceduralRoad::GetLaneLineColor);
    ClassDB::bind_method(D_METHOD("set_center_line_color", "color"), &ProceduralRoad::SetCenterLineColor);
    ClassDB::bind_method(D_METHOD("get_center_line_color"), &ProceduralRoad::GetCenterLineColor);

    ClassDB::bind_method(D_METHOD("set_road_material", "material"), &ProceduralRoad::SetRoadMaterial);
    ClassDB::bind_method(D_METHOD("get_road_material"), &ProceduralRoad::GetRoadMaterial);
    ClassDB::bind_method(D_METHOD("set_markings_material", "material"), &ProceduralRoad::SetMarkingsMaterial);
    ClassDB::bind_method(D_METHOD("get_markings_material"), &ProceduralRoad::GetMarkingsMaterial);

    ClassDB::bind_method(D_METHOD("set_trigger_auto_smooth", "trigger"), &ProceduralRoad::SetTriggerAutoSmooth);
    ClassDB::bind_method(D_METHOD("get_trigger_auto_smooth"), &ProceduralRoad::GetTriggerAutoSmooth);
    ClassDB::bind_method(D_METHOD("set_use_collision", "use"), &ProceduralRoad::SetUseCollision);
    ClassDB::bind_method(D_METHOD("get_use_collision"), &ProceduralRoad::GetUseCollision);

    ClassDB::bind_method(D_METHOD("_on_curve_changed"), &ProceduralRoad::OnCurveChanged);

    ClassDB::bind_method(D_METHOD("set_line_offset", "offset"), &ProceduralRoad::SetLineOffset);
    ClassDB::bind_method(D_METHOD("get_line_offset"), &ProceduralRoad::GetLineOffset);

    ClassDB::bind_method(D_METHOD("set_edge_line_type", "type"), &ProceduralRoad::SetEdgeLineType);
    ClassDB::bind_method(D_METHOD("get_edge_line_type"), &ProceduralRoad::GetEdgeLineType);
    ClassDB::bind_method(D_METHOD("set_lane_line_type", "type"), &ProceduralRoad::SetLaneLineType);
    ClassDB::bind_method(D_METHOD("get_lane_line_type"), &ProceduralRoad::GetLaneLineType);
    ClassDB::bind_method(D_METHOD("set_center_line_type", "type"), &ProceduralRoad::SetCenterLineType);
    ClassDB::bind_method(D_METHOD("get_center_line_type"), &ProceduralRoad::GetCenterLineType);
    ClassDB::bind_method(D_METHOD("set_double_line_spacing", "spacing"), &ProceduralRoad::SetDoubleLineSpacing);
    ClassDB::bind_method(D_METHOD("get_double_line_spacing"), &ProceduralRoad::GetDoubleLineSpacing);

    ClassDB::bind_method(D_METHOD("set_dash_length", "length"), &ProceduralRoad::SetDashLength);
    ClassDB::bind_method(D_METHOD("get_dash_length"), &ProceduralRoad::GetDashLength);
    ClassDB::bind_method(D_METHOD("set_dash_gap", "gap"), &ProceduralRoad::SetDashGap);
    ClassDB::bind_method(D_METHOD("get_dash_gap"), &ProceduralRoad::GetDashGap);

    ClassDB::bind_method(D_METHOD("set_chunk_length", "length"), &ProceduralRoad::SetChunkLength);
    ClassDB::bind_method(D_METHOD("get_chunk_length"), &ProceduralRoad::GetChunkLength);

    ClassDB::bind_method(D_METHOD("set_terrain_path", "path"), &ProceduralRoad::SetTerrainPath);
    ClassDB::bind_method(D_METHOD("get_terrain_path"), &ProceduralRoad::GetTerrainPath);
    ClassDB::bind_method(D_METHOD("set_terrain_falloff", "falloff"), &ProceduralRoad::SetTerrainFalloff);
    ClassDB::bind_method(D_METHOD("get_terrain_falloff"), &ProceduralRoad::GetTerrainFalloff);
    ClassDB::bind_method(D_METHOD("set_trigger_bake_terrain", "trigger"), &ProceduralRoad::SetTriggerBakeTerrain);
    ClassDB::bind_method(D_METHOD("get_trigger_bake_terrain"), &ProceduralRoad::GetTriggerBakeTerrain);
    ClassDB::bind_method(D_METHOD("set_terrain_depth_offset", "offset"), &ProceduralRoad::SetTerrainDepthOffset);
    ClassDB::bind_method(D_METHOD("get_terrain_depth_offset"), &ProceduralRoad::GetTerrainDepthOffset);

    ClassDB::bind_method(D_METHOD("set_connected_start", "path"), &ProceduralRoad::SetConnectedStart);
    ClassDB::bind_method(D_METHOD("get_connected_start"), &ProceduralRoad::GetConnectedStart);
    ClassDB::bind_method(D_METHOD("set_connected_end", "path"), &ProceduralRoad::SetConnectedEnd);
    ClassDB::bind_method(D_METHOD("get_connected_end"), &ProceduralRoad::GetConnectedEnd);

    ClassDB::bind_method(D_METHOD("set_trigger_snap_to_terrain", "trigger"), &ProceduralRoad::SetTriggerSnapToTerrain);
    ClassDB::bind_method(D_METHOD("get_trigger_snap_to_terrain"), &ProceduralRoad::GetTriggerSnapToTerrain);
    ClassDB::bind_method(D_METHOD("snap_points_to_terrain"), &ProceduralRoad::SnapPointsToTerrain);

    ADD_GROUP("Road Geometry", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "road_thickness"), "set_road_thickness", "get_road_thickness");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "profile_resolution"), "set_profile_resolution", "get_profile_resolution");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "uv_scale"), "set_uv_scale", "get_uv_scale");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "line_offset"), "set_line_offset", "get_line_offset");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "chunk_length"), "set_chunk_length", "get_chunk_length");

    ADD_GROUP("Road Architecture", "");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lane_count"), "set_lane_count", "get_lane_count");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lane_width"), "set_lane_width", "get_lane_width");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "shoulder_width"), "set_shoulder_width", "get_shoulder_width");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_one_way"), "set_is_one_way", "get_is_one_way");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "center_line_index"), "set_center_line_index", "get_center_line_index");

    ADD_GROUP("Markings Colors", "");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "edge_line_color"), "set_edge_line_color", "get_edge_line_color");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "lane_line_color"), "set_lane_line_color", "get_lane_line_color");
    ADD_PROPERTY(PropertyInfo(Variant::COLOR, "center_line_color"), "set_center_line_color", "get_center_line_color");

    ADD_GROUP("Materials", "");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "road_material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_road_material", "get_road_material");
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "markings_material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_markings_material", "get_markings_material");

    ADD_GROUP("Tools", "");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trigger_auto_smooth"), "set_trigger_auto_smooth", "get_trigger_auto_smooth");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_collision"), "set_use_collision", "get_use_collision");

    ADD_GROUP("Markings Types", "");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "edge_line_type", PROPERTY_HINT_ENUM, "Continue,Pointillée,Double Continue"), "set_edge_line_type", "get_edge_line_type");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "lane_line_type", PROPERTY_HINT_ENUM, "Continue,Pointillée,Double Continue"), "set_lane_line_type", "get_lane_line_type");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "center_line_type", PROPERTY_HINT_ENUM, "Continue,Pointillée,Double Continue"), "set_center_line_type", "get_center_line_type");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "double_line_spacing"), "set_double_line_spacing", "get_double_line_spacing");

    ADD_GROUP("Markings Dash Settings", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "dash_length"), "set_dash_length", "get_dash_length");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "dash_gap"), "set_dash_gap", "get_dash_gap");

    ADD_GROUP("Terraforming", "");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "terrain_path", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "Terrain3D"), "set_terrain_path", "get_terrain_path");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "terrain_falloff"), "set_terrain_falloff", "get_terrain_falloff");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "terrain_depth_offset"), "set_terrain_depth_offset", "get_terrain_depth_offset");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "trigger_bake_terrain"), "set_trigger_bake_terrain", "get_trigger_bake_terrain");

    ADD_GROUP("Connections (Graph)", "");
    // PROPERTY_HINT_NODE_PATH_VALID_TYPES force l'inspecteur Godot à n'accepter QUE les ProceduralIntersection !
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "connected_start", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "ProceduralIntersection"), "set_connected_start", "get_connected_start");
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "connected_end", PROPERTY_HINT_NODE_PATH_VALID_TYPES, "ProceduralIntersection"), "set_connected_end", "get_connected_end");

    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "snap_points_to_terrain_button"), "set_trigger_snap_to_terrain", "get_trigger_snap_to_terrain");

}

ProceduralRoad::ProceduralRoad() {
    // Vide. Les chunks sont gérés par UpdateChunkCount() lors du RebuildRoad.
}

ProceduralRoad::~ProceduralRoad() {}

void ProceduralRoad::_notification(int p_what) {
    if (p_what != NOTIFICATION_READY) {
        return;
    }

    Ref<Curve3D> Curve = get_curve();
    if (Curve.is_valid()) {
        Curve->set_bake_interval(0.5f);
        if (!Curve->is_connected("changed", Callable(this, "_on_curve_changed"))) {
            Curve->connect("changed", Callable(this, "_on_curve_changed"));
        }
    }

    RebuildRoad();
}

void ProceduralRoad::SetRoadThickness(float p_thickness) {
    RoadThickness = p_thickness;
    RebuildRoad();
}

float ProceduralRoad::GetRoadThickness() const {
    return RoadThickness;
}

void ProceduralRoad::SetProfileResolution(int p_resolution) {
    ProfileResolution = Math::max(1, p_resolution);
    RebuildRoad();
}

int ProceduralRoad::GetProfileResolution() const {
    return ProfileResolution;
}

void ProceduralRoad::SetUvScale(Vector2 p_scale) {
    UvScale = p_scale;
    RebuildRoad();
}

Vector2 ProceduralRoad::GetUvScale() const {
    return UvScale;
}

void ProceduralRoad::SetLaneCount(int p_count) {
    LaneCount = Math::max(1, p_count);
    RebuildRoad();
}

int ProceduralRoad::GetLaneCount() const {
    return LaneCount;
}

void ProceduralRoad::SetLaneWidth(float p_width) {
    LaneWidth = p_width;
    RebuildRoad();
}

float ProceduralRoad::GetLaneWidth() const {
    return LaneWidth;
}

void ProceduralRoad::SetShoulderWidth(float p_width) {
    ShoulderWidth = p_width;
    RebuildRoad();
}

float ProceduralRoad::GetShoulderWidth() const {
    return ShoulderWidth;
}

void ProceduralRoad::SetIsOneWay(bool p_one_way) {
    IsOneWay = p_one_way;
    RebuildRoad();
}

bool ProceduralRoad::GetIsOneWay() const {
    return IsOneWay;
}

void ProceduralRoad::SetCenterLineIndex(int p_index) {
    CenterLineIndex = p_index;
    RebuildRoad();
}

int ProceduralRoad::GetCenterLineIndex() const {
    return CenterLineIndex;
}

void ProceduralRoad::SetEdgeLineColor(Color p_color) {
    EdgeLineColor = p_color;
    RebuildRoad();
}

Color ProceduralRoad::GetEdgeLineColor() const {
    return EdgeLineColor;
}

void ProceduralRoad::SetLaneLineColor(Color p_color) {
    LaneLineColor = p_color;
    RebuildRoad();
}

Color ProceduralRoad::GetLaneLineColor() const {
    return LaneLineColor;
}

void ProceduralRoad::SetCenterLineColor(Color p_color) {
    CenterLineColor = p_color;
    RebuildRoad();
}

Color ProceduralRoad::GetCenterLineColor() const {
    return CenterLineColor;
}

void ProceduralRoad::SetRoadMaterial(Ref<Material> p_material) {
    RoadMaterial = p_material;
    RebuildRoad();
}

Ref<Material> ProceduralRoad::GetRoadMaterial() const {
    return RoadMaterial;
}

void ProceduralRoad::SetMarkingsMaterial(Ref<Material> p_material) {
    MarkingsMaterial = p_material;
    RebuildRoad();
}

Ref<Material> ProceduralRoad::GetMarkingsMaterial() const {
    return MarkingsMaterial;
}

void ProceduralRoad::SetTriggerAutoSmooth(bool p_trigger) {
    if (p_trigger) {
        AutoSmoothCurve();
    }
}

bool ProceduralRoad::GetTriggerAutoSmooth() const {
    return false;
}

void ProceduralRoad::SetUseCollision(bool p_use) {
    UseCollision = p_use;
    RebuildRoad();
}

bool ProceduralRoad::GetUseCollision() const {
    return UseCollision;
}

void ProceduralRoad::OnCurveChanged() {
    RebuildRoad();
}

float ProceduralRoad::GetTotalRoadWidth() const {
    return (LaneCount * LaneWidth) + (ShoulderWidth * 2.0f);
}

void ProceduralRoad::SetLineOffset(float p_offset) {
    LineOffset = p_offset;
    RebuildRoad();
}
float ProceduralRoad::GetLineOffset() const {
    return LineOffset;
}

void ProceduralRoad::SetEdgeLineType(int p_type) {
    EdgeLineType = p_type;
    RebuildRoad();
}

int ProceduralRoad::GetEdgeLineType() const {
    return EdgeLineType;
}

void ProceduralRoad::SetLaneLineType(int p_type) {
    LaneLineType = p_type;
    RebuildRoad();
}

int ProceduralRoad::GetLaneLineType() const {
    return LaneLineType;
}

void ProceduralRoad::SetCenterLineType(int p_type) {
    CenterLineType = p_type;
    RebuildRoad();
}

int ProceduralRoad::GetCenterLineType() const {
    return CenterLineType;
}

void ProceduralRoad::SetDoubleLineSpacing(float p_spacing) {
    DoubleLineSpacing = p_spacing;
    RebuildRoad();
}

float ProceduralRoad::GetDoubleLineSpacing() const {
    return DoubleLineSpacing;
}

void ProceduralRoad::SetDashLength(float p_length) { DashLength = p_length; RebuildRoad(); }
float ProceduralRoad::GetDashLength() const { return DashLength; }
void ProceduralRoad::SetDashGap(float p_gap) { DashGap = p_gap; RebuildRoad(); }
float ProceduralRoad::GetDashGap() const { return DashGap; }

void ProceduralRoad::SetTerrainPath(NodePath p_path) { TerrainPath = p_path; }
NodePath ProceduralRoad::GetTerrainPath() const { return TerrainPath; }
void ProceduralRoad::SetTerrainFalloff(float p_falloff) { TerrainFalloff = p_falloff; }
float ProceduralRoad::GetTerrainFalloff() const { return TerrainFalloff; }

void ProceduralRoad::SetTriggerBakeTerrain(bool p_trigger) {
    if (p_trigger) {
        BakeTerrain();
    }
}
bool ProceduralRoad::GetTriggerBakeTerrain() const { return false; }

void ProceduralRoad::SetTerrainDepthOffset(float p_offset) { TerrainDepthOffset = p_offset; }
float ProceduralRoad::GetTerrainDepthOffset() const { return TerrainDepthOffset; }

void ProceduralRoad::SetConnectedStart(NodePath p_path) {
    ConnectedStart = p_path;
    RebuildRoad(); // On met à jour si la connexion change
}
NodePath ProceduralRoad::GetConnectedStart() const { return ConnectedStart; }

void ProceduralRoad::SetConnectedEnd(NodePath p_path) {
    ConnectedEnd = p_path;
    RebuildRoad();
}
NodePath ProceduralRoad::GetConnectedEnd() const { return ConnectedEnd; }

void ProceduralRoad::SetTriggerSnapToTerrain(bool p_trigger) {
    TriggerSnapToTerrain = p_trigger;
    if (p_trigger) {
        SnapPointsToTerrain();
        TriggerSnapToTerrain = false;
    }
}

bool ProceduralRoad::GetTriggerSnapToTerrain() const {
    return TriggerSnapToTerrain;
}

void ProceduralRoad::SnapPointsToTerrain() {
    if (TerrainPath.is_empty()) {
        UtilityFunctions::printerr("ProceduralRoad: Impossible de plaquer au sol. TerrainPath est vide.");
        return;
    }

    Node* terrain_node = get_node_or_null(TerrainPath);
    if (!terrain_node) return;

    Object* storage = nullptr;
    if (terrain_node->has_method("get_data")) storage = terrain_node->call("get_data").get_validated_object();
    else if (terrain_node->has_method("get_storage")) storage = terrain_node->call("get_storage").get_validated_object();

    if (!storage || !storage->has_method("get_height")) return;

    Ref<Curve3D> curve = get_curve();
    if (curve.is_null() || curve->get_point_count() == 0) return;

    Transform3D my_global = get_global_transform();
    Transform3D my_inv = my_global.affine_inverse();
    bool modified = false;

    for (int i = 0; i < curve->get_point_count(); ++i) {
        Vector3 local_p = curve->get_point_position(i);
        Vector3 global_p = my_global.xform(local_p);

        // On demande à Terrain3D la hauteur exacte à ces coordonnées (X, Z)
        Variant height_var = storage->call("get_height", global_p);

        if (height_var.get_type() == Variant::FLOAT) {
            float height = height_var;

            // Sécurité : Terrain3D renvoie parfois NaN si on est hors des limites de la map
            if (!Math::is_nan(height)) {
                global_p.y = height;
                Vector3 new_local = my_inv.xform(global_p);
                curve->set_point_position(i, new_local);
                modified = true;
            }
        }
    }

    if (modified) {
        RebuildRoad();
    }
}

void ProceduralRoad::RegisterWidthModifier(const String& p_id, float p_start, float p_end, float p_width, int p_side, int p_type, float p_solid_ratio, float p_taper_length) {
    WidthModifiers[p_id] = {p_start, p_end, p_width, p_side, p_type, p_solid_ratio, p_taper_length};
    RebuildRoad();
}

void ProceduralRoad::UnregisterWidthModifier(const String& p_id) {
    if (WidthModifiers.has(p_id)) {
        WidthModifiers.erase(p_id);
        RebuildRoad();
    }
}

float ProceduralRoad::GetExtraWidthAt(float p_offset, int p_side) const {
    float total = 0.0f;
    for (const KeyValue<String, RoadWidthModifier>& E : WidthModifiers) {
        const RoadWidthModifier& mod = E.value;
        if (mod.Side != p_side) continue;
        if (p_offset < mod.StartOffset) continue;

        // Sécurité : le biseau ne peut pas être plus grand que la zone totale
        float actual_taper = MIN(mod.TaperLength, mod.EndOffset - mod.StartOffset);
        if (actual_taper <= 0.0f) actual_taper = 0.001f;

        if (mod.Type == 0) { // Sortie
            if (p_offset <= mod.EndOffset) {
                if (p_offset < mod.StartOffset + actual_taper) {
                    // Phase d'élargissement (Biseau)
                    float t = (p_offset - mod.StartOffset) / actual_taper;
                    float smooth_t = t * t * (3.0f - 2.0f * t);
                    total += smooth_t * mod.WidthDelta;
                } else {
                    // Phase parallèle (Pleine largeur)
                    total += mod.WidthDelta;
                }
            }
        } else { // Entrée
            if (p_offset > mod.EndOffset) continue;
            if (p_offset >= mod.StartOffset) {
                if (p_offset > mod.EndOffset - actual_taper) {
                    // Phase de rétrécissement (Biseau)
                    float t = (p_offset - (mod.EndOffset - actual_taper)) / actual_taper;
                    float smooth_t = t * t * (3.0f - 2.0f * t);
                    total += (1.0f - smooth_t) * mod.WidthDelta;
                } else {
                    // Phase parallèle
                    total += mod.WidthDelta;
                }
            }
        }
    }
    return total;
}

void ProceduralRoad::AutoSmoothCurve() {
    Ref<Curve3D> CurrentCurve = get_curve();
    if (CurrentCurve.is_null() || CurrentCurve->get_point_count() < 2) {
        return;
    }

    int PointCount = CurrentCurve->get_point_count();

    bool was_connected = CurrentCurve->is_connected("changed", Callable(this, "_on_curve_changed"));
    if (was_connected) {
        CurrentCurve->disconnect("changed", Callable(this, "_on_curve_changed"));
    }

    // LE FIX : On écoute l'API native de Godot 4 pour les courbes fermées
    bool is_loop = CurrentCurve->is_closed() || (PointCount > 2 && CurrentCurve->get_point_position(0).distance_to(CurrentCurve->get_point_position(PointCount - 1)) < 0.05f);

    float Tension = 0.33f;

    for (int i = 0; i < PointCount; ++i) {
        Vector3 p_curr = CurrentCurve->get_point_position(i);
        Vector3 p_prev, p_next;

        if (is_loop) {
            // Le point précédent du départ est l'arrivée, et vice-versa
            p_prev = CurrentCurve->get_point_position((i == 0) ? PointCount - 1 : i - 1);
            p_next = CurrentCurve->get_point_position((i == PointCount - 1) ? 0 : i + 1);
        } else {
            p_prev = (i == 0) ? p_curr : CurrentCurve->get_point_position(i - 1);
            p_next = (i == PointCount - 1) ? p_curr : CurrentCurve->get_point_position(i + 1);
        }

        Vector3 Dir;
        float DistIn = p_curr.distance_to(p_prev) * Tension;
        float DistOut = p_curr.distance_to(p_next) * Tension;

        if (!is_loop && i == 0) {
            Dir = (p_next - p_curr).normalized();
            CurrentCurve->set_point_in(i, Vector3(0, 0, 0));
            CurrentCurve->set_point_out(i, Dir * DistOut);
        } else if (!is_loop && i == PointCount - 1) {
            Dir = (p_curr - p_prev).normalized();
            CurrentCurve->set_point_in(i, -Dir * DistIn);
            CurrentCurve->set_point_out(i, Vector3(0, 0, 0));
        } else {
            Vector3 v_in = (p_curr - p_prev).normalized();
            Vector3 v_out = (p_next - p_curr).normalized();

            Dir = (v_in + v_out).normalized();

            if (Dir.length_squared() < 0.001f) {
                Dir = v_in.cross(Vector3(0, 1, 0)).normalized();
                if (Dir.length_squared() < 0.001f) {
                    Dir = Vector3(0, 0, -1);
                }
            }

            CurrentCurve->set_point_in(i, -Dir * DistIn);
            CurrentCurve->set_point_out(i, Dir * DistOut);
        }
    }

    if (was_connected) {
        CurrentCurve->connect("changed", Callable(this, "_on_curve_changed"));
    }

    CurrentCurve->emit_signal("changed");
    RebuildRoad();
}

Vector<ProfileVertex> ProceduralRoad::BuildCrossSectionProfile() const {
    Vector<ProfileVertex> Profile;
    const float Radius = RoadThickness * 0.5f;

    for (int i = 0; i <= ProfileResolution; ++i) {
        float Angle = Math_PI * 0.5f - (Math_PI * float(i) / float(ProfileResolution));
        ProfileVertex Vertex;
        Vertex.Normal = Vector2(Math::cos(Angle), Math::sin(Angle));
        Vertex.Position = Vertex.Normal * Radius; // Uniquement le rayon
        Profile.push_back(Vertex);
    }
    for (int i = 0; i <= ProfileResolution; ++i) {
        float Angle = -Math_PI * 0.5f - (Math_PI * float(i) / float(ProfileResolution));
        ProfileVertex Vertex;
        Vertex.Normal = Vector2(Math::cos(Angle), Math::sin(Angle));
        Vertex.Position = Vertex.Normal * Radius;
        Profile.push_back(Vertex);
    }
    return Profile;
}

void ProceduralRoad::RebuildRoad() {
    Ref<Curve3D> CurrentCurve = get_curve();

    if (CurrentCurve != LastCurve) {
        if (LastCurve.is_valid() && LastCurve->is_connected("changed", Callable(this, "_on_curve_changed"))) {
            LastCurve->disconnect("changed", Callable(this, "_on_curve_changed"));
        }
        LastCurve = CurrentCurve;
        if (LastCurve.is_valid()) {
            if (!LastCurve->is_connected("changed", Callable(this, "_on_curve_changed"))) {
                LastCurve->connect("changed", Callable(this, "_on_curve_changed"));
            }
        }
    }

    if (CurrentCurve.is_null() || CurrentCurve->get_baked_points().size() < 2) {
        UpdateChunkCount(0);
        return;
    }

    // ====================================================================
    // --- LE CERVEAU : SYSTÈME D'AIMANTATION (GRAPH SNAPPING) ---
    // ====================================================================
    bool curve_modified = false;
    CurrentCurve->set_block_signals(true);

    Transform3D road_global_trans = get_global_transform();
    Transform3D road_global_inv = road_global_trans.affine_inverse();
    const float TangentForce = 10.0f;

    if (!ConnectedStart.is_empty()) {
        ProceduralIntersection* inter_start = Object::cast_to<ProceduralIntersection>(get_node_or_null(ConnectedStart));
        if (inter_start) {
            Vector3 ref_global_pos = road_global_trans.xform(CurrentCurve->get_point_position(1));
            Transform3D target_trans = inter_start->GetConnectionTransform(ref_global_pos);
            Transform3D local_target = road_global_inv * target_trans;

            CurrentCurve->set_point_position(0, local_target.origin);
            // La tangente pointe vers l'extérieur de l'intersection (direction de la route)
            CurrentCurve->set_point_out(0, -local_target.basis.get_column(2).normalized() * TangentForce);
            CurrentCurve->set_point_in(0, Vector3());
            CurrentCurve->set_point_tilt(0, 0.0f);
            curve_modified = true;
        }
    }

    if (!ConnectedEnd.is_empty()) {
        int last_idx = CurrentCurve->get_point_count() - 1;
        ProceduralIntersection* inter_end = Object::cast_to<ProceduralIntersection>(get_node_or_null(ConnectedEnd));
        if (inter_end) {
            Vector3 ref_global_pos = road_global_trans.xform(CurrentCurve->get_point_position(last_idx - 1));
            Transform3D target_trans = inter_end->GetConnectionTransform(ref_global_pos);
            Transform3D local_target = road_global_inv * target_trans;

            CurrentCurve->set_point_position(last_idx, local_target.origin);
            // La tangente pointe vers l'extérieur de l'intersection (vers le point précédent de la route)
            CurrentCurve->set_point_in(last_idx, -local_target.basis.get_column(2).normalized() * TangentForce);
            CurrentCurve->set_point_out(last_idx, Vector3());
            CurrentCurve->set_point_tilt(last_idx, 0.0f);
            curve_modified = true;
        }
    }

    CurrentCurve->set_block_signals(false);
    // ====================================================================

    const float TotalWidth = GetTotalRoadWidth();
    const float StartX = -TotalWidth * 0.5f + ShoulderWidth;
    Vector<RibbonDef> Ribbons;

    auto add_ribbons = [&](int anchor_type, float relative_offset, Color base_color, int line_type) {
        float alpha = (line_type == 1) ? 0.0f : 1.0f; // 1 = Pointillé
        Color final_color = Color(base_color.r, base_color.g, base_color.b, alpha);
        if (line_type == 2) { // Double Continue
            Ribbons.push_back({anchor_type, relative_offset - (DoubleLineSpacing * 0.5f), final_color});
            Ribbons.push_back({anchor_type, relative_offset + (DoubleLineSpacing * 0.5f), final_color});
        } else {
            Ribbons.push_back({anchor_type, relative_offset, final_color});
        }
    };

    add_ribbons(0, 0.0f, EdgeLineColor, EdgeLineType); // Bord Gauche
    add_ribbons(1, 0.0f, EdgeLineColor, EdgeLineType); // Bord Droit

    for (int Lane = 1; Lane < LaneCount; ++Lane) {
        float Offset = StartX + (Lane * LaneWidth);
        bool IsCenter = (!IsOneWay && Lane == CenterLineIndex);
        add_ribbons(2, Offset, IsCenter ? CenterLineColor : LaneLineColor, IsCenter ? CenterLineType : LaneLineType);
    }

    add_ribbons(3, 0.0f, EdgeLineColor, 1); // Ligne de dissuasion (Sortie Gauche)
    add_ribbons(4, 0.0f, EdgeLineColor, 1); // Ligne de dissuasion (Sortie Droite)

    // ====================================================================
    // --- 1. ÉCHANTILLONNAGE HAUTE PRÉCISION (AVEC CASSURES) ---
    // ====================================================================
    float TotalLength = CurrentCurve->get_baked_length();
    float SamplingStep = 1.0f;

    Vector<float> Offsets;
    for (float d = 0.0f; d < TotalLength; d += SamplingStep) {
        Offsets.push_back(d);
    }
    Offsets.push_back(TotalLength);

    // INJECTION : On force le générateur à créer des vertices pile sur les limites
    for (const KeyValue<String, RoadWidthModifier>& E : WidthModifiers) {
        if (E.value.Type == 0) { // Sortie
            float split = E.value.EndOffset;
            if (split > 0.0f && split < TotalLength) {
                Offsets.push_back(split);
                Offsets.push_back(split + 0.001f); // La brisure parfaite : +1 millimètre
            }
        } else { // Entrée
            float split = E.value.StartOffset;
            if (split > 0.0f && split < TotalLength) {
                Offsets.push_back(split - 0.001f); // La brisure
                Offsets.push_back(split);
            }
        }
    }

    Offsets.sort();

    // On retire les doublons liés au SamplingStep, tout en gardant nos brisures
    Vector<float> FinalOffsets;
    for (int i = 0; i < Offsets.size(); ++i) {
        if (i == 0 || (Offsets[i] - FinalOffsets[FinalOffsets.size() - 1]) >= 0.0005f) {
            FinalOffsets.push_back(Offsets[i]);
        }
    }

    int PointCount = FinalOffsets.size();
    PackedVector3Array Points; Points.resize(PointCount);
    PackedVector3Array UpVectors; UpVectors.resize(PointCount);

    for (int i = 0; i < PointCount; ++i) {
        float offset = MIN(FinalOffsets[i], TotalLength);
        Transform3D t = CurrentCurve->sample_baked_with_rotation(offset, true, true);
        Points.set(i, t.origin);
        UpVectors.set(i, t.basis.get_column(1).normalized());
    }

    bool is_loop = (PointCount > 2) && (Points[0].distance_to(Points[PointCount - 1]) < 0.05f);
    if (is_loop) {
        Points.set(PointCount - 1, Points[0]);
        Vector3 seam_up = (UpVectors[0] + UpVectors[PointCount - 1]).normalized();
        UpVectors.set(0, seam_up);
        UpVectors.set(PointCount - 1, seam_up);
    }

    // ====================================================================
    // --- 2. CALCUL MANUEL DES TANGENTES (FORWARDS) ---
    // ====================================================================
    PackedVector3Array Forwards; Forwards.resize(PointCount);

    for (int i = 0; i < PointCount; ++i) {
        Vector3 forward;
        if (i == 0) {
            forward = is_loop ? (Points[1] - Points[PointCount - 2]) : (Points[1] - Points[0]);
        } else if (i == PointCount - 1) {
            forward = is_loop ? (Points[1] - Points[PointCount - 2]) : (Points[i] - Points[i - 1]);
        } else {
            forward = (Points[i + 1] - Points[i - 1]);
        }

        if (forward.length_squared() < 0.0001f) {
            forward = Vector3(0, 0, -1);
        }

        Forwards.set(i, forward.normalized());
    }

    Vector<ProfileVertex> Profile = BuildCrossSectionProfile();

    // ====================================================================
    // --- 3. CHUNKING ---
    // ====================================================================
    struct ChunkRange {
        int StartIdx;
        int EndIdx;
        float StartDistance;
    };
    Vector<ChunkRange> Ranges;

    float AccumulatedDistance = 0.0f;
    float CurrentChunkStartDist = 0.0f;
    int CurrentStartIdx = 0;

    for (int i = 0; i < PointCount - 1; ++i) {
        float DistToNext = Points[i].distance_to(Points[i + 1]);
        AccumulatedDistance += DistToNext;

        if (AccumulatedDistance - CurrentChunkStartDist >= ChunkLength) {
            Ranges.push_back({CurrentStartIdx, i + 1, CurrentChunkStartDist});
            CurrentStartIdx = i + 1;
            CurrentChunkStartDist = AccumulatedDistance;
        }
    }

    if (CurrentStartIdx < PointCount - 1) {
        Ranges.push_back({CurrentStartIdx, PointCount - 1, CurrentChunkStartDist});
    }

    UpdateChunkCount(Ranges.size());

    for (int c = 0; c < Ranges.size(); ++c) {
        GenerateChunkMesh(
            c, Ranges[c].StartIdx, Ranges[c].EndIdx,
            Points, UpVectors, Forwards, Ranges[c].StartDistance, Profile, Ribbons
        );
    }

    // On notifie les intersections de reconstruire leur géométrie centrale
    if (!ConnectedStart.is_empty() && is_inside_tree()) {
        ProceduralIntersection* inter = Object::cast_to<ProceduralIntersection>(get_node_or_null(ConnectedStart));
        if (inter) inter->RebuildIntersection();
    }
    if (!ConnectedEnd.is_empty() && is_inside_tree()) {
        ProceduralIntersection* inter = Object::cast_to<ProceduralIntersection>(get_node_or_null(ConnectedEnd));
        if (inter) inter->RebuildIntersection();
    }

    update_gizmos(); // Force Godot à redessiner notre wireframe
}

void ProceduralRoad::SetChunkLength(float p_length) {
    ChunkLength = Math::max(10.0f, p_length); // Sécurité : pas de chunks de 1 mètre
    RebuildRoad();
}

float ProceduralRoad::GetChunkLength() const { return ChunkLength; }

void ProceduralRoad::UpdateChunkCount(int p_target_count) {
    // 1. Détruire les chunks en trop
    while (Chunks.size() > p_target_count) {
        RoadChunk Chunk = Chunks[Chunks.size() - 1];
        if (Chunk.MeshInst) Chunk.MeshInst->queue_free();
        if (Chunk.StaticBody) Chunk.StaticBody->queue_free();
        Chunks.remove_at(Chunks.size() - 1);
    }

    // 2. Créer les chunks manquants (SANS set_owner)
    while (Chunks.size() < p_target_count) {
        RoadChunk NewChunk;

        NewChunk.MeshInst = memnew(MeshInstance3D);
        add_child(NewChunk.MeshInst);

        NewChunk.StaticBody = memnew(StaticBody3D);
        add_child(NewChunk.StaticBody);

        NewChunk.CollisionShape = memnew(CollisionShape3D);
        NewChunk.StaticBody->add_child(NewChunk.CollisionShape);
        NewChunk.CollisionShape->set_visible(false);

        Chunks.push_back(NewChunk);
    }
}

void ProceduralRoad::GenerateChunkMesh(int p_chunk_index, int p_start_idx, int p_end_idx, const PackedVector3Array& p_points, const PackedVector3Array& p_up_vectors, const PackedVector3Array& p_forwards, float p_start_distance, const Vector<ProfileVertex>& p_profile, const Vector<RibbonDef>& p_ribbons) {

    RoadChunk& Chunk = Chunks.ptrw()[p_chunk_index];
    const int ProfileCount = p_profile.size();
    const int RibbonCountTotal = p_ribbons.size();
    const int LocalPointCount = p_end_idx - p_start_idx + 1;

    const Vector3 ChunkOrigin = p_points[p_start_idx];
    Chunk.MeshInst->set_position(ChunkOrigin);
    Chunk.StaticBody->set_position(ChunkOrigin);

    Ref<SurfaceTool> RoadSurface;
    RoadSurface.instantiate();
    RoadSurface->begin(Mesh::PRIMITIVE_TRIANGLES);

    Ref<SurfaceTool> LineSurface;
    LineSurface.instantiate();
    LineSurface->begin(Mesh::PRIMITIVE_TRIANGLES);

    auto get_ribbon_offset = [&](const RibbonDef& ribbon, float hw_l, float hw_r) -> float {
        float AnchorPos = 0.0f;
        switch(ribbon.AnchorType) {
            case 0: AnchorPos = -hw_l + ShoulderWidth; break;
            case 1: AnchorPos = hw_r - ShoulderWidth; break;
            case 2: AnchorPos = ribbon.RelativeOffset; break;
            case 3: AnchorPos = -(GetTotalRoadWidth() * 0.5f) + ShoulderWidth; break;
            case 4: AnchorPos = (GetTotalRoadWidth() * 0.5f) - ShoulderWidth; break;
        }
        float FinalOffset = AnchorPos;
        if (ribbon.AnchorType != 2) FinalOffset += ribbon.RelativeOffset;
        return FinalOffset;
    };

    float CenterDistance = 0.0f;
    Vector<float> RibbonDistances;
    RibbonDistances.resize(RibbonCountTotal);
    RibbonDistances.fill(0.0f);

    Vector3 PrevCenter = p_points[0];
    Vector<Vector3> PrevRibbonCenters;
    PrevRibbonCenters.resize(RibbonCountTotal);

    const Vector3 InitUp = p_up_vectors[0].normalized();
    const Vector3 InitForward = p_forwards[0];
    const Vector3 InitRight = InitForward.cross(InitUp).normalized();
    const Vector3 InitOrthogonalUp = InitRight.cross(InitForward).normalized();

    float init_offset = 0.0f;
    float init_hw_l = (GetTotalRoadWidth() * 0.5f) + GetExtraWidthAt(init_offset, -1);
    float init_hw_r = (GetTotalRoadWidth() * 0.5f) + GetExtraWidthAt(init_offset, 1);

    for (int k = 0; k < RibbonCountTotal; ++k) {
        float r_offset = get_ribbon_offset(p_ribbons[k], init_hw_l, init_hw_r);
        PrevRibbonCenters.write[k] = p_points[0] + (InitRight * r_offset) + (InitOrthogonalUp * (RoadThickness * 0.5f + LineOffset));
    }

    for (int i = 0; i <= p_end_idx; ++i) {
        const Vector3 CurrentPoint = p_points[i];
        const Vector3 Forward = p_forwards[i];
        const Vector3 Up = p_up_vectors[i].normalized();
        const Vector3 Right = Forward.cross(Up).normalized();
        const Vector3 OrthogonalUp = Right.cross(Forward).normalized();

        if (i > 0) CenterDistance += PrevCenter.distance_to(CurrentPoint);
        PrevCenter = CurrentPoint;

        float CurrentOffset = CenterDistance;
        float extra_l = GetExtraWidthAt(CurrentOffset, -1);
        float extra_r = GetExtraWidthAt(CurrentOffset, 1);

        float hw_l = (GetTotalRoadWidth() * 0.5f) + extra_l;
        float hw_r = (GetTotalRoadWidth() * 0.5f) + extra_r;
        float sw_l = Math::max(0.0f, hw_l - (RoadThickness * 0.5f));
        float sw_r = Math::max(0.0f, hw_r - (RoadThickness * 0.5f));

        Vector<float> CurrentRibbonOffsets;
        CurrentRibbonOffsets.resize(RibbonCountTotal);

        for (int k = 0; k < RibbonCountTotal; ++k) {
            CurrentRibbonOffsets.write[k] = get_ribbon_offset(p_ribbons[k], hw_l, hw_r);
            Vector3 RibbonCenter = CurrentPoint + (Right * CurrentRibbonOffsets[k]) + (OrthogonalUp * (RoadThickness * 0.5f + LineOffset));
            if (i > 0) RibbonDistances.write[k] += PrevRibbonCenters[k].distance_to(RibbonCenter);
            PrevRibbonCenters.write[k] = RibbonCenter;
        }

        if (i >= p_start_idx) {
            float CurrentPerimeter = 0.0f;
            for (int j = 0; j < ProfileCount; ++j) {
                const Vector2 ProfileNormal = p_profile[j].Normal;

                float side_sign = (j < (ProfileCount / 2)) ? 1.0f : -1.0f;
                float sw = (side_sign > 0.0f) ? sw_r : sw_l;

                Vector2 FinalProfilePos = Vector2(sw * side_sign, 0.0f) + p_profile[j].Position;
                Vector3 Vertex3D = (CurrentPoint + (Right * FinalProfilePos.x) + (OrthogonalUp * FinalProfilePos.y)) - ChunkOrigin;
                const Vector3 Normal3D = ((Right * ProfileNormal.x) + (OrthogonalUp * ProfileNormal.y)).normalized();

                RoadSurface->set_normal(Normal3D);
                RoadSurface->set_uv(Vector2(CurrentPerimeter * UvScale.x, CenterDistance * UvScale.y));
                RoadSurface->add_vertex(Vertex3D);

                const int NextJ = (j + 1) % ProfileCount;
                float next_side_sign = (NextJ < (ProfileCount / 2)) ? 1.0f : -1.0f;
                float next_sw = (next_side_sign > 0.0f) ? sw_r : sw_l;

                Vector2 NextProfilePos = Vector2(next_sw * next_side_sign, 0.0f) + p_profile[NextJ].Position;
                CurrentPerimeter += FinalProfilePos.distance_to(NextProfilePos);
            }

            for (int k = 0; k < RibbonCountTotal; ++k) {
                Color R_Color = p_ribbons[k].ColorValue;
                float current_line_width = LineWidth;

                // Application du contrôleur d'état sur les lignes de dissuasion
                if (p_ribbons[k].AnchorType == 3 || p_ribbons[k].AnchorType == 4) {
                    int side = (p_ribbons[k].AnchorType == 3) ? -1 : 1;
                    float d_alpha, d_width_mult;
                    GetDissuasionStateAt(CurrentOffset, side, d_alpha, d_width_mult);

                    R_Color.a = d_alpha;
                    current_line_width *= d_width_mult;
                }

                float FinalOffset = CurrentRibbonOffsets[k];

                Vector3 Center = CurrentPoint + (Right * FinalOffset) + (OrthogonalUp * (RoadThickness * 0.5f + LineOffset));

                // Si la largeur est à 0, les vertices se superposent : la ligne disparait sans trouer l'index buffer
                Vector3 LeftVertex = (Center - (Right * (current_line_width * 0.5f))) - ChunkOrigin;
                Vector3 RightVertex = (Center + (Right * (current_line_width * 0.5f))) - ChunkOrigin;

                LineSurface->set_color(R_Color);
                LineSurface->set_normal(OrthogonalUp);
                LineSurface->set_uv(Vector2(0.0f, RibbonDistances[k] * UvScale.y));
                LineSurface->add_vertex(LeftVertex);

                LineSurface->set_color(R_Color);
                LineSurface->set_normal(OrthogonalUp);
                LineSurface->set_uv(Vector2(1.0f, RibbonDistances[k] * UvScale.y));
                LineSurface->add_vertex(RightVertex);
            }
        }
    }

    for (int local_i = 0; local_i < LocalPointCount - 1; ++local_i) {
        int global_i = p_start_idx + local_i;

        // LA CASSURE 0mm : On saute la génération de triangles si on est sur la brisure
        if (p_points[global_i].distance_to(p_points[global_i + 1]) < 0.005f) {
            continue;
        }

        const int CurrentRing = local_i * ProfileCount;
        const int NextRing = (local_i + 1) * ProfileCount;

        for (int j = 0; j < ProfileCount; ++j) {
            const int NextJ = (j + 1) % ProfileCount;
            RoadSurface->add_index(CurrentRing + j);
            RoadSurface->add_index(NextRing + j);
            RoadSurface->add_index(CurrentRing + NextJ);

            RoadSurface->add_index(CurrentRing + NextJ);
            RoadSurface->add_index(NextRing + j);
            RoadSurface->add_index(NextRing + NextJ);
        }

        for (int k = 0; k < RibbonCountTotal; ++k) {
            const int CurrentLeft = (local_i * RibbonCountTotal * 2) + (k * 2);
            const int CurrentRight = CurrentLeft + 1;
            const int NextLeft = ((local_i + 1) * RibbonCountTotal * 2) + (k * 2);
            const int NextRight = NextLeft + 1;

            LineSurface->add_index(CurrentLeft);
            LineSurface->add_index(NextLeft);
            LineSurface->add_index(CurrentRight);

            LineSurface->add_index(NextLeft);
            LineSurface->add_index(NextRight);
            LineSurface->add_index(CurrentRight);
        }
    }

    RoadSurface->generate_tangents();
    LineSurface->generate_tangents();

    Ref<ArrayMesh> FinalMesh = RoadSurface->commit();
    if (RoadMaterial.is_valid()) FinalMesh->surface_set_material(0, RoadMaterial);

    if (UseCollision && FinalMesh.is_valid()) {
        Ref<Shape3D> Trimesh = FinalMesh->create_trimesh_shape();
        Chunk.CollisionShape->set_shape(Trimesh);
    } else {
        Chunk.CollisionShape->set_shape(Ref<Shape3D>());
    }

    LineSurface->commit(FinalMesh);
    if (MarkingsMaterial.is_valid()) {
        FinalMesh->surface_set_material(1, MarkingsMaterial);
        MarkingsMaterial->set("shader_parameter/dash_length", DashLength);
        MarkingsMaterial->set("shader_parameter/dash_gap", DashGap);
    }

    Chunk.MeshInst->set_mesh(FinalMesh);
}

void ProceduralRoad::BakeTerrain() {
    if (TerrainPath.is_empty()) return;

    Node* terrain_node = get_node_or_null(TerrainPath);
    if (!terrain_node) {
        UtilityFunctions::printerr("ProceduralRoad: Terrain3D node not found.");
        return;
    }

    Object* storage = nullptr;
    if (terrain_node->has_method("get_data")) {
        storage = terrain_node->call("get_data").get_validated_object();
    } else if (terrain_node->has_method("get_storage")) {
        storage = terrain_node->call("get_storage").get_validated_object();
    }

    if (!storage) {
        UtilityFunctions::printerr("ProceduralRoad: Failed to get Terrain3D Data/Storage API.");
        return;
    }

    Ref<Curve3D> CurrentCurve = get_curve();
    if (CurrentCurve.is_null() || CurrentCurve->get_point_count() < 2) return;

    const float SamplingStep = 1.0f;
    const float TotalLength = CurrentCurve->get_baked_length();
    const int PointCount = MAX(2, Math::ceil(TotalLength / SamplingStep) + 1);
    const Transform3D NodeTransform = get_global_transform();
    const float RoadHalfWidth = GetTotalRoadWidth() * 0.5f;

    const float SafetyBuffer = 1.5f;
    const float FlatZone = RoadHalfWidth + SafetyBuffer;
    const float SearchRadius = FlatZone + TerrainFalloff;

    Vector<Transform3D> GlobalTransforms;
    GlobalTransforms.resize(PointCount);

    float min_x = 1e9, max_x = -1e9, min_z = 1e9, max_z = -1e9;

    for (int i = 0; i < PointCount; ++i) {
        float offset = MIN(i * SamplingStep, TotalLength);
        Transform3D local_t = CurrentCurve->sample_baked_with_rotation(offset, true, true);
        GlobalTransforms.write[i] = NodeTransform * local_t;

        Vector3 p = GlobalTransforms[i].origin;
        if (p.x < min_x) min_x = p.x;
        if (p.x > max_x) max_x = p.x;
        if (p.z < min_z) min_z = p.z;
        if (p.z > max_z) max_z = p.z;
    }

    min_x -= SearchRadius; max_x += SearchRadius;
    min_z -= SearchRadius; max_z += SearchRadius;

    bool modified = false;

    // Approche Pixel-Centric : on itère sur la grille et on trouve le meilleur segment
    for (float x = Math::floor(min_x); x <= Math::ceil(max_x); x += 1.0f) {
        for (float z = Math::floor(min_z); z <= Math::ceil(max_z); z += 1.0f) {
            Vector3 PixelPos(x, 0, z);
            Vector2 P(x, z);

            float MinDistSq = 1e9;
            int BestSegment = -1;
            float BestT = 0.0f;

            // Trouver LE segment de route le plus proche pour ce pixel
            for (int i = 0; i < PointCount - 1; ++i) {
                Vector2 A(GlobalTransforms[i].origin.x, GlobalTransforms[i].origin.z);
                Vector2 B(GlobalTransforms[i+1].origin.x, GlobalTransforms[i+1].origin.z);
                Vector2 AB = B - A;
                float length_sq = AB.length_squared();

                float t = length_sq > 0.0001f ? (P - A).dot(AB) / length_sq : 0.0f;
                t = CLAMP(t, 0.0f, 1.0f);

                Vector2 ClosestPoint2D = A + AB * t;
                float DistSq = P.distance_squared_to(ClosestPoint2D);

                if (DistSq < MinDistSq) {
                    MinDistSq = DistSq;
                    BestSegment = i;
                    BestT = t;
                }
            }

            if (MinDistSq <= SearchRadius * SearchRadius && BestSegment != -1) {
                float Dist = Math::sqrt(MinDistSq);

                const Transform3D& T1 = GlobalTransforms[BestSegment];
                const Transform3D& T2 = GlobalTransforms[BestSegment + 1];

                Vector3 Origin = T1.origin.lerp(T2.origin, BestT);
                Vector3 Up = T1.basis.get_column(1).lerp(T2.basis.get_column(1), BestT).normalized();

                float TargetHeight = Origin.y;

                // Équation mathématique stricte d'un plan (N.P = 0)
                // Permet de trouver la hauteur Y exacte d'une surface inclinée aux coordonnées X et Z
                if (Math::abs(Up.y) > 0.0001f) {
                    TargetHeight = Origin.y - (Up.x * (x - Origin.x) + Up.z * (z - Origin.z)) / Up.y;
                }
                TargetHeight -= TerrainDepthOffset;

                float CurrentHeight = 0.0f;
                if (storage->has_method("get_height")) {
                    CurrentHeight = storage->call("get_height", PixelPos);
                } else continue;

                float FinalHeight = CurrentHeight;

                if (Dist <= FlatZone) {
                    FinalHeight = TargetHeight;
                } else {
                    float falloff_t = (Dist - FlatZone) / TerrainFalloff;
                    float smooth_t = falloff_t * falloff_t * (3.0f - 2.0f * falloff_t);
                    FinalHeight = Math::lerp(TargetHeight, CurrentHeight, smooth_t);
                }

                if (Math::abs(FinalHeight - CurrentHeight) > 0.01f) {
                    if (storage->has_method("set_height")) {
                        storage->call("set_height", PixelPos, FinalHeight);
                    } else if (storage->has_method("set_pixel")) {
                        storage->call("set_pixel", 0, PixelPos, Color(FinalHeight, 0, 0, 1));
                    }
                    modified = true;
                }
            }
        }
    }

    if (modified) {
        if (storage->has_method("update_maps")) {
            storage->call("update_maps", 0);
        } else if (storage->has_method("force_update_maps")) {
            storage->call("force_update_maps", 0);
        }
    }
}

void ProceduralRoad::GetDissuasionStateAt(float p_offset, int p_side, float& r_alpha, float& r_width_mult) const {
    r_alpha = 0.0f;
    r_width_mult = 0.0f;

    for (const KeyValue<String, RoadWidthModifier>& E : WidthModifiers) {
        const RoadWidthModifier& mod = E.value;
        if (mod.Side != p_side) continue;

        if (p_offset >= mod.StartOffset && p_offset <= mod.EndOffset) {
            float t = (p_offset - mod.StartOffset) / (mod.EndOffset - mod.StartOffset);
            r_width_mult = 1.0f;

            if (mod.Type == 0) { // Sortie
                if (t < 0.05f) r_width_mult = 0.0f;
                else if (t < 1.0f - mod.SolidLineRatio) r_alpha = 0.0f; // Pointillés au début
                else r_alpha = 1.0f;                                    // Continue à la fin
            } else { // Entrée
                if (t < mod.SolidLineRatio) r_alpha = 1.0f;             // Continue au début
                else if (t < 0.95f) r_alpha = 0.0f;                     // Pointillés à la fin
                else r_width_mult = 0.0f;
            }
            return;
        }
    }
}