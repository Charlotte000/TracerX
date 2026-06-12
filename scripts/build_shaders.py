import subprocess
from os.path import dirname, exists, join


def preprocess_shader(mainPath: str) -> str:
    proc = subprocess.run(["glslc", "-Werror", "-O", "-E", mainPath ], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    if proc.returncode != 0:
        raise ValueError(f"Preprocessing error:\n{proc.stderr.decode()}")

    return proc.stdout.decode()

def compile_shader(mainPath: str) -> str:
    proc = subprocess.run(["glslc", "-Werror", "-O", "-mfmt=c", mainPath, "-o", "-"], stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    if proc.returncode != 0:
        raise ValueError(f"Compilation error:\n{proc.stderr.decode()}")

    return proc.stdout.decode()

def write_shader(path: str, accumShaderSrc: str, accumShaderBin: str, toneMapShaderSrc: str, toneMapShaderBin: str) -> bool:
    dataFormat = """\
#include <iterator>
#include <TracerX/Renderer.h>

using namespace TracerX;

#if TX_SPIRV
const uint32_t Renderer::accumShaderSrc[] =
{};

const size_t Renderer::accumShaderSrcSize = std::size(Renderer::accumShaderSrc) * sizeof(uint32_t);

const uint32_t Renderer::toneMapShaderSrc[] =
{};

const size_t Renderer::toneMapShaderSrcSize = std::size(Renderer::toneMapShaderSrc) * sizeof(uint32_t);
#else
const char Renderer::accumShaderSrc[] = R"AccumShaderSrc(
{}
)AccumShaderSrc";

const char Renderer::toneMapShaderSrc[] = R"ToneMapShaderSrc(
{}
)ToneMapShaderSrc";
#endif
"""

    newData = dataFormat.format(accumShaderBin, toneMapShaderBin, accumShaderSrc, toneMapShaderSrc)

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
accumPath = join(shaderPath, "accumulate", "main.comp")
toneMapPath = join(shaderPath, "toneMap", "main.comp")
outPath = join(project, "tracerX", "src", "RendererShaderSrc.cpp")

try:
    accumShaderSrc = preprocess_shader(accumPath)
    toneMapShaderSrc = preprocess_shader(toneMapPath)
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
