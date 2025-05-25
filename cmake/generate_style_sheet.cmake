# generate_style_sheet.cmake
if(EXISTS "${INPUT}")
    file(READ "${INPUT}" STYLE_CONTENT)
else()
    set(STYLE_CONTENT "")
endif()

file(WRITE "${OUTPUT}" "#pragma once\n")
file(APPEND "${OUTPUT}" "static constexpr const char* __default_style_sheet = R\"(${STYLE_CONTENT})\";\n")
