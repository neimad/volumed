// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright © 2019 Damien Flament
// This file is part of volumed.

#include "VolumeManager.h"

#include "ALSAPlaybackMixerElement.h"
#include "glib-helpers/glib-object-helpers.h"

#define CARD_NAME "default"
#define ELEMENT_NAME "Master"
#define STEPS_COUNT 10

struct _VolumeManager
{
  GObject parent;

  snd_mixer_t *             mixer;
  ALSAPlaybackMixerElement *mixer_element;
};

G_DEFINE_TYPE (VolumeManager, volume_manager, G_TYPE_OBJECT);

typedef enum _VolumeAction
{
  VOLUME_DECREASE = -1,
  VOLUME_INCREASE = 1,
} VolumeAction;

static void         handle_alsa_error (gint error_code, const gchar *message);
static snd_mixer_t *get_card_mixer (const gchar *name);
static snd_mixer_elem_t *get_mixer_element (snd_mixer_t *mixer,
                                            const gchar *name);
static void perform_volume_action (ALSAPlaybackMixerElement *mixer_element,
                                   VolumeAction              action);

static void
volume_manager_init (VolumeManager *self G_GNUC_UNUSED)
{
  self->mixer = get_card_mixer (CARD_NAME);

  self->mixer_element = alsa_playback_mixer_element_new (
    get_mixer_element (self->mixer, ELEMENT_NAME));
}

static void
volume_manager_dispose (VolumeManager *self)
{
  g_object_unref (self->mixer_element);

  G_OBJECT_CLASS (volume_manager_parent_class)->dispose (G_OBJECT (self));
}

static void
volume_manager_finalize (VolumeManager *self)
{
  self->mixer_element = NULL;

  G_OBJECT_CLASS (volume_manager_parent_class)->finalize (G_OBJECT (self));
}

static void
volume_manager_class_init (VolumeManagerClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose  = (GObjectDisposeFunc)volume_manager_dispose;
  object_class->finalize = (GObjectFinalizeFunc)volume_manager_finalize;
}

VolumeManager *
volume_manager_new (void)
{
  return g_object_new (VOLUME_TYPE_MANAGER, NULL);
}

void
volume_manager_increase (VolumeManager *self)
{
  perform_volume_action (self->mixer_element, VOLUME_INCREASE);
}

void
volume_manager_decrease (VolumeManager *self)
{
  perform_volume_action (self->mixer_element, VOLUME_DECREASE);
}

void
volume_manager_toggle (VolumeManager *self)
{
  gboolean muted = FALSE;

  g_object_get (self->mixer_element, "muted", &muted, NULL);
  g_object_set (self->mixer_element, "muted", !muted, NULL);
}

static void
handle_alsa_error (gint error_code, const gchar *message)
{
  if (error_code < 0)
    {
      g_error ("%s: %s", message, snd_strerror (error_code));
    }
}

static snd_mixer_t *
get_card_mixer (const gchar *name)
{
  snd_mixer_t *mixer = NULL;

  handle_alsa_error (snd_mixer_open (&mixer, 0),
                     "Failed to open an ALSA mixer");

  handle_alsa_error (snd_mixer_attach (mixer, name),
                     "Failed to attach the card to the mixer");

  return mixer;
}

static snd_mixer_elem_t *
get_mixer_element (snd_mixer_t *mixer, const gchar *name)
{
  snd_mixer_selem_id_t *sid;
  snd_mixer_elem_t *    element = NULL;

  handle_alsa_error (snd_mixer_selem_register (mixer, NULL, NULL),
                     "Failed to register a simple element class for the mixer");

  handle_alsa_error (snd_mixer_load (mixer),
                     "Failed to load the mixer elements");

  snd_mixer_selem_id_alloca (&sid);
  snd_mixer_selem_id_set_index (sid, 0);
  snd_mixer_selem_id_set_name (sid, name);
  element = snd_mixer_find_selem (mixer, sid);

  if (element == NULL)
    {
      g_error ("Failed to find the mixer element `%s`", name);
    }

  return element;
}

void
perform_volume_action (ALSAPlaybackMixerElement *mixer_element,
                       VolumeAction              action)
{
  glong minimum    = 0;
  glong maximum    = 0;
  glong step_value = 0;
  glong value      = 0;

  g_object_get (mixer_element,
                "min-volume",
                &minimum,
                "max-volume",
                &maximum,
                "volume",
                &value,
                NULL);

  step_value = (maximum - minimum) / STEPS_COUNT;
  value      = value + (step_value * action);
  value      = CLAMP (value, minimum, maximum);

  g_object_set (mixer_element, "volume", value, NULL);
}
