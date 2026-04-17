#ifndef PROCEDURAL_ROAD_GIZMOS_H
#define PROCEDURAL_ROAD_GIZMOS_H

#include <godot_cpp/classes/editor_node3d_gizmo_plugin.hpp>
#include <godot_cpp/classes/editor_node3d_gizmo.hpp>
#include <godot_cpp/classes/camera3d.hpp>

namespace godot {

    class ProceduralRoad; // Forward declaration

    class ProceduralRoadGizmosPlugin : public EditorNode3DGizmoPlugin {
        GDCLASS(ProceduralRoadGizmosPlugin, EditorNode3DGizmoPlugin)

    private:
        // NOUVEAU : Mémoire du joystick actuellement manipulé
        ProceduralRoad* active_road = nullptr;
        int active_handle = -1;

    protected:
        static void _bind_methods();

    public:
        ProceduralRoadGizmosPlugin();
        ~ProceduralRoadGizmosPlugin();

        bool _has_gizmo(Node3D *p_spatial) const override;
        String _get_gizmo_name() const override;
        void _redraw(const Ref<EditorNode3DGizmo> &p_gizmo) override;

        String _get_handle_name(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary) const override;
        Variant _get_handle_value(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary) const override;
        void _set_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary, Camera3D *p_camera, const Vector2 &p_screen_pos) override;
        void _commit_handle(const Ref<EditorNode3DGizmo> &p_gizmo, int32_t p_handle_id, bool p_secondary, const Variant &p_restore, bool p_cancel) override;
    };
}
#endif