cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(couttast CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(couttast_VERSION_MAJOR "0")
set(couttast_VERSION_MINOR "2")

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(tast_main src/tast_main.cpp)

add_executable(tast_drive src/tast_drive.cpp)
target_link_libraries(tast_drive
    PRIVATE dl
)

option(BUILD_EXAMPLE "build example" ON)

if(BUILD_EXAMPLE)
    enable_testing()
    add_subdirectory(example)
endif()

set(HPP_HEADER
    include/tinytast.hpp
    include/logprint.hpp
)

# package config
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/couttastConfig.cmake"
    INSTALL_DESTINATION "lib/cmake/couttast"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/couttastConfigVersion.cmake"
    VERSION "${couttast_VERSION_MAJOR}.${couttast_VERSION_MINOR}"
    COMPATIBILITY AnyNewerVersion
)

# install
set(CMAKE_INSTALL_PREFIX ${HOME})
install(FILES ${HPP_HEADER}
    DESTINATION "include/couttast"
)

install(TARGETS tast_main
    DESTINATION "lib"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/couttastConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/couttastConfigVersion.cmake
    DESTINATION lib/cmake/couttast
)
