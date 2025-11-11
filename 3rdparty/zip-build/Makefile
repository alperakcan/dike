
CMAKE ?= cmake

target.extra-y = \
	install

include ${DIKE_LIBMAKEFILE_PATH}/Makefile.lib

ifeq (${__DARWIN__},y)

ARCHFLAGS     := -arch arm64 -arch x86_64
HOSTARCHFLAGS := -arch arm64 -arch x86_64

CFLAGS  := -mmacosx-version-min=10.14
LDFLAGS := -mmacosx-version-min=10.14

endif

patch:
	${Q}if [ ! -e .patch ]; then \
	  ( \
	    true \
	  ) && \
	  touch .patch; \
	fi

config: patch
	${Q}if [ ! -e .config ]; then \
	  ( \
	      rm -rf ../zip/build && \
	      mkdir -p ../zip/build && \
	      cd ../zip/build && \
	      CFLAGS="-g ${ARCHFLAGS} ${CFLAGS} -Wno-unknown-warning-option -Wno-strict-prototypes -Wno-calloc-transposed-args" \
	      LDFLAGS="${LDFLAGS}" \
	      MAKEFLAGS= \
	      CC=${CROSS_COMPILE_PREFIX}gcc \
	      ${CMAKE} \
	        -DCMAKE_INSTALL_PREFIX=/usr/local \
	        -DCMAKE_DISABLE_TESTING=ON \
	        .. \
	  ) && \
	  touch .config; \
	  rm -f .build; \
	fi

build: config
	${Q}if [ ! -e .build ]; then \
	  ( \
	      mkdir -p ../zip/build && \
	      cd ../zip/build && \
	      make \
	  ) && \
	  touch .build; \
	  rm -f .install; \
	fi

install: build
	${Q}if [ ! -e .install ]; then \
	  ( \
	      mkdir -p ../zip/build && \
	      cd ../zip/build && \
	      DESTDIR=${DIKE_3RDPARTY_INSTALL_PATH} make install \
	  ) && \
	  touch .install; \
	fi

clean:
	${Q}if [ -e .patch ]; then \
	  ( \
	    true \
	  ) && \
	  rm -rf .patch; \
	fi
	${Q}${RM} ../zip/build
	${Q}${RM} .config
	${Q}${RM} .build
	${Q}${RM} .install
