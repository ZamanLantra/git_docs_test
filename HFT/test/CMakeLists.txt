cmake_minimum_required(VERSION 3.14)
project(TestOrderBook)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(abseil-cpp)
add_executable(TestOrderBook TestOrderBook.cpp)
target_compile_definitions(TestOrderBook PRIVATE USE_ABSL_FLAT_HASH_MAP)
target_link_libraries(TestOrderBook PRIVATE absl::flat_hash_map)
