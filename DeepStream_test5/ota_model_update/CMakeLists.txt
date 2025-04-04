cmake_minimum_required(VERSION 3.16)

project(ota_model_update_sample)
find_package(nvds_service_maker REQUIRED PATHS /opt/nvidia/deepstream/deepstream/service-maker/cmake)

add_executable(ota_model_update_sample ota_model_update_sample.cpp)
target_link_libraries(ota_model_update_sample PRIVATE  nvds_service_maker)