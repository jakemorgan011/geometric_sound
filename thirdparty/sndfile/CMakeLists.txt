find_package(SndFile QUIET)
if(SndFile_FOUND)
  message(STATUS "found preinstalled sndfile library at ${SndFile_LIBRARIES}")
  add_library(SndFile_external INTERFACE)
else()
  message(STATUS "Preinstalled sndfile not found.")
  message(STATUS "will download and install sndfile in ${STAGED_INSTAL_PREFIX}")
include(ExternalProject)
  ExternalProject_Add(SndFile_external
    GIT_REPOSITORY
    https://github.com/libsndfile/libsndfile
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
    -DBUILD_SHARED_LIBS=OFF
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
  set(SndFile_ROOT_DIR ${STAGED_INSTALL_PREFIX})
  file(TO_NATIVE_PATH "${SndFile_ROOT_DIR}" SndFile_ROOT_DIR)
  set(SndFile_ROOT_DIR ${SndFile_ROOT_DIR} CACHE INTERNAL "SndFile installation dir")
  
  set(ENV_SNDFILE_CONFIG ${STAGED_INSTALL_PREFIX}/bin/SndFile-config)
  file(TO_NATIVE_PATH "${ENV_SNDFILE_CONFIG}" ENV_SNDFILE_CONFIG)
  set(ENV_SNDFILE_CONFIG ${ENV_SNDFILE_CONFIG} CACHE INTERNAL "SndFile-config dir")
endif()
