######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGccGridcombine
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_GRIDCOMBINE_LIBRARY

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
# Input
HEADERS += gcc_gridcombine_global.h \
           gridcreatingconditioncreatorgridcombine.h \
           gridcreatingconditiongridcombine.h \
           gridcreatingconditiongridcombinesettingdialog.h
FORMS += gridcreatingconditiongridcombinesettingdialog.ui
SOURCES += gridcreatingconditioncreatorgridcombine.cpp \
           gridcreatingconditiongridcombine.cpp \
           gridcreatingconditiongridcombinesettingdialog.cpp
TRANSLATIONS += languages/iricGccGridcombine_es_ES.ts \
                languages/iricGccGridcombine_fr_FR.ts \
                languages/iricGccGridcombine_id_ID.ts \
                languages/iricGccGridcombine_ja_JP.ts \
                languages/iricGccGridcombine_ko_KR.ts \
                languages/iricGccGridcombine_ru_RU.ts \
                languages/iricGccGridcombine_th_TH.ts \
                languages/iricGccGridcombine_vi_VN.ts \
                languages/iricGccGridcombine_zh_CN.ts
