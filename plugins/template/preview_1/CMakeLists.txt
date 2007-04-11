set(template
  preview_1.kmft
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

add_custom_target(${template} ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${template}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${template} DESTINATION ${DATA_INSTALL_DIR}/kmediafactory_template)

# add_custom_target(kmft-messages
#   COMMAND ${EXTRACTRC_EXECUTABLE} --tag=text --tag-group=none ./template.xml > rc.cpp
#   COMMAND ${XGETTEXT_EXECUTABLE} --keyword=i18n rc.cpp -o template.pot
#   COMMAND echo "//i18n: file ./template.xml" > rc.cpp
#   COMMAND echo "//xgettext: no-c-format" >> rc.cpp
#   COMMAND tmp=`grep \"<template\" template.xml | sed -e 's/^.*name=\"//' | sed -e 's/\".*//'` && echo \"i18n\(\\"$$tmp\\\"\);" >> rc.cpp
#   COMMAND ${EXTRACTRC_EXECUTABLE} ./settings.ui >> rc.cpp
#   COMMAND ${XGETTEXT_EXECUTABLE} --keyword=i18n rc.cpp -o ui.pot
#   COMMAND rm rc.cpp
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )
#
# add_custom_target(kmft-merge
#   COMMAND
#     for f in locale/*\; do
#       if test -d \"$$f\"\; then
#         for pot in \"template\" \"ui\"\; do
#           if test -e \"$$pot.pot\"\; then
#             ${MSGMERGE_EXECUTABLE} -U \"$$f/LC_MESSAGES/$$pot.po\" \"$$pot.pot\"\;
#           fi\;
#         done\;
#       fi\;
#     done\;
#   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
# )

