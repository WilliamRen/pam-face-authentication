FIND_PACKAGE( KDE4 REQUIRED )

INCLUDE_DIRECTORIES( ./  ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
	)
SET(kgreet_faceauthenticate_PART_SRCS 
kgreet_faceauthenticate.cpp
	)
KDE4_ADD_PLUGIN(kgreet_faceauthenticate ${kgreet_faceauthenticate_PART_SRCS})
TARGET_LINK_LIBRARIES( kgreet_faceauthenticate ${KDE4_KDEUI_LIBS} )
INSTALL(TARGETS kgreet_faceauthenticate DESTINATION ${PLUGIN_INSTALL_DIR} )
