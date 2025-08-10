cmake_minimum_required(VERSION 3.24.2)

project(ZED LANGUAGES CXX VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED 23)
# set(CMAKE_CXX_EXTENSIONS ON)

add_compile_options("-Wall")
add_compile_options("-Wextra")
add_compile_options("-Werror")
add_compile_options("-Wundef")
add_compile_options("-Wsign-compare")


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    add_compile_definitions(NEED_ZEDIO_LOG)
    message(STATUS "need zedio log")
endif ()

message(STATUS "Version: " "${CMAKE_PROJECT_VERSION}")
message(STATUS "C++ standard: " "${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: " "${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build type: " "${CMAKE_BUILD_TYPE}")

# Required dependencies (liburing)
find_package(PkgConfig)
pkg_check_modules(liburing REQUIRED IMPORTED_TARGET GLOBAL liburing>=2.0)

set(DEPEND_LIBS
        pthread
        PkgConfig::liburing
)

include_directories("${PROJECT_SOURCE_DIR}")

if (WITH_LOG)
   message(STATUS "Enable logs to console")
   add_definitions(-DNEED_ZEDIO_LOG)
endif ()

install(DIRECTORY "${PROJECT_SOURCE_DIR}/zedio"
        DESTINATION "include")


add_subdirectory("${PROJECT_SOURCE_DIR}/examples")

if (WITH_TESTS)
    message(STATUS "Enable tests")
    enable_testing()
    find_package(ut CONFIG REQUIRED)
    list(APPEND DEPEND_LIBS Boost::ut)
    add_subdirectory("${PROJECT_SOURCE_DIR}/tests")
endif ()

