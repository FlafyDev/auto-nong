if [ "$1" = "win" ]; then
  cmake \
     -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
     -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_REPO/clang-cl-msvc.cmake \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -B build \
     -G Ninja

  cmake --build build
else
  cmake \
     -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
     -DCMAKE_TOOLCHAIN_FILE=/home/flafy/android-ndk/android-ndk-r26d/build/cmake/android.toolchain.cmake \
     -DCMAKE_BUILD_TYPE=RelWithDebInfo \
     -B build \
     -G Ninja \
     -DANDROID_ABI=arm64-v8a \
     -DANDROID_PLATFORM=android-23

  cmake --build build --config RelWithDebInfo
fi
