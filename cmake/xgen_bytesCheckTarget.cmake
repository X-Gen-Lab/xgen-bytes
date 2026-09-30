include_guard(GLOBAL)

# Share the target identity contract between source and installed consumption.
function(xgb_check_target target expected_version expected_abi result)
    get_target_property(actual_version ${target} XGB_VERSION)
    get_target_property(actual_abi ${target} XGB_ABI_VERSION)
    if(NOT actual_version STREQUAL "${expected_version}" OR
       NOT actual_abi STREQUAL "${expected_abi}")
        set(${result}
            "${target} has incompatible version/ABI '${actual_version}/${actual_abi}'; expected '${expected_version}/${expected_abi}'"
            PARENT_SCOPE)
    else()
        set(${result} "" PARENT_SCOPE)
    endif()
endfunction()
