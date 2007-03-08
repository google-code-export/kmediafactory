include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BUILD_DIR})

set(kmediafactory_SRCS
	sizewidget.cpp
	kmfuiinterface.cpp
	kmfprojectinterface.cpp
	main.cpp
	tools.cpp
	kmfoptions.cpp
	kmfapplication.cpp
	mediapage.cpp
	templatepage.cpp
	outputpage.cpp
	kmediafactory.cpp
	kmfproject.cpp
	projectoptions.cpp
	kmficonview.cpp
	kmflogger.cpp
	logview.cpp
	kmfprogresslistview.cpp
	kmfdbusinterface.cpp
	toolproperties.cpp
	kmfnewstuff.cpp
)

kde4_add_ui_files(kmediafactory_SRCS
	sizewidget.ui
	tools.ui
	kmfoptions.ui
	mediapage.ui
	templatepage.ui
	outputpage.ui
	projectoptions.ui
	logview.ui
	toolproperties.ui
)

kde4_automoc(${kmediafactory_SRCS})
qt4_add_dbus_adaptor(kmediafactory_SRCS org.kde.kmediafactory.KMediaFactory.xml kmfdbusinterface.h KMFDbusInterface)
kde4_add_kcfg_files(kmediafactory_SRCS kmediafactorysettings.kcfgc)
kde4_add_executable(kmediafactory ${kmediafactory_SRCS})
target_link_libraries(kmediafactory kmf kmediafactoryinterfaces ${KDE4_KPARTS_LIBS} ${KDE4_KHTML_LIBS} ${KDE4_KNEWSTUFF_LIBS})

install(TARGETS kmediafactory DESTINATION ${BIN_INSTALL_DIR})
install(FILES kmediafactoryui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES kmediafactory.kcfg DESTINATION ${KCFG_INSTALL_DIR})
