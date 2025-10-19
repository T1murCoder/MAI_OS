cmake_minimum_required(VERSION 3.10)

set(LAB_NAME "lab2")

project(${LAB_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Добавление опций компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")

find_package(Threads REQUIRED)


add_executable(main_exe src/thread_pool.cpp src/gaussian.cpp main.cpp)
target_link_libraries(main_exe PRIVATE Threads::Threads)