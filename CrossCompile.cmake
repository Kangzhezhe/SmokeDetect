set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(install_path /home/kangzhehao/Documents/Smoke/SmokeDetect/3dpart)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)

set(CMAKE_C_FLAGS --specs=nosys.specs)
set(CMAKE_CXX_FLAGS --specs=nosys.specs)

add_compile_options(-mcpu=cortex-m3 -mthumb)
add_compile_options(-Wall -fdata-sections -ffunction-sections -Wno-missing-braces)

SET(CMAKE_INSTALL_PREFIX ${install_path})
SET(CMAKE_INSTALL_INCLUDEDIR ${install_path})
SET(CMAKE_INSTALL_LIBDIR ${install_path})
SET(CMAKE_INSTALL_BINDIR ${install_path})