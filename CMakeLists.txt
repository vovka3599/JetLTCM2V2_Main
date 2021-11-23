cmake_minimum_required (VERSION 3.8)

project ("JetLTCM2V2_Main" LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    main.cpp
    can_h/can.h
    can_h/adapter.h
    can_h/command.h
    can_h/control.h
    can_h/groups.h
    can_h/scan.h

    can_s/can.cpp
    can_s/control.cpp
    can_s/scan.cpp

    udp.h
    udp.cpp

    JetLTCM.h
    JetLTCM.cpp

    config.h

    cuda_f.h
    )

add_library(cuda STATIC
    cuda.cu
)

target_compile_features(cuda PUBLIC cxx_std_11)
set_target_properties(cuda PROPERTIES CUDA_SEPARABLE_COMPILATION ON)

add_executable (JetLTCM2V2_Main ${SOURCES})

set_property(TARGET JetLTCM2V2_Main PROPERTY CUDA_SEPARABLE_COMPILATION ON)
target_link_libraries(JetLTCM2V2_Main pthread cuda)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/can_h)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/can_h)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/can_s)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/can_s)