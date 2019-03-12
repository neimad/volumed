// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2019 Damien Flament
// This file is part of volumed.

#pragma once

#include <alsa/asoundlib.h>
#include <glib-object.h>

/**
 * ALSAPlaybackMixerElement:
 * An ALSA playback mixer element.
 */
/**
 * ALSAPlaybackMixerElement:volume:
 *
 * The actual audio volume value.
 */
/**
 * ALSAPlaybackMixerElement:min-volume:
 *
 * The minimum audio volume value.
 */
/**
 * ALSAPlaybackMixerElement:max-volume:
 *
 * The maximum audio volume value.
 */
/**
 * ALSAPlaybackMixerElement:muted:
 *
 * The muted state.
 */
G_DECLARE_FINAL_TYPE (ALSAPlaybackMixerElement,
                      alsa_playback_mixer_element,
                      ALSA,
                      PLAYBACK_MIXER_ELEMENT,
                      GObject);
#define ALSA_TYPE_PLAYBACK_MIXER_ELEMENT alsa_playback_mixer_element_get_type ()

/**
 * alsa_playback_mixer_element_new: (contructor)
 * @mixer_element: an ALSA mixer element
 *
 * Creates an ALSA playback mixer element.
 *
 * Returns: (transfer full): a new #ALSAPlaybackMixerElement
 */
ALSAPlaybackMixerElement *
alsa_playback_mixer_element_new (snd_mixer_elem_t *mixer_element);
