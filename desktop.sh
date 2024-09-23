#!/bin/sh


if [[ $# -eq 0 ]]; then
    echo "use -h for help"
    exit 1
fi

configure=0
build=0
install=0
run=0

while getopts "cbirh" opt; do
    case "$opt" in
        c) configure=1 ;;
        b) build=1 ;;
        i) install=1 ;;
        r) run=1 ;;
        h)
            echo "./android.sh [command]"
            echo "    -c        configure"
            echo "    -b        build"
            echo "    -r        run"
            exit 1
            ;;
    esac
done

if [ "$configure" -eq 1 ]; then
    cmake -B build/desktop -G Ninja
    cp build/desktop/compile_commands.json build/
fi

if [ "$build" -eq 1 ]; then
    ninja -C build/desktop -j16
fi

if [ "$run" -eq 1 ]; then
    ./build/desktop/customplayer
fi
