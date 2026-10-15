# Run by the package_release target. Not meant to be called by hand.
# Builds release/<version>_<date>/ containing the program folder and a zip.

string(TIMESTAMP DATE "%Y-%m-%d")

set(NAME       "OpenGLRenderer-${VERSION}")
set(OUT_DIR    "${SOURCE_DIR}/release/${VERSION}_${DATE}")
set(STAGE_DIR  "${OUT_DIR}/${NAME}")
set(ZIP_NAME   "${NAME}-${PLATFORM}.zip")

file(REMOVE_RECURSE "${OUT_DIR}")
file(MAKE_DIRECTORY "${OUT_DIR}")

execute_process(
    COMMAND ${CMAKE_COMMAND} --install "${BINARY_DIR}" --prefix "${STAGE_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar cf "${ZIP_NAME}" --format=zip -- "${NAME}"
    WORKING_DIRECTORY "${OUT_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
)

message(STATUS "Release ready: ${OUT_DIR}/${ZIP_NAME}")
