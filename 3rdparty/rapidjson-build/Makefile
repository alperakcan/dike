
CMAKE ?= cmake

target.extra-y = \
	install

include ${DIKE_LIBMAKEFILE_PATH}/Makefile.lib

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
	      mkdir -p ../rapidjson/build && \
	      cd ../rapidjson/build && \
	      CFLAGS=-g \
	      LDFLAGS= \
	      MAKEFLAGS= \
	      ${CMAKE} \
	      	-DCMAKE_INSTALL_PREFIX=/usr/local \
	        -DRAPIDJSON_BUILD_DOC=OFF \
	        -DRAPIDJSON_BUILD_EXAMPLES=OFF \
	        -DRAPIDJSON_BUILD_TESTS=OFF \
	        -DRAPIDJSON_BUILD_THIRDPARTY_GTEST=OFF \
	        .. \
	  ) && \
	  touch .config; \
	  rm -f .build; \
	fi

build: config
	${Q}if [ ! -e .build ]; then \
	  ( \
	      mkdir -p ../rapidjson/build && \
	      cd ../rapidjson/build && \
	      make \
	  ) && \
	  touch .build; \
	  rm -f .install; \
	fi

install: build
	${Q}if [ ! -e .install ]; then \
	  ( \
	      mkdir -p ../rapidjson/build && \
	      cd ../rapidjson/build && \
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
	${Q}${RM} ../rapidjson/build
	${Q}${RM} .config
	${Q}${RM} .build
	${Q}${RM} .install
