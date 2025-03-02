cmake_minimum_required(VERSION 3.16)

project(ServiceMakerCustomPayload)
find_package(nvds_service_maker REQUIRED PATHS /opt/nvidia/deepstream/deepstream/service-maker/cmake)

add_executable(custom-payload custom_payload.cpp)
target_link_libraries(custom-payload PRIVATE  nvds_service_maker)