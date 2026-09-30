# Exercise package contracts in fresh, isolated producer/consumer build trees.
foreach(required XGB_SOURCE_DIR XGB_TEST_ROOT XGB_TEST_CASE
        XGB_C_COMPILER XGB_CXX_COMPILER XGB_GENERATOR)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "${required} is required")
    endif()
endforeach()
if(NOT DEFINED XGB_CONFIG OR XGB_CONFIG STREQUAL "")
    set(XGB_CONFIG Debug)
endif()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run_id)
set(work "${XGB_TEST_ROOT}/${XGB_TEST_CASE}/${run_id}")
file(MAKE_DIRECTORY "${work}")

function(run_checked step)
    execute_process(COMMAND ${ARGN} RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE errors)
    file(WRITE "${work}/${step}.log" "${output}\n${errors}")
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "${step} failed (${result}); log: ${work}/${step}.log\n${output}\n${errors}")
    endif()
endfunction()

set(generator_args -G "${XGB_GENERATOR}")
if(DEFINED XGB_GENERATOR_PLATFORM AND NOT XGB_GENERATOR_PLATFORM STREQUAL "")
    list(APPEND generator_args -A "${XGB_GENERATOR_PLATFORM}")
endif()
if(DEFINED XGB_GENERATOR_TOOLSET AND NOT XGB_GENERATOR_TOOLSET STREQUAL "")
    list(APPEND generator_args -T "${XGB_GENERATOR_TOOLSET}")
endif()
set(compiler_args "-DCMAKE_C_COMPILER=${XGB_C_COMPILER}"
    "-DCMAKE_BUILD_TYPE=${XGB_CONFIG}")
set(consumer_args -S "${XGB_SOURCE_DIR}/tests/integration/consumer"
    -B "${work}/consumer" ${generator_args} ${compiler_args}
    "-DCMAKE_CXX_COMPILER=${XGB_CXX_COMPILER}"
    "-DXGB_CONSUMER_MODE=${XGB_TEST_CASE}"
    "-DXGB_CONSUMER_SOURCE_DIR=${XGB_SOURCE_DIR}"
    -DXGB_CONSUMER_CHECK_CXX=ON
    -DCMAKE_FIND_USE_PACKAGE_REGISTRY=OFF
    -DCMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY=OFF)

if(NOT XGB_TEST_CASE STREQUAL "source")
    run_checked(producer_configure "${CMAKE_COMMAND}" -S "${XGB_SOURCE_DIR}"
        -B "${work}/producer" ${generator_args} ${compiler_args}
        -DXGB_BUILD_TESTS=OFF -DXGB_ENABLE_COVERAGE=OFF
        -DCMAKE_INSTALL_LIBDIR=lib)
    run_checked(producer_build "${CMAKE_COMMAND}" --build "${work}/producer"
        --config "${XGB_CONFIG}")
    run_checked(producer_install "${CMAKE_COMMAND}" --install "${work}/producer"
        --config "${XGB_CONFIG}" --prefix "${work}/prefix")
    list(APPEND consumer_args "-Dxgen_bytes_DIR=${work}/prefix/lib/cmake/xgen_bytes")
endif()

if(XGB_TEST_CASE MATCHES "_(unknown|wrong_version|bad_version|bad_abi|no_identity)$")
    execute_process(COMMAND "${CMAKE_COMMAND}" ${consumer_args}
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors)
    file(WRITE "${work}/expected_failure.log" "${output}\n${errors}")
    if(result EQUAL 0)
        message(FATAL_ERROR "Invalid package request was accepted: ${XGB_TEST_CASE}")
    endif()
    if(XGB_TEST_CASE STREQUAL "installed_wrong_version")
        set(expected "0\\.2\\.0")
    elseif(XGB_TEST_CASE STREQUAL "installed_unknown")
        set(expected "is not available; available: bytes")
    else()
        set(expected "incompatible version/ABI")
    endif()
    if(NOT "${output}\n${errors}" MATCHES "${expected}")
        message(FATAL_ERROR "Failure did not diagnose the requested contract: ${errors}")
    endif()
    message(STATUS "Rejected ${XGB_TEST_CASE}; evidence: ${work}")
    return()
endif()

run_checked(consumer_configure "${CMAKE_COMMAND}" ${consumer_args})
run_checked(consumer_build "${CMAKE_COMMAND}" --build "${work}/consumer"
    --config "${XGB_CONFIG}")
run_checked(consumer_test "${CMAKE_CTEST_COMMAND}" --test-dir "${work}/consumer"
    -C "${XGB_CONFIG}" --output-on-failure --no-tests=error)
message(STATUS "Verified ${XGB_TEST_CASE}; evidence: ${work}")
