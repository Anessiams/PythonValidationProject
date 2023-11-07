cmake_minimum_required(VERSION 3.26)
project(PythonValidationProject)
project(PythonValidationProjectTestManager)
project(PythonValidationProjectTestConfig)

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

add_executable(PythonValidationProjectTestConfig
        src/config.cpp
        src/config.h
        src/config.test.cpp
)

target_link_libraries(PythonValidationProject PUBLIC rt)