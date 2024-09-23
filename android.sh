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
            echo "    -i        install using adb"
            echo "    -r        run using adb"
            exit 1
            ;;
    esac
done

if [ "$configure" -eq 1 ]; then
    /usr/bin/android-aarch64-cmake -B build/android -G Ninja \
                -DQT_ANDROID_ABIS="arm64-v8a" \
                -DQT_DEFAULT_ANDROID_ABIS="arm64-v8a" \
                -DCMAKE_SYSTEM_NAME=Android \
                -DANDROID_STL="c++_shared" \
                -DQT_ANDROID_BUILD_ALL_ABIS=FALSE \
                -DANDROID_SDK_ROOT="/home/hong19/.android/sdk" \
                -DANDROID_NDK="/home/hong19/.android/sdk/ndk/26.3.11579264" \
                -DANDROID_PLATFORM=android-23 \
                -DQT_PATH_ANDROID_ABI_arm64-v8a="/opt/android-libs/aarch64/" \
                -DQT_ANDROID_PATH_CMAKE_DIR_arm64-v8a="/opt/android-libs/aarch64/lib/cmake/" \
                -DCMAKE_SYSTEM_NAME=Android \
                -DCMAKE_SYSTEM_VERSION=24 \
                -DQT_NO_GLOBAL_APK_TARGET_PART_OF_ALL=OFF \
                -DCMAKE_TOOLCHAIN_FILE="/home/hong19/.android/sdk/ndk/26.3.11579264/build/cmake/android.toolchain.cmake" \
                -DCMAKE_PREFIX_PATH="/opt/android-libs/aarch64" \
                -DCMAKE_FIND_ROOT_PATH="/opt/android-libs/aarch64" \
                -DQT_HOST_PATH="/usr" \
                -DQT_DIR="/opt/android-libs/aarch64/lib/cmake/Qt6" \
                -DQT_HOST_PATH_CMAKE_DIR="/usr/lib/cmake" \
                -DQt6_DIR="/opt/android-libs/aarch64/lib/cmake/Qt6" \
                -DQt6Core_DIR="/opt/android-libs/aarch64/lib/cmake/Qt6Core" \
                -DQt6Widgets_DIR="/opt/android-libs/aarch64/lib/cmake/Qt6Widgets" \
                -DQt6HostInfo_DIR="/usr/lib/cmake/Qt6HostInfo" \
                -DQt6WidgetsTools_DIR="/usr/lib/cmake/Qt6WidgetsTools" \
                -DQt6GuiTools_DIR="/usr/lib/cmake/Qt6GuiTools" \
                -DQt6CoreTools_DIR="/usr/lib/cmake/Qt6CoreTools" \
                -DQt6Gui_DIR="/opt/android-libs/aarch64/lib/cmake/Qt6Gui"
fi

if [ "$build" -eq 1 ]; then
    ninja -C build/android -j16 | grep -v "ninja: Entering directory"
fi

if [ "$install" -eq 1 ]; then
    ninja -C build/android install_apk | grep -v "ninja: Entering directory"
fi

if [ "$run" -eq 1 ]; then
    ninja -C build/android run_apk | grep -v "ninja: Entering directory"
fi
