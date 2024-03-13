cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(Nong VERSION 1.0.0)

file(GLOB_RECURSE SOURCES
    libs/geometrize-lib/geometrize/geometrize/*.cpp
    libs/geometrize-lib/geometrize/geometrize/bitmap/*.cpp
    src/*.cpp
)

# add_library(${PROJECT_NAME} SHARED ${SOURCES})


# Set up the mod binary
add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    # Add your cpp files here
)


if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "Unable to find Geode SDK! Please define GEODE_SDK environment variable to point to Geode")
else()
    message(STATUS "Found Geode: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)


target_include_directories(${PROJECT_NAME} PRIVATE libs/stb)
target_include_directories(${PROJECT_NAME} PRIVATE libs/geometrize-lib/geometrize)

# add_library(${PROJECT_NAME} ${GEOMETRIZE_SRC})

# find_package(Python3 REQUIRED)
# target_include_directories(${PROJECT_NAME} PRIVATE ${Python3_INCLUDE_DIRS})
# target_link_libraries(${PROJECT_NAME} "${Python3_LIBRARIES}")

# Uncomment the following lines if 'python3-embed' is required
# find_package(Python3 COMPONENTS Embed)
# target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Embed)

target_link_libraries(${PROJECT_NAME} ${SPLAT_DIR}/sdk/lib/um/x86/Gdi32.Lib)
# add_compile_options("-march=skylake")
# add_compile_definitions(WIN32_LEAN_AND_MEAN=1) # geode moment
# add_definitions(/FI"WinSock2.h")
# target_compile_options(${PROJECT_NAME} PRIVATE "/arch:SSE")

# Set up dependencies, resources, link Geode
setup_geode_mod(${PROJECT_NAME})
