find_package(rtaudio QUIET)
if(rtaudio_FOUND)
  message(STATUS "found preinstalled rtaudio library at ${rtaudio_LIBRARIES}")
  add_library(rtaudio_external INTERFACE)
else()
  message(STATUS "preinstalled rtaudio not found.")
  message(STATUS "will download and install rtaudio in ${STAGED_INSTALL_PREFIX}")
  include(ExternalProject)
  ExternalProject_Add(rtaudio_external
    GIT_REPOSITORY
    https://github.com/thestk/rtaudio
    GIT_TAG
    master
    GIT_SHALLOW
    1
    UPDATE_COMMAND
    ""
    BUILD_COMMAND
    ""
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=${STAGED_INSTALL_PREFIX}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    TEST_AFTER_INSTALL
    0
    DOWNLOAD_NO_PROGRESS
    1
    LOG_CONFIGURE
    1
    LOG_BUILD
    1
    LOG_INSTALL
    1
  )
  set(rtaudio_ROOT_DIR ${STAGED_INSTALL_PREFIX})
  file(TO_NATIVE_PATH "${rtaudio_ROOT_DIR}" rtaudio_ROOT_DIR)
  set(rtaudio_ROOT_DIR ${rtaudio_ROOT_DIR} CACHE INTERNAL "rtaudio installation dir")

  set(ENV_RTAUDIO_CONFIG ${STAGED_INSTALL_PREFIX}/bin/rtaudio-config)
  file(TO_NATIVE_PATH "${ENV_RTAUDIO_CONFIG}" ENV_RTAUDIO_CONFIG)
  set(ENV_RTAUDIO_CONFIG ${ENV_SNDFILE_CONFIG} CACHE INTERNAL "rtaudio-config dir")
endif()
