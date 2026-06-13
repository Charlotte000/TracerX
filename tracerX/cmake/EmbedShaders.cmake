# Check if there are compiled shaders to embed
if(
    NOT EXISTS ${SHADERS_DIR}/accum.comp       OR
    NOT EXISTS ${SHADERS_DIR}/toneMap.comp     OR
    NOT EXISTS ${SHADERS_DIR}/accum.comp.spv   OR
    NOT EXISTS ${SHADERS_DIR}/toneMap.comp.spv
)
    message(FATAL_ERROR "Missing shader files")
endif()

# Embed compiled shaders into cpp file
file(READ ${SHADERS_DIR}/accum.comp accumShaderText)

file(READ ${SHADERS_DIR}/toneMap.comp toneMapShaderText)

file(READ ${SHADERS_DIR}/accum.comp.spv accumShaderBin HEX)
string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1, " accumShaderBin "${accumShaderBin}")

file(READ ${SHADERS_DIR}/toneMap.comp.spv toneMapShaderBin HEX)
string(REGEX REPLACE "([0-9a-fA-F][0-9a-fA-F])" "0x\\1, " toneMapShaderBin "${toneMapShaderBin}")

file(
    WRITE
    ${OUTPUT_FILE}
"\
#include <cstdint>
#include <iterator>

#if TX_SPIRV
extern const uint8_t accumShaderSrc[] =
{
    ${accumShaderBin}
};

extern const size_t accumShaderSrcSize = std::size(accumShaderSrc) * sizeof(uint8_t);

extern const uint8_t toneMapShaderSrc[] =
{
    ${toneMapShaderBin}
};

extern const size_t toneMapShaderSrcSize = std::size(toneMapShaderSrc) * sizeof(uint8_t);
#else
extern const char accumShaderSrc[] = R\"AccumShaderSrc(
${accumShaderText}
)AccumShaderSrc\";

extern const char toneMapShaderSrc[] = R\"ToneMapShaderSrc(
${toneMapShaderText}
)ToneMapShaderSrc\";
#endif
"
)
