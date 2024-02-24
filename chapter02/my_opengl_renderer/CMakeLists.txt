cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# use custom file to find libraries
if(WIN32)
  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
endif()

project(Main)

file(GLOB SOURCES
  Main.cpp
  opengl_renderer/*.cpp
  window/*.cpp
  model/*.cpp
  tools/*.cpp
  src/glad.c
)

file(GLOB HEADERS
  opengl_renderer/*.h
  model/*.h
  window/*.h
  tools/*.h
)

add_executable(Main ${SOURCES} ${HEADERS})

target_include_directories(Main PUBLIC include src window tools opengl_renderer model)

find_package(glfw3 3.3 REQUIRED)
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# variable is set by FindGLFW3.cmake, reuse for Linux
if(UNIX)
  set(GLFW3_LIBRARY glfw)
endif()

include_directories(${GLFW3_INCLUDE_DIR} ${GLM_INCLUDE_DIR})

if(MSVC)
  target_link_libraries(Main ${GLFW3_LIBRARY} OpenGL::GL)
else()
  # Clang and GCC may need libstd++
  target_link_libraries(Main ${GLFW3_LIBRARY} OpenGL::GL stdc++ m)
endif()
