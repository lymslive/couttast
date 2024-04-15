cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(couttast CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(couttast_VERSION_MAJOR "0")
set(couttast_VERSION_MINOR "6")

#include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# libtinytast.a only has a (week) main function.
add_library(tinytast STATIC src/tinytast.cpp)

# libcouttast.a
# Has many extended features as well as a (week) main function.
add_library(couttast STATIC
    src/couttast.cpp
    src/agent.cpp
    src/colour.cpp
    src/filter.cpp
    src/format.cpp
    src/parallel.cpp
    src/tinyini.cpp
    src/file-compare.cpp
    src/xmltast.cpp
    src/systemtast.cpp
    #src/tast_drive.cpp
    #src/tinytast.cpp
)

# A simple program to run dynamic library compileed from client unittest.
add_executable(tast_drive src/tast_drive.cpp)
target_link_libraries(tast_drive
    PRIVATE dl
)

# An unittest program to test libcouttast.a itself.
add_executable(utCoutTast
    utest/test-agent.cpp
    utest/test-exception.cpp
    utest/test-filter.cpp
    utest/test-format.cpp
    utest/test-parallel.cpp
    utest/test-tinyini.cpp
    utest/test-tinytast.cpp
    utest/test-file-compare.cpp
    utest/test-classtast.cpp
    utest/test-systemtast.cpp
)

target_link_libraries(utCoutTast
    PRIVATE couttast
)

# An unitttest to test tinytast.hpp only for C++98 compatibility.
add_executable(utCxx98Tast
    utest/test-tinytast.cpp
    utest/test-classtast.cpp
)

target_compile_definitions(utCxx98Tast PRIVATE USE_TINY_MAIN)

# enable ctest, make test
enable_testing()
add_test(NAME utCoutTast
    COMMAND utCoutTast
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/utest
)

add_test(NAME utCxx98Tast
    COMMAND utCxx98Tast
)

option(BUILD_EXAMPLE "build example" OFF)
if(BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

# Only install this necessary header files to include/couttast
set(COUTTAST_HEADER
    include/couttast.h
    include/tinytast.hpp
    include/tinymain.hpp
    include/coutstd.hpp
    include/extra-macros.hpp
    include/gtest-macros.hpp
    include/logprint.hpp
    include/tast_util.hpp
    include/private-access.hpp
    include/classtast.hpp
    #include/tinyini.h
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

# to install $HOME use:
# cmake .. -DCMAKE_INSTALL_PREFIX=$HOME
message("would install to: ${CMAKE_INSTALL_PREFIX}")

install(FILES ${COUTTAST_HEADER}
    DESTINATION "include/couttast"
)

install(TARGETS tinytast
    DESTINATION "lib"
)

install(TARGETS couttast
    DESTINATION "lib"
)

install(TARGETS tast_drive
    DESTINATION "bin"
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/couttastConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/couttastConfigVersion.cmake
    DESTINATION lib/cmake/couttast
)
