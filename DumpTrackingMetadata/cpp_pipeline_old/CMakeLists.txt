cmake_minimum_required(VERSION 3.20)
project(OldDeepstream)

find_package(PkgConfig REQUIRED)
# Packages needed for the build
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)

# Directories to include
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(/opt/nvidia/deepstream/deepstream/sources/includes/)

# Libraries to build
link_directories(${GSTREAMER_LIBRARY_DIRS})
link_directories(/opt/nvidia/deepstream/deepstream/lib)


# json 
find_package(nlohmann_json 3.2.0 REQUIRED)



add_executable(${PROJECT_NAME} deepstream_pipeline.cpp)

target_link_libraries(${PROJECT_NAME} 
                        PRIVATE
                        ${GSTREAMER_LIBRARIES} 
                        nvdsgst_meta
                        nlohmann_json::nlohmann_json
                        )