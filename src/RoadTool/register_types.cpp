#include "register_types.h"

#include "ProceduralRoad.h"
#include "ProceduralIntersection.h"
#include "ProceduralRoadGizmos.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_roadtool_module(ModuleInitializationLevel p_level) {
    if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
        ClassDB::register_class<ProceduralRoad>();
        ClassDB::register_class<ProceduralIntersection>();
    }

    // Le Gizmo est un outil d'éditeur, il reste dans ce niveau !
    if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
        ClassDB::register_class<ProceduralRoadGizmosPlugin>();
    }
}

void uninitialize_roadtool_module(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
    // Initialization.
    GDExtensionBool GDE_EXPORT roadtool_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
        godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

        init_obj.register_initializer(initialize_roadtool_module);
        init_obj.register_terminator(uninitialize_roadtool_module);
        init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

        return init_obj.init();
    }
}