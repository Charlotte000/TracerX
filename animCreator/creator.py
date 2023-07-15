from typing import  Callable
from dataclasses import dataclass
from math import cos, sin, pi, acos, sqrt

@dataclass
class Vector3:
    x: float
    y: float
    z: float

    @staticmethod
    def slerp(a: 'Vector3', b: 'Vector3', t: float) -> 'Vector3':
        angle = acos(Vector3.dot(a, b))
        return (a * sin((1 - t) * angle) + b * sin(t * angle)) / sin(angle) if angle != 0 else b
    
    @staticmethod
    def lerp(a: 'Vector3', b: 'Vector3', t: float) -> 'Vector3':
        return a + (b - a) * t

    @staticmethod
    def dot(a: 'Vector3', b: 'Vector3') -> float:
        return a.x * b.x + a.y * b.y + a.z * b.z
    
    def normalize(self) -> 'Vector3':
        length = sqrt(self.x * self.x + self.y * self.y + self.z * self.z)
        return Vector3(self.x / length, self.y / length, self.z / length)

    def __add__(self, vector: 'Vector3') -> 'Vector3':
        return Vector3(self.x + vector.x, self.y + vector.y, self.z + vector.z)
    
    def __sub__(self, vector: 'Vector3') -> 'Vector3':
        return Vector3(self.x - vector.x, self.y - vector.y, self.z - vector.z)
    
    def __mul__(self, scalar: float) -> 'Vector3':
        return Vector3(self.x * scalar, self.y * scalar, self.z * scalar)
    
    def __truediv__(self, scalar: float) -> 'Vector3':
        return Vector3(self.x / scalar, self.y / scalar, self.z / scalar)
    
    def __neg__(self) -> 'Vector3':
        return Vector3(-self.x, -self.y, -self.z)
        
    def __str__(self) -> str:
        return f'{self.x} {self.y} {self.z}'

@dataclass
class CameraState:
    Position: Vector3
    Forward: Vector3
    Up: Vector3

def create_animation(file_name: str, fps: float, duration: float, animation: Callable[[float], CameraState]):
    with open(file_name, 'w') as file:
        total_frames = int(fps * duration)
        for frame in range(total_frames):
            camera_state = animation(frame / (total_frames - 1))
            file.write(f'{camera_state.Position} {camera_state.Forward} {camera_state.Up}\n')


def orbit_animation(origin: Vector3, radius: float, angle_offset: float = 0) -> Callable[[float], CameraState]:
    def animation(time: float) -> CameraState:
        angle = time * 2 * pi + angle_offset
        dir = Vector3(cos(angle), 0, sin(angle))
        return CameraState(origin + dir * radius, -dir, Vector3(0, 1, 0))

    return animation

def linear_move(origin: CameraState, destination: CameraState) -> Callable[[float], CameraState]:
    def animation(time: float) -> CameraState:
        return CameraState(
            Vector3.lerp(origin.Position, destination.Position, time),
            Vector3.slerp(origin.Forward, destination.Forward, time),
            Vector3.slerp(origin.Up, destination.Up, time))
    
    return animation

def linear_move(origin: CameraState, destination: Vector3) -> Callable[[float], CameraState]:
    def animation(time: float) -> CameraState:
        return CameraState(Vector3.lerp(origin.Position, destination, time), origin.Forward, origin.Up)
    
    return animation