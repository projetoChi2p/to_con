###########################################################
#
# TO_CON mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TO_CON configuration
set(TO_CON_MISSION_CONFIG_FILE_LIST
  to_con_fcncodes.h
  to_con_interface_cfg.h
  to_con_mission_cfg.h
  to_con_perfids.h
  to_con_msg.h
  to_con_msgdefs.h
  to_con_msgstruct.h
  to_con_tbl.h
  to_con_tbldefs.h
  to_con_tblstruct.h
  to_con_topicids.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(TO_CON_CFGFILE_SRC_to_con_interface_cfg "to_con_eds_designparameters.h")
  set(TO_CON_CFGFILE_SRC_to_con_tbldefs       "to_con_eds_typedefs.h")
  set(TO_CON_CFGFILE_SRC_to_con_tblstruct     "to_con_eds_typedefs.h")
  set(TO_CON_CFGFILE_SRC_to_con_msgdefs       "to_con_eds_typedefs.h")
  set(TO_CON_CFGFILE_SRC_to_con_msgstruct     "to_con_eds_typedefs.h")
  set(TO_CON_CFGFILE_SRC_to_con_fcncodes      "to_con_eds_cc.h")

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(TO_CON_CFGFILE ${TO_CON_MISSION_CONFIG_FILE_LIST})
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
