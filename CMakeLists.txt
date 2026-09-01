cmake_minimum_required(VERSION 3.16)
project(sort_practice CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# The reported measurements assume -O2. A Debug build (-O0) runs roughly 4-8x
# slower and will not match the report, so make that impossible to miss.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(WARNING
        "Building in Debug (-O0). Timings will be 4-8x slower than the ones "
        "reported in Report.docx. Switch the CMake profile to Release before "
        "measuring or demonstrating.")
endif()

# Main program: runs QuickSort, HeapSort and the AVL tree over dataset.txt.
add_executable(sort_practice src/main.cpp)

# Helper: rebuilds dataset.txt from words_alpha.txt (shuffle + take 100,000).
add_executable(generate_dataset src/generate_dataset.cpp)

# Link the C++/GCC runtime into the executables so they also run outside the
# IDE (double-click, plain terminal) without needing the MinGW DLLs on PATH.
if(MINGW)
    foreach(target sort_practice generate_dataset)
        target_link_options(${target} PRIVATE -static-libgcc -static-libstdc++)
    endforeach()

    # libwinpthread cannot be linked statically with CLion's bundled MinGW
    # (its libwinpthread.a is missing __intrinsic_setjmpex), so ship the DLL
    # next to the executables instead.
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
    find_file(WINPTHREAD_DLL libwinpthread-1.dll HINTS "${MINGW_BIN_DIR}" NO_DEFAULT_PATH)
    if(WINPTHREAD_DLL)
        file(COPY "${WINPTHREAD_DLL}" DESTINATION "${CMAKE_SOURCE_DIR}")
    endif()
endif()

# Put the binaries next to dataset.txt so "dataset.txt" resolves when run.
set_target_properties(sort_practice generate_dataset PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)
