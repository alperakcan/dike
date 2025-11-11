
# dike

Dike is the goddess of justice and the spirit of moral order and fair judgement as a transcendent universal ideal or based on immemorial custom, in the sense of socially enforced norms and conventional rules.

This application is designed for to compare recorded routes with the planned routes and calculate the percentage of success based on how closely the intended path was followed.


## checkout

    git clone --recurse-submodules git@github.com:alperakcan/dike.git

    cd dike.git
    git pull
    git submodule update --init --recursive

# build

    make

## mingw

    make \
        CROSS_COMPILE_HOST=x86_64-w64-mingw32 \
        CROSS_COMPILE_PREFIX=x86_64-w64-mingw32-

## msys2

    make \
        __WINDOWS__=y \
        CXXFLAGS="-D__WINDOWS__=y"

## emsdk

    emmake make \
        __EMSDK__=y \
        CMAKE="emcmake cmake" \
        CXX=em++

## nginx

    types {
        application/wasm wasm;
    }

    server {
        listen       80;
        server_name  _;

        root /var/www/html;

        location /dike/ {
            alias /dike/src/;
            index dikeWasmES6.html;
            try_files $uri $uri/ =404;
        }
    }

### emscripten

#### clone

    git clone https://github.com/emscripten-core/emsdk.git emsdk.git
    cd emsdk.git
    git pull
    git submodule update --init --recursive
    git submodule sync

#### build

    ./emsdk install latest
    ./emsdk activate latest

#### activate

    source emsdk_env.sh
