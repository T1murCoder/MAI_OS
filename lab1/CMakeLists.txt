cmake_minimum_required(VERSION 3.10)

set(LAB_NAME "lab1")

project(${LAB_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Добавление опций компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")


# add_library(${LAB_NAME}_lib src/day.cpp src/fear_check.cpp)
add_executable(parent_exe parent.cpp)
add_executable(child_exe child.cpp)

# target_link_libraries(${LAB_NAME}_exe PRIVATE ${LAB_NAME}_lib)