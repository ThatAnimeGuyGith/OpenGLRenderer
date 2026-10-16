cmake_minimum_required(VERSION 3.24)

# Run by the package_release target. Not meant to be called by hand.
# Builds release/<version>_<date>/ containing the program folder and a zip.

string(TIMESTAMP DATE "%Y-%m-%d")

set(NAME       "OpenGLRenderer-${VERSION}")
set(OUT_DIR    "${SOURCE_DIR}/release/${VERSION}_${DATE}")
set(STAGE_DIR  "${OUT_DIR}/${NAME}")
set(ZIP_NAME   "${NAME}-${PLATFORM}.zip")
set(ARCHIVE_PATH "${OUT_DIR}/${ZIP_NAME}")

file(REMOVE_RECURSE "${STAGE_DIR}")
file(REMOVE "${ARCHIVE_PATH}" "${PACKAGE_PATH_FILE}")
file(MAKE_DIRECTORY "${OUT_DIR}")

execute_process(
    COMMAND ${CMAKE_COMMAND} --install "${BINARY_DIR}" --prefix "${STAGE_DIR}" --component Runtime
    COMMAND_ERROR_IS_FATAL ANY
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar cf "${ZIP_NAME}" --format=zip -- "${NAME}"
    WORKING_DIRECTORY "${OUT_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar tf "${ARCHIVE_PATH}"
    OUTPUT_VARIABLE ARCHIVE_CONTENTS
    COMMAND_ERROR_IS_FATAL ANY
)
string(REPLACE "\r\n" "\n" ARCHIVE_CONTENTS "${ARCHIVE_CONTENTS}")
string(REPLACE "\n" ";" ARCHIVE_ENTRIES "${ARCHIVE_CONTENTS}")
list(REMOVE_ITEM ARCHIVE_ENTRIES "")

set(REQUIRED_ENTRIES
    "${NAME}/OpenGLRenderer${EXECUTABLE_SUFFIX}"
    "${NAME}/assets/config/settings.json"
    "${NAME}/assets/meshes/cube/cube.obj"
    "${NAME}/assets/shaders/vertexShader.vert"
    "${NAME}/assets/shaders/fragmentShader.frag"
    "${NAME}/LICENSE"
    "${NAME}/README.md"
    "${NAME}/THIRD_PARTY_LICENSES.txt"
)
foreach(required_entry IN LISTS REQUIRED_ENTRIES)
    if(NOT required_entry IN_LIST ARCHIVE_ENTRIES)
        message(FATAL_ERROR "Release package is missing required file: ${required_entry}")
    endif()
endforeach()

foreach(entry IN LISTS ARCHIVE_ENTRIES)
    if(NOT entry MATCHES "^${NAME}/")
        message(FATAL_ERROR "Release package contains an unexpected path: ${entry}")
    endif()
    if(entry MATCHES "(^|/)(build|tests|CMakeFiles|_deps)(/|$)"
       OR entry MATCHES "(^|/)(CMakeCache\\.txt|compile_commands\\.json)$")
        message(FATAL_ERROR "Release package contains a development file: ${entry}")
    endif()
endforeach()

file(WRITE "${PACKAGE_PATH_FILE}" "${ARCHIVE_PATH}\n")

message(STATUS "Release ready: ${ARCHIVE_PATH}")
