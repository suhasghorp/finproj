cmake_minimum_required(VERSION 3.14...3.26)
project(
        finproj
        VERSION 0.1
        DESCRIPTION "An example project with CMake"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20) # C14 required to compile Torch
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here

    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDEs
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Testing only available if this is the main app
    # Note this needs to be done in the main CMakeLists
    # since it calls enable_testing, which must be in the
    # main CMakeLists.
    include(CTest)
endif()

# FetchContent added in CMake 3.11, downloads during the configure step
# FetchContent_MakeAvailable was added in CMake 3.14; simpler usage
include(FetchContent)

# Accumulator library
# This is header only, so could be replaced with git submodules or FetchContent
#find_package(Boost REQUIRED)
# Adds Boost::boost
find_package(Eigen3 REQUIRED NO_MODULE)

find_package(libInterpolate REQUIRED)

#find_package(dlib REQUIRED)

find_package(Boost REQUIRED)
set(Python3_ROOT_DIR /home/sghorp/miniconda3)

find_package (Python3 COMPONENTS Interpreter Development)

#add_library(QuantLib::QuantLib STATIC IMPORTED)

# The compiled library code is here
add_subdirectory(src)
# The executable code is here
add_subdirectory(app)

# Testing only available if this is the main app
# Emergency override MODERN_CMAKE_BUILD_TESTING provided as well
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR MODERN_CMAKE_BUILD_TESTING)
        AND BUILD_TESTING)
    add_subdirectory(tests)
endif()

