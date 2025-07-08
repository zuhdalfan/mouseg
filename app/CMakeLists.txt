#-------------------------------------------------------------------------------
# Zephyr Example Application
#
# Copyright (c) 2021 Nordic Semiconductor ASA
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)

list(APPEND ZEPHYR_EXTRA_MODULES
  ${CMAKE_CURRENT_SOURCE_DIR}/../../modules/paw3395
)

set(BOARD nrf52840dk/nrf52840)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app)

file(GLOB app_sources src/*.c)
target_sources(app PRIVATE ${app_sources})
