FIND_PACKAGE( KDE4 REQUIRED )
ADD_DEFINITIONS( -g -O0 -Wall )

INCLUDE_DIRECTORIES( ./  ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
                   )
SET(BIN_INSTALL_DIR
    "/bin"
    CACHE PATH "The subdirectory relative to the install prefix for executables (default is bin)"
   )
SET( BINDIR "${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_DIR}" )

ADD_DEFINITIONS (-DBINDIR="\\"${BINDIR}\\"")

SET(kgreet_faceauthenticate_PART_SRCS
    kgreet_faceauthenticate.cpp
   )
SET(kgreet_faceauthenticate_PART_HDRS
    kgreet_faceauthenticate.h
   )

QT4_WRAP_CPP(kgreet_faceauthenticate_moc_SRCS ${kgreet_faceauthenticate_PART_HDRS} )
KDE4_ADD_PLUGIN(kgreet_faceauthenticate ${kgreet_faceauthenticate_PART_SRCS})
TARGET_LINK_LIBRARIES( kgreet_faceauthenticate ${KDE4_KDEUI_LIBS}  )
SET_TARGET_PROPERTIES( kgreet_faceauthenticate PROPERTIES PREFIX "")
INSTALL(TARGETS
        kgreet_faceauthenticate
        DESTINATION
        ${PLUGIN_INSTALL_DIR} )
