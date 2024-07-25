import subprocess
from os import remove
from os.path import dirname, join


def compile_shader(mainPath: str) -> bytes:
    temp = join(dirname(mainPath), "out.spv")
    proc = subprocess.run(
        ["glslc", join(shaders, "main.comp"), "-o", temp], stderr=subprocess.PIPE
    )
    if proc.returncode != 0:
        raise ValueError(f"Compilation error:\n{proc.stderr.decode()}")

    with open(temp, "rb") as file:
        data = file.read()

    remove(temp)
    return data


def write_shader(path: str, shaderBin: bytes) -> None:
    with open(path, "w") as file:
        file.write("#include <TracerX/Renderer.h>\n\n")
        file.write("using namespace TracerX;\n\n")

        file.write("const std::vector<unsigned char> Renderer::shaderSrc =\n{\n    ")
        file.write(",\n    ".join(list(map(str, shaderBin))))
        file.write("\n};\n")


project = join(dirname(__file__), "..")
shaders = join(project, "shaders")

try:
    shaderBin = compile_shader(join(shaders, "main.comp"))
    print("[Info] Compilation completed")

    write_shader(
        join(project, "core", "src", "RendererShaderSrc.cpp"),
        shaderBin,
    )

    print("[Info] Write completed")
    print("[Info] Build completed")
except ValueError as err:
    print(f"[Error] {err}")
    exit(1)
