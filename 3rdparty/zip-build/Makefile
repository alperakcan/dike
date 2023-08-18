
target.extra-y = \
	install

include ${RIDETURKEY_GPXCOMPARE_LIBMAKEFILE_PATH}/Makefile.lib

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
	      CFLAGS="-g ${ARCHFLAGS} ${CFLAGS} -Wno-strict-prototypes" \
	      LDFLAGS="${LDFLAGS}" \
	      MAKEFLAGS= \
	      CC=${CROSS_COMPILE_PREFIX}gcc \
	      cmake \
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
	      DESTDIR=${RIDETURKEY_GPXCOMPARE_3RDPARTY_INSTALL_PATH} make install \
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
