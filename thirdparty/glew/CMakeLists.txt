# check if GLEW is already installed in the system - using CMake's built in script FindGLEW
find_package(GLEW QUIET)

if(GLEW_FOUND)
    message(STATUS "Found preinstalled GLEW libraries at ${GLEW_LIBRARIES}")
    add_library(GLEW_external INTERFACE)
else()
  set(GLEW_ROOT_DIR "/opt/homebrew/opt/glew")
  find_package(GLEW REQUIRED)
  add_library(GLEW_external INTERFACE)
endif()
