cmake_minimum_required(VERSION 3.26)
project(chess C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 REQUIRED)

add_executable(chess)

target_sources(chess
PRIVATE
  chess.cpp
)

target_link_libraries(chess SDL3::SDL3)