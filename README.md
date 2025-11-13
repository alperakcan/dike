
# dike

Dike is the goddess of justice and the spirit of moral order and fair judgement as a transcendent universal ideal or based on immemorial custom, in the sense of socially enforced norms and conventional rules.

## overview

Dike is a route success calculator specifically designed for RideTurkey to analyze GPS tracking data and measure route compliance. This application provides an objective assessment of how closely recorded GPS tracks follow their intended planned routes, delivering success percentage calculations.

## features

- **Route Analysis**: Compare recorded GPS tracks against planned base routes
- **Success Metrics**: Calculate percentage-based success rates for both point coverage and distance coverage
- **Multiple Calculation Methods**:
  - QuadTree algorithm for efficient spatial indexing and fast calculations
  - Brute Force method (available for comprehensive analysis)
- **Configurable Parameters**: Adjustable coverage radius to fine-tune matching sensitivity
- **Web-Based Interface**: Modern, responsive web application built with Bootstrap
- **File Support**: Upload and process multiple GPS track files simultaneously
- **Real-Time Processing**: WebAssembly-powered calculations for optimal performance

## use cases

- **Route Compliance Verification**: Ensure riders follow designated paths during organized events
- **Performance Analysis**: Measure how accurately participants navigate planned routes
- **Quality Assessment**: Evaluate GPS tracking accuracy and route adherence
- **Event Management**: Validate completion rates and identify deviations from planned routes

## implementation

The application uses advanced spatial algorithms to perform proximity-based matching between planned routes (tracks) and actual GPS recordings (records). Results include detailed statistics on matched points, total distance coverage, and comprehensive performance metrics with millisecond-precision timing information.

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
