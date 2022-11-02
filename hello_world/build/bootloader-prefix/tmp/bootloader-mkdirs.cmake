# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/APP/ESP32/esp/esp-idf/components/bootloader/subproject"
  "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader"
  "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader-prefix"
  "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader-prefix/tmp"
  "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader-prefix/src/bootloader-stamp"
  "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader-prefix/src"
  "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/MyProject/ESP32-DEMO/hello_world/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
