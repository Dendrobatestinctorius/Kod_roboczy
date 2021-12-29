#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>

//Dźwiek, wczytywanie i obsługa
class sound
{
private:
    Mix_Music* mmusic;
public:
    sound();
    void loadmusic( std::string path );
    void playsnd();
    void stopsnd();
    void endsnd();

};

//konstruktor dżwieku
sound::sound()
{
    mmusic = NULL;
}

//załadowanie dzwieku do pamięci
void sound::loadmusic( std::string path )
{
    mmusic = Mix_LoadMUS( path.c_str() );
    if( mmusic == NULL )
    {
        printf( "Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError);
    }
}

void sound::playsnd()
{
    if( Mix_PlayingMusic() == 0 )
    {
        Mix_PlayMusic( mmusic, -1 );
    }
}

void sound::stopsnd()
{
    Mix_HaltMusic();
}

//zwolnienie zasobów dzwieku
void sound::endsnd()
{
    Mix_FreeMusic( mmusic );
    mmusic = NULL;
}
