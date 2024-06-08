cmake \
   -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
   -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_REPO/clang-cl-msvc.cmake \
   -DCMAKE_BUILD_TYPE=RelWithDebInfo -B build \
   -DGEODE_DISABLE_FMT_CONSTEVAL=1
  
cmake --build build --config RelWithDebInfo
