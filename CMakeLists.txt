cmake_minimum_required(VERSION 3.16)

project(trie_exe VERSION 1.0 LANGUAGES C)

# https://stackoverflow.com/questions/26693381/cmake-does-not-link-ncurses
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_library(ipc STATIC "src/ipc.c" "include/ipc.h")
add_library(rec_array STATIC "src/rec_array.c" "include/rec_array.h")
add_library(htrie_structs STATIC "src/htrie_structs.c" "include/htrie_structs.h")
add_library(htrie STATIC "src/trie_hybrid.c" "include/trie_hybrid.h")

add_executable(trie_exe "src/main.c")

target_link_libraries(trie_exe htrie htrie_structs rec_array ipc ${CURSES_LIBRARIES})

target_compile_options(trie_exe PUBLIC "-Wall" "-Wextra" "-Wpedantic" "-O2")

add_executable(autotyper "autotyper/autotyper.c")
target_link_libraries(autotyper ipc ${CURSES_LIBRARIES})
target_compile_options(autotyper PUBLIC "-Wall" "-Wextra" "-Wpedantic" "-O2")
