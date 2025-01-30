cmake_minimum_required(VERSION 3.16)

project(ServiceMaker)
find_package(nvds_service_maker REQUIRED PATHS /opt/nvidia/deepstream/deepstream/service-maker/cmake)

add_executable(my-deepstream-app my_deepstream_app.cpp)
target_link_libraries(my-deepstream-app PRIVATE  nvds_service_maker)