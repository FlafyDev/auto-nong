# this is supposed to default to x86 but it doesnt for some reason
export HOST_ARCH=x86

# you can also use `-G Ninja` here
cmake \
  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN \
  -DCMAKE_BUILD_TYPE=Release \
  -B build
  # -DPython3_INCLUDE_DIRS=$PYTHON3INCLUDES  \
  # -DPython3_EXECUTABLE=$PYTHON3EXEC \
  # -DPython3_LIBRARIES=/home/flafy/Downloads/python3-11.8-nuget/tools/libs/python311.lib
  # -DANDROID_PLATFORM=android-28 \
  # -A win32
  # -G Ninja
  # -DSKIP_BUILDING_CODEGEN=OFF
  
cmake --build build --config Release
