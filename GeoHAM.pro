#-------------------------------------------------
#
# Project created by QtCreator 2014-08-05T15:17:04
#
#-------------------------------------------------

QT          += core gui core-private

QT          += qml quick pdfwidgets svg

QT          += widgets xml


CONFIG += c++11

#QMAKE_LFLAGS += -fopenmp
#QMAKE_CXXFLAGS = += fopenmp
#LIBS += -fopenmp

#CONFIG += debug

RC_FILE = myapp.rc

TARGET = Geo0670
TEMPLATE = app

DEFINES += QPDFLIB_BUILD

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_USE_QSTRINGBUILDER


equals(QT_VERSION, 6.4.0) {
   message(Qt $$QT_VERSION Teste1.)
}

greaterThan(QT_MAJOR_VERSION, 5) {
   message(Qt $$QT_VERSION Teste2.)
}

lessThan(QT_MAJOR_VERSION, 6) {
} else {
   message(Qt $$QT_VERSION Teste3.)
}


SOURCES +=      main.cpp \
    Ajuda.cpp \
    Calc_Geral.cpp \
    Desloca_IMG.cpp \
    Dir_Principal.cpp \
    GDAL_HAM.cpp \
    ICE_TIFF.cpp \
    KML.cpp \
    Lista_Files.cpp \
    Mesclagem.cpp \
    Muda_resolucao.cpp \
    On_Checkbox.cpp \
    Processamento.cpp \
    Reducao.cpp \
    TIFF_KMZ.cpp \
    action.cpp \
    config_file.cpp \
    conversor_coordenadas.cpp \
    dialog_blend.cpp \
    dialog_conv.cpp \
    dialog_desloca_img.cpp \
    dialog_mescla_deslocando.cpp \
    dialog_rename.cpp \
    dialogexif.cpp \
    dialogmanual.cpp \
    dialogmanual2.cpp \
    dialogmudares.cpp \
    dialogpontos.cpp \
    dialogsobre.cpp \
    mainwindow.cpp \
    exif.cpp \
    geo_exif_xmp.cpp \
    translate.cpp \
    warp.cpp \
    mythread.cpp \
    gdalinfo.cpp \
    LatLong-UTM.cpp \
    stdoutredirector.cpp \
    widget.cpp \
    pdf.cpp \
    zoomselector.cpp \
    Screen_Res.cpp



HEADERS  +=     mainwindow.h \
                GDAL_HAM.h \
                config_file.h \
                conversor_coordenadas.h \
                dialog_blend.h \
                dialog_conv.h \
                dialog_desloca_img.h \
                dialog_mescla_deslocando.h \
                dialog_rename.h \
                dialogmanual2.h \
                dialogexif.h \
                dialogmanual.h \
                dialogmudares.h \
                dialogpontos.h \
                dialogsobre.h \
                exif.h \
                geo_exif_xmp.h \
                info_H.h \
                translate.h \
                warp.h \
                mythread.h \
                LatLong-UTM.h \
                stdoutredirector.h \
                widget.h \
                pdf.h \
                zoomselector.h

RESOURCES += \
             imgH.qrc \
             resources.qrc

RESOURCES +=

FORMS    += mainwindow.ui \
            dialog_blend.ui \
            dialog_conv.ui \
            dialog_desloca_img.ui \
            dialog_mescla_deslocando.ui \
            dialog_rename.ui \
            dialogexif.ui \
            dialogmanual.ui \
            dialogmanual2.ui \
            dialogmudares.ui \
            dialogpontos.ui \
            dialogsobre.ui \
            pdf.ui


INCLUDEPATH += "c:\\Dev\\opencv\\build\\include"

INCLUDEPATH += c:/Dev/GeoLibs/include

INCLUDEPATH += c:/Dev/gdal/include

TRANSLATIONS += \
    english.ts
CONFIG += lrelease
CONFIG += embed_translations


CONFIG(debug, debug|release) {
    message(debug build)

LIBS += -L"c:/Dev/opencv/build/lib/Debug" \
    -lopencv_calib3d455d  \
    -lopencv_core455d  \
#    -lopencv_dnn455d  \
    -lopencv_features2d455d  \
    -lopencv_flann455d  \
#    -lopencv_gapi455d  \
    -lopencv_highgui455d  \
    -lopencv_imgcodecs455d  \
    -lopencv_imgproc455d  \
    -lopencv_ml455d  \
#    -lopencv_objdetect455d  \
    -lopencv_photo455d  \
    -lopencv_stitching455d  \
#    -lopencv_ts455d  \
#    -lopencv_video455d  \
    -lopencv_videoio455d
#    -lopencv_world455d

#LIBS += -L"c:/Dev/Geolibs/lib/debug" \
#    -lGeographic_d #1.50

LIBS += -L"c:/Dev/Geolibs/lib/Debug" \
    -lGeographic_d-i #1.52

LIBS += -L"C:/Dev/Expat/Debug" \
    -llibexpatd

LIBS += -L"c:/Dev/gdal/lib" \
    -lgdal


LIBS += -L"C:/Dev/MS" \
    -luser32

#LIBS += -L"C:/ProgramData/Anaconda3/Library/lib" \
#    -llibssl-1_1-x64 \
#    -lsqlite3 \
#    -llibcurl \
#    -llibxml2 \
#    -llibdeflate \
#    -lliblz4 \
#    -lblosc \
#    -llerc \
#    -lzstd \
#    -lhdf5 \
#    -lcfitsio \
#    -lopenjp2 \
#    -liconv \
#    -lcharset


}

CONFIG(release, debug|release) {
    message(release build)

LIBS += -L"c:/Dev/opencv/build/lib/Release" \
    -lopencv_calib3d455  \
    -lopencv_core455  \
#    -lopencv_dnn455  \
    -lopencv_features2d455  \
    -lopencv_flann455  \
#    -lopencv_gapi455  \
    -lopencv_highgui455  \
    -lopencv_imgcodecs455  \
    -lopencv_imgproc455  \
    -lopencv_ml455  \
#    -lopencv_objdetect455  \
    -lopencv_photo455  \
    -lopencv_stitching455  \
#    -lopencv_ts455  \
#    -lopencv_video455  \
    -lopencv_videoio455
#    -lopencv_world455


LIBS += -L"C:/Dev/Expat/Release" \
    -llibexpat

LIBS += -L"C:/Dev/Geos/lib/Release" \
    -lgeos


LIBS += -L"c:/Dev/gdal/lib" \
    -lgdal

LIBS += -L"c:/Dev/Geolibs/lib/Release" \
    -lGeographic-i

LIBS += -L"C:/Dev/Proj/lib/Release" \
    -lproj

LIBS += -L"C:/Dev/MS" \
    -luser32

#LIBS += -L"C:/ProgramData/Anaconda3/Library/lib" \
#    -llibssl \
#    -lsqlite3 \
#    -llibcurl \
#    -llibxml2 \
#    -llibdeflate \
#    -lliblz4 \
#    -lblosc \
#    -llerc \
#    -lzstd \
#    -lhdf5 \
#    -lcfitsio \
#    -lopenjp2 \
#    -liconv \
#    -lcharset

#LIBS += -L"C:/QT_Projects/Diminui/build-Geo-Desktop_Qt_5_14_2_MSVC2017_64bit-Release/qpdflib/release" \
#LIBS += -L"C:/QT_Projects/Deploy/Geo_641" \
#    -lqpdf

}
