cmake_minimum_required(VERSION 3.26)
project(PythonValidationProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(handler)

add_executable(PythonValidationProject
        handler/config.cpp
        handler/config.h
        handler/main.cpp
        handler/proxy.cpp
        handler/proxy.h
        handler/utils.h)
target_link_libraries(PythonValidationProject PUBLIC rt)
