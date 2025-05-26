# check if wxWidgets is already installed in the system - using CMake's built in script FindwxWidgets
# probably gonna have to do local install cause this shit is breaking non stop.
set(wxWidgets_CONFIGURATION "Debug")

find_package(wxWidgets QUIET)
if(wxWidgets_FOUND)
    message(STATUS "Found preinstalled wxWidgets libraries at ${wxWidgets_LIBRARIES}")
    add_library(wxWidgets_external INTERFACE)
else()
  # doing local install because external project is breaking.
  set(wxWidgets_ROOT_DIR "/opt/homebrew/opt/wxwidgets")
  find_package(wxWidgets COMPONENTS core base gl REQUIRED)
  include(${wxWidgets_USE_FILE})
  if(wxWidgets_USE_DEBUG)
    add_definitions(-D__WXDEBUG__)
    message(STATUS "Using DEBUG wxWidgets build")
  endif()
  if(NOT wxWidgets_USE_DEBUG)
    message(FATAL_ERROR "wxWidgets debug libraries not found!")
  endif()
  add_library(wxWidgets INTERFACE)
  
  set(ENV_WX_CONFIG ${STAGED_INSTALL_PREFIX}/bin/wx-config)
  file(TO_NATIVE_PATH "${ENV_WX_CONFIG}" ENV_WX_CONFIG)
  set(ENV_WX_CONFIG ${ENV_WX_CONFIG} CACHE INTERNAL "wx-config dir")
endif()
