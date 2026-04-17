extends VehicleBody3D

@export_category("Engine")
@export var max_engine_torque: float = 2500.0 # Un vrai camion a un couple monstrueux
@export var idle_rpm: float = 600.0
@export var max_rpm: float = 2200.0 # Redline très basse comparée à une voiture

@export_category("Transmission (12-Speed Auto)")
# Ratios d'une vraie boîte 12 rapports. La 1ère (14.94) est une "Crawler gear", max 8 km/h.
@export var gear_ratios: Array[float] = [14.94, 11.73, 9.04, 7.09, 5.54, 4.35, 3.44, 2.70, 2.08, 1.63, 1.27, 1.00]
@export var reverse_ratio: float = 14.0
@export var final_drive_ratio: float = 3.4
@export var shift_up_rpm: float = 1700.0
@export var shift_down_rpm: float = 1000.0
@export var shift_duration: float = 0.6 # Les grosses boîtes mettent du temps à passer un rapport

@export_category("Steering & Brakes")
@export var max_turn_amount: float = 0.4
@export var turn_speed: float = 2.0
@export var max_brake_force: float = 150.0

var current_gear: int = 0 
var engine_rpm: float = idle_rpm
var is_shifting: bool = false
var shift_timer: float = 0.0

# Simule le temps de réponse du turbo et l'inertie du gros moteur
var actual_throttle: float = 0.0 

@onready var wheel_bl = $VehicleWheel3D_BL
@onready var wheel_br = $VehicleWheel3D_BR
@onready var spring_arm = $SpringArm 

func _physics_process(delta: float) -> void:
	# TODO: Sortir le SpringArm du VehicleBody. L'attacher en enfant direct crée du stuttering 
	# à haute vitesse. Il faut le mettre à la racine de la scène et interpoler son Transform.
	spring_arm.position = position 
	
	var gas_input = Input.get_action_strength("Gas")
	var brake_input = Input.get_action_strength("Brake")
	var steer_input = Input.get_action_strength("Left") - Input.get_action_strength("Right")
	
	# Lissage de la pédale (Throttle Response). Plus la valeur est petite, plus le camion est lourd à lancer.
	actual_throttle = lerp(actual_throttle, gas_input, delta * 2.5)
	
	var current_speed_kph = linear_velocity.length() * 3.6
	
	_process_transmission(actual_throttle, brake_input, current_speed_kph, delta)
	_process_engine(actual_throttle, brake_input)
	_process_steering(steer_input, current_speed_kph, delta)

func _process_transmission(gas: float, brake: float, speed_kph: float, delta: float) -> void:
	if is_shifting:
		shift_timer -= delta
		if shift_timer <= 0.0:
			is_shifting = false
		return 

	var avg_wheel_rpm = (abs(wheel_bl.get_rpm()) + abs(wheel_br.get_rpm())) / 2.0
	var current_ratio = reverse_ratio if current_gear == -1 else gear_ratios[current_gear]
	
	engine_rpm = avg_wheel_rpm * current_ratio * final_drive_ratio
	engine_rpm = max(engine_rpm, idle_rpm) 
	
	if current_gear >= 0: 
		if engine_rpm > shift_up_rpm and current_gear < gear_ratios.size() - 1:
			var next_gear = current_gear + 1
			# Skip Shifting : Si on a trop de puissance d'un coup, la boîte saute un rapport
			if engine_rpm > shift_up_rpm + 300 and current_gear < gear_ratios.size() - 2:
				next_gear = current_gear + 2 
			_execute_shift(next_gear)
		elif engine_rpm < shift_down_rpm and current_gear > 0:
			_execute_shift(current_gear - 1)
			
	if speed_kph < 2.0:
		if brake > 0.5 and current_gear >= 0:
			current_gear = -1
		elif gas > 0.5 and current_gear == -1:
			current_gear = 0

func _execute_shift(target_gear: int) -> void:
	current_gear = target_gear
	is_shifting = true
	shift_timer = shift_duration
	engine_force = 0.0 

func _process_engine(gas: float, brake: float) -> void:
	if current_gear >= 0:
		self.brake = brake * max_brake_force
	elif current_gear == -1:
		self.brake = gas * max_brake_force 

	if self.brake > 0.0 or is_shifting:
		engine_force = 0.0
		return

	var rpm_factor = clamp((engine_rpm - idle_rpm) / (max_rpm - idle_rpm), 0.0, 1.0)
	var torque_curve = sin(rpm_factor * PI) 
	torque_curve = clamp(torque_curve, 0.4, 1.0) 
	
	var available_torque = max_engine_torque * torque_curve
	var current_ratio = reverse_ratio if current_gear == -1 else gear_ratios[current_gear]
	
	var final_force = available_torque * current_ratio * final_drive_ratio
	
	if current_gear == -1:
		engine_force = -brake * final_force
	else:
		engine_force = gas * final_force

func _process_steering(steer_input: float, speed_kph: float, delta: float) -> void:
	# Rigidifie la direction à haute vitesse pour éviter les tonneaux sur l'autoroute
	var speed_factor = clamp(1.0 - (speed_kph / 90.0), 0.1, 1.0)
	var target_steer = steer_input * max_turn_amount * speed_factor
	steering = lerp(steering, target_steer, turn_speed * delta)
