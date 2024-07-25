from os.path import dirname, exists, join, relpath


def link_shader(shader: str) -> str:
    if not exists(shader):
        raise ValueError(f"Shader not found ({relpath(shader)})")

    result = ""
    with open(shader, "r") as file:
        for line in file.readlines():
            if line.startswith("#include"):
                include = line.removeprefix("#include ").strip()
                result += link_shader(join(dirname(shader), include))
            else:
                result += line

    return result


def write_shaders(path: str, shader: str) -> None:
    with open(path, "w") as file:  
        file.write("#include <TracerX/Renderer.h>\n\n")
        file.write("using namespace TracerX;\n\n")

        file.write('const char* Renderer::shaderSrc =\nR"(\n')
        file.write(shader)
        file.write('\n)";\n')


project = join(dirname(__file__), "..")
shaders = join(project, "shaders")

try:
    shader = link_shader(join(shaders, "main.comp"))
    print("[Info] Link completed")

    write_shaders(
        join(project, "core", "src", "RendererShaderSrc.cpp"),
        shader,
    )

    print("[Info] Write completed")
    print("[Info] Build completed")
except ValueError as err:
    print(f"[Error] {err}")
    exit(1)
