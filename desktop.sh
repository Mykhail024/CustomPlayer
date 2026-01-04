#!/bin/sh


if [[ $# -eq 0 ]]; then
    echo "use -h for help"
    exit 1
fi

configure=0
build=0
run=0
update_translations=0

while getopts "cbrth" opt; do
    case "$opt" in
        c) configure=1 ;;
        b) build=1 ;;
        r) run=1 ;;
        t) update_translations=1 ;;
        h)
            echo "./android.sh [command]"
            echo "    -c        configure"
            echo "    -b        build"
            echo "    -r        run"
            echo "    -t        update translations"
            exit 1
            ;;
    esac
done

if [ "$configure" -eq 1 ]; then
    cmake -B build/desktop
    cp build/desktop/compile_commands.json build/
fi

if [ "$update_translations" -eq 1 ]; then
    cmake --build build/desktop --target update_translations
fi

if [ "$build" -eq 1 ]; then
    cmake --build build/desktop -j `(nproc)`
fi

if [ "$run" -eq 1 ]; then
    ./build/desktop/bin/customplayer
fi
