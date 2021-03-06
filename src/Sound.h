#ifndef SOUND_H
#define SOUND_H

#include "Options.h"
#include "allegro5/allegro_audio.h"
#include "tools.h"

class Sound {
 public:
  Sound();
  virtual ~Sound();
  void load_wav(std::string);
  void load_ogg(std::string);
  void play();
  void stop();
  void play_random_frequency(int, int);
  void play_at_volume(float newVolume);
  ALLEGRO_SAMPLE_ID* getSampleId() { return sample_id; }
  ALLEGRO_SAMPLE* getSample() { return sample; }

  bool getIsPlaying() { return is_playing; }

 private:
  ALLEGRO_SAMPLE* sample;
  ALLEGRO_SAMPLE_ID* sample_id;

  bool is_wav;
  bool is_playing;
};

#endif  // SOUND_H
