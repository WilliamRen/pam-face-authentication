CMAKE_MINIMUM_REQUIRED(VERSION 2.6)



 PROJECT( PAM_FACE_AUTHENTICATION )

add_subdirectory(kgreet_plugin)
  

SET( QT_FACETRAINER_SRCS
      main.cpp
      opencvWebcam.cpp           
      detector.cpp               
      faceDetector.cpp          
      eyesDetector.cpp            
      tracker.cpp               
      verifier.cpp
      utils.cpp
      qtUtils.cpp
      webcamImagePaint.cpp
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
                 pam_face_authentication.cpp
                 opencvWebcam.cpp         
                 detector.cpp             
                 faceDetector.cpp       
                 eyesDetector.cpp

                 tracker.cpp              
                 verifier.cpp           
                 utils.cpp                
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



  FIND_PACKAGE( Qt4 REQUIRED )
  SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")
  FIND_PACKAGE(OpenCV REQUIRED)
SET( VERSION "0.3-SVN" )  
SET( PKGDATADIR "${CMAKE_INSTALL_PREFIX}/${DATA_INSTALL_DIR}" )
SET( BINDIR "${CMAKE_INSTALL_PREFIX}/${BIN_INSTALL_DIR}" )
ADD_DEFINITIONS( -g -O0 -Wall )
ADD_DEFINITIONS (-DVERSION='"${VERSION}"' -DPKGDATADIR="${PKGDATADIR}" -DBINDIR="${BINDIR}")
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
  ADD_EXECUTABLE( qt-facetrainer ${QT_FACETRAINER_SRCS} ${QT_FACETRAINER_MOC_SRCS} ${QT_FACETRAINER_RC_SRCS} ${QT_FACETRAINER_UI_HDRS} )
  TARGET_LINK_LIBRARIES( pam_face_authentication  ${PAM_LIBRARIES} ${OPENCV_LIBRARIES})
  TARGET_LINK_LIBRARIES( qt-facetrainer ${QT_LIBRARIES} ${OPENCV_LIBRARIES})

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
  FILES
   ${HAAR_CASCADES}
  DESTINATION
   ${PKGDATADIR}
)


