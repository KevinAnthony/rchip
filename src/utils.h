#ifndef UTILS_H
#define UTILS_H

struct playing_info_music{
        char *Artist;
        char *Album;
        char *Song;
        int Elapised_time;
        int Duration;
        int isPlaying;
};

void init_playing_info_music(struct playing_info_music*);
void free_playing_info_music(struct playing_info_music*);
char* replace_str(char*, char*, char*);

#endif
