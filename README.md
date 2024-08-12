
# dike

Dike is the goddess of justice and the spirit of moral order and fair judgement as a transcendent universal ideal or based on immemorial custom, in the sense of socially enforced norms and conventional rules.

This application is designed for RideTurkey to compare recorded routes with the planned routes and calculate the percentage of success based on how closely the intended path was followed.


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

    make \
        CMAKE="emcmake cmake" \
        CXX=em++
