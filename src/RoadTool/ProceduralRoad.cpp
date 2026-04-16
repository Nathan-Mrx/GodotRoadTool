#include "ProceduralRoad.h"
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/curve3d.hpp>
#include <godot_cpp/classes/surface_tool.hpp>
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

    ADD_GROUP("Road Geometry", "");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "road_thickness"), "set_road_thickness", "get_road_thickness");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "profile_resolution"), "set_profile_resolution", "get_profile_resolution");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "uv_scale"), "set_uv_scale", "get_uv_scale");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "line_offset"), "set_line_offset", "get_line_offset");

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
}

ProceduralRoad::ProceduralRoad() {
    RoadMeshInstance = memnew(MeshInstance3D);
    add_child(RoadMeshInstance);

    RoadStaticBody = memnew(StaticBody3D);
    add_child(RoadStaticBody);

    RoadCollisionShape = memnew(CollisionShape3D);
    RoadStaticBody->add_child(RoadCollisionShape);
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

    // --- 0. GESTION ROBUSTE DU CYCLE DE VIE ---
    if (CurrentCurve != LastCurve) {
        if (LastCurve.is_valid() && LastCurve->is_connected("changed", Callable(this, "_on_curve_changed"))) {
            LastCurve->disconnect("changed", Callable(this, "_on_curve_changed"));
        }

        LastCurve = CurrentCurve;
        if (LastCurve.is_valid()) {
            // AJOUT DE LA VÉRIFICATION is_connected ICI
            if (!LastCurve->is_connected("changed", Callable(this, "_on_curve_changed"))) {
                LastCurve->connect("changed", Callable(this, "_on_curve_changed"));
            }
        }
    }

    // Sécurité si la courbe est vide
    if (CurrentCurve.is_null() || CurrentCurve->get_baked_points().size() < 2) {
        if (RoadMeshInstance != nullptr) RoadMeshInstance->set_mesh(Ref<ArrayMesh>());
        if (RoadCollisionShape != nullptr) RoadCollisionShape->set_shape(Ref<Shape3D>());
        return;
    }

    // --- 1. CONFIGURATION DES RUBANS DE MARQUAGE (RIBBONS) ---
    struct RibbonDef {
        float OffsetX;
        Color ColorValue; // Alpha 1.0 = Continue, Alpha 0.0 = Pointillée
    };

    Vector<RibbonDef> Ribbons;
    const float TotalWidth = GetTotalRoadWidth();
    const float StartX = -TotalWidth * 0.5f + ShoulderWidth;

    auto add_ribbons = [&](float base_offset, Color base_color, int line_type) {
        float alpha = (line_type == 1) ? 0.0f : 1.0f;
        Color final_color = Color(base_color.r, base_color.g, base_color.b, alpha);

        if (line_type == 2) { // Double Continue
            Ribbons.push_back({base_offset - (DoubleLineSpacing * 0.5f), final_color});
            Ribbons.push_back({base_offset + (DoubleLineSpacing * 0.5f), final_color});
        } else { // Continue ou Pointillée
            Ribbons.push_back({base_offset, final_color});
        }
    };

    // Lignes de rive (bords)
    add_ribbons(StartX, EdgeLineColor, EdgeLineType);
    add_ribbons(TotalWidth * 0.5f - ShoulderWidth, EdgeLineColor, EdgeLineType);

    // Lignes de voies (intérieures)
    for (int Lane = 1; Lane < LaneCount; ++Lane) {
        const float Offset = StartX + (Lane * LaneWidth);
        const bool IsCenter = (!IsOneWay && Lane == CenterLineIndex);

        const Color MarkColor = IsCenter ? CenterLineColor : LaneLineColor;
        const int MarkType = IsCenter ? CenterLineType : LaneLineType;

        add_ribbons(Offset, MarkColor, MarkType);
    }

    const int RibbonCountTotal = Ribbons.size();

    // --- 2. PRÉPARATION DES SURFACES ---
    Ref<SurfaceTool> RoadSurface;
    RoadSurface.instantiate();
    RoadSurface->begin(Mesh::PRIMITIVE_TRIANGLES);

    Ref<SurfaceTool> LineSurface;
    LineSurface.instantiate();
    LineSurface->begin(Mesh::PRIMITIVE_TRIANGLES);

    PackedVector3Array Points = CurrentCurve->get_baked_points();
    PackedVector3Array UpVectors = CurrentCurve->get_baked_up_vectors();
    Vector<ProfileVertex> Profile = BuildCrossSectionProfile();

    const int PointCount = Points.size();
    const int ProfileCount = Profile.size();
    float DistanceAlongCurve = 0.0f;

    // --- 3. GÉNÉRATION DES SOMMETS (VERTICES) ---
    for (int i = 0; i < PointCount; ++i) {
        const Vector3 CurrentPoint = Points[i];
        const Vector3 CurrentUp = UpVectors[i].normalized();

        Vector3 Forward = Vector3(0, 0, -1);
        if (i < PointCount - 1) {
            Forward = (Points[i + 1] - CurrentPoint).normalized();
        } else if (i > 0) {
            Forward = (CurrentPoint - Points[i - 1]).normalized();
        }

        const Vector3 Right = Forward.cross(CurrentUp).normalized();
        const Vector3 OrthogonalUp = Right.cross(Forward).normalized();

        // 3a. Vertices Asphalte
        float CurrentPerimeter = 0.0f;
        for (int j = 0; j < ProfileCount; ++j) {
            const Vector2 ProfilePos = Profile[j].Position;
            const Vector2 ProfileNormal = Profile[j].Normal;

            const Vector3 Vertex3D = CurrentPoint + (Right * ProfilePos.x) + (OrthogonalUp * ProfilePos.y);
            const Vector3 Normal3D = ((Right * ProfileNormal.x) + (OrthogonalUp * ProfileNormal.y)).normalized();

            RoadSurface->set_normal(Normal3D);
            RoadSurface->set_uv(Vector2(CurrentPerimeter * UvScale.x, DistanceAlongCurve * UvScale.y));
            RoadSurface->add_vertex(Vertex3D);

            const int NextJ = (j + 1) % ProfileCount;
            CurrentPerimeter += Profile[j].Position.distance_to(Profile[NextJ].Position);
        }

        // 3b. Vertices Lignes (Rubans)
        for (int k = 0; k < RibbonCountTotal; ++k) {
            const Vector3 Center = CurrentPoint + (Right * Ribbons[k].OffsetX) + (OrthogonalUp * (RoadThickness * 0.5f + LineOffset));
            const Vector3 LeftVertex = Center - (Right * (LineWidth * 0.5f));
            const Vector3 RightVertex = Center + (Right * (LineWidth * 0.5f));

            LineSurface->set_color(Ribbons[k].ColorValue);
            LineSurface->set_normal(OrthogonalUp);
            LineSurface->set_uv(Vector2(0.0f, DistanceAlongCurve * UvScale.y));
            LineSurface->add_vertex(LeftVertex);

            LineSurface->set_color(Ribbons[k].ColorValue);
            LineSurface->set_normal(OrthogonalUp);
            LineSurface->set_uv(Vector2(1.0f, DistanceAlongCurve * UvScale.y));
            LineSurface->add_vertex(RightVertex);
        }

        if (i < PointCount - 1) {
            DistanceAlongCurve += CurrentPoint.distance_to(Points[i + 1]);
        }
    }

    // --- 4. TISSAGE DES TRIANGLES (INDEXATION) ---
    for (int i = 0; i < PointCount - 1; ++i) {
        // 4a. Indexation Asphalte
        const int CurrentRing = i * ProfileCount;
        const int NextRing = (i + 1) * ProfileCount;

        for (int j = 0; j < ProfileCount; ++j) {
            const int NextJ = (j + 1) % ProfileCount;
            RoadSurface->add_index(CurrentRing + j);
            RoadSurface->add_index(NextRing + j);
            RoadSurface->add_index(CurrentRing + NextJ);

            RoadSurface->add_index(CurrentRing + NextJ);
            RoadSurface->add_index(NextRing + j);
            RoadSurface->add_index(NextRing + NextJ);
        }

        // 4b. Indexation Lignes (Winding Order Inversé)
        for (int k = 0; k < RibbonCountTotal; ++k) {
            const int CurrentLeft = (i * RibbonCountTotal * 2) + (k * 2);
            const int CurrentRight = CurrentLeft + 1;
            const int NextLeft = ((i + 1) * RibbonCountTotal * 2) + (k * 2);
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

    // --- 5. ASSEMBLAGE BASE (ASPHALTE) ET PHYSIQUE ---
    Ref<ArrayMesh> FinalMesh = RoadSurface->commit();
    if (RoadMaterial.is_valid()) {
        FinalMesh->surface_set_material(0, RoadMaterial);
    }

    // Génération de la collision UNIQUEMENT sur l'asphalte
    if (UseCollision && FinalMesh.is_valid()) {
        Ref<Shape3D> Trimesh = FinalMesh->create_trimesh_shape();
        RoadCollisionShape->set_shape(Trimesh);
    } else {
        RoadCollisionShape->set_shape(Ref<Shape3D>());
    }

    // --- 6. INJECTION DES LIGNES ET SHADER ---
    // On ajoute la surface des lignes (Surface 1) au FinalMesh
    LineSurface->commit(FinalMesh);

    if (MarkingsMaterial.is_valid()) {
        FinalMesh->surface_set_material(1, MarkingsMaterial);

        // C'est ici qu'on pousse les paramètres de l'inspecteur vers le Shader GPU
        MarkingsMaterial->set("shader_parameter/dash_length", DashLength);
        MarkingsMaterial->set("shader_parameter/dash_gap", DashGap);
    }

    // --- 7. RENDU FINAL ---
    RoadMeshInstance->set_mesh(FinalMesh);
}