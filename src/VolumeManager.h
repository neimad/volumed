// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2019 Damien Flament
// This file is part of volumed.

#pragma once

#include <glib-object.h>

/**
 * VolumeManager:
 * A manager to set the audio volume.
 */
G_DECLARE_FINAL_TYPE (VolumeManager, volume_manager, VOLUME, MANAGER, GObject);
#define VOLUME_TYPE_MANAGER volume_manager_get_type ()

/**
 * volume_manager_new: (constructor)
 *
 * Creates a volume manager.
 *
 * Returns: (transfer full): a new #VolumeManager
 */
VolumeManager *volume_manager_new (void);

/**
 * volume_manager_increase: (method)
 * @manager: the manager
 *
 * Increases the audio volume by one step.
 */
void volume_manager_increase (VolumeManager *manager);

/**
 * volume_manager_decrease: (method)
 * @manager: the manager
 *
 * Decreases the audio volume by one step.
 */
void volume_manager_decrease (VolumeManager *manager);

/**
 * volume_manager_toggle: (method)
 * @manager: the manager
 *
 * Toggle the audio volume on/off.
 *
 * After toggling of the audio volume, the actual value must be recovered when
 * the volume is toggled back on.
 */
void volume_manager_toggle (VolumeManager *manager);
