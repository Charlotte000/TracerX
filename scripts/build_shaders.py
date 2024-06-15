from os.path import dirname, exists, join, relpath


def build_shader(shader: str) -> str:
    if not exists(shader):
        raise ValueError(f"shader not found ({relpath(shader)})")

    result = ""
    with open(shader, "r") as file:
        for line in file.readlines():
            if line.startswith("#include"):
                include = line.removeprefix("#include ").strip()
                result += build_shader(join(dirname(shader), include))
            else:
                result += line

    return result


def write_shaders(
    path: str, accumulator: str, toneMapper: str, albedo: str, normal: str, vertex: str
) -> None:
    with open(path, "w") as file:
        file.write("#include <TracerX/Renderer.h>\n\n")
        file.write("using namespace TracerX;\n\n")

        file.write('const char* Renderer::accumulatorShaderSrc =\nR"(\n')
        file.write(accumulator)
        file.write('\n)";\n\n')

        file.write('const char* Renderer::toneMapperShaderSrc =\nR"(\n')
        file.write(toneMapper)
        file.write('\n)";\n\n')

        file.write('const char* Renderer::albedoShaderSrc =\nR"(\n')
        file.write(albedo)
        file.write('\n)";\n\n')

        file.write('const char* Renderer::normalShaderSrc =\nR"(\n')
        file.write(normal)
        file.write('\n)";\n\n')

        file.write('const char* Renderer::vertexShaderSrc =\nR"(\n')
        file.write(vertex)
        file.write('\n)";\n')


project = join(dirname(__file__), "..")
shaders = join(project, "core", "shaders")

try:
    accumulator = build_shader(join(shaders, "fragment", "accumulator.glsl"))
    print("[Info] Build path tracer shader")

    toneMapper = build_shader(join(shaders, "fragment", "toneMapper.glsl"))
    print("[Info] Build tone mapper shader")

    albedo = build_shader(join(shaders, "fragment", "albedo.glsl"))
    print("[Info] Build denoise shader")

    normal = build_shader(join(shaders, "fragment", "normal.glsl"))
    print("[Info] Build denoise shader")

    vertex = build_shader(join(shaders, "vertex", "main.glsl"))
    print("[Info] Build vertex shader")

    write_shaders(
        join(project, "core", "src", "RendererShaderSrc.cpp"),
        accumulator,
        toneMapper,
        albedo,
        normal,
        vertex,
    )

    print("[Info] Build completed")
except ValueError as err:
    print(f"[Error] {err}")
    exit(1)
