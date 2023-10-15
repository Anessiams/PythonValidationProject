cmake_minimum_required(VERSION 3.26)
project(PythonValidationProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(handler)

add_executable(PythonValidationProject
        config.cpp
        config.h
        main.cpp
        proxy.cpp
        proxy.h
        utils.h)
target_link_libraries(PythonValidationProject PUBLIC rt)
