#include <SDL2/SDL.h>
//#include <SDL2/SDL_image.h>
//#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <array>

usung namespace std;

void nowy_plik();
void odczyt_plik();


int main( int argc, char* argd[] )
{
    cout << "Spioch rozdział 1. Program CLI do tworzenia pliku bin sceny" << endl;
    cout << "Dendrobates Studio, Adam Machowski\n" << endl;
    bool exit = false;
    int menu;
    while(!exit)
    {
        cout << "utwórz plik[1], odczytaj plik[2], Zakończ program[0]" << endl;
        cin >> menu;
        switch( menu )
        {
            case 1:
                nowy_plik();
            case 2:
                odczyt_plik();
            case 0:
                exit = true
                break;
        }
    }
    return 0;
}



void nowy_plik()
{
    int ln;
    size_t lnlng;
    string fname;
    cout << "Wprowadz nazwę pliku: " << endl;
    cin >> fname;
    SDL_RWops* nfile = SDL_RWFromFile( fname.c_str() , "w+b" );
    if( !nfile )
    {
        cout << "Plik " << fname << " utworzony" << endl;
        cout << "Podaj liczbę wierszy do zapisu: " << endl;
        cin >> ln;
        new array<string, ln> lnarr;
        SDK_RWwrite( nfile, ln, sizeof( int ), 1 );
        for(int a = 0; a <= ln; a++)
        {
            cout << "Wprowadz wiersz:" << endl;
            cin >> lnarr[a];
            lnlng = lnarr[a].size();
            SDL_RWwrite( nfile, &lnlng, sizeof( size_t ), 1 );
            SDL_RWwrite( nfile, &lnarr[a][0], lnarr[a].size(), 1 );
        }
        SDL_RWclose( nfile );
        delete lnarr;
    }
    else
    {
        cout << "Plik już istnieje, usuń go przed kontynuacją." << endl;
    }


}

void odczyt_plik()
{
    string fname;
    cout << "Podaj nazwę pliku do odczytania: " << endl;
    cin >> fname;
    SDL_RWops* nfile = SDL_RWFromFile( fname.c_str() , "r+b");
    if( !nfile )
    {
        cout << "Nie ma takiego pliku!" << endl;
    }    
    else
    {
        cout << "Odczyt " << fname << endl;
        int ln;
        size_t lnlng;
        SDL_RWread( nfile, &ln, sizeof( int ), 1 );
        new array<string, ln> lnarr;
        for( int a = 0; a <= ln; a++ )
        {
            SDL_RWread( nfile, &lnlng, sizeof( size_t ), 1 );
            lnarr[a].resize( lnlng );
            SDL_RWread( nfile, &lnarr[a][0], lnarr[a].size(), 1 );
        }
        SDL_RWclose( nfile );
        for( int a = 0; a <= 0; a++ )
        {
            cout << lnarr[a] << endl;
        }
        delete lnarr;
    }
}
