import subprocess
from pathlib import Path


def preprocess_shader(mainPath: Path, outputPath: Path):
    proc = subprocess.run([ "glslc", "-Werror", "-O", "-E", mainPath, "-o", outputPath ], stderr=subprocess.PIPE)
    if proc.returncode != 0:
        raise RuntimeError(f"Preprocessing error:\n{proc.stderr.decode()}")

def compile_shader(mainPath: Path, outputPath: Path):
    proc = subprocess.run([ "glslc", "-Werror", "-O", mainPath, "-o", outputPath ], stderr=subprocess.PIPE)
    if proc.returncode != 0:
        raise RuntimeError(f"Compilation error:\n{proc.stderr.decode()}")

project = (Path(__file__).parent / "..").resolve()
shaderPath = project / "tracerX" / "shaders"
accumPath = shaderPath / "accumulate" / "main.comp"
toneMapPath = shaderPath / "toneMap" / "main.comp"
outputPath = shaderPath / "out"

outputPath.mkdir(exist_ok=True)

try:
    preprocess_shader(accumPath, outputPath / "accum.comp")
    preprocess_shader(toneMapPath, outputPath / "toneMap.comp")
    print("[Info] Assemble completed")

    compile_shader(accumPath, outputPath / "accum.comp.spv")
    compile_shader(toneMapPath, outputPath / "toneMap.comp.spv")
    print("[Info] Compilation completed")

    print("[Info] Build completed")
except RuntimeError as err:
    print(f"[Error] {err}")
    exit(1)
