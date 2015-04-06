######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGccCenterandwidth
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_CENTERANDWIDTH_LIBRARY

include( ../../../paths.pri )

QT += widgets xml

######################
# Internal libraries #
######################

# iricMisc

CONFIG(debug, debug|release) {
	LIBS += -L"../../misc/debug"
} else {
	LIBS += -L"../../misc/release"
}
LIBS += -liricMisc

# iricGuibase

CONFIG(debug, debug|release) {
	LIBS += -L"../../guibase/debug"
} else {
	LIBS += -L"../../guibase/release"
}
LIBS += -liricGuibase

# iricGuicore

CONFIG(debug, debug|release) {
	LIBS += -L"../../guicore/debug"
} else {
	LIBS += -L"../../guicore/release"
}
LIBS += -liricGuicore

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonComputationalGeometry-6.1 \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1

# Input
HEADERS += gcc_centerandwidth_global.h \
           gridcreatingconditioncenterandwidth.h \
           gridcreatingconditioncenterandwidthcoordinateseditdialog.h \
           gridcreatingconditioncenterandwidthdialog.h \
           gridcreatingconditioncreatorcenterandwidth.h
FORMS += gridcreatingconditioncenterandwidthcoordinateseditdialog.ui \
         gridcreatingconditioncenterandwidthdialog.ui
SOURCES += gridcreatingconditioncenterandwidth.cpp \
           gridcreatingconditioncenterandwidthcoordinateseditdialog.cpp \
           gridcreatingconditioncenterandwidthdialog.cpp \
           gridcreatingconditioncreatorcenterandwidth.cpp
RESOURCES += centerandwidth.qrc
TRANSLATIONS += languages/iricGccCenterandwidth_es_ES.ts \
                languages/iricGccCenterandwidth_fr_FR.ts \
                languages/iricGccCenterandwidth_id_ID.ts \
                languages/iricGccCenterandwidth_ja_JP.ts \
                languages/iricGccCenterandwidth_ko_KR.ts \
                languages/iricGccCenterandwidth_ru_RU.ts \
                languages/iricGccCenterandwidth_th_TH.ts \
                languages/iricGccCenterandwidth_vi_VN.ts \
                languages/iricGccCenterandwidth_zh_CN.ts
