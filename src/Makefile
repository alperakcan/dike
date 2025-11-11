
ifeq (${__EMSDK__}, y)

target.extra-y = \
	DikeWasm.js \
	DikeWasmES6.js

else

target-y = \
	dike${target_extension}

endif

dike${target_extension}_sources-y += \
	dike.cpp \
	DikeDebug.cpp \
	DikeMethod.cpp \
	DikeMethodBruteForce.cpp \
	DikeMethodQuadTree.cpp \
	DikePoint.cpp \
	DikePath.cpp

dike${target_extension}_files-y += \
	dike.cpp \
	${dike${target_extension}_sources-y}

dike${target_extension}_files-${__WINDOWS__} += \
	strptime.c

dike${target_extension}_ldflags-y += \
	-lzip \
	-lexpat \
	-lm

dike${target_extension}_ldflags-${__WINDOWS__} += \
	-static

dike${target_extension}_includes-y += \
	${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/include

dike${target_extension}_libraries-y += \
	${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/lib

ifeq (${__EMSDK__}, y)

dist.dir        = ../dist
dist.base       = dike
dist.share-y    = DikeWasm.js
dist.share-y   += DikeWasm.wasm
dist.share-y   += DikeWasmES6.js
dist.share-y   += DikeWasmES6.wasm
dist.share-y   += dikeWasmES6.html

else

dist.dir        = ../dist
dist.bin-y      = dike${target_extension}

endif

include ${DIKE_LIBMAKEFILE_PATH}/Makefile.lib

DikeWasm.js: wasm.cpp ${dike${target_extension}_sources-y} Makefile
	em++ -o DikeWasm.js wasm.cpp ${dike${target_extension}_sources-y} \
	  -I ${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/include \
	  -L ${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/lib \
	  -O3 \
	  -s MODULARIZE=1 \
	  -s EXPORT_NAME="DikeWasm" \
	  -s EXPORT_ES6=0 \
	  -s WASM=1 \
	  -s ASSERTIONS=0 \
	  -s NO_EXIT_RUNTIME=1 \
	  -s ALLOW_MEMORY_GROWTH=1 \
	  -s EXPORTED_FUNCTIONS='["_malloc", "_free", "_create", "_destroy", "_reset", "_setMethod", "_addTrack", "_addRecord", "_calculate"]' \
	  -s EXPORTED_RUNTIME_METHODS='["HEAPU8", "getValue", "cwrap"]' \
	  --minify 0 \
	  -lzip \
	  -lexpat \
	  -lm

DikeWasmES6.js: wasm.cpp ${dike${target_extension}_sources-y} Makefile
	em++ -o DikeWasmES6.js wasm.cpp ${dike${target_extension}_sources-y} \
	  -I ${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/include \
	  -L ${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/lib \
	  -O3 \
	  -s MODULARIZE=1 \
	  -s EXPORT_NAME="DikeWasm" \
	  -s EXPORT_ES6=1 \
	  -s WASM=1 \
	  -s ASSERTIONS=0 \
	  -s NO_EXIT_RUNTIME=1 \
	  -s ALLOW_MEMORY_GROWTH=1 \
	  -s EXPORTED_FUNCTIONS='["_malloc", "_free", "_create", "_destroy", "_reset", "_setMethod", "_addTrack", "_addRecord", "_calculate"]' \
	  -s EXPORTED_RUNTIME_METHODS='["HEAPU8", "getValue", "cwrap"]' \
	  --minify 0 \
	  -lzip \
	  -lexpat \
	  -lm

clean:
	${Q}${RM} DikeWasmES6.js
	${Q}${RM} DikeWasmES6.wasm
	${Q}${RM} DikeWasm.js
	${Q}${RM} DikeWasm.wasm
	${Q}true
