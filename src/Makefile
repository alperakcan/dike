
ifeq (${__EMSDK__}, y)

target.extra-y = \
	DikeWasm.js \
	DikeWasmES6.js

else

target-y = \
	DikeCalculate${target_extension} \
	DikeInflate${target_extension} \

endif

Dike_files-y += \
	DikeDebug.cpp \
	DikeMethod.cpp \
	DikeMethodBruteForce.cpp \
	DikeMethodQuadTree.cpp \
	DikePoint.cpp \
	DikePath.cpp \
	DikeProjection.cpp \
	DikeProjectionUTM.cpp \
	DikeProjectionMercator.cpp

Dike_files-${__WINDOWS__} += \
	strptime.c

Dike_ldflags-y += \
	-lClipper2 \
	-lzip \
	-lexpat \
	-lm

Dike_ldflags-${__WINDOWS__} += \
	-static

Dike_includes-y += \
	${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/include

Dike_libraries-y += \
	${DIKE_3RDPARTY_INSTALL_PATH}/usr/local/lib

DikeCalculate${target_extension}_files-y += \
	${Dike_files-y} \
	DikeCalculate.cpp

DikeCalculate${target_extension}_ldflags-y += \
	${Dike_ldflags-y}

DikeCalculate${target_extension}_includes-y += \
	${Dike_includes-y}

DikeCalculate${target_extension}_libraries-y += \
	${Dike_libraries-y}

DikeInflate${target_extension}_files-y += \
	${Dike_files-y} \
	DikeInflate.cpp

DikeInflate${target_extension}_ldflags-y += \
	${Dike_ldflags-y}

DikeInflate${target_extension}_includes-y += \
	${Dike_includes-y}

DikeInflate${target_extension}_libraries-y += \
	${Dike_libraries-y}

ifeq (${__EMSDK__}, y)

dist.dir        = ../dist
dist.base       = dike
dist.share-y    = DikeWasm.js
dist.share-y   += DikeWasm.wasm
dist.share-y   += DikeWasmES6.js
dist.share-y   += DikeWasmES6.wasm
dist.share-y   += DikeCalculate.html
dist.share-y   += DikeInflate.html

else

dist.dir        = ../dist
dist.bin-y     += DikeCalculate${target_extension}
dist.bin-y     += DikeInflate${target_extension}

endif

include ${DIKE_LIBMAKEFILE_PATH}/Makefile.lib

EXPORTED_FUNCTIONS  = '[
EXPORTED_FUNCTIONS += "_malloc", "_free",
EXPORTED_FUNCTIONS += "_calculateCreate", "_calculateDestroy", "_calculateReset", "_calculateSetMethod", "_calculateAddTrack", "_calculateAddRecord", "_calculateCalculate",
EXPORTED_FUNCTIONS += "_inflateCreate", "_inflateDestroy", "_inflateReset", "_inflateSetOptions", "_inflateAddTrack", "_inflateInflate"
EXPORTED_FUNCTIONS += ]'

DikeWasm.js: DikeWasm.cpp ${Dike_files-y} Makefile
	em++ -o DikeWasm.js DikeWasm.cpp ${Dike_files-y} \
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
	  -s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS} \
	  -s EXPORTED_RUNTIME_METHODS='["HEAPU8", "getValue", "cwrap"]' \
	  --minify 0 \
	  -lClipper2 \
	  -lzip \
	  -lexpat \
	  -lm

DikeWasmES6.js: DikeWasm.cpp ${Dike_files-y} Makefile
	em++ -o DikeWasmES6.js DikeWasm.cpp ${Dike_files-y} \
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
	  -s EXPORTED_FUNCTIONS=${EXPORTED_FUNCTIONS} \
	  -s EXPORTED_RUNTIME_METHODS='["HEAPU8", "getValue", "cwrap"]' \
	  --minify 0 \
	  -lClipper2 \
	  -lzip \
	  -lexpat \
	  -lm

clean:
	${Q}${RM} DikeWasmES6.js
	${Q}${RM} DikeWasmES6.wasm
	${Q}${RM} DikeWasm.js
	${Q}${RM} DikeWasm.wasm
	${Q}true
