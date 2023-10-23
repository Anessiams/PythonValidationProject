cmake_minimum_required(VERSION 3.26)
project(PythonValidationProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(handler)

add_executable(PythonValidationProject
        src/config.cpp
        src/config.h
        src/main.cpp
        src/inproxy.cpp
        src/inproxy.h
        src/utils.h
        src/outproxy.cpp
        src/outproxy.h
        src/metadata.h
        src/runner.cpp
        src/runner.h
)
target_link_libraries(PythonValidationProject PUBLIC rt)
