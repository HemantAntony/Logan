cmake_minimum_required(VERSION 3.16)

project(logan LANGUAGES CXX)

add_executable(logan
    src/logging/logger.cpp
    src/sink/file_sink.cpp
    src/sink/console_sink.cpp
    src/main.cpp
)

target_include_directories(logan PRIVATE
	${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party
)

target_compile_definitions(logan PRIVATE
    BUILD_DIR="${CMAKE_BINARY_DIR}"
)

target_compile_features(logan PRIVATE cxx_std_17)
