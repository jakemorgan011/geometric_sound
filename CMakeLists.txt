cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(wx_gl_cube_tutorial LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ExternalProject base
set_property(DIRECTORY PROPERTY EP_BASE ${CMAKE_BINARY_DIR}/subprojects)

set(STAGED_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/stage)
set(EP_SOURCE_DIR_PREFIX ${CMAKE_BINARY_DIR}/subprojects/Source)

add_subdirectory(thirdparty)

include(ExternalProject)
ExternalProject_Add(${PROJECT_NAME}_core
  DEPENDS
  wxWidgets_external
  GLEW_external
  glm_external
  SndFile_external
  rtaudio_external
  SOURCE_DIR
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  CMAKE_ARGS
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  -DGLEW_DIR=${GLEW_DIR}
  -DwxWidgets_ROOT_DIR=${wxWidgets_ROOT_DIR}
  -DENV_WX_CONFIG=${ENV_WX_CONFIG}
  CMAKE_CACHE_ARGS
  -DCMAKE_PREFIX_PATH:PATH=${CMAKE_PREFIX_PATH}
  BUILD_ALWAYS
  1
  INSTALL_COMMAND
  ""
)
