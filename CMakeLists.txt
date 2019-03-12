cmake_minimum_required(VERSION 3.12)

project(volumed
        DESCRIPTION "An audio volume daemon"
        HOMEPAGE_URL "https://github.com/neimad/volumed"
        VERSION 0.1
        LANGUAGES C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
include(TargetModules)

require_module(alsa MINIMUM 1.1.8)
require_module(gio-unix-2.0 MINIMUM 2.58)
require_module(glib-2.0 MINIMUM 2.58)
require_module(gobject-2.0 MINIMUM 2.58)

add_subdirectory(dbus)
add_subdirectory(src)
add_subdirectory(systemd)
