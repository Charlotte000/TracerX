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


def write_shader(path: str, accumShaderSrc: str, accumShaderBin: bytes, toneMapShaderSrc: str, toneMapShaderBin: bytes) -> bool:
    accumShaderBinRepr = list(map(lambda v: f"{v:#04x}", accumShaderBin))
    toneMapShaderBinRepr = list(map(lambda v: f"{v:#04x}", toneMapShaderBin))

    newData = (
        "#include <TracerX/Renderer.h>\n\n"
        + "using namespace TracerX;\n\n"
        + "#if TX_SPIRV\n"
        + "const unsigned char Renderer::accumShaderSrc[] =\n{\n    "
        + ",\n    ".join(", ".join(accumShaderBinRepr[i : i + 10]) for i in range(0, len(accumShaderBinRepr), 10))
        + "\n};\n"
        + f"const size_t Renderer::accumShaderSrcSize = {len(accumShaderBinRepr)};\n\n"
        + "const unsigned char Renderer::toneMapShaderSrc[] =\n{\n    "
        + ",\n    ".join(", ".join(toneMapShaderBinRepr[i : i + 10]) for i in range(0, len(toneMapShaderBinRepr), 10))
        + "\n};\n"
        + f"const size_t Renderer::toneMapShaderSrcSize = {len(toneMapShaderBinRepr)};\n"
        + "#else\n"
        + f'const char Renderer::accumShaderSrc[] = R"AccumShaderSrc({accumShaderSrc})AccumShaderSrc";\n\n'
        + f'const char Renderer::toneMapShaderSrc[] = R"ToneMapShaderSrc({toneMapShaderSrc})ToneMapShaderSrc";\n'
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
shaderPath = join(project, "tracerX", "shaders")
accumPath = join(shaderPath, "accumulate.comp")
toneMapPath = join(shaderPath, "toneMap.comp")
outPath = join(project, "tracerX", "src", "RendererShaderSrc.cpp")

try:
    accumShaderSrc = assemble_shader(accumPath)
    toneMapShaderSrc = assemble_shader(toneMapPath)
    print("[Info] Assemble completed")

    accumShaderBin = compile_shader(accumPath)
    toneMapShaderBin = compile_shader(toneMapPath)
    print("[Info] Compilation completed")

    override = write_shader(outPath, accumShaderSrc, accumShaderBin, toneMapShaderSrc, toneMapShaderBin)

    if override:
        print("[Info] Write completed")
    else:
        print("[Info] Write skipped")

    print("[Info] Build completed")
except ValueError as err:
    print(f"[Error] {err}")
    exit(1)
