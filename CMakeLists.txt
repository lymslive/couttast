cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(couttast CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

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

set(CMAKE_INSTALL_PREFIX ${HOME})
install(FILES ${HPP_HEADER}
    DESTINATION "include/couttast"
)

install(TARGETS tast_main
    DESTINATION "lib"
)
