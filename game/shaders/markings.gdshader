shader_type spatial;
render_mode cull_back; // On retire unshaded pour avoir du relief

uniform sampler2D road_normal_map : hint_normal;
uniform sampler2D road_roughness_map : hint_roughness_gray;
uniform float dash_length = 3.0;
uniform float dash_gap = 10.0;

void fragment() {
	// 1. Logique de découpe (Pointillés)
	float is_solid = round(COLOR.a);
	float cycle = dash_length + dash_gap;
	float pos = mod(UV.y, cycle);
	float dash_mask = 1.0 - step(dash_length, pos);
	
	// On mélange le masque avec l'état "Ligne continue"
	float final_alpha = mix(dash_mask, 1.0, is_solid);
	
	// Si le shader décide que c'est du vide, on discard le pixel
	if (final_alpha < 0.5) {
		discard;
	}

	// 2. Rendu PBR
	ALBEDO = COLOR.rgb;
	
	// On récupère le relief de l'asphalte pour l'appliquer à la peinture
	NORMAL_MAP = texture(road_normal_map, UV).rgb;
	ROUGHNESS = texture(road_roughness_map, UV).r;
}