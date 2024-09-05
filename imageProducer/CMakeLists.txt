cmake_minimum_required(VERSION 3.19)
project(ImageProducer)

set(CMAKE_CXX_STANDARD 17)

include_directories(../include/boost_1_77_0/)


add_executable(ImageProducer imageProducer.cpp)

#find_package(Boost REQUIRED COMPONENTS system)
#if (Boost_FOUND)
#    include_directories(${Boost_INCLUDE_DIRS})
#    target_link_libraries(ImageProducer ${Boost_LIBRARIES})
#endif()

find_package(Threads REQUIRED)
target_link_libraries(ImageProducer pthread)

