cmake_minimum_required(VERSION 3.17)
project(aux-library LANGUAGES C)
set(CMAKE_C_STANDARD 11)


add_library(aux-library src/aux/hashmap.c src/aux/vector.c include/aux/time.h src/aux/bit-stream.c)
target_include_directories(aux-library PUBLIC include)
