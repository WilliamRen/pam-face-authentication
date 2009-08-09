CMAKE_MINIMUM_REQUIRED(VERSION 2.6)



PROJECT( PAM_FACE_AUTHENTICATION )
SET( XWINDOWFACEAUTH_SRCS
     ${CMAKE_SOURCE_DIR}/src/xwindowFaceAuth.c
   )
SET( QT_FACETRAINER_SRCS
     ${CMAKE_SOURCE_DIR}/src/main.cpp
     ${CMAKE_SOURCE_DIR}/src/opencvWebcam.cpp
     ${CMAKE_SOURCE_DIR}/src/detector.cpp
     ${CMAKE_SOURCE_DIR}/src/faceDetector.cpp
     ${CMAKE_SOURCE_DIR}/src/eyesDetector.cpp
     ${CMAKE_SOURCE_DIR}/src/tracker.cpp
     ${CMAKE_SOURCE_DIR}/src/verifier.cpp
     ${CMAKE_SOURCE_DIR}/src/utils.cpp
     ${CMAKE_SOURCE_DIR}/src/qtUtils.cpp
     ${CMAKE_SOURCE_DIR}/src/webcamImagePaint.cpp
   )

SET( QT_FACETRAINER_MOC_HDRS
     ${CMAKE_SOURCE_DIR}/include/faceTrainer.h
     ${CMAKE_SOURCE_DIR}/include/faceTrainerAdvSettings.h
     ${CMAKE_SOURCE_DIR}/include/aboutBox.h
   )

SET( QT_FACETRAINER_UIS
     ${CMAKE_SOURCE_DIR}/data/ui/uifiles/faceTrainer.ui
     ${CMAKE_SOURCE_DIR}/data/ui/uifiles/faceTrainerAdvSettings.ui
     ${CMAKE_SOURCE_DIR}/data/ui/uifiles/aboutDialog.ui

   )

SET( QT_FACETRAINER_RCS
     graphics.qrc
   )


SET( PAM_FACE_AUTHENTICATE_SRCS
     ${CMAKE_SOURCE_DIR}/src/pam_face_authentication.cpp
     ${CMAKE_SOURCE_DIR}/src/opencvWebcam.cpp
     ${CMAKE_SOURCE_DIR}/src/detector.cpp
     ${CMAKE_SOURCE_DIR}/src/faceDetector.cpp
     ${CMAKE_SOURCE_DIR}/src/eyesDetector.cpp
     ${CMAKE_SOURCE_DIR}/src/webcamImagePaint.cpp
     ${CMAKE_SOURCE_DIR}/src/tracker.cpp
     ${CMAKE_SOURCE_DIR}/src/verifier.cpp
     ${CMAKE_SOURCE_DIR}/src/utils.cpp
   )



SET( HAAR_CASCADES
     ${CMAKE_SOURCE_DIR}/data/haarcascades/haarcascade_eye.xml
     ${CMAKE_SOURCE_DIR}/data/haarcascades/haarcascade_eye_tree_eyeglasses.xml
     ${CMAKE_SOURCE_DIR}/data/haarcascades/haarcascade_nose.xml
     ${CMAKE_SOURCE_DIR}/data/haarcascades/haarcascade.xml
   )

SET(BIN_INSTALL_DIR
    "/bin"
    CACHE PATH "The subdirectory relative to the install prefix for executables (default is bin)"
   )

SET(DATA_INSTALL_DIR
    "/share"
    CACHE PATH "The subdirectory relative to the install prefix where shared data will be installed (default is share)."
   )


FIND_PACKAGE( X11 REQUIRED )
FIND_PACKAGE( Qt4 REQUIRED )
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
FIND_PACKAGE(OpenCV REQUIRED)
SET( VERSION "0.3" )
SET( PKGDATADIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}" )
SET( BINDIR "${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_DIR}" )
SET( XWINDOW_BINDIR "${CMAKE_INSTALL_PREFIX}/kde/4/${BIN_INSTALL_DIR}" )
ADD_DEFINITIONS (-DVERSION='"${VERSION}"' -DPKGDATADIR=\"${PKGDATADIR}\" -DBINDIR=\"${BINDIR}\")
FIND_PACKAGE( PAM REQUIRED )

INCLUDE( ${QT_USE_FILE} )

QT4_ADD_RESOURCES( QT_FACETRAINER_RC_SRCS ${QT_FACETRAINER_RCS} )
QT4_WRAP_UI( QT_FACETRAINER_UI_HDRS ${QT_FACETRAINER_UIS} )
QT4_WRAP_CPP( QT_FACETRAINER_MOC_SRCS ${QT_FACETRAINER_MOC_HDRS} )

INCLUDE_DIRECTORIES( "${CMAKE_SOURCE_DIR}/include" )
INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${PAM_INCLUDE_DIR})
ADD_LIBRARY(pam_face_authentication SHARED ${PAM_FACE_AUTHENTICATE_SRCS})
SET_TARGET_PROPERTIES( pam_face_authentication PROPERTIES PREFIX "")
ADD_EXECUTABLE( xwindowFaceAuth ${XWINDOWFACEAUTH_SRCS})
TARGET_LINK_LIBRARIES( xwindowFaceAuth ${X11_LIBRARIES})

ADD_EXECUTABLE( qt-facetrainer ${QT_FACETRAINER_SRCS} ${QT_FACETRAINER_MOC_SRCS} ${QT_FACETRAINER_RC_SRCS} ${QT_FACETRAINER_UI_HDRS} )
TARGET_LINK_LIBRARIES( pam_face_authentication  ${PAM_LIBRARIES} ${OPENCV_LIBRARIES})
TARGET_LINK_LIBRARIES( qt-facetrainer ${QT_LIBRARIES} ${OPENCV_LIBRARIES})
 
if( UNIX AND NOT APPLE )
    if( NOT DESKTOP_ENTRY )
      set( DESKTOP_ENTRY qt-facetrainer.desktop )
    endif( NOT DESKTOP_ENTRY )
  
    add_custom_command( OUTPUT ${DESKTOP_ENTRY}
      COMMAND touch ${DESKTOP_ENTRY}
      COMMAND sh ${CMAKE_SOURCE_DIR}/qt-facetrainer-desktop.sh ${CMAKE_INSTALL_PREFIX} >${DESKTOP_ENTRY}
      DEPENDS ${CMAKE_SOURCE_DIR}/qt-facetrainer-desktop.sh
      COMMENT "Generating desktop entry file"
      )
    add_custom_target( DESKTOP_ENTRY_FILE ALL
      DEPENDS ${DESKTOP_ENTRY}
      )
    set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DESKTOP_ENTRY}" )
    set( APP_ICON ${CMAKE_SOURCE_DIR}/data/ui/images/pfa-logo.png )
  endif( UNIX AND NOT APPLE )



INSTALL(
    TARGETS
    pam_face_authentication
    DESTINATION
    /lib/security/
)

INSTALL(
    TARGETS
    qt-facetrainer
    DESTINATION
    ${BINDIR}
)

INSTALL(
    TARGETS
    xwindowFaceAuth
    DESTINATION
    ${XWINDOW_BINDIR}
)

INSTALL(
    FILES
    ${HAAR_CASCADES}
    DESTINATION
    ${PKGDATADIR}
)

INSTALL(
    FILES
    ${APP_ICON}
    DESTINATION
    ${PKGDATADIR}/icons/
)

INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_ENTRY} DESTINATION ${PKGDATADIR}/applications )

