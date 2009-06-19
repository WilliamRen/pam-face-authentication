DEBUG = -g -O0
INCLUDES = \
	-DVERSION=\"".3"\"  \
	-DPKGDATADIR=\""$(pkgdatadir)"\"  \
	-DBINDIR=\""$(bindir)"\"  	\
	-DSYSCONFDIR=\""$(sysconfdir)"\"   \
	$(OPENCV_CFLAGS) \
	$(QTCORE_CFLAGS) \
	$(QTGUI_CFLAGS)

bin_PROGRAMS = qt-facemanager
qt_facemanager_LDFLAGS = -export-dynamic
qt_facemanager_SOURCES = main.cpp \
			qrc_graphics.cpp \
			opencvWebcam.cpp \
			detector.cpp \
			detector.h\
			faceDetector.cpp \
			faceDetector.h \
			eyesDetector.cpp \
			eyesDetector.h \
			tracker.cpp \
			tracker.h \
			utils.cpp \
			opencvWebcam.h \
			faceTrainer.h \
			faceTrainerAdvSettings.h \
			ui_faceTrainerAdvSettings.h \
			ui_faceTrainer.h

qt_facemanager_LDADD = \
			$(QTCORE_LIBS) \
			$(OPENCV_LIBS) \
			$(QTGUI_LIBS)

nodist_qt_facemanager_SOURCES = \
	moc_faceTrainer.cpp \
	moc_faceTrainerAdvSettings.cpp


EXTRA_DIST = \
	graphics.qrc


# This rule lets GNU make create any moc_*.cpp from the equivalent *.h
ui_%.h: %.ui
	uic $< -o $@

moc_%.cpp: %.h
	moc $< -o $@

qrc_graphics.cpp : graphics.qrc
	rcc -name graphics graphics.qrc -o qrc_graphics.cpp


CLEANFILES =$(nodist_qt_facemanager_SOURCES) ui_faceTrainerAdvSettings.h ui_faceTrainer.h qrc_graphics.cpp

