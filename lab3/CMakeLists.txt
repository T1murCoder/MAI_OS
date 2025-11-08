cmake_minimum_required(VERSION 3.10)

set(LAB_NAME "lab3")

project(${LAB_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Добавление опций компиляции
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=maybe-uninitialized")


add_library(shared_message_lib src/shared_message.cpp)
target_include_directories(shared_message_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_executable(parent_exe parent.cpp)
target_link_libraries(parent_exe PRIVATE shared_message_lib)

add_executable(child_exe child.cpp)
target_link_libraries(child_exe PRIVATE shared_message_lib)