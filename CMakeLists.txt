cmake_minimum_required(VERSION 3.24)
project(xgen_bytes VERSION 0.1.0 DESCRIPTION "Independent alignment-safe C11 integer byte access" LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(XGB_ABI_VERSION 1)
include(cmake/xgen_bytesCheckTarget.cmake)

option(XGB_BUILD_TESTS "Build host GoogleTest and package consumption tests" OFF)
option(XGB_ENABLE_COVERAGE "Instrument owned production objects for host coverage" OFF)

if(TARGET xgb::bytes)
    xgb_check_target(xgb::bytes "${PROJECT_VERSION}" "${XGB_ABI_VERSION}" compatibility_error)
    if(compatibility_error)
        message(FATAL_ERROR "${compatibility_error}")
    endif()
    return()
endif()
if(XGB_BUILD_TESTS AND CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Build host tests separately from MCU targets")
endif()
if(XGB_ENABLE_COVERAGE)
    if(CMAKE_CROSSCOMPILING OR NOT XGB_BUILD_TESTS)
        message(FATAL_ERROR "Coverage requires an explicit host test build")
    endif()
    if(NOT CMAKE_C_COMPILER_ID MATCHES "^(GNU|Clang|AppleClang)$" OR
       CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        message(FATAL_ERROR "Coverage requires a GCC-compatible host compiler")
    endif()
endif()

function(xgb_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /WX)
    elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Werror -Wconversion -Wshadow
            $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>)
    endif()
endfunction()

configure_file(include/xgen/bytes/version.h.in generated/xgen/bytes/version.h @ONLY)
add_library(xgb_bytes STATIC src/bytes.c)
add_library(xgb::bytes ALIAS xgb_bytes)
set_target_properties(xgb_bytes PROPERTIES
    EXPORT_NAME bytes
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    XGB_VERSION ${PROJECT_VERSION}
    XGB_ABI_VERSION ${XGB_ABI_VERSION}
    EXPORT_PROPERTIES "XGB_VERSION;XGB_ABI_VERSION")
target_compile_features(xgb_bytes PUBLIC c_std_11)
target_include_directories(xgb_bytes PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
xgb_enable_warnings(xgb_bytes)
if(XGB_ENABLE_COVERAGE)
    target_compile_options(xgb_bytes PRIVATE --coverage -O0 -g)
    target_link_options(xgb_bytes INTERFACE --coverage)
endif()

install(TARGETS xgb_bytes EXPORT xgenBytesTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(EXPORT xgenBytesTargets FILE xgenBytesTargets.cmake NAMESPACE xgb::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_bytes)
install(FILES include/xgen/bytes/bytes.h
    "${CMAKE_CURRENT_BINARY_DIR}/generated/xgen/bytes/version.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/xgen/bytes)
configure_package_config_file(cmake/xgen_bytesConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_bytesConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_bytes)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_bytesConfigVersion.cmake"
    VERSION ${PROJECT_VERSION} COMPATIBILITY SameMinorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/xgen_bytesConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/xgen_bytesConfigVersion.cmake"
    cmake/xgen_bytesCheckTarget.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/xgen_bytes)

if(XGB_BUILD_TESTS)
    enable_language(CXX)
    find_package(GTest 1.16.0 EXACT CONFIG REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()
