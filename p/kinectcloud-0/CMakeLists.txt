cmake_minimum_required(VERSION 3.16)
project(kinectcloud VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(KINECT_V1 "Build Kinect v1 backend" ON)
option(KINECT_V2 "Reserve Kinect v2 backend" OFF)

find_package(PkgConfig REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(Threads REQUIRED)

if(KINECT_V1)
    pkg_check_modules(FREENECT REQUIRED freenect)
    add_compile_definitions(KINECT_V1)
endif()

add_executable(kinectcloud
    src/main.cpp
    src/camera/Camera.cpp
    src/camera/KinectV1.cpp
    src/cloud/PointCloud.cpp
    src/recording/Sequence.cpp
    src/recording/Recorder.cpp
    src/playback/Player.cpp
    src/rendering/Renderer.cpp
)

target_include_directories(kinectcloud PRIVATE
    src
    ${FREENECT_INCLUDE_DIRS}
)

target_link_directories(kinectcloud PRIVATE ${FREENECT_LIBRARY_DIRS})

target_link_libraries(kinectcloud PRIVATE
    glfw
    OpenGL::GL
    Threads::Threads
    ${FREENECT_LIBRARIES}
)

target_compile_options(kinectcloud PRIVATE
    -Wall -Wextra -Wpedantic
    -O3
)

if(KINECT_V2)
    message(STATUS "Kinect v2 reserved: backend not implemented in this first version.")
endif()
