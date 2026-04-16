extends VehicleBody3D

@export var MaxEngineForce: float = 800.0   # Augmenté pour le poids
@export var MaxSteerAngle: float = 0.4     # Un peu moins pour éviter les coups de volant brusques
@export var SteeringSpeed: float = 4.0
@export var BrakingForce: float = 50.0      # Freinage plus sec
@export var AntiRollForce: float = 8000.0   # On stabilise fort

var CurrentSteering: float = 0.0

func _physics_process(delta: float) -> void:
	# 1. Direction avec typage explicite pour ton compilateur
	var target_steer: float = Input.get_axis("ui_right", "ui_left") * MaxSteerAngle
	CurrentSteering = lerp(CurrentSteering, target_steer, SteeringSpeed * delta)
	steering = CurrentSteering

	# 2. Engine Force
	var throttle: float = Input.get_axis("ui_down", "ui_up")
	engine_force = throttle * MaxEngineForce

	# 3. Frein
	if Input.is_action_pressed("ui_select"):
		brake = BrakingForce
	else:
		brake = 0.0
		
	_apply_anti_roll()

func _apply_anti_roll() -> void:
	_check_axle_roll($VehicleWheel3D_FL, $VehicleWheel3D_FR)
	_check_axle_roll($VehicleWheel3D_RL, $VehicleWheel3D_RR)

func _check_axle_roll(wheel_left: VehicleWheel3D, wheel_right: VehicleWheel3D) -> void:
	# On définit explicitement les types (bool et float) au lieu d'utiliser :=
	var ground_l: bool = wheel_left.is_in_contact()
	var ground_r: bool = wheel_right.is_in_contact()

	# suspension_travel est un float
	var travel_limit: float = max(wheel_left.suspension_travel, 0.01)
	
	var compression_l: float = 1.0
	if ground_l:
		# .distance_to() renvoie un float
		var dist: float = wheel_left.global_position.distance_to(wheel_left.get_contact_point())
		compression_l = clamp(dist / travel_limit, 0.0, 1.0)
	
	var compression_r: float = 1.0
	if ground_r:
		var dist: float = wheel_right.global_position.distance_to(wheel_right.get_contact_point())
		compression_r = clamp(dist / travel_limit, 0.0, 1.0)

	# roll_force est un float
	var roll_force: float = (compression_l - compression_r) * AntiRollForce

	if ground_l:
		apply_force(global_transform.basis.y * -roll_force, wheel_left.position)
	if ground_r:
		apply_force(global_transform.basis.y * roll_force, wheel_right.position)
