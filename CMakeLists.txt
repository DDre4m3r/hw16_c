cmake_minimum_required(VERSION 3.10)
project(hw16 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)

include_directories(
        external/glfw/include
        external/glad/include
        external/
        include
)



add_executable(rotating_cube
        external/glad/src/gl.c
        src/main.c
        src/shader.c
)

target_link_libraries(rotating_cube glfw ${CMAKE_DL_LIBS})

if(APPLE)
    target_link_libraries(rotating_cube "-framework OpenGL")
elseif(UNIX)
    target_link_libraries(rotating_cube GL)
elseif(WIN32)
    target_link_libraries(rotating_cube opengl32)
endif()
