import subprocess
from os import remove
from os.path import dirname, join, exists


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


def write_shader(path: str, shaderBin: bytes) -> bool:
    src = list(map(lambda v: f"{v:#04x}", shaderBin))

    newData = (
        "#include <TracerX/Renderer.h>\n\n"
        + "using namespace TracerX;\n\n"
        + "#if TX_SPIRV\n"
        + "const unsigned char Renderer::shaderBin[] =\n{\n    "
        + ",\n    ".join(', '.join(src[i:i+10]) for i in range(0, len(src), 10))
        + "\n};\n\n"
        + f"const size_t Renderer::shaderBinSize = {len(src)};\n"
        + "#endif\n"
    )

    if exists(path):
        with open(path, "r") as file:
            oldData = file.read()
    else:
        oldData = ""

    if oldData == newData:
        return False

    with open(path, "w") as file:
        file.write(newData)

    return True


project = join(dirname(__file__), "..")
shaders = join(project, "shaders")

try:
    shaderBin = compile_shader(join(shaders, "main.comp"))
    print("[Info] Compilation completed")

    override = write_shader(
        join(project, "core", "src", "RendererShaderBin.cpp"),
        shaderBin,
    )

    if override:
        print("[Info] Write completed")
    else:
        print("[Info] Write skipped")

    print("[Info] Build completed")
except ValueError as err:
    print(f"[Error] {err}")
    exit(1)
