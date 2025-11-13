
# dike

Dike is the goddess of justice and the spirit of moral order and fair judgement as a transcendent universal ideal or based on immemorial custom, in the sense of socially enforced norms and conventional rules.

The Dike suite provides both native command-line applications for terminal use and WebAssembly-powered HTML interfaces for easy browser-based access, enabling flexible deployment across development and production environments.

## calculator

Dike is a route success calculator specifically designed for RideTurkey to analyze GPS tracking data and measure route compliance. This application provides an objective assessment of how closely recorded GPS tracks follow their intended planned routes, delivering success percentage calculations.

The application uses advanced spatial algorithms to perform proximity-based matching between planned routes (tracks) and actual GPS recordings (records). Results include detailed statistics on matched points, total distance coverage, and comprehensive performance metrics with millisecond-precision timing information.

## inflator

Dike inflator creates buffer corridors around GPS tracks to generate coverage zones for route planning and analysis. It transforms linear GPS tracks (GPX/KML/KMZ) into polygon corridors with a specified coverage radius, representing the accessible area around the route.

The inflator uses UTM (Universal Transverse Mercator) projection for accurate ground-distance buffering, automatically detecting the appropriate zone with support for Norway and Svalbard exceptions.

Output formats include KML and GPX, with configurable coverage radius in meters. The implementation ensures coordinate precision across native and WebAssembly builds, maintaining sub-meter accuracy for geospatial applications.

## developer

### checkout

    git clone --recurse-submodules git@github.com:alperakcan/dike.git

    cd dike.git
    git pull
    git submodule update --init --recursive

### build

    make

### mingw

    make \
        CROSS_COMPILE_HOST=x86_64-w64-mingw32 \
        CROSS_COMPILE_PREFIX=x86_64-w64-mingw32-

### msys2

    make \
        __WINDOWS__=y \
        CXXFLAGS="-D__WINDOWS__=y"

### emsdk

    emmake make \
        __EMSDK__=y \
        CMAKE="emcmake cmake" \
        CXX=em++

#### emscripten

##### clone

    git clone https://github.com/emscripten-core/emsdk.git emsdk.git
    cd emsdk.git
    git pull
    git submodule update --init --recursive
    git submodule sync

##### build

    ./emsdk install latest
    ./emsdk activate latest

##### activate

    source emsdk_env.sh

### nginx

    types {
        application/wasm wasm;
    }

    server {
        listen       80;
        server_name  _;

        root /var/www/html;

        location /dike {
            alias /dike/src/;
            index DikeCalculate.html;
            try_files $uri $uri/ =404;
        }

        location /dike/calculate {
            alias /dike/src/;
            index DikeCalculate.html;
            try_files $uri $uri/ =404;
        }
    }
