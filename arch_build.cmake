###########################################################
#
# TO_CON platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TO_CON configuration
set(TO_CON_PLATFORM_CONFIG_FILE_LIST
  to_con_internal_cfg.h
  to_con_platform_cfg.h
  to_con_perfids.h
  to_con_msgids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(TO_CON_CFGFILE ${TO_CON_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${TO_CON_CFGFILE}" NAME_WE)
  if (DEFINED TO_CON_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${TO_CON_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${TO_CON_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${TO_CON_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
