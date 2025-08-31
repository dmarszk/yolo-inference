cmake_minimum_required(VERSION 3.23)

option(WITH_LIBTORCH "build _YOLO_LIBTORCH" ON)
option(WITH_ONNXRUNTIME "build _YOLO_ONNXRUNTIME" ON)
option(WITH_OPENCV "build _YOLO_OPENCV" ON)
option(WITH_OPENVINO "build _YOLO_OPENVINO" ON)
option(WITH_TENSORRT "build _YOLO_TENSORRT" ON)
option(WITH_CUDA "build _WITH_CUDA" ON)
option(WITH_CUDA_PREPROCESS "build _CUDA_PREPROCESS" ON)
option(WITH_CUDA_POSTPROCESS "build _CUDA_POSTPROCESS" ON)

if(WITH_CUDA)
    # Try to find nvcc in PATH, common CUDA locations or via CUDA_HOME/CUDA_PATH
    find_program(NVCC_EXECUTABLE nvcc
        HINTS
            ENV CUDA_HOME
            ENV CUDA_PATH
            /usr/local/cuda
            /usr/local
            /opt/cuda
        PATHS
            /usr/bin
            /usr/local/bin
        PATH_SUFFIXES bin
    )

    if(NVCC_EXECUTABLE)
        message(STATUS "Found nvcc: ${NVCC_EXECUTABLE}")
        set(CMAKE_CUDA_COMPILER "${NVCC_EXECUTABLE}" CACHE FILEPATH "CUDA compiler" FORCE)
        project(yolo LANGUAGES C CXX CUDA)
    else()
        message(WARNING "WITH_CUDA is ON but nvcc was not found. Configuring project without CUDA language.")
        set(WITH_CUDA OFF CACHE BOOL "Disable CUDA support because nvcc not found" FORCE)
        project(yolo LANGUAGES C CXX)
    endif()
else()
    project(yolo LANGUAGES C CXX)
endif()

file(GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_compile_options(-w)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Release")

if(WITH_LIBTORCH)
    add_compile_options(-D_YOLO_LIBTORCH)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/libtorch SRCS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libtorch)
endif()

if(WITH_ONNXRUNTIME)
    add_compile_options(-D_YOLO_ONNXRUNTIME)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime SRCS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/onnxruntime)
endif()

if(WITH_OPENCV)
    add_compile_options(-D_YOLO_OPENCV)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/opencv SRCS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/opencv)
endif()

if(WITH_OPENVINO)
    add_compile_options(-D_YOLO_OPENVINO)
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/openvino SRCS)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/openvino)
endif()

if(WITH_CUDA AND WITH_TENSORRT)
    add_compile_options(-D_YOLO_TENSORRT)
    list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tensorrt/yolo_tensorrt.h ${CMAKE_CURRENT_SOURCE_DIR}/tensorrt/yolo_tensorrt.cpp)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/tensorrt)
    if(WITH_CUDA_PREPROCESS)
        list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tensorrt/preprocess.cuh ${CMAKE_CURRENT_SOURCE_DIR}/tensorrt/preprocess.cu)
        add_compile_options(-D_CUDA_PREPROCESS)
        if(WITH_CUDA_POSTPROCESS)
            list(APPEND SRCS ${CMAKE_CURRENT_SOURCE_DIR}/tensorrt/decode.cuh ${CMAKE_CURRENT_SOURCE_DIR}/tensorrt/decode.cu)
            add_compile_options(-D_CUDA_POSTPROCESS)
        endif()
    endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")   
    add_executable(yolo ${SRCS})

    if(WITH_CUDA)
        find_package(CUDAToolkit REQUIRED)
        include_directories(${CUDA_INCLUDE_DIRS})
        link_directories(${CUDA_LIB_PATH})
        target_link_libraries(yolo CUDA::cudart)
    endif()

    set(OPENCV_INCLUDE_DIRS D:/opencv-4.10.0/install/include)
    set(OPENCV_LIBRARY_DIRS D:/opencv-4.10.0/install/x64/vc17/lib)
    include_directories(${OPENCV_INCLUDE_DIRS})
    link_directories(${OPENCV_LIBRARY_DIRS})
    target_link_libraries(yolo ${OPENCV_LIBRARY_DIRS}/*.lib)

    if(WITH_LIBTORCH)
        set(LIBTORCH_INCLUDE_DIRS D:/libtorch-win-shared-with-deps-2.5.0+cu124/include D:/libtorch-win-shared-with-deps-2.5.0+cu124/include/torch/csrc/api/include)
        set(LIBTORCH_LIBRARY_DIRS D:/libtorch-win-shared-with-deps-2.5.0+cu124/lib)
        include_directories(${LIBTORCH_INCLUDE_DIRS})
        link_directories(${LIBTORCH_LIBRARY_DIRS})
        target_link_libraries(yolo ${LIBTORCH_LIBRARY_DIRS}/*.lib)
        target_link_options(yolo PRIVATE "/INCLUDE:\"?ignore_this_library_placeholder@@YAHXZ\"")
    endif()

    if(WITH_ONNXRUNTIME)
        set(ONNXRUNTIME_INCLUDE_DIRS D:/onnxruntime-win-x64-gpu-1.20.0/include)
        set(ONNXRUNTIME_LIBRARY_DIRS D:/onnxruntime-win-x64-gpu-1.20.0/lib)
        include_directories(${ONNXRUNTIME_INCLUDE_DIRS})
        link_directories(${ONNXRUNTIME_LIBRARY_DIRS})
        target_link_libraries(yolo ${ONNXRUNTIME_LIBRARY_DIRS}/*.lib)
    endif()

    if(WITH_OPENVINO)
        set(OPENVINO_INCLUDE_DIRS D:/w_openvino_toolkit_windows_2024.4.0.16579.c3152d32c9c_x86_64/runtime/include)
        set(OPENVINO_LIBRARY_DIRS D:/w_openvino_toolkit_windows_2024.4.0.16579.c3152d32c9c_x86_64/runtime/lib/intel64/Release)
        include_directories(${OPENVINO_INCLUDE_DIRS})
        link_directories(${OPENVINO_LIBRARY_DIRS})
        target_link_libraries(yolo ${OPENVINO_LIBRARY_DIRS}/*.lib)
    endif()

    if(WITH_TENSORRT)
	set(TENSORRT_INCLUDE_DIRS D:/TensorRT-10.6.0.26/include)
	set(TENSORRT_LIBRARY_DIRS D:/TensorRT-10.6.0.26/lib)
        include_directories(${TENSORRT_INCLUDE_DIRS})
        link_directories(${TENSORRT_LIBRARY_DIRS})
        target_link_libraries(yolo ${TENSORRT_LIBRARY_DIRS}/*.lib)
    endif()
endif()

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    add_executable(yolo ${SRCS})

    if(WITH_CUDA)
        find_package(CUDAToolkit REQUIRED)
        target_link_libraries(yolo PRIVATE CUDA::cudart)
    endif()

    # OpenCV (preferred modern target or fall back to variables)
    find_package(OpenCV REQUIRED)
    target_include_directories(yolo PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(yolo PRIVATE ${OpenCV_LIBS})

    if(WITH_LIBTORCH)
        find_package(Torch REQUIRED)
        # Torch provides imported targets; prefer using them
        if(TARGET Torch::Torch)
            target_link_libraries(yolo PRIVATE Torch::Torch)
        else()
            target_include_directories(yolo PRIVATE ${TORCH_INCLUDE_DIRS})
            target_link_libraries(yolo PRIVATE ${TORCH_LIBRARIES})
        endif()
    endif()

    if(WITH_ONNXRUNTIME)
        find_path(ONNXRUNTIME_INCLUDE_DIR onnxruntime_cxx_api.h
            HINTS /usr/local/include /usr/include /opt/onnxruntime/include PATH_SUFFIXES onnxruntime)
        find_library(ONNXRUNTIME_LIBRARY NAMES onnxruntime
            HINTS /usr/local/lib /usr/lib /opt/onnxruntime/lib /usr/lib/x86_64-linux-gnu PATH_SUFFIXES onnxruntime)
        if(ONNXRUNTIME_INCLUDE_DIR AND ONNXRUNTIME_LIBRARY)
            target_include_directories(yolo PRIVATE ${ONNXRUNTIME_INCLUDE_DIR})
            target_link_libraries(yolo PRIVATE ${ONNXRUNTIME_LIBRARY})
        else()
            message(FATAL_ERROR "ONNX Runtime requested but not found. Set ONNXRUNTIME_* variables or install ONNX Runtime.")
        endif()
    endif()

    if(WITH_OPENVINO)
        find_path(OPENVINO_INCLUDE_DIR openvino/runtime/core.hpp
            HINTS /usr/local/include /opt/intel/openvino/include /opt/openvino/include)
        find_library(OPENVINO_LIB NAMES openvino
            HINTS /usr/local/lib /opt/intel/openvino/lib /opt/openvino/lib /usr/lib)
        # optional plugins
        find_library(OPENVINO_CPU_PLUGIN NAMES openvino_intel_cpu_plugin
            HINTS /usr/local/lib /opt/intel/openvino/lib /opt/openvino/lib /usr/lib)
        find_library(OPENVINO_GPU_PLUGIN NAMES openvino_intel_gpu_plugin
            HINTS /usr/local/lib /opt/intel/openvino/lib /opt/openvino/lib /usr/lib)

        if(OPENVINO_INCLUDE_DIR AND OPENVINO_LIB)
            target_include_directories(yolo PRIVATE ${OPENVINO_INCLUDE_DIR})
            target_link_libraries(yolo PRIVATE ${OPENVINO_LIB})
            if(OPENVINO_CPU_PLUGIN)
                target_link_libraries(yolo PRIVATE ${OPENVINO_CPU_PLUGIN})
            endif()
            if(OPENVINO_GPU_PLUGIN)
                target_link_libraries(yolo PRIVATE ${OPENVINO_GPU_PLUGIN})
            endif()
        else()
            message(FATAL_ERROR "OpenVINO requested but not found. Set OPENVINO_* variables or install OpenVINO.")
        endif()
    endif()

    if(WITH_TENSORRT)
        find_path(TENSORRT_INCLUDE_DIR NvInfer.h
            HINTS /usr/local/include /usr/include /usr/include/x86_64-linux-gnu)
        find_library(TENSORRT_LIBRARY NAMES nvinfer
            HINTS /usr/local/lib /usr/lib /usr/lib/x86_64-linux-gnu)
        if(TENSORRT_INCLUDE_DIR AND TENSORRT_LIBRARY)
            target_include_directories(yolo PRIVATE ${TENSORRT_INCLUDE_DIR})
            target_link_libraries(yolo PRIVATE ${TENSORRT_LIBRARY})
        else()
            message(FATAL_ERROR "TensorRT requested but not found. Set TENSORRT_* variables or install TensorRT.")
        endif()
    endif()
endif()
