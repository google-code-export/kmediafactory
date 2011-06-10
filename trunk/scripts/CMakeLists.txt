set(kmediafactory_SCRIPTS
  kmf_frame
  kmf_info
  kmf_make_mpeg
  kmf_oo2pdf
  kmf_dvdauthor_version
  tools
)

if(NOT KEXIV2_FOUND)
set(kmediafactory_SCRIPTS ${kmediafactory_SCRIPTS} kmf_comment)
endif(NOT KEXIV2_FOUND)

install(FILES kmf_converter.oxt DESTINATION ${DATA_INSTALL_DIR}/kmediafactory/scripts)
install(PROGRAMS ${kmediafactory_SCRIPTS} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory/scripts)
