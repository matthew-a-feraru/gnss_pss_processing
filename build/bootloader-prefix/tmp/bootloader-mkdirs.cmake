# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/matthewf/skyworks/prep/esp-idf/components/bootloader/subproject"
  "/home/matthewf/esp/gnss_pss_processing/build/bootloader"
  "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix"
  "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix/tmp"
  "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix/src/bootloader-stamp"
  "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix/src"
  "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/matthewf/esp/gnss_pss_processing/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
