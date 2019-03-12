// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2019 Damien Flament
// This file is part of volumed.

#include "ALSAPlaybackMixerElement.h"

#include "glib-helpers/glib-object-helpers.h"

#define DEFAULT_CHANNEL SND_MIXER_SCHN_FRONT_LEFT

struct _ALSAPlaybackMixerElement
{
  GObject parent;

  snd_mixer_elem_t *mixer_element;
  glong             minimum;
  glong             maximum;
};

G_DEFINE_TYPE (ALSAPlaybackMixerElement,
               alsa_playback_mixer_element,
               G_TYPE_OBJECT);

enum
{
  PROPERTY_MIXER_ELEMENT = 1,
  PROPERTY_VOLUME,
  PROPERTY_MIN_VOLUME,
  PROPERTY_MAX_VOLUME,
  PROPERTY_MUTED,
  N_PROPERTIES
};

static GParamSpec *alsa_playback_mixer_element_properties[N_PROPERTIES] = {
  NULL,
};

static void
alsa_playback_mixer_element_init (ALSAPlaybackMixerElement *self G_GNUC_UNUSED)
{
}

static void
alsa_playback_mixer_element_constructed (ALSAPlaybackMixerElement *self)
{
  GParamSpecLong *volume_property = NULL;

  volume_property = G_PARAM_SPEC_LONG (
    alsa_playback_mixer_element_properties[PROPERTY_VOLUME]);
  g_assert (G_IS_PARAM_SPEC_LONG (volume_property));

  // Initialize the volume allowed range and default value
  g_object_get (self,
                "min-volume",
                &volume_property->minimum,
                "max-volume",
                &volume_property->maximum,
                "volume",
                &volume_property->default_value,
                NULL);

  G_OBJECT_CLASS (alsa_playback_mixer_element_parent_class)
    ->constructed (G_OBJECT (self));
}

static void
alsa_playback_mixer_element_finalize (ALSAPlaybackMixerElement *self)
{
  g_clear_pointer (&self->mixer_element, snd_mixer_elem_free);

  G_OBJECT_CLASS (alsa_playback_mixer_element_parent_class)
    ->finalize (G_OBJECT (self));
}

static void
alsa_playback_mixer_element_get_property (ALSAPlaybackMixerElement *self,
                                          guint                     property_id,
                                          GValue *                  value,
                                          GParamSpec *              spec)
{
  switch (property_id)
    {
    case PROPERTY_VOLUME:
      g_assert (self->mixer_element != NULL);
      {
        glong raw_value;
        gint  error = 0;

        error = snd_mixer_selem_get_playback_volume (self->mixer_element,
                                                     DEFAULT_CHANNEL,
                                                     &raw_value);

        if (error < 0)
          {
            g_error ("Failed to get the mixer element volume.");
          }

        g_value_set_long (value, raw_value);
      }
      break;

    case PROPERTY_MIN_VOLUME:
      g_value_set_long (value, self->minimum);
      break;

    case PROPERTY_MAX_VOLUME:
      g_value_set_long (value, self->maximum);
      break;

    case PROPERTY_MUTED:
      g_assert (self->mixer_element != NULL);
      {
        gboolean raw_value;
        gint     error = 0;

        error = snd_mixer_selem_get_playback_switch (self->mixer_element,
                                                     DEFAULT_CHANNEL,
                                                     &raw_value);
        if (error < 0)
          {
            g_error ("Failed to get the mixer element muted state.");
          }

        g_value_set_boolean (value, raw_value);
      }
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, spec);
    }
}

static void
alsa_playback_mixer_element_set_property (ALSAPlaybackMixerElement *self,
                                          guint                     property_id,
                                          const GValue *            value,
                                          GParamSpec *              spec)
{
  switch (property_id)
    {
    case PROPERTY_MIXER_ELEMENT:
      g_assert (self->mixer_element == NULL);

      self->mixer_element = g_value_get_pointer (value);
      snd_mixer_selem_get_playback_volume_range (self->mixer_element,
                                                 &self->minimum,
                                                 &self->maximum);
      break;

    case PROPERTY_VOLUME:
      g_assert (self->mixer_element != NULL);
      {
        glong raw_value = g_value_get_long (value);
        gint  error     = 0;

        error = snd_mixer_selem_set_playback_volume_all (self->mixer_element,
                                                         raw_value);

        if (error < 0)
          {
            g_error ("Failed to set the mixer element volume.");
          }
      }
      break;

    case PROPERTY_MUTED:
      g_assert (self->mixer_element != NULL);
      {
        gboolean raw_value = g_value_get_boolean (value);
        gint     error     = 0;

        error = snd_mixer_selem_set_playback_switch_all (self->mixer_element,
                                                         raw_value);
        if (error < 0)
          {
            g_error ("Failed to set the mixer element muted state.");
          }
      }
      break;

    case PROPERTY_MIN_VOLUME:
    case PROPERTY_MAX_VOLUME:
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (self, property_id, spec);
    }
}

static void
alsa_playback_mixer_element_class_init (ALSAPlaybackMixerElementClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructed
    = (GObjectConstructedFunc)alsa_playback_mixer_element_constructed;
  object_class->finalize
    = (GObjectFinalizeFunc)alsa_playback_mixer_element_finalize;
  object_class->set_property
    = (GObjectSetPropertyFunc)alsa_playback_mixer_element_set_property;
  object_class->get_property
    = (GObjectGetPropertyFunc)alsa_playback_mixer_element_get_property;

  alsa_playback_mixer_element_properties[PROPERTY_MIXER_ELEMENT]
    = g_param_spec_pointer ("mixer-element",
                            "Mixer element",
                            "ALSA mixer element",
                            G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE);
  alsa_playback_mixer_element_properties[PROPERTY_VOLUME]
    = g_param_spec_long ("volume",
                         "Actual volume",
                         "Actual audio volume value",
                         G_MINLONG,
                         G_MAXLONG,
                         0,
                         G_PARAM_READWRITE);
  alsa_playback_mixer_element_properties[PROPERTY_MIN_VOLUME]
    = g_param_spec_long ("min-volume",
                         "Minimum volume",
                         "Minimum audio volume value",
                         G_MINLONG,
                         G_MAXLONG,
                         0,
                         G_PARAM_READABLE);
  alsa_playback_mixer_element_properties[PROPERTY_MAX_VOLUME]
    = g_param_spec_long ("max-volume",
                         "Maximum volume",
                         "Maximum audio volume value",
                         G_MINLONG,
                         G_MAXLONG,
                         0,
                         G_PARAM_READABLE);
  alsa_playback_mixer_element_properties[PROPERTY_MUTED]
    = g_param_spec_boolean ("muted",
                            "Muted state",
                            "Is the element muted",
                            FALSE,
                            G_PARAM_READWRITE);

  g_object_class_install_properties (object_class,
                                     N_PROPERTIES,
                                     alsa_playback_mixer_element_properties);
}

ALSAPlaybackMixerElement *
alsa_playback_mixer_element_new (snd_mixer_elem_t *mixer_element)
{
  return g_object_new (ALSA_TYPE_PLAYBACK_MIXER_ELEMENT,
                       "mixer-element",
                       mixer_element,
                       NULL);
}
