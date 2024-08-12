
override DIKE_LIBMAKEFILE_PATH      = ${CURDIR}/3rdparty/libmakefile
override DIKE_3RDPARTY_PATH         = ${CURDIR}/3rdparty
override DIKE_3RDPARTY_INSTALL_PATH = ${CURDIR}/3rdparty/install

subdir-y = \
	3rdparty \
	src \
	test

3rdparty_makeflags-y = \
	DIKE_LIBMAKEFILE_PATH=${DIKE_LIBMAKEFILE_PATH} \
	DIKE_3RDPARTY_PATH=${DIKE_3RDPARTY_PATH} \
	DIKE_3RDPARTY_INSTALL_PATH=${DIKE_3RDPARTY_INSTALL_PATH}

src_makeflags-y = \
	DIKE_LIBMAKEFILE_PATH=${DIKE_LIBMAKEFILE_PATH} \
	DIKE_3RDPARTY_PATH=${DIKE_3RDPARTY_PATH} \
	DIKE_3RDPARTY_INSTALL_PATH=${DIKE_3RDPARTY_INSTALL_PATH}

src_depends-y = \
	3rdparty

test_makeflags-y = \
	DIKE_LIBMAKEFILE_PATH=${DIKE_LIBMAKEFILE_PATH} \
	DIKE_3RDPARTY_PATH=${DIKE_3RDPARTY_PATH} \
	DIKE_3RDPARTY_INSTALL_PATH=${DIKE_3RDPARTY_INSTALL_PATH}

test_depends-y = \
	src

include ${DIKE_LIBMAKEFILE_PATH}/Makefile.lib

tests: all
	${Q}+${MAKE} ${test_makeflags-y} -C test tests
