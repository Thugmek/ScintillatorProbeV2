function(objcopy target format suffix)
  set(bin_filename "${CMAKE_CURRENT_BINARY_DIR}/${target}${suffix}")
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND "${CMAKE_OBJCOPY}" -O ${format} -S "$<TARGET_FILE:${target}>" "${bin_filename}"
    COMMENT "Generating ${format} from ${target}..."
    BYPRODUCTS "${bin_filename}"
    )
endfunction()

function(report_size target)
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND echo ""
    COMMAND "${CMAKE_SIZE_UTIL}" -B "$<TARGET_FILE:${target}>"
    USES_TERMINAL
    )
endfunction()

function(target_set_linker_script target linker_script)
  target_link_options(${target} PRIVATE "-Wl,-T,${linker_script}")
  add_custom_target("${target}_linker_script" DEPENDS "${linker_script}")
  add_dependencies(${target} "${target}_linker_script")
endfunction()
