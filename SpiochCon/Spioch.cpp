#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>
#include "lib/graphic.hpp"
#include "lib/sound.hpp"
#include "lib/postac.hpp"


enum buttonsprite
{
    BUTTON_SPRITE_MOUSE_OUT = 0,
    BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
    BUTTON_SPRITE_MOUSE_DOWN = 2,
    BUTTON_SPRITE_MOUSE_UP = 3,
    BUTTON_SPRITE_TOTAL = 4
};


const SDL_Color tcolor = {0, 0, 0};
const SDL_Color gcolor = {0, 255, 0};
const SDL_Color rcolor = {255, 0, 0};
const SDL_Color bcolor = {0, 0, 255};
const SDL_Color wcolor = {255, 255, 255};





//przycisk i obsługa przycisku
class btt
{
public:
    btt();
    void setpos( int x, int y);
    bool handleEvent( SDL_Event* e);
    void loadbtt( std::string path );
    void renderbtt(Uint8 alpha);
    void endbtt();

private:
    SDL_Point ePos;
    DBtexture exitspritesheet;
    buttonsprite exitcurrentsprite;
    SDL_Rect bexitclip[BUTTON_SPRITE_TOTAL];
};





//zmienne globalne
SDL_Renderer* SpiochR = NULL;
Window SpiochW;
Postac player;

//deklaracje funkcji
bool init();
void close();
void title();
void intro();
bool game();
bool scena1();
bool menu();
bool opcje();





//konstruktor przycisku
btt::btt()
{
    ePos.x = 0;
    ePos.y = 0;
    exitcurrentsprite = BUTTON_SPRITE_MOUSE_OUT;
}

void btt::setpos( int x, int y )
{
    ePos.x = x;
    ePos.y = y;
}

//obsługa przycisku, mausebuttonup zwraca TRUE
bool btt::handleEvent( SDL_Event* e )
{
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
    {
        int x,y;
        SDL_GetMouseState( &x, &y );
        bool inside = true;
        if( x < ePos.x )
        {
            inside = false;
        }
        else if( x > ePos.x + exitspritesheet.getWidth() )
        {
            inside = false;
        }
        else if( y < ePos.y )
        {
            inside = false;
        }
        else if( y > ePos.y + (exitspritesheet.getHeight()/4) )
        {
            inside = false;
        }

        if( !inside )
        {
            exitcurrentsprite = BUTTON_SPRITE_MOUSE_OUT;
        }
        else
        {
            switch( e->type ){
                case SDL_MOUSEMOTION:
                    exitcurrentsprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    exitcurrentsprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                case SDL_MOUSEBUTTONUP:
                    exitcurrentsprite = BUTTON_SPRITE_MOUSE_UP;
                    return true;
                    break;
            }
        }
    }
    return false;
}

//wyświetlenie przycisku z obsługą alpha blending
void btt::renderbtt( Uint8 alpha )
{
    exitspritesheet.setBlendMode( SDL_BLENDMODE_BLEND );
    exitspritesheet.setAplha( alpha );
    exitspritesheet.render( ePos.x, ePos.y, &bexitclip[exitcurrentsprite], SpiochR );
}

//Załadowanie tekstury przycisku i podzielenie sprite
void btt::loadbtt( std::string path )
{
    if(!exitspritesheet.LFF( path, SpiochR ) )
    {
        printf( "Failed to load exit button texture\n" );
    }
    else{
        for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
        {
            bexitclip[i].x = 0;
            bexitclip[i].y = i * (exitspritesheet.getHeight()/4);
            bexitclip[i].w = exitspritesheet.getWidth();
            bexitclip[i].h = exitspritesheet.getHeight()/4;
        }
    }
}

//zwolnienie zasobów przycisku
void btt::endbtt()
{
    exitspritesheet.free();
}


//inicjalizacja SDL
bool init()
{
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO |  SDL_INIT_AUDIO  ) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else{
        if (!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Linear texture filtering not enabled" );
            success = false;
        }
        
        if( !SpiochW.init() )
        {
            printf( "Window could not be created! SDL Error: %S\n", SDL_GetError() );
            success = false;
        }
        else
        {
            SpiochR = SpiochW.createRenderer();
            if(SpiochR == NULL )
            {
                printf( "Renderer could not be created! SDL Error %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255);
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not be initialize! Error: %s\n", IMG_GetError() );
                    success = false;
                }
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                } 
            }
        }        
    }
    return success;
}


//zakończenie obsługi SDL, zwolnienie zasobów
void close()
{
    SpiochW.savesettings();
    player.savegame();
    SDL_DestroyRenderer( SpiochR );
    SpiochW.free();
    SpiochR = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}



void title()
{
    DBtexture intro;
    intro.LFF( "PNG/intro.png" );
    int i = 0;
    while ( i <= 150 )
    {
        SDL_SetRenderDrawColor( SpiochR, 214, 193, 143, 255 );
        SDL_RenderClear( SpiochR );
        intro.render( SpiochW.getScrnW()/2 - intro.getWidth()/2 , SpiochW.getScrnH()/2 - intro.getHeight()/2, SpiochR );
        SDL_RenderPresent( SpiochR );
        ++i;
    }

    intro.free();

}

void intro()
{
    btt btt1;
    DBtexture door;
    btt1.loadbtt( "PNG/Candle_btt.png" );
    door.LFF( "PNG/door.png", SpiochR );
    DBtexture text;
    text.setFont( 20 );
    SDL_Rect doorclip[2];
    for(int d = 0; d < 2; ++d)
    {
            doorclip[d].x = d * ( door.getWidth()/2 );
            doorclip[d].y = 0;
            doorclip[d].w = SpiochW.getScrnW()/2;
            doorclip[d].h = SpiochW.getScrnH();
    }
    bool quit = false;
    SDL_Event e;
    int fintxt = 0;
    int fintxt1 = 0;
    int fintxt2 = 0;
    int fintxt3 = 0;
    int txtin = 0;
    int doorcounter = 0;
    int a = 255;
    text.loadText("W latach trzydziestych XXI wieku,  odbyło się globalne referendum. W nim obywatele ziemi", tcolor );
    int txtln = text.getWidth();
    while(txtin < 840 )
    {
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        text.fintxt( "W latach trzydziestych XXI wieku,  odbyło się globalne referendum. W nim obywatele ziemi", tcolor,SpiochW.getScrnW()/2 - txtln/2, 100, fintxt );
        text.fintxt( "zadecydowali, że oddają władzę z rąk skostniałych i nieefektywnych struktur państwowych.", tcolor,SpiochW.getScrnW()/2 - txtln/2, 130, fintxt );
        text.fintxt( "Po plebiscycie władzę objęli faktyczni posiadacze większości majątku i dóbr planety.    ", tcolor,SpiochW.getScrnW()/2 - txtln/2, 160, fintxt );
        if( fintxt < 255 )
        {
            fintxt += 17;
        }
        
        if( txtin > 240 )
        {
            text.fintxt( "Korporacje.", rcolor, SpiochW.getScrnW()/2 - txtln/2, 190, fintxt1 );
            if( fintxt1 < 255 )
            {
                fintxt1 += 17;
            }
        }
        if( txtin > 480 )
        {
            text.fintxt( "W Europie największe wpływy miała Candle Corporation. Stawiająca na rozwój nie bacząc na", tcolor, SpiochW.getScrnW()/2 - txtln/2, 250, fintxt2 );
            text.fintxt( "jego koszty. Mieszkańcy czuli się swobodnie i żyli dostatnio. Wiedzieli jednak, że najgorsze co", tcolor, SpiochW.getScrnW()/2 - txtln/2, 280, fintxt2 );
            text.fintxt( "mogliby uczynić, to wejść w drogę planom Candle.", tcolor, SpiochW.getScrnW()/2 - txtln/2, 310, fintxt2 );
            if( fintxt2 < 255 )
            {
                fintxt2 += 17;
            }
        }
        if( txtin > 600 )
        {
            text.fintxt( "Tak już od kilkunastu lat wszędzie gdzie dzieje się coś ważnego, widnieje znak złotej świecy...", tcolor, SpiochW.getScrnW()/2 - txtln/2, 350, fintxt3 );
            if( fintxt3 < 255 )
            {
                fintxt3 += 17;
            }
        }
        txtin++;
        SDL_RenderPresent( SpiochRenderer );

    }
    while( doorcounter < 960 )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if(quit == false)
            {
                quit = btt1.handleEvent( &e );
            }
        }
        if(quit == true)
        {
            if( a > 0 )
            {
                a -= 5;
            }
            if( a == 0)
            {
                doorcounter += 5;
            }
            
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 0, 0, 0, 255 );
        SDL_RenderClear( SpiochRenderer );
        door.render( 0 - doorcounter, 0, &doorclip[0] );
        door.render( (SpiochW.getScrnW()/2) + doorcounter, 0, &doorclip[1] );
        btt1.setpos(SpiochW.getScrnW()/2 - 51, SpiochW.getScrnH()/2 - 51);
        btt1.renderbtt(a);
        SDL_RenderPresent( SpiochRenderer );
    }
    txtin = 0;
    fintxt = 0;
    while( txtin < 160 )
    {
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        text.fintxt( "Rok 2043, okolice dawnej Mangalii", tcolor, SpiochW.getScrnW()/2 - txtln/2, SpiochW.getScrnH()/2 - text.getHeight()/2 , fintxt );
        if( fintxt < 255 )
        {
            fintxt += 17;
        }
        SDL_RenderPresent( SpiochRenderer );
        txtin++;
    }
    txtin = 0;
    fintxt = 0;
    while( txtin < 220 )
    {
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        text.fintxt( "W nieczynnym kominie geotermalnym korporacja Candle prowadzi ściśle tajny projekt. ", tcolor, SpiochW.getScrnW()/2 - txtln/2, SpiochW.getScrnH()/2 - text.getHeight()/2 , fintxt );
        text.fintxt( "Tak się składa drogi graczu, że właśnie zostałeś wybrany by wziąć w nim udział. ", tcolor, SpiochW.getScrnW()/2 - txtln/2, SpiochW.getScrnH()/2 - text.getHeight()/2 + 30 , fintxt );
        if( fintxt < 255 )
        {
            fintxt += 17;
        }
        SDL_RenderPresent( SpiochRenderer );
        txtin++;
    }
    door.free();
    btt1.endbtt();
    text.fontend();
    text.free();
}

bool game()
{
    bool quit = false;
    intro();
    while( !quit )
    {
        switch( player.scena )
        {
            case 1:
            {
                quit = scena1();
            }
        }
    }
    if( quit )
    {
        player.savegame();
    }
}

bool scena1()
{
    bool quit = false;
    btt bttq;
    DBtexture text;
    SDL_Event e;
    text.setFont( 20 );
    bttq.loadbtt( "PNG/btt.png" );
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if(!quit)
            {
                quit = bttq.handleEvent( &e );
                
            }
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        text.fintxt( "tekst", tcolor, 50, 50, 255 );
        text.fintxt( player.name, tcolor, 50, 80, 255 );
        bttq.setpos( SpiochW.getScrnW()/2 - 50, SpiochW.getScrnH() - 100 );
        bttq.renderbtt( 255 );
        SDL_RenderPresent( SpiochRenderer );
    }

    bttq.endbtt();
    text.fontend();
    text.free();
    return true;
}

bool menu()
{
    bool quit = false;
    bool opc = false;
    bool ng = false;
    btt btt1, btt2, btt3;
    DBtexture text;
    btt1.loadbtt("PNG/btt.png");
    btt2.loadbtt("PNG/NG.png");
    btt3.loadbtt("PNG/OP.png");
    text.setFont( 20 );
    SDL_Event e;
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if(!quit)
            {
                quit = btt1.handleEvent( &e );
                
            }
            if(!opc)
            {
                opc = btt3.handleEvent( &e );
            }
            if( !ng )
            {
                ng = btt2.handleEvent( &e );
            }
        }
        if( opc )
        {
            opc = opcje();
        }
        if( ng )
        {
            ng = game();
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        btt2.setpos(50, SpiochW.getScrnH()/4);
        btt2.renderbtt(255);
        btt3.setpos( 50, SpiochW.getScrnH()/4 + 80);
        btt3.renderbtt( 255 );
        btt1.setpos( SpiochW.getScrnW()/2 - 50, SpiochW.getScrnH() - 100);
        btt1.renderbtt(255);
        text.fintxt( "Zakończ", tcolor, SpiochW.getScrnW()/2 - 40, SpiochW.getScrnH() - 90, 255 );
        SDL_RenderPresent( SpiochRenderer );
    }
    btt1.endbtt();
    btt2.endbtt();
    text.free();
    return true;
}

bool opcje()
{
    bool quit = false;
    bool fscr = false;
    bool resset = false;
    btt btt1, btt2, btt3, btt4, btt5;
    DBtexture text;
    btt1.loadbtt("PNG/btt.png");
    btt2.loadbtt("PNG/btt.png");
    btt4.loadbtt("PNG/btt.png");
    text.setFont( 20 );
    SDL_Event e;
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if(!quit)
            {
                quit = btt1.handleEvent( &e );
            }
            if(!fscr)
            {
                fscr = btt2.handleEvent( &e );
            }

            if(!resset)
            {
                resset = btt4.handleEvent( &e );
            }
        }
        if( fscr )
            {
                if(!SpiochW.fscrn)
                {
                    SpiochW.setfullscrn( true );
                }
                else
                {
                    SpiochW.setfullscrn( false );
                }
                    fscr = false;
                }
        if(resset)
            {
                if(!SpiochW.resfhd)
                {
                    SpiochW.setresolution( 1920, 1080 );
                    SpiochW.resfhd = true;
                }
                else
                {
                    SpiochW.setresolution( 1280, 720 );
                    SpiochW.resfhd = false;
                }
                    resset = false;
                }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        text.fintxt( "Opcje", bcolor, SpiochW.getScrnW()/2, 10, 255 );
        text.fintxt( "Tryb wyświetlania:", tcolor, 50, 100, 255 );
        btt2.setpos(50, 150);
        btt2.renderbtt(255);
        btt4.setpos( 50, 250 );
        btt4.renderbtt( 255 );
        if(SpiochW.fscrn)
        {
            text.fintxt("okno", tcolor, 55, 160, 255);
        }
        else
        {
            text.fintxt("Pełny Ekran", tcolor, 55, 160, 255);
        }
        
        if( !SpiochW.resfhd )
        {
            text.fintxt("1920x1080", tcolor, 55, 260, 255);
        }
        else
        {
            text.fintxt("1280x720", tcolor, 55, 260, 255);
        }
        btt1.setpos( SpiochW.getScrnW()/2 - 50, SpiochW.getScrnH() - 100);
        btt1.renderbtt(255);
        text.fintxt( "Wróć", tcolor, SpiochW.getScrnW()/2 - 25, SpiochW.getScrnH() - 90, 255 );
        SDL_RenderPresent( SpiochRenderer );
    }
    btt1.endbtt();
    btt2.endbtt();
    btt4.endbtt();
    text.free();
    return false;
}



int main( int argc, char* argd[] )
{
    bool end = false;
    if( !init() )
    {
        printf( "Failed to initialize " );
    }
    else
    {
        sound tlo;
        tlo.loadmusic( "Sound/music.mp3" );
        title();
        while (!end)
        {
            
            tlo.playsnd();
            end = menu();
        }
        tlo.stopsnd();
        tlo.endsnd();
    } 
    close();
    return 0;
}