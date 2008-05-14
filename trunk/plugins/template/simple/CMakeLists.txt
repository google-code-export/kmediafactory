set(template
  simple.kmft
)

set(template_files
  mimetype
  template.xml
  settings.kcfg
  settings.ui
  icon.png
  preview.jpg
  images/button.png
  images/prev.png
  images/back.png
  images/next.png
)

FILE(GLOB dirs "${CMAKE_CURRENT_SOURCE_DIR}/locale/??")
FOREACH(dir ${dirs})
  LIST(APPEND po_files ${dir}/LC_MESSAGES/template.po ${dir}/LC_MESSAGES/ui.po)
ENDFOREACH(dir)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${template}
  COMMAND ${CMAKE_COMMAND}
      -Dtemplate="${template}"
      -Dtemplate_files="${template_files}"
      -Dsrc="${CMAKE_CURRENT_SOURCE_DIR}"
      -Ddest="${CMAKE_CURRENT_BINARY_DIR}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/../template.cmake"
  DEPENDS ${template_files} ${po_files}
)

add_custom_target(template ALL 
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${template}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${template} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory_template)
