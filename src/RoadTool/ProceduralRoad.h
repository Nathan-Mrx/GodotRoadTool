#ifndef PROCEDURAL_ROAD_H
#define PROCEDURAL_ROAD_H

#include <godot_cpp/classes/path3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/classes/static_body3d.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/shape3d.hpp>
#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/curve3d.hpp>

namespace godot {

    struct ProfileVertex {
        Vector2 Position;
        Vector2 Normal;
    };

    struct RibbonDef {
        float OffsetX;
        Color ColorValue; // Alpha 1.0 = Continue, Alpha 0.0 = Pointillée
    };

    // Nouveau conteneur pour regrouper les nœuds d'un segment
    struct RoadChunk {
        MeshInstance3D* MeshInst = nullptr;
        StaticBody3D* StaticBody = nullptr;
        CollisionShape3D* CollisionShape = nullptr;
    };

    class ProceduralRoad : public Path3D {
        GDCLASS(ProceduralRoad, Path3D)

    private:
        float RoadThickness = 0.5f;
        int ProfileResolution = 8;
        Vector2 UvScale = Vector2(1.0f, 1.0f);
        float ChunkLength = 50.0f;

        int LaneCount = 2;
        float LaneWidth = 3.5f;
        float ShoulderWidth = 1.0f;
        bool IsOneWay = false;
        int CenterLineIndex = 1;
        float LineWidth = 0.15f;
        float LineOffset = 0.02f;

        Color EdgeLineColor = Color(1, 1, 1);
        Color LaneLineColor = Color(1, 1, 1);
        Color CenterLineColor = Color(1, 0.8, 0);

        Ref<Material> RoadMaterial;
        Ref<Material> MarkingsMaterial;

        Vector<RoadChunk> Chunks;

        bool UseCollision = true;
        Ref<Curve3D> LastCurve;

        void RebuildRoad();
        void OnCurveChanged();
        Vector<ProfileVertex> BuildCrossSectionProfile() const;
        void AutoSmoothCurve();

        void UpdateChunkCount(int p_target_count);
        void GenerateChunkMesh(int p_chunk_index, int p_start_idx, int p_end_idx, const PackedVector3Array& p_points, const PackedVector3Array& p_up_vectors, const PackedVector3Array& p_forwards, float p_start_distance, const Vector<ProfileVertex>& p_profile, const Vector<RibbonDef>& p_ribbons);

        int EdgeLineType = 0;
        int LaneLineType = 1;
        int CenterLineType = 2;
        float DoubleLineSpacing = 0.15f;
        float DashLength = 3.0f;
        float DashGap = 10.0f;

        NodePath TerrainPath;
        float TerrainFalloff = 5.0f;
        float TerrainDepthOffset = 0.1f;
        void BakeTerrain();

        NodePath ConnectedStart;
        NodePath ConnectedEnd;

    protected:
        static void _bind_methods();
        void _notification(int p_what);

    public:
        ProceduralRoad();
        ~ProceduralRoad();

        void SetRoadThickness(float p_thickness);
        float GetRoadThickness() const;
        void SetProfileResolution(int p_resolution);
        int GetProfileResolution() const;
        void SetUvScale(Vector2 p_scale);
        Vector2 GetUvScale() const;
        void SetLaneCount(int p_count);
        int GetLaneCount() const;
        void SetLaneWidth(float p_width);
        float GetLaneWidth() const;
        void SetShoulderWidth(float p_width);
        float GetShoulderWidth() const;
        void SetIsOneWay(bool p_one_way);
        bool GetIsOneWay() const;
        void SetCenterLineIndex(int p_index);
        int GetCenterLineIndex() const;
        void SetEdgeLineColor(Color p_color);
        Color GetEdgeLineColor() const;
        void SetLaneLineColor(Color p_color);
        Color GetLaneLineColor() const;
        void SetCenterLineColor(Color p_color);
        Color GetCenterLineColor() const;
        void SetRoadMaterial(Ref<Material> p_material);
        Ref<Material> GetRoadMaterial() const;
        void SetMarkingsMaterial(Ref<Material> p_material);
        Ref<Material> GetMarkingsMaterial() const;
        void SetTriggerAutoSmooth(bool p_trigger);
        bool GetTriggerAutoSmooth() const;
        void SetUseCollision(bool p_use);
        bool GetUseCollision() const;
        void SetLineOffset(float p_offset);
        float GetLineOffset() const;
        void SetEdgeLineType(int p_type);
        int GetEdgeLineType() const;
        void SetLaneLineType(int p_type);
        int GetLaneLineType() const;
        void SetCenterLineType(int p_type);
        int GetCenterLineType() const;
        void SetDoubleLineSpacing(float p_spacing);
        float GetDoubleLineSpacing() const;
        void SetDashLength(float p_length);
        float GetDashLength() const;
        void SetDashGap(float p_gap);
        float GetDashGap() const;
        void SetChunkLength(float p_length);
        float GetChunkLength() const;
        void SetTerrainPath(NodePath p_path);
        NodePath GetTerrainPath() const;
        void SetTerrainFalloff(float p_falloff);
        float GetTerrainFalloff() const;
        void SetTriggerBakeTerrain(bool p_trigger);
        bool GetTriggerBakeTerrain() const;
        void SetTerrainDepthOffset(float p_offset);
        float GetTerrainDepthOffset() const;
        void SetConnectedStart(NodePath p_path);
        NodePath GetConnectedStart() const;
        void SetConnectedEnd(NodePath p_path);
        NodePath GetConnectedEnd() const;
        float GetTotalRoadWidth() const;

    };
}
#endif