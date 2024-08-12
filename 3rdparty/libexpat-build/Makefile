
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
	      mkdir -p ../libexpat/build && \
	      cd ../libexpat/build && \
	      CFLAGS=-g \
	      LDFLAGS= \
	      MAKEFLAGS= \
	      cmake \
	        -DEXPAT_BUILD_DOCS=OFF \
	        -DEXPAT_BUILD_EXAMPLES=OFF \
	        -DEXPAT_BUILD_TESTS=OFF \
	        -DEXPAT_BUILD_TOOLS=OFF \
		-DEXPAT_SHARED_LIBS=OFF \
	        ../expat \
	  ) && \
	  touch .config; \
	  rm -f .build; \
	fi

build: config
	${Q}if [ ! -e .build ]; then \
	  ( \
	      mkdir -p ../libexpat/build && \
	      cd ../libexpat/build && \
	      make \
	  ) && \
	  touch .build; \
	  rm -f .install; \
	fi

install: build
	${Q}if [ ! -e .install ]; then \
	  ( \
	      mkdir -p ../libexpat/build && \
	      cd ../libexpat/build && \
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
	${Q}${RM} ../libexpat/build
	${Q}${RM} .config
	${Q}${RM} .build
	${Q}${RM} .install
