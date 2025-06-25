import subprocess
from os import remove
from os.path import dirname, exists, join


def compile_shader(mainPath: str) -> bytes:
    temp = join(dirname(mainPath), "out.spv")
    proc = subprocess.run(["glslc", mainPath, "-o", temp], stderr=subprocess.PIPE)
    if proc.returncode != 0:
        raise ValueError(f"Compilation error:\n{proc.stderr.decode()}")

    with open(temp, "rb") as file:
        data = file.read()

    remove(temp)
    return data


def assemble_shader(mainPath: str) -> str:
    result = ""
    with open(mainPath, "r") as file:
        for line in file:
            if line.startswith('#include "'):
                includePath = line.removeprefix('#include "').removesuffix('"\n')
                result += assemble_shader(join(dirname(mainPath), includePath))
            else:
                result += line
    return result


def write_shader(path: str, shaderBin: bytes, shaderSrc: str) -> bool:
    src = list(map(lambda v: f"{v:#04x}", shaderBin))

    newData = (
        "#include <TracerX/Renderer.h>\n\n"
        + "using namespace TracerX;\n\n"
        + "#if TX_SPIRV\n"
        + "const unsigned char Renderer::shaderSrc[] =\n{\n    "
        + ",\n    ".join(", ".join(src[i : i + 10]) for i in range(0, len(src), 10))
        + "\n};\n"
        + f"const size_t Renderer::shaderSrcSize = {len(src)};\n"
        + "#else\n"
        + f'const char Renderer::shaderSrc[] = R"ShaderSrc({shaderSrc})ShaderSrc";\n'
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
mainPath = join(project, "tracerX", "shaders", "main.comp")
srcPath = join(project, "tracerX", "src", "RendererShaderSrc.cpp")

try:
    shaderSrc = assemble_shader(mainPath)
    print("[Info] Assemble completed")

    shaderBin = compile_shader(mainPath)
    print("[Info] Compilation completed")

    override = write_shader(srcPath, shaderBin, shaderSrc)

    if override:
        print("[Info] Write completed")
    else:
        print("[Info] Write skipped")

    print("[Info] Build completed")
except ValueError as err:
    print(f"[Error] {err}")
    exit(1)
