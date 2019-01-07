cmake_minimum_required(VERSION 3.10)
project(natural_lang_process C)

set(CMAKE_C_STANDARD 99)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(natural_lang_process ${PROJECT_SOURCE_DIR}/main.c ${PROJECT_SOURCE_DIR}/my_lib/token_list.c ${PROJECT_SOURCE_DIR}/my_lib/lexical_analysis.c ${PROJECT_SOURCE_DIR}/my_lib/common.c ${PROJECT_SOURCE_DIR}/my_lib/id_list.c ${PROJECT_SOURCE_DIR}/my_lib/syntatic_analysis.c ${PROJECT_SOURCE_DIR}/my_lib/cross_reference.c)
