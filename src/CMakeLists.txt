include(TargetModules)
include(GNUInstallDirs)

configure_file(config.h.in config.h)

add_library(volumed-units STATIC ALSAPlaybackMixerElement.c
                                 VolumeManager.c)
target_required_modules(volumed-units PUBLIC gobject-2.0
                                      PRIVATE alsa)

add_executable(volumed main.c)
target_required_modules(volumed PRIVATE glib-2.0)
target_link_libraries(volumed volumed-dbus volumed-units)
target_include_directories(volumed PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

install(TARGETS volumed DESTINATION ${CMAKE_INSTALL_BINDIR})
