// Thanks to KingUndeadCodes for the idea, the pc speaker code, and the melody data!

#ifdef AUDIO_ENABLED
#include "utils/typedefs.h"
#include "drivers/pc_speaker.h"
#include "cpu/timer/timer.h"

void roll() {
    // https://create.arduino.cc/projecthub/410027/rickroll-piezo-buzzer-a1cd11?ref=part&ref_id=8233&offset=3
    int song1_intro_melody[] = {554, 622, 622, 698, 831, 740, 698, 622, 554, 622, -1, 415, 415};
    int song1_intro_rhythmn[] = {6, 10, 6, 6, 1, 1, 1, 1, 6, 10, 4, 2, 10};
    int song1_verse1_melody[] = {-1, 277, 277, 277, 277, 311, -1, 261, 233, 208, -1, 233, 233, 261, 277, 208, 415, 415, 311, -1, 233, 233, 261, 277, 233, 277, 311, -1, 261, 233, 233, 208, -1, 233, 233, 261, 277, 208, 208, 311, 311, 311, 349, 311, 277, 311, 349, 277, 311, 311, 311, 349, 311, 208, -1, 233, 261, 277, 208, -1, 311, 349, 311};
    int song1_verse1_rhythmn[] = {2, 1, 1, 1, 1, 2, 1, 1, 1, 5, 1, 1, 1, 1, 3, 1, 2, 1, 5, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 3, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 4, 5, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 3, 1, 1, 1, 3};

    const uint32_t time_scale = 5;
    for (int i = 0; i < 13; i += 1) {
        song1_intro_rhythmn[i] *= time_scale;
    }
    for (int i = 0; i / sizeof(int) < sizeof(song1_intro_melody) / sizeof(int); i += 1) {
        song1_verse1_rhythmn[i] *= time_scale;
    }

    for (int i = 0; i < 13; i += 1) song1_intro_melody[i] == -1 ? sleep(song1_intro_rhythmn[i]) : beep((uint32_t)song1_intro_melody[i], (uint32_t)song1_intro_rhythmn[i]);
    for (int i = 0; i / sizeof(int) < sizeof(song1_intro_melody) / sizeof(int); i += 1) song1_verse1_melody[i] == -1 ? sleep(song1_verse1_rhythmn[i]) : beep((uint32_t)song1_verse1_melody[i], (uint32_t)song1_verse1_rhythmn[i] * 3);
}

#endif