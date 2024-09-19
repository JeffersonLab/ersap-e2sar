cmake_minimum_required(VERSION 3.5.1)
project(E2SAR_ERSAP CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    Debug Release MinSizeRel RelWithDebInfo)
endif()

if(DEFINED ENV{ERSAP_HOME} AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
# Provide useful default paths when $ERSAP_HOME is defined
set(CMAKE_INSTALL_PREFIX "$ENV{ERSAP_HOME}" CACHE PATH "" FORCE)
list(APPEND CMAKE_PREFIX_PATH "$ENV{ERSAP_HOME}")
endif()


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Ersap REQUIRED)
find_package(PkgConfig REQUIRED)
find_program(PKG_CONFIG_EXECUTABLE pkg-config)
pkg_check_modules(E2SAR REQUIRED e2sar)

include_directories(${E2SAR_INCLUDE_DIRS})
link_directories(${E2SAR_LIBRARY_DIRS})
message(STATUS "E2SAR_INCLUDE_DIRS: ${E2SAR_INCLUDE_DIRS}")
message(STATUS "E2SAR_LIBRARY_DIRS: ${E2SAR_LIBRARY_DIRS}")

add_library(ersapSegmentor SHARED src/segmentor_actor.cpp include/segmentor_actor.hpp)
target_link_libraries(ersapSegmentor Ersap::ersap ${E2SAR_LIBS})
install(TARGETS ersapSegmentor DESTINATION)