cmake_minimum_required(VERSION 3.10)
project(wow)
set(CMAKE_CXX_STANDARD 17)

# Adding sources
set(SOURCES
    src/main.cc
    src/WordModel.cc
    src/utf8.cc
)

# Binary
add_executable(wow ${SOURCES})
set_property(TARGET wow PROPERTY CXX_STANDARD 17)
target_compile_features(wow PRIVATE cxx_std_17)