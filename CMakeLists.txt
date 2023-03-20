cmake_minimum_required(VERSION 3.0.0)
project(learnopengl VERSION 0.1.0)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(./src SrcFiles)
add_executable(learnopengl ./src/stb_image.cpp ./src/Camera.cpp ./src/Shader.cpp ./src/Mesh.cpp ./src/Model.cpp ./src/Modeling.cpp)

include(CPack)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)

target_link_libraries(learnopengl PRIVATE glad::glad)
target_link_libraries(learnopengl PRIVATE glfw)
target_link_libraries(learnopengl PRIVATE assimp::assimp)