from os.path import dirname, join


def build_shader(shader: str) -> str:
    result = ""
    with open(shader, "r") as file:
        for line in file.readlines():
            if line.startswith("#include"):
                include = line.removeprefix("#include ").strip()
                result += build_shader(join(dirname(shader), include))
            else:
                result += line

    return result


def write_shaders(path: str, pathTracer: str, toneMapper: str, vertex: str) -> None:
    with open(path, "w") as file:
        file.write("#pragma once\n\n\n")

        file.write(f'const char* pathTracerShader =\nR"(\n')
        file.write(pathTracer)
        file.write(f'\n)";\n\n')

        file.write(f'const char* toneMapperShader =\nR"(\n')
        file.write(toneMapper)
        file.write(f'\n)";\n\n')

        file.write(f'const char* vertexShader =\nR"(\n')
        file.write(vertex)
        file.write(f'\n)";\n\n')


project = join(dirname(__file__), "..")
shaders = join(project, "shaders")

pathTracer = build_shader(join(shaders, "fragment", "pathTracer", "main.glsl"))
toneMapper = build_shader(join(shaders, "fragment", "toneMapper", "main.glsl"))
vertex = build_shader(join(shaders, "vertex", "main.glsl"))

write_shaders(
    join(project, "include", "TracerX", "ShaderSrc.h"), pathTracer, toneMapper, vertex
)
