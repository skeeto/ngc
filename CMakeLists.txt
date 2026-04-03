cmake_minimum_required(VERSION 3.14)
project(ngc C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

# Shared sources (print, dynarr) compiled once, linked into both binaries
add_library(ngc_common OBJECT src/print.c src/dynarr.c)
target_include_directories(ngc_common PUBLIC src)

# ngc-asm
add_executable(ngc-asm
  src/asm/str.c
  src/asm/err.c
  src/asm/parsed.c
  src/asm/parse.c
  src/asm/assemble.c
  src/asm/assemble_basic.c
  src/asm/assemble_full.c
  src/asm/cli.c
)
target_link_libraries(ngc-asm PRIVATE ngc_common)
target_include_directories(ngc-asm PRIVATE src/asm)

# ngc-emu
find_package(Curses REQUIRED)
add_executable(ngc-emu
  src/emu/emu.c
  src/emu/tui.c
)
target_link_libraries(ngc-emu PRIVATE ngc_common ${CURSES_LIBRARIES})
target_include_directories(ngc-emu PRIVATE src/emu ${CURSES_INCLUDE_DIRS})

# Install (mirrors makefile: PREFIX=/usr/local, binaries to bin/)
include(GNUInstallDirs)
install(TARGETS ngc-asm ngc-emu RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Fuzzer (clang only, not built by default: cmake --build build -t fuzz_asm)
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
  add_executable(fuzz_asm EXCLUDE_FROM_ALL
    test/fuzz/fuzz_asm.c
    src/print.c
    src/dynarr.c
    src/asm/str.c
    src/asm/err.c
    src/asm/parsed.c
    src/asm/parse.c
    src/asm/assemble.c
    src/asm/assemble_basic.c
    src/asm/assemble_full.c
  )
  target_include_directories(fuzz_asm PRIVATE src src/asm)
  target_compile_options(fuzz_asm PRIVATE -fsanitize=fuzzer,address,undefined -g -O1)
  target_link_options(fuzz_asm PRIVATE -fsanitize=fuzzer,address,undefined)
endif()

# Tests
enable_testing()
set(run_one ${CMAKE_SOURCE_DIR}/test/asm/run_one.sh)

file(GLOB pos_tests test/asm/positive/*.in)
foreach(in_file ${pos_tests})
  get_filename_component(test_name ${in_file} NAME_WE)
  add_test(NAME asm/positive/${test_name}
    COMMAND ${run_one} $<TARGET_FILE:ngc-asm> positive ${in_file}
  )
endforeach()

file(GLOB neg_tests test/asm/negative/*.in)
foreach(in_file ${neg_tests})
  get_filename_component(test_name ${in_file} NAME_WE)
  add_test(NAME asm/negative/${test_name}
    COMMAND ${run_one} $<TARGET_FILE:ngc-asm> negative ${in_file}
  )
endforeach()
