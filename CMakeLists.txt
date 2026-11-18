cmake_minimum_required(VERSION 3.20)

project(alt_std_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    set(gtest_force_shared_crt ON CACHE BOOL
        "Use the shared MSVC runtime in GoogleTest" FORCE
    )
endif()

option(ALT_USE_SYSTEM_GTEST "Use an installed GoogleTest package instead of downloading it" OFF)
option(ALT_ENABLE_ASAN "Enable AddressSanitizer in Debug builds" OFF)
option(ALT_STD_ADD_SUBDIRECTORY "Add the sibling alt-std project as a CMake subdirectory" OFF)

set(ALT_STD_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../alt-std" CACHE PATH "Path to the sibling alt-std project")
set(ALT_STD_TARGET "" CACHE STRING "CMake target provided by the sibling alt-std project")

if(BUILD_TESTING)

    if(ALT_USE_SYSTEM_GTEST)
        find_package(GTest CONFIG REQUIRED)
    else()
        include(FetchContent)

        set(BUILD_GMOCK OFF CACHE BOOL
            "Do not build GoogleMock" FORCE
        )

        set(INSTALL_GTEST OFF CACHE BOOL
            "Do not install GoogleTest" FORCE
        )

        FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.17.0
            GIT_SHALLOW    TRUE
        )

        FetchContent_MakeAvailable(googletest)
    endif()

    set(ALT_STD_TEST_SOURCES
        tests/ArenaAllocatorTest.cpp
        tests/ArenaAllocatorVirtualTest.cpp
        tests/AtomicsTest.cpp
        tests/Base64Test.cpp
        tests/BinarySearchTest.cpp
        tests/BitSetTest.cpp
        tests/BoundedMPMCTest.cpp
        tests/ConcurrencyTest.cpp
        tests/EntityPoolAllocatorTest.cpp
        tests/EnumClassBitFlagsTest.cpp
        tests/FuncTest.cpp
        tests/HashTest.cpp
        tests/IDPoolTest.cpp
        tests/IfErrTest.cpp
        tests/MathTest.cpp
        tests/PlatformAtomicsTest.cpp
        tests/PoolAllocatorVirtualTest.cpp
        tests/QueueTest.cpp
        tests/RcConcPtrTest.cpp
        tests/RcPtrTest.cpp
        tests/IteratorTest.cpp
        tests/main.cpp
        tests/MaybeUninitTest.cpp
        tests/NonCopyableMovableTest.cpp
        tests/NumTest.cpp
        tests/OptionalTest.cpp
        tests/PairTest.cpp
        tests/pch.cpp
        tests/PlatformTest.cpp
        tests/PoolAllocatorTest.cpp
        tests/RadixSortTest.cpp
        tests/RefPtrTest.cpp
        tests/ResultTest.cpp
        tests/ScopeDeferTest.cpp
        tests/SliceTest.cpp
        tests/SortTest.cpp
        tests/StaticBitSetTest.cpp
        tests/StaticQueueTest.cpp
        tests/StaticStringTest.cpp
        tests/StaticVectorTest.cpp
        tests/StringTest.cpp
        tests/StringUTF16Test.cpp
        tests/TimeTest.cpp
        tests/UnionTest.cpp
        tests/UniquePtrTest.cpp
        tests/UtfTest.cpp
        tests/VectorTest.cpp
    )

    add_executable(tests ${ALT_STD_TEST_SOURCES})

    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        PREFIX "tests"
        FILES ${ALT_STD_TEST_SOURCES}
    )

    if(ALT_STD_ADD_SUBDIRECTORY)
        add_subdirectory(
            "${ALT_STD_SOURCE_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}/alt-std"
        )
    endif()

    target_include_directories(tests PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        "${ALT_STD_SOURCE_DIR}/src"
    )

    if(IS_DIRECTORY "${ALT_STD_SOURCE_DIR}/include")
        target_include_directories(tests PRIVATE
            "${ALT_STD_SOURCE_DIR}/include"
        )
    endif()

    file(GLOB_RECURSE ALT_STD_IDE_HEADERS CONFIGURE_DEPENDS
        "${ALT_STD_SOURCE_DIR}/src/*.h"
        "${ALT_STD_SOURCE_DIR}/src/*.hpp"
        "${ALT_STD_SOURCE_DIR}/src/*.inl"
    )

    if(ALT_STD_IDE_HEADERS)
        target_sources(tests PRIVATE ${ALT_STD_IDE_HEADERS})

        source_group(
            TREE "${ALT_STD_SOURCE_DIR}"
            PREFIX "alt-std"
            FILES ${ALT_STD_IDE_HEADERS}
        )
    endif()

    set(_alt_std_natvis "${ALT_STD_SOURCE_DIR}/src/alt.natvis")

    if(EXISTS "${_alt_std_natvis}")
        target_sources(tests PRIVATE "${_alt_std_natvis}")
        source_group("Visualizers" FILES "${_alt_std_natvis}")
    endif()

    set(_alt_std_ide_files)

    foreach(_file IN ITEMS pch.h Utils.h UtfTest.inl)
        foreach(_base IN ITEMS
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_SOURCE_DIR}/tests"
        )
            if(EXISTS "${_base}/${_file}")
                list(APPEND _alt_std_ide_files "${_base}/${_file}")
            endif()
        endforeach()
    endforeach()

    if(_alt_std_ide_files)
        list(REMOVE_DUPLICATES _alt_std_ide_files)
        target_sources(tests PRIVATE ${_alt_std_ide_files})
        source_group("Test Support" FILES ${_alt_std_ide_files})
    endif()

    if(NOT ALT_STD_TARGET STREQUAL "")
        if(NOT TARGET "${ALT_STD_TARGET}")
            message(FATAL_ERROR
                "ALT_STD_TARGET='${ALT_STD_TARGET}' was specified, "
                "but that CMake target does not exist."
            )
        endif()

        target_link_libraries(tests PRIVATE "${ALT_STD_TARGET}")

    elseif(TARGET alt_std)
        target_link_libraries(tests PRIVATE alt_std)

    elseif(TARGET alt-std)
        target_link_libraries(tests PRIVATE alt-std)
    endif()

    target_link_libraries(tests PRIVATE
        GTest::gtest
    )

    target_compile_features(tests PRIVATE cxx_std_17)

    include(CheckIPOSupported)

    check_ipo_supported(
        RESULT ALT_IPO_SUPPORTED
        OUTPUT ALT_IPO_ERROR
    )

    if(ALT_IPO_SUPPORTED)
        set_property(
            TARGET tests
            PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE
        )
    else()
        message(STATUS
            "IPO/LTO is unavailable; Release will use normal optimization: "
            "${ALT_IPO_ERROR}"
        )
    endif()

    target_compile_definitions(tests PRIVATE
        "$<$<CONFIG:Debug>:ALT_DEV>"
        "$<$<CONFIG:Debug>:_DEBUG>"
    )

    if(WIN32)
        target_compile_definitions(tests PRIVATE
            _CONSOLE
            UNICODE
            _UNICODE
        )

        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            target_compile_definitions(tests PRIVATE X64)
        else()
            target_compile_definitions(tests PRIVATE WIN32)
        endif()
    endif()

    if(MSVC)
        target_compile_options(tests PRIVATE
            /W4
            /wd4456
            /utf-8
            /permissive-
            /Zc:preprocessor
            /GR-
            /EHs-c-
            /Oi
            "$<$<CONFIG:Debug>:/RTC1>"
        )

        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options(tests PRIVATE /sdl)
        endif()

    elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
        target_compile_options(tests PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -fno-exceptions
            -fno-rtti
        )
    endif()


    if(CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")

        include(CheckCXXCompilerFlag)

        check_cxx_compiler_flag(
            "-mcx16"
            ALT_COMPILER_HAS_MCX16
        )

        if(ALT_COMPILER_HAS_MCX16)
            target_compile_options(tests PRIVATE -mcx16)
        endif()
    endif()

    set(_alt_std_pch_header "")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pch.h")
        set(_alt_std_pch_header
            "${CMAKE_CURRENT_SOURCE_DIR}/pch.h"
        )
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/pch.h")
        set(_alt_std_pch_header
            "${CMAKE_CURRENT_SOURCE_DIR}/tests/pch.h"
        )
    endif()

    if(NOT _alt_std_pch_header STREQUAL "")
        target_precompile_headers(tests PRIVATE
            "${_alt_std_pch_header}"
        )

        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/pch.cpp")
            set_source_files_properties(
                "${CMAKE_CURRENT_SOURCE_DIR}/tests/pch.cpp"
                PROPERTIES SKIP_PRECOMPILE_HEADERS ON
            )
        elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp")
            set_source_files_properties(
                "${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp"
                PROPERTIES SKIP_PRECOMPILE_HEADERS ON
            )
        endif()
    endif()


    if(ALT_ENABLE_ASAN)
        if(MSVC)
            target_compile_options(tests PRIVATE
                "$<$<CONFIG:Debug>:/fsanitize=address>"
            )

            target_link_options(tests PRIVATE
                "$<$<CONFIG:Debug>:/fsanitize=address>"
            )

        elseif(CMAKE_CXX_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$")
            target_compile_options(tests PRIVATE
                "$<$<CONFIG:Debug>:-fsanitize=address>"
                "$<$<CONFIG:Debug>:-fno-omit-frame-pointer>"
            )

            target_link_options(tests PRIVATE
                "$<$<CONFIG:Debug>:-fsanitize=address>"
            )

        else()
            message(FATAL_ERROR
                "ALT_ENABLE_ASAN is enabled, but AddressSanitizer flags "
                "are not configured for compiler "
                "'${CMAKE_CXX_COMPILER_ID}'."
            )
        endif()
    endif()

    include(GoogleTest)

    gtest_add_tests(
        TARGET tests
        SOURCES ${ALT_STD_TEST_SOURCES}
    )
endif()