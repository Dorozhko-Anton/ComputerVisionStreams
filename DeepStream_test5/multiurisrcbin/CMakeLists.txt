cmake_minimum_required(VERSION 3.16)

project(ServiceMaker)
find_package(nvds_service_maker REQUIRED PATHS /opt/nvidia/deepstream/deepstream/service-maker/cmake)

add_executable(multiutisrc_sample multiutisrc_sample.cpp)
target_link_libraries(multiutisrc_sample PRIVATE  nvds_service_maker)