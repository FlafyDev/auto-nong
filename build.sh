export HOST_ARCH=x86

cmake \
  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \
  -DSKIP_BUILDING_CODEGEN=ON \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -B build
  
cmake --build build --config Release
