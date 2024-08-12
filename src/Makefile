
target-y = \
	dike${target_extension}

dike${target_extension}_files-y += \
	dike.cpp \
	DikeDebug.cpp \
	DikeMethod.cpp \
	DikeMethodBrute.cpp \
	DikeMethodQuadTree.cpp \
	DikePoint.cpp \
	DikePath.cpp

dike${target_extension}_files-${__WINDOWS__} += \
	strptime.c

dike${target_extension}_ldflags-y += \
	-lzip \
	-lexpat

dike${target_extension}_includes-y += \
	${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/include

dike${target_extension}_libraries-y += \
	${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/lib

dist.dir        = ../dist
dist.bin-y      = dike${target_extension}

include ${DIKE_LIBMAKEFILE_PATH}/Makefile.lib
