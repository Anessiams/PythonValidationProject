cmake_minimum_required(VERSION 3.26)
project(PythonValidationProject)

set(CMAKE_CXX_STANDARD 14)

include_directories(handler)

add_executable(PythonValidationProject
        src/config.cpp
        src/config.h
        src/utils.h
        src/test.cpp
        src/test.yaml
)
target_link_libraries(PythonValidationProject PUBLIC rt)
