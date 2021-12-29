#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>


class Postac
{
public:
    std::string name;
    size_t namelng;
    bool alive;
    int scena;
    Postac();
    void newcharacter();
    void characterdead();
    void savegame();
};

//kostruktor postaci
Postac::Postac()
{
    SDL_RWops* postac = SDL_RWFromFile( "bin/postac.bin", "r+b");
    if( postac == NULL )
    {
        printf( "new character created\n");
        postac = SDL_RWFromFile( "bin/postac.bin", "w+b" );
        if(postac != NULL )
        {
            scena = 1;
            name = "Bin";
            namelng = name.size();
            SDL_RWwrite( postac, &name, sizeof(namelng), 1);
            SDL_RWwrite( postac, &scena, sizeof(int), 1);
            SDL_RWclose( postac );
        }
        else
        {
            printf( "Error: unable to create character file! SDL Error: %s\n", SDL_GetError );
        }
    }
    else
    {
        printf( "Reading character\n" );
        SDL_RWread( postac, &name, sizeof(namelng), 1 );
        SDL_RWread( postac, &scena, sizeof(int), 1 );
        SDL_RWclose( postac );
    }

}

void Postac::newcharacter()
{
    alive = true;
    scena = 1;
}

void Postac::characterdead()
{
    alive = false;
    scena = 0;
}

//zapisane danych sesji
void Postac::savegame()
{
    SDL_RWops* postac = SDL_RWFromFile("bin/postac.bin", "w+b");
    if( postac != NULL )
    {
        namelng = name.size();
        SDL_RWwrite( postac, &name, sizeof( namelng ), 1);
        SDL_RWwrite( postac, &scena, sizeof(int), 1);
        SDL_RWclose( postac );
    }
    else
    {
        printf( "Error: unable to save settings %s\n", SDL_GetError );
    }
}
