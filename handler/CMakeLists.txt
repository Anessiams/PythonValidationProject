cmake_minimum_required(VERSION 3.26)
project(PythonValidationProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(handler)

add_executable(PythonValidationProject
        config.cpp
        config.h
        main.cpp
        inproxy.cpp
        inproxy.h
        utils.h
        outproxy.cpp
        outproxy.h
        metadata.h
        runner.cpp
        runner.h
)
target_link_libraries(PythonValidationProject PUBLIC rt)
