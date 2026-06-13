function(compile_shaders i_file o_file spir_v)
    if(spir_v)
        execute_process(
            COMMAND glslc -Werror -O ${i_file} -o ${o_file}
            RESULT_VARIABLE RES
        )
    else()
        execute_process(
            COMMAND glslc -Werror -O -E ${i_file} -o ${o_file}
            RESULT_VARIABLE RES
        )
    endif()

    if(RES)
        message(FATAL_ERROR ${RES})
    endif()
endfunction()

# Check if glslc is available
execute_process(COMMAND glslc -h OUTPUT_QUIET RESULT_VARIABLE MISSING_GLSLC)
if(MISSING_GLSLC)
    message(NOTICE "glslc tool missing. Cannot compile shaders")
    return()
endif()

# Compile shaders
file(MAKE_DIRECTORY ${OUTPUT_DIR})
compile_shaders(${SHADERS_DIR}/accumulate/main.comp ${OUTPUT_DIR}/accum.comp       FALSE)
compile_shaders(${SHADERS_DIR}/toneMap/main.comp    ${OUTPUT_DIR}/toneMap.comp     FALSE)
compile_shaders(${SHADERS_DIR}/accumulate/main.comp ${OUTPUT_DIR}/accum.comp.spv   TRUE )
compile_shaders(${SHADERS_DIR}/toneMap/main.comp    ${OUTPUT_DIR}/toneMap.comp.spv TRUE )
