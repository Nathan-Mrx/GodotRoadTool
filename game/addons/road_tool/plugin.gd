@tool
extends EditorPlugin

var gizmo_plugin : ProceduralRoadGizmosPlugin

func _enter_tree() -> void:
    # On instancie la classe C++
    gizmo_plugin = ProceduralRoadGizmosPlugin.new()
    add_node_3d_gizmo_plugin(gizmo_plugin)

func _exit_tree() -> void:
    if gizmo_plugin:
        remove_node_3d_gizmo_plugin(gizmo_plugin)