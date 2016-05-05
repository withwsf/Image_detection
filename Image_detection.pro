#-------------------------------------------------
#
# Project created by QtCreator 2016-04-21T10:27:10
#
#-------------------------------------------------

QT       += core gui
CONFIG +=c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Image_detection
TEMPLATE = app
SOURCES += main.cpp\
        mainwindow.cpp \
    image_detection.cpp \
    nms.cpp \
    dockwidget.cpp \
    detector_warpper.cpp

HEADERS  += mainwindow.h \
        image_detection.h \
    nms.h \
    dockwidget.h \
    detector_warpper.h

FORMS    += mainwindow.ui \
    dockwidget.ui

INCLUDEPATH += /home/wsf/store/caffes/faster/py-faster-rcnn/caffe-fast-rcnn/include \
               /usr/include/opencv /usr/include/opencv2 \



LIBS += -L/home/wsf/store/caffes/faster_rcnn_for_cpu_only/py-faster-rcnn/caffe-fast-rcnn/.build_release/lib -lcaffe

LIBS+= -L/usr/local/lib  -lglog -lgflags -lprotobuf -lleveldb -lsnappy -llmdb -lboost_system -lhdf5_hl -lhdf5 -lm -lopencv_core -lopencv_highgui -lopencv_imgproc -lboost_thread -lstdc++  -lcblas -latlas  -lprotobuf
INCLUDEPATH +=/usr/include/python2.7/
INCLUDEPATH +=/usr/include/
LIBS += -lboost_python -lpython2.7 -lboost_system
