cmake_minimum_required(VERSION 3.20)
project(monte_carlo_greeks_simd VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/W4 /O2 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

add_library(derivatives_core INTERFACE)
target_include_directories(derivatives_core INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_executable(derivatives_engine src/main.cpp)
target_link_libraries(derivatives_engine PRIVATE derivatives_core)

enable_testing()
add_executable(derivatives_tests tests/test_derivatives.cpp)
target_link_libraries(derivatives_tests PRIVATE derivatives_core)

add_test(NAME DerivativesTests COMMAND derivatives_tests)
