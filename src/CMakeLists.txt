cmake_minimum_required(VERSION 3.6 FATAL_ERROR)

project(wx_gl_cube_tutorial_core LANGUAGES CXX)

set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE INTERNAL "")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# hack for buggy CMake's FindwxWidgets script
#if(DEFINED ENV_WX_CONFIG)
#    set(ENV{WX_CONFIG} ${ENV_WX_CONFIG})
#endif()

set(wxWidgets_USE_STATIC 1)

find_package(wxWidgets COMPONENTS core base gl REQUIRED)
include(${wxWidgets_USE_FILE})
message(STATUS "wxwidgets library: ${wxWidgets_LIRARIES}")
message(STATUS "wxWidgets lib dir: ${wxWidgets_LIB_DIR}")

message(STATUS "GLEW_DIR is... ${GLEW_DIR}")

set(GLEW_USE_STATIC_LIBS ON)
find_package(GLEW REQUIRED CONFIG)
# if you are confused about this remember that for some reason you have sndfile installed through hombrew
# this way of using sndfile is only doable with the global install of a library.
find_package(SndFile REQUIRED)


set(SRCS main.cpp openglcanvas.cpp audio.cpp)


if(APPLE)
    # create bundle on apple compiles
    add_executable(main MACOSX_BUNDLE ${SRCS})

    # Set a custom plist file for the app bundle - needed for Mac OS Retina display
    set_target_properties(main PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
    target_link_libraries(main PRIVATE "-framework IOKit -framework Carbon -framework Cocoa") 
else()
    # the WIN32 is needed for Windows in order for it to look for WinMain
    # instead of the main function. This is ignored on other systems,
    # so it works on all platforms
    add_executable(main WIN32 ${SRCS} main.exe.manifest)
endif()
# GLEW::glew_S
target_link_libraries(main PRIVATE ${wxWidgets_LIBRARIES} SndFile::sndfile GLEW::glew_s rtaudio)
target_include_directories(main PRIVATE ${wxWidgets_INCLUDE_DIRS} PUBLIC /opt/homebrew/include)
