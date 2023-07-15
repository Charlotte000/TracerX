from creator import CameraState, Vector3, create_animation, orbit_animation, linear_move

create_animation('anim.txt', 30, 1, orbit_animation(Vector3(0, 0, 0), 4))

