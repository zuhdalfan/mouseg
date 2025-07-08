# Install script for directory: /home/zuhdalfan/Project/zephyr-mouseg/zephyr

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/home/zuhdalfan/zephyr-sdk-0.17.2/arm-zephyr-eabi/bin/arm-zephyr-eabi-objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/arch/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/lib/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/soc/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/boards/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/subsys/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/drivers/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/modules/cmsis_6/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/modules/hal_nordic/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/modules/paw3395/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/kernel/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/cmake/flash/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/cmake/usage/cmake_install.cmake")
  include("/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/cmake/reports/cmake_install.cmake")

endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/home/zuhdalfan/Project/zephyr-mouseg/mouseg-app/app/build/zephyr/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
