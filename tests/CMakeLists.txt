include(GoogleTest)

add_executable(xgb_tests unit/test_bytes.cpp)
set_target_properties(xgb_tests PROPERTIES
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_compile_features(xgb_tests PRIVATE cxx_std_17)
target_link_libraries(xgb_tests PRIVATE xgb::bytes GTest::gtest_main)
xgb_enable_warnings(xgb_tests)
gtest_discover_tests(xgb_tests DISCOVERY_MODE POST_BUILD NO_PRETTY_VALUES
    PROPERTIES LABELS "unit" TIMEOUT 30)

add_library(xgb_header_checks OBJECT
    integration/include_bytes.c integration/include_bytes.cpp
    integration/include_version.c integration/include_version.cpp)
set_target_properties(xgb_header_checks PROPERTIES
    C_STANDARD 11 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    CXX_STANDARD 17 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_link_libraries(xgb_header_checks PRIVATE xgb::bytes)
xgb_enable_warnings(xgb_header_checks)

foreach(test_case source installed installed_default installed_optional
        installed_unknown installed_wrong_version installed_preexisting
        installed_bad_version installed_bad_abi installed_no_identity
        source_preexisting source_bad_version source_bad_abi source_no_identity)
    add_test(NAME xgb_${test_case}
        COMMAND "${CMAKE_COMMAND}"
            "-DXGB_SOURCE_DIR=${PROJECT_SOURCE_DIR}"
            "-DXGB_TEST_ROOT=${PROJECT_BINARY_DIR}/integration"
            "-DXGB_TEST_CASE=${test_case}"
            "-DXGB_C_COMPILER=${CMAKE_C_COMPILER}"
            "-DXGB_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DXGB_GENERATOR=${CMAKE_GENERATOR}"
            "-DXGB_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}"
            "-DXGB_GENERATOR_TOOLSET=${CMAKE_GENERATOR_TOOLSET}"
            "-DXGB_CONFIG=$<CONFIG>"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/integration/check_consumer.cmake")
    set_tests_properties(xgb_${test_case} PROPERTIES LABELS "integration" TIMEOUT 120)
endforeach()
