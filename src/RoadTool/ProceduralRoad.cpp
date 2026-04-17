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

    // Internal callback name kept snake_case for Godot signal Callable lookup.
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

    ClassDB::bind_method(D_METHOD("set_snap_to_terrain", "snap"), &ProceduralRoad::SetSnapToTerrain);
    ClassDB::bind_method(D_METHOD("get_snap_to_terrain"), &ProceduralRoad::GetSnapToTerrain);
    ClassDB::bind_method(D_METHOD("set_terrain_collision_mask", "mask"), &ProceduralRoad::SetTerrainCollisionMask);
    ClassDB::bind_method(D_METHOD("get_terrain_collision_mask"), &ProceduralRoad::GetTerrainCollisionMask);
    ClassDB::bind_method(D_METHOD("set_terrain_offset", "offset"), &ProceduralRoad::SetTerrainOffset);
    ClassDB::bind_method(D_METHOD("get_terrain_offset"), &ProceduralRoad::GetTerrainOffset);

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

    ADD_GROUP("Terrain Snapping", "");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "snap_to_terrain"), "set_snap_to_terrain", "get_snap_to_terrain");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "terrain_collision_mask", PROPERTY_HINT_LAYERS_3D_PHYSICS), "set_terrain_collision_mask", "get_terrain_collision_mask");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "terrain_offset"), "set_terrain_offset", "get_terrain_offset");
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

void ProceduralRoad::SetSnapToTerrain(bool p_snap)
{
    SnapToTerrain = p_snap;
    RebuildRoad();
}

bool ProceduralRoad::GetSnapToTerrain() const
{
    return SnapToTerrain;
}

void ProceduralRoad::SetTerrainCollisionMask(uint32_t p_mask)
{
    TerrainCollisionMask = p_mask;
    RebuildRoad();
}

uint32_t ProceduralRoad::GetTerrainCollisionMask() const
{
    return TerrainCollisionMask;
}

void ProceduralRoad::SetTerrainOffset(float p_offset)
{
    TerrainOffset = p_offset;
    RebuildRoad();
}

float ProceduralRoad::GetTerrainOffset() const
{
    return TerrainOffset;
}

void ProceduralRoad::AutoSmoothCurve() {
    Ref<Curve3D> Curve = get_curve();
    if (Curve.is_null() || Curve->get_point_count() < 3) {
        return;
    }

    const int Count = Curve->get_point_count();
    const float Tension = 0.33f;

    for (int i = 0; i < Count; ++i) {
        if (i == 0) {
            Vector3 CurrentPoint = Curve->get_point_position(0);
            Vector3 NextPoint = Curve->get_point_position(1);
            Vector3 Direction = (NextPoint - CurrentPoint).normalized();
            Curve->set_point_in(0, Vector3(0, 0, 0));
            Curve->set_point_out(0, Direction * (CurrentPoint.distance_to(NextPoint) * Tension));
            continue;
        }

        if (i == Count - 1) {
            Vector3 PreviousPoint = Curve->get_point_position(i - 1);
            Vector3 CurrentPoint = Curve->get_point_position(i);
            Vector3 Direction = (CurrentPoint - PreviousPoint).normalized();
            Curve->set_point_in(i, -Direction * (CurrentPoint.distance_to(PreviousPoint) * Tension));
            Curve->set_point_out(i, Vector3(0, 0, 0));
            continue;
        }

        Vector3 PreviousPoint = Curve->get_point_position(i - 1);
        Vector3 CurrentPoint = Curve->get_point_position(i);
        Vector3 NextPoint = Curve->get_point_position(i + 1);
        Vector3 Direction = (NextPoint - PreviousPoint).normalized();
        Curve->set_point_in(i, -Direction * (CurrentPoint.distance_to(PreviousPoint) * Tension));
        Curve->set_point_out(i, Direction * (CurrentPoint.distance_to(NextPoint) * Tension));
    }
}

Vector<ProfileVertex> ProceduralRoad::BuildCrossSectionProfile() const {
    Vector<ProfileVertex> Profile;
    const float HalfWidth = GetTotalRoadWidth() * 0.5f;
    const float HalfThickness = RoadThickness * 0.5f;
    const float Radius = HalfThickness;
    const float StraightWidth = Math::max(0.0f, HalfWidth - Radius);

    for (int i = 0; i <= ProfileResolution; ++i) {
        float Angle = Math_PI * 0.5f - (Math_PI * float(i) / float(ProfileResolution));
        ProfileVertex Vertex;
        Vertex.Normal = Vector2(Math::cos(Angle), Math::sin(Angle));
        Vertex.Position = Vector2(StraightWidth, 0.0f) + (Vertex.Normal * Radius);
        Profile.push_back(Vertex);
    }

    for (int i = 0; i <= ProfileResolution; ++i) {
        float Angle = -Math_PI * 0.5f - (Math_PI * float(i) / float(ProfileResolution));
        ProfileVertex Vertex;
        Vertex.Normal = Vector2(Math::cos(Angle), Math::sin(Angle));
        Vertex.Position = Vector2(-StraightWidth, 0.0f) + (Vertex.Normal * Radius);
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

    const float TotalWidth = GetTotalRoadWidth();
    const float StartX = -TotalWidth * 0.5f + ShoulderWidth;
    Vector<RibbonDef> Ribbons;

    auto add_ribbons = [&](float base_offset, Color base_color, int line_type) {
        float alpha = (line_type == 1) ? 0.0f : 1.0f;
        Color final_color = Color(base_color.r, base_color.g, base_color.b, alpha);
        if (line_type == 2) {
            Ribbons.push_back({base_offset - (DoubleLineSpacing * 0.5f), final_color});
            Ribbons.push_back({base_offset + (DoubleLineSpacing * 0.5f), final_color});
        } else {
            Ribbons.push_back({base_offset, final_color});
        }
    };

    add_ribbons(StartX, EdgeLineColor, EdgeLineType);
    add_ribbons(TotalWidth * 0.5f - ShoulderWidth, EdgeLineColor, EdgeLineType);
    for (int Lane = 1; Lane < LaneCount; ++Lane) {
        const float Offset = StartX + (Lane * LaneWidth);
        const bool IsCenter = (!IsOneWay && Lane == CenterLineIndex);
        add_ribbons(Offset, IsCenter ? CenterLineColor : LaneLineColor, IsCenter ? CenterLineType : LaneLineType);
    }

    PackedVector3Array Points = CurrentCurve->get_baked_points();
    PackedVector3Array UpVectors = CurrentCurve->get_baked_up_vectors();
    const int PointCount = Points.size();

    // ====================================================================
    // --- NOUVEAU : PROJECTION SUR LE TERRAIN (RAYCAST) ---
    // ====================================================================
    if (SnapToTerrain && is_inside_tree()) {
        PhysicsDirectSpaceState3D* SpaceState = get_world_3d()->get_direct_space_state();

        if (SpaceState) {
            // Création de la liste d'exclusion pour ne pas s'auto-détecter
            TypedArray<RID> ExcludeArray;
            for (int c = 0; c < Chunks.size(); ++c) {
                if (Chunks[c].StaticBody && Chunks[c].StaticBody->is_inside_tree()) {
                    ExcludeArray.push_back(Chunks[c].StaticBody->get_rid());
                }
            }

            // --- LE FIX EST ICI : Les Matrices de Transformation ---
            Transform3D NodeTransform = get_global_transform();
            Transform3D InverseTransform = NodeTransform.affine_inverse();

            for (int i = 0; i < PointCount; ++i) {
                // 1. Convertir le point local de la courbe en position globale absolue
                Vector3 GlobalPoint = NodeTransform.xform(Points[i]);

                // 2. Tirer le rayon dans l'espace global
                Vector3 RayOrigin = GlobalPoint;
                RayOrigin.y += 1000.0f;
                Vector3 RayEnd = GlobalPoint;
                RayEnd.y -= 1000.0f;

                Ref<PhysicsRayQueryParameters3D> Query = PhysicsRayQueryParameters3D::create(RayOrigin, RayEnd, TerrainCollisionMask);
                Query->set_exclude(ExcludeArray);

                Dictionary Hit = SpaceState->intersect_ray(Query);

                if (!Hit.is_empty()) {
                    Vector3 GlobalHitPos = Hit["position"];

                    // 3. Reconvertir le point d'impact global en coordonnées locales
                    Vector3 LocalHitPos = InverseTransform.xform(GlobalHitPos);

                    // 4. Appliquer la hauteur locale snappée
                    Points.set(i, Vector3(Points[i].x, LocalHitPos.y + TerrainOffset, Points[i].z));
                }
            }
        }
    }
    // ====================================================================

    Vector<ProfileVertex> Profile = BuildCrossSectionProfile();
    PackedVector3Array Forwards;
    Forwards.resize(PointCount);

    bool is_loop = (PointCount > 2) && (Points[0].distance_to(Points[PointCount - 1]) < 0.05f);

    if (is_loop) {
        Points.set(PointCount - 1, Points[0]);
    }

    for (int i = 0; i < PointCount; ++i) {
        Vector3 forward;
        if (i == 0) {
            if (is_loop) {
                forward = (Points[1] - Points[PointCount - 2]).normalized();
            } else {
                forward = (Points[1] - Points[0]).normalized();
            }
        } else if (i == PointCount - 1) {
            if (is_loop) {
                forward = (Points[1] - Points[PointCount - 2]).normalized();
            } else {
                forward = (Points[i] - Points[i - 1]).normalized();
            }
        } else {
            forward = (Points[i + 1] - Points[i - 1]).normalized();
        }
        Forwards.set(i, forward);
    }

    if (is_loop) {
        Vector3 seam_up = (UpVectors[0] + UpVectors[PointCount - 1]).normalized();
        UpVectors.set(0, seam_up);
        UpVectors.set(PointCount - 1, seam_up);
    }

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
        GenerateChunkMesh(c, Ranges[c].StartIdx, Ranges[c].EndIdx, Points, UpVectors, Forwards, Ranges[c].StartDistance, Profile, Ribbons);
    }
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

    float DistanceAlongCurve = p_start_distance;

    for (int i = p_start_idx; i <= p_end_idx; ++i) {
        const Vector3 CurrentPoint = p_points[i];
        const Vector3 CurrentUp = p_up_vectors[i].normalized();

        // --- UTILISATION DIRECTE DU FORWARD LISSÉ ---
        const Vector3 Forward = p_forwards[i];

        const Vector3 Right = Forward.cross(CurrentUp).normalized();
        const Vector3 OrthogonalUp = Right.cross(Forward).normalized();

        float CurrentPerimeter = 0.0f;
        for (int j = 0; j < ProfileCount; ++j) {
            const Vector2 ProfilePos = p_profile[j].Position;
            const Vector2 ProfileNormal = p_profile[j].Normal;

            const Vector3 Vertex3D = (CurrentPoint + (Right * ProfilePos.x) + (OrthogonalUp * ProfilePos.y)) - ChunkOrigin;
            const Vector3 Normal3D = ((Right * ProfileNormal.x) + (OrthogonalUp * ProfileNormal.y)).normalized();

            RoadSurface->set_normal(Normal3D);
            RoadSurface->set_uv(Vector2(CurrentPerimeter * UvScale.x, DistanceAlongCurve * UvScale.y));
            RoadSurface->add_vertex(Vertex3D);

            const int NextJ = (j + 1) % ProfileCount;
            CurrentPerimeter += p_profile[j].Position.distance_to(p_profile[NextJ].Position);
        }

        for (int k = 0; k < RibbonCountTotal; ++k) {
            const Vector3 Center = CurrentPoint + (Right * p_ribbons[k].OffsetX) + (OrthogonalUp * (RoadThickness * 0.5f + LineOffset));
            const Vector3 LeftVertex = (Center - (Right * (LineWidth * 0.5f))) - ChunkOrigin;
            const Vector3 RightVertex = (Center + (Right * (LineWidth * 0.5f))) - ChunkOrigin;

            LineSurface->set_color(p_ribbons[k].ColorValue);
            LineSurface->set_normal(OrthogonalUp);
            LineSurface->set_uv(Vector2(0.0f, DistanceAlongCurve * UvScale.y));
            LineSurface->add_vertex(LeftVertex);

            LineSurface->set_color(p_ribbons[k].ColorValue);
            LineSurface->set_normal(OrthogonalUp);
            LineSurface->set_uv(Vector2(1.0f, DistanceAlongCurve * UvScale.y));
            LineSurface->add_vertex(RightVertex);
        }

        if (i < p_end_idx) {
            DistanceAlongCurve += CurrentPoint.distance_to(p_points[i + 1]);
        }
    }

    for (int local_i = 0; local_i < LocalPointCount - 1; ++local_i) {
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