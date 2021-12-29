///#include <SDL2/SDL.h>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


bool odczyt_plik();


int main( int argc, char* argd[] )
{
    cout << "Spioch rozdział 1. Program CLI do kontroli pliku s sceny" << endl;
    cout << "Dendrobates Studio, Adam Machowski\n" << endl;
    bool exit = false;
    int menu;
    while(!exit)
    {
        cout << "odczyt pliku[1], Zakończ program[0]" << endl;
        cin >> menu;
        switch( menu )
        {
            case 1:
                if( !odczyt_plik() );
                {
                    cout << "Błąd odczytu pliku!!"
                }
                break;
            case 0:
                exit = true;
                break;
        }
    }
    return 0;
}


void odczyt_plik()
{
    string fname;
    int lnr;
    cout << "Podaj nazwę pliku do odczytania: ";
    cin >> fname;
    ifstream plik;
    plik.open( fname.cstr() )
    if( !plik.good() )
    {
        return false;
    }    
    else
    {
        int ln = 0;
        int a = 0;
        cout << "Podaj liczbę wierszy do odcztu: ";
        cin >> lnr;
        cout << "Odczyt " << fname << endl;
        vector<string> lnarr;
        lnarr.push_back( lnr );
        while( ln < lnr )
        {
            getline( plik, lnarr[ln] );
            ln++;
        }
        plik.close();
        for( int a = 0; a < lnr; a++ )
        {
            cout << lnarr[a] << endl;
        }
        return true;        
    }
}
