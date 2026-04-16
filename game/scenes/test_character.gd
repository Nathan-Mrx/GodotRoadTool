extends CharacterBody3D

@export var speed: float = 10.0
@export var jump_velocity: float = 5.0
@export var turn_speed: float = 3.0

# Récupère la gravité globale du projet
var gravity: float = ProjectSettings.get_setting("physics/3d/default_gravity")

func _physics_process(delta: float) -> void:
	# 1. Application de la gravité
	if not is_on_floor():
		velocity.y -= gravity * delta

	# 2. Gestion du saut (Barre espace / ui_accept)
	if Input.is_action_just_pressed("ui_accept") and is_on_floor():
		velocity.y = jump_velocity

	# 3. Récupération des inputs
	# ui_up/down = avancer/reculer. ui_left/right = tourner.
	var input_dir := Input.get_vector("ui_left", "ui_right", "ui_up", "ui_down")
	
	# 4. Rotation du cube (Direction (Tank controls))
	if input_dir.x != 0:
		rotate_y(-input_dir.x * turn_speed * delta)

	# 5. Mouvement avant/arrière
	# On se déplace toujours par rapport à l'axe Z local du cube
	var forward_dir := -transform.basis.z 
	var move_speed := input_dir.y * speed
	
	velocity.x = forward_dir.x * move_speed
	velocity.z = forward_dir.z * move_speed

	# 6. Application du mouvement et résolution des collisions
	move_and_slide()
