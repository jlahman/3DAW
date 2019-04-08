cmake_minimum_required (VERSION 2.6)
project (3DAW)
#Project Version number
set (3DAW_VERSION_MAJOR 0.0)
set (3DAW_VERSION_MINOR 0.1)
set (PROJECT_ROOT_DIR "..")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_ROOT_DIR}/lib)
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_ROOT_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_ROOT_DIR}/bin)
set (PROJECT_SOURCE_DIR "${PROJECT_ROOT_DIR}/src")
set (PROJECT_BINARY_DIR "${PROJECT_ROOT_DIR}/bin")

#libraries system
set (PROJECT_SYSTEM_LIBS avformat avcodec swresample avutil portaudio matio stdc++ m)

file(GLOB PROJECT_SRC
    "${PROJECT_SOURCE_DIR}/*.h"
    "${PROJECT_SOURCE_DIR}/*.cpp"
)

add_executable(audio_test ${PROJECT_SRC})
target_link_libraries(audio_test ${PROJECT_SYSTEM_LIBS})

install(TARGETS audio_test RUNTIME DESTINATION ${PROJECT_BINARY_DIR})
