cmake_minimum_required(VERSION 3.5.1)
project(E2SAR_ERSAP_REASSEMBLER CXX)

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

find_package(Boost COMPONENTS system url thread chrono random program_options REQUIRED )
find_package(Protobuf REQUIRED)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Ersap REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_program(PKG_CONFIG_EXECUTABLE pkg-config)
pkg_check_modules(E2SAR REQUIRED e2sar)
pkg_check_modules(GRPC REQUIRED grpc++)

include_directories(${E2SAR_INCLUDE_DIRS})
link_directories(${E2SAR_LIBRARY_DIRS})
message(STATUS "E2SAR_INCLUDE_DIRS: ${E2SAR_INCLUDE_DIRS}")
message(STATUS "E2SAR linker flags: ${E2SAR_LIBRARIES}")
message(STATUS "GRPC linker flags: ${GRPC_LIBRARIES}")
message(STATUS "Protobuf linker flags: ${Protobuf_LIBRARIES}")

add_library(ersapReassembler SHARED src/cpp/reassembler_actor.cpp )
target_link_libraries(ersapReassembler Ersap::ersap ${E2SAR_LIBRARIES}
${Boost_LIBRARIES} ${GRPC_LIBRARIES} ${Protobuf_LIBRARIES} Threads::Threads)

if (CMAKE_CXX_COMPILER_ID MATCHES Clang)
  target_compile_options(ersapReassembler PRIVATE -Wno-return-type-c-linkage)
endif()
set_target_properties(ersapReassembler PROPERTIES OUTPUT_NAME "reassembler_actor")
install(TARGETS ersapReassembler DESTINATION "$ENV{ERSAP_HOME}/lib") 