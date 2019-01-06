cmake_minimum_required(VERSION 3.10)
project(natural_lang_process C)

set(CMAKE_C_STANDARD 99)

add_executable(natural_lang_process main.c my_lib/token_list.c my_lib/lexical_analysis.c my_lib/common.c my_lib/id_list.c my_lib/syntatic_analysis.c my_lib/cross_reference.c)
