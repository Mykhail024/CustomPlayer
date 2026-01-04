#!/bin/sh

if [[ $# -eq 0 ]]; then
    echo "use -h for help"
    exit 1
fi

configure=0
build=0
install=0
run=0
waydroid="OFF"

while getopts "wcbirh" opt; do
    case "$opt" in
        c) configure=1 ;;
        b) build=1 ;;
        i) install=1 ;;
        r) run=1 ;;
        w) waydroid="ON" ;;
        h)
            echo "./android.sh [command]"
            echo "    -w        enable waydroid container support"
            echo "    -c        configure"
            echo "    -b        build"
            echo "    -i        install using adb"
            echo "    -r        run using adb"
            exit 1
            ;;
    esac
done

if [ "$configure" -eq 1 ]; then
    /usr/bin/android-aarch64-cmake -B build/android -DCMAKE_BUILD_TYPE=Debug \
                -DQT_ANDROID_ABIS="arm64-v8a" \
                -DANDROID_PLATFORM=android-23 \
                -DANDROID_SDK_ROOT="/home/hong19/.android/sdk" \
                -DANDROID_NDK="/home/hong19/.android/sdk/ndk/26.3.11579264" \
                -DCMAKE_TOOLCHAIN_FILE="/home/hong19/.android/sdk/ndk/26.3.11579264/build/cmake/android.toolchain.cmake" \
                -DCMAKE_FIND_ROOT_PATH="/opt/android-libs/aarch64" \
                -DWAYDROID=$waydroid
fi

if [ "$build" -eq 1 ]; then
    cmake --build build/android -j16
fi

if [ "$install" -eq 1 ]; then
    if [ "$waydroid" = "ON" ]; then
        waydroid app remove org.mykhailo.customplayer
        waydroid app install build/android/src/android-build/customplayer.apk
    else
        cmake --build build/android --target install_apk
    fi
fi

if [ "$run" -eq 1 ]; then
    if [ "$waydroid" = "ON" ]; then
        waydroid app launch org.mykhailo.customplayer
    else
        cmake --build build/android --target run_apk
    fi

fi
