######################################################################
# Automatically generated by qmake (2.01a) ? 10 23 18:49:25 2014
######################################################################

TARGET = iricGccRiversurvey
TEMPLATE = lib
INCLUDEPATH += ../..

DEFINES += GCC_RIVERSURVEY_LIBRARY

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

# iricRdRiversurvey

CONFIG(debug, debug|release) {
	LIBS += -L"../../rawdata/riversurvey/debug"
} else {
	LIBS += -L"../../rawdata/riversurvey/release"
}
LIBS += -liricRdRiversurvey

######################
# External libraries #
######################

# VTK

LIBS += \
	-lvtkCommonCore-6.1 \
	-lvtkCommonDataModel-6.1 \
	-lvtkCommonExecutionModel-6.1 \
	-lvtkFiltersCore-6.1 \
	-lvtkFiltersExtraction-6.1 \
	-lvtkFiltersGeometry-6.1 \
	-lvtkIoCore-6.1 \
	-lvtkIoLegacy-6.1 \
	-lvtkRenderingCore-6.1 \
	-lvtkRenderingLabel-6.1 \
	-lvtkRenderingLOD-6.1

# Input
HEADERS += gcc_riversurvey_global.h \
					 gridcreatingconditioncreatorriversurvey.h \
					 gridcreatingconditionriversurvey.h \
					 gridcreatingconditionriversurveypointadddialog.h \
					 gridcreatingconditionriversurveypointmovedialog.h \
					 gridcreatingconditionriversurveypointregionadddialog.h \
					 gridcreatingconditionriversurveypointrepositiondialog.h \
					 gridcreatingconditionriversurveyregiondialog.h
FORMS += gridcreatingconditionriversurveypointadddialog.ui \
				 gridcreatingconditionriversurveypointmovedialog.ui \
				 gridcreatingconditionriversurveypointregionadddialog.ui \
				 gridcreatingconditionriversurveypointrepositiondialog.ui \
				 gridcreatingconditionriversurveyregiondialog.ui
SOURCES += gridcreatingconditioncreatorriversurvey.cpp \
					 gridcreatingconditionriversurvey.cpp \
					 gridcreatingconditionriversurveypointadddialog.cpp \
					 gridcreatingconditionriversurveypointmovedialog.cpp \
					 gridcreatingconditionriversurveypointregionadddialog.cpp \
					 gridcreatingconditionriversurveypointrepositiondialog.cpp \
					 gridcreatingconditionriversurveyregiondialog.cpp
RESOURCES += riversurvey.qrc
TRANSLATIONS += languages/iricGccRiversurvey_es_ES.ts \
                languages/iricGccRiversurvey_fr_FR.ts \
                languages/iricGccRiversurvey_id_ID.ts \
                languages/iricGccRiversurvey_ja_JP.ts \
                languages/iricGccRiversurvey_ko_KR.ts \
                languages/iricGccRiversurvey_ru_RU.ts \
                languages/iricGccRiversurvey_th_TH.ts \
                languages/iricGccRiversurvey_vi_VN.ts \
                languages/iricGccRiversurvey_zh_CN.ts
