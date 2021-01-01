cmake_minimum_required(VERSION 3.17)
project(aux-library LANGUAGES C)
set(CMAKE_C_STANDARD 11)

include(CheckTypeSize)
check_type_size(uintptr_t UINTPTR_T LANGUAGE C)
check_type_size(size_t SIZE_T LANGUAGE C)

add_library(aux-library src/aux/hashmap.c src/aux/vector.c include/aux/time.h src/aux/bit-stream.c src/aux/string-builder.c src/aux/strings.c)
target_include_directories(aux-library PUBLIC include)
