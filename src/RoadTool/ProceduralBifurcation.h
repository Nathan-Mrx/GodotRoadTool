#ifndef PROCEDURAL_BIFURCATION_H
#define PROCEDURAL_BIFURCATION_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>

namespace godot {

    class ProceduralRoad;

    class ProceduralBifurcation : public Node3D {
        GDCLASS(ProceduralBifurcation, Node3D)

    private:
        NodePath MainRoadPath;
        NodePath ExitRoadPath;

        float SplittingOffset = 50.0f;
        float GoreLength = 20.0f;

        float ChevronSpacing = 4.0f;
        float ChevronWidth = 0.3f;

        Ref<PackedScene> AttenuatorScene;

        MeshInstance3D* GoreMeshInstance = nullptr;
        Node3D* AttenuatorInstance = nullptr;

        void AutoDetectOffset();

        int BifurcationSide = 1; // 1 = Droit, -1 = Gauche
        int BifurcationType = 0; // 0 = Sortie, 1 = Entrée
        float TransitionLength = 40.0f;
        float SolidLineLength = 15.0f;
        float TaperLength = 15.0f;

    protected:
        static void _bind_methods();
        void _notification(int p_what);

    public:
        ProceduralBifurcation();
        ~ProceduralBifurcation();

        void SetMainRoadPath(NodePath p_path);
        NodePath GetMainRoadPath() const;
        void SetExitRoadPath(NodePath p_path);
        NodePath GetExitRoadPath() const;
        void SetSplittingOffset(float p_offset);
        float GetSplittingOffset() const;
        void SetChevronSpacing(float p_spacing);
        float GetChevronSpacing() const;
        void SetAttenuatorScene(const Ref<PackedScene>& p_scene);
        Ref<PackedScene> GetAttenuatorScene() const;
        void SetBifurcationSide(int p_side);
        int GetBifurcationSide() const;
        void SetBifurcationType(int p_type);
        int GetBifurcationType() const;
        void SetTransitionLength(float p_length);
        float GetTransitionLength() const;
        void SetSolidLineLength(float p_length);
        float GetSolidLineLength() const;
        void SetTaperLength(float p_length);
        float GetTaperLength() const;

        void RebuildBifurcation();
    };
}
#endif