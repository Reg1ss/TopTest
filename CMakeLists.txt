cmake_minimum_required(VERSION 3.5)
project(Test)
set(ProjDir ${CMAKE_CURRENT_LIST_DIR})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fPIE")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall")

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_PREFIX_PATH $ENV{TPV_SERVER_ROOT_DIR})
set(CMAKE_INSTALL_PREFIX $ENV{TPV_SERVER_ROOT_DIR})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenCV)



add_executable(Test main.cpp )
target_link_libraries(Test TpDLStream TopTrace TpInference TpRender TpVideoStream TpGPUCore TpCPUCore TopDripLeaveDect boost_filesystem boost_chrono boost_thread boost_system  ${OpenCV_LIBS}  glog gflags boost_filesystem boost_thread)




