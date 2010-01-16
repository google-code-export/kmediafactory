include_directories(${CMAKE_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BUILD_DIR})

set(kmediafactory_SRCS
	sizewidget.cpp
	kmfplugininterface.cpp
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
	logview.cpp
	kmfprogressitemdelegate.cpp
	kmfdbusinterface.cpp
	toolproperties.cpp
)

kde4_add_ui_files(kmediafactory_SRCS
	tools.ui
	kmfoptions.ui
	mediapage.ui
	templatepage.ui
	outputpage.ui
	projectoptions.ui
	logview.ui
	toolproperties.ui
)

qt4_add_dbus_adaptor(kmediafactory_SRCS org.kde.kmediafactory.KMediaFactory.xml kmfdbusinterface.h KMFDbusInterface)
kde4_add_kcfg_files(kmediafactory_SRCS kmediafactorysettings.kcfgc)
kde4_add_executable(kmediafactory ${kmediafactory_SRCS})
if (KDE_4_4)
target_link_libraries(kmediafactory kmf kmediafactoryinterfaces ${KDE4_KNEWSTUFF3_LIBS} ${QT_QTWEBKIT_LIBRARY})
else (KDE_4_4)
target_link_libraries(kmediafactory kmf kmediafactoryinterfaces ${KDE4_KNEWSTUFF2_LIBS} ${QT_QTWEBKIT_LIBRARY})
endif (KDE_4_4)

install(TARGETS kmediafactory DESTINATION ${BIN_INSTALL_DIR})
install(FILES kmediafactoryui.rc DESTINATION ${DATA_INSTALL_DIR}/kmediafactory)
install(FILES kmediafactory.kcfg DESTINATION ${KCFG_INSTALL_DIR})
if (KDE_4_4)
install(FILES kmediafactory-kns3.knsrc DESTINATION ${CONFIG_INSTALL_DIR} RENAME kmediafactory.knsrc)
else (KDE_4_4)
install(FILES kmediafactory.knsrc DESTINATION ${CONFIG_INSTALL_DIR})
endif (KDE_4_4)
install(FILES kmediafactory.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
