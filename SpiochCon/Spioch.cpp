#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>


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

//okno, obsługa okna
class Window
{
private:
    
    SDL_Window* SpiochWindow;
    int SCREEN_W = 0;
    int SCREEN_H = 0;

public:
    Window();
    bool init();
    SDL_Renderer* createRenderer();
    void setresolution( int scrnW, int scrnH );
    void setfullscrn( bool fs );
    void free();
    void savesettings();
    int getScrnW();
    int getScrnH();
    bool fscrn;
    bool resfhd;
};

//tekstura, manipulacja teksturą
class DBtexture
{
public:
    DBtexture();
    ~DBtexture();
    void setFont(int size);
    void fontend();
    bool LFF( std::string path);
    bool loadText( std::string textureText, SDL_Color textColor );
    void free();
    void setColor( Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode( SDL_BlendMode blending );
    void setAplha( Uint8 alpha );
    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
    void fintxt( std::string textureText, SDL_Color textColor, int x, int y, Uint8 alpha);
    int getWidth();
    int getHeight();
private:
    int mWidth;
    int mHeight;
    TTF_Font* DBfont;
    SDL_Texture* mtexture;

};

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

class Postac
{
public:
    char name[10] = "Bin      ";
    bool alive;
    int scena;
    Postac();
    void savegame();
};

//zmienne globalne
SDL_Renderer* SpiochRenderer = NULL;
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


//konstruktor klasy okna
Window::Window()
{
    SpiochWindow = NULL;
    SDL_RWops* settings = SDL_RWFromFile("bin/settings.bin", "r+b");
    if( settings == NULL )
    {
        printf(" new setting created\n");
        settings = SDL_RWFromFile( "bin/settings.bin", "w+b" );
        if(settings != NULL )
        {
            SCREEN_W = 1280;
            SCREEN_H = 720;
            resfhd = false;
            fscrn = true;
            SDL_RWwrite( settings, &SCREEN_W, sizeof(int), 1);
            SDL_RWwrite( settings, &SCREEN_H, sizeof(int), 1);
            SDL_RWwrite( settings, &resfhd, sizeof(bool), 1);
            SDL_RWwrite( settings, &fscrn, sizeof(bool), 1);
            SDL_RWclose( settings );
        }
        else
        {
            printf( "Error: unable to create setting file! SDL Error: %s\n", SDL_GetError );
        }
    }
    else
    {
        printf( "Reading settings\n" );
        SDL_RWread( settings, &SCREEN_W, sizeof(int), 1);
        SDL_RWread( settings, &SCREEN_H, sizeof(int), 1);
        SDL_RWread( settings, &resfhd, sizeof(bool), 1);
        SDL_RWread( settings, &fscrn, sizeof(bool), 1);
        SDL_RWclose( settings );
    }
    
}

//zbudowanie okna, ustawienie fullscreen 720p default
bool Window::init()
{
    SpiochWindow = SDL_CreateWindow( "Spioch Gra Fabularna", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );
    if( SpiochWindow != NULL )
    {
        if( !fscrn )
        {
            setfullscrn( false );
        }
        else
        {
            setfullscrn( true );
        }
    }
    return SpiochWindow != NULL;

}

//ustawienie właściwości renderer
SDL_Renderer* Window::createRenderer()
{
    return SDL_CreateRenderer( SpiochWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

//zmiana rozdzielczości okna
void Window::setresolution( int scrnW, int scrnH )
{
    if(!fscrn)
    {
        SDL_SetWindowSize( SpiochWindow, scrnW, scrnH );
        SCREEN_W = scrnW;
        SCREEN_H = scrnH;
    }
    else
    {
        SDL_SetWindowFullscreen(SpiochWindow, SDL_FALSE );
        SDL_SetWindowSize( SpiochWindow, scrnW, scrnH );
        SCREEN_W = scrnW;
        SCREEN_H = scrnH;
        SDL_SetWindowFullscreen( SpiochWindow, SDL_TRUE );
    }
    
    SDL_RenderPresent( SpiochRenderer );
}

//pełny ekran/okno
void Window::setfullscrn( bool fs )
{
    if( !fs )
    {
        SDL_SetWindowFullscreen( SpiochWindow, SDL_FALSE );
        fscrn = false;
    }
    else
    {
        SDL_SetWindowFullscreen( SpiochWindow, SDL_TRUE );
        fscrn = true;
    }
}

//zwolnienie zasobów okna
void Window::free()
{
    if( SpiochWindow != NULL )
    {
        SDL_DestroyWindow( SpiochWindow );
    }
    SCREEN_H = 0;
    SCREEN_W = 0;
}

void Window::savesettings()
{
    SDL_RWops* settings = SDL_RWFromFile("bin/settings.bin", "w+b");
    if( settings != NULL )
    {
        SDL_RWwrite( settings, &SCREEN_W, sizeof(int), 1);
        SDL_RWwrite( settings, &SCREEN_H, sizeof(int), 1);
        SDL_RWwrite( settings, &resfhd, sizeof(bool), 1);
        SDL_RWwrite( settings, &fscrn, sizeof(bool), 1);
        SDL_RWclose( settings );
    }
    else
    {
        printf( "Error: unable to save settings %s\n", SDL_GetError );
    }
}

int Window::getScrnW()
{
    return SCREEN_W;
}

int Window::getScrnH()
{
    return SCREEN_H;
}

//konstruktor tekstury
DBtexture::DBtexture()
{
    mtexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

//dekonstruktor tekstury
DBtexture::~DBtexture()
{
    free();
}

//Ładowanie tektury do pamięci
bool DBtexture::LFF( std::string path)
{
    free();
    SDL_Texture* newTexture = NULL;
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
        printf( "Unabel to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    else
    {
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xff, 0xff) );
        newTexture = SDL_CreateTextureFromSurface( SpiochRenderer, loadedSurface );
        if( newTexture == NULL )
            printf("Unabel to create texture from %s! SDL_Error: %s\n", path.c_str(), SDL_GetError() );
        else
        {
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        SDL_FreeSurface( loadedSurface );
    }
    mtexture = newTexture;
    return mtexture != NULL;
}

//ttf i rozmiar czcionki
void DBtexture::setFont(int size)
{
    DBfont = TTF_OpenFont( "Font/ComicSansMS3.ttf", size );
}

//zwolnienie zasobów ttf
void DBtexture::fontend()
{
    TTF_CloseFont( DBfont );
    DBfont = NULL;
}

//Ładowanie zmiennej string do tektury
bool DBtexture::loadText( std::string textureText, SDL_Color textColor )
{
	free();
    SDL_Surface* textSurface = TTF_RenderUTF8_Solid( DBfont, textureText.c_str() , textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
        mtexture = SDL_CreateTextureFromSurface( SpiochRenderer, textSurface );
		if( mtexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		SDL_FreeSurface( textSurface );
	}
	return mtexture != NULL;
}


//Zwolnienie zasobów tekstury
void DBtexture::free()
{
    if( mtexture != NULL )
    {
        SDL_DestroyTexture( mtexture );
        mtexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void DBtexture::setColor( Uint8 red, Uint8 green, Uint8 blue)
{
    SDL_SetTextureColorMod( mtexture, red, green, blue);
}

void DBtexture::setBlendMode( SDL_BlendMode blending )
{
    SDL_SetTextureBlendMode( mtexture, blending );
}

void DBtexture::setAplha( Uint8 alpha )
{
    SDL_SetTextureAlphaMod( mtexture, alpha );
}

//wyświetlenie tekstury do aktualnego renderer
void DBtexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
    SDL_Rect RendererQ = {x, y, mWidth, mHeight};
    if( clip != NULL )
    {
        RendererQ.w = clip->w;
        RendererQ.h = clip->h;
    }
    SDL_RenderCopyEx( SpiochRenderer, mtexture, clip, &RendererQ, angle, center, flip );
}

int DBtexture::getWidth()
{
    return mWidth;
}

int DBtexture::getHeight()
{
    return mHeight;
}

//Wyświetlenie tekstury ttf z obsługą alpha blending
void DBtexture::fintxt( std::string textureText, SDL_Color textColor, int x, int y, Uint8 alpha)
{
    loadText( textureText, textColor );
    setBlendMode( SDL_BLENDMODE_BLEND );
    setAplha( alpha );
    render(x, y);
}

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
    exitspritesheet.render( ePos.x, ePos.y, &bexitclip[exitcurrentsprite] );
}

//Załadowanie tekstury przycisku i podzielenie sprite
void btt::loadbtt( std::string path )
{
    if(!exitspritesheet.LFF( path ) )
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
            for(int i = 0; i < 10; i++ )
            {
                SDL_RWwrite( postac, &name[i], sizeof(char), 1);
            }
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
        for(int i = 0; i < 10; i++ )
        {
            SDL_RWread( postac, &name[i], sizeof(char), 1 );
        }
        SDL_RWread( postac, &scena, sizeof(int), 1 );
        SDL_RWclose( postac );
    }

}

//zapisane danych sesji
void Postac::savegame()
{
    SDL_RWops* postac = SDL_RWFromFile("bin/postac.bin", "w+b");
    if( postac != NULL )
    {
        for(int i = 0; i < 10; i++ )
        {
            SDL_RWwrite( postac, &name[i], sizeof(char), 1);
        }
        SDL_RWwrite( postac, &scena, sizeof(int), 1);
        SDL_RWclose( postac );
    }
    else
    {
        printf( "Error: unable to save settings %s\n", SDL_GetError );
    }
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
            SpiochRenderer = SpiochW.createRenderer();
            if(SpiochRenderer == NULL )
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

//zakończenie obsługi SDL, zwolnienie zasobów
void close()
{
    SpiochW.savesettings();
    player.savegame();
    SDL_DestroyRenderer( SpiochRenderer );
    SpiochW.free();
    SpiochRenderer = NULL;
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
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        intro.render( SpiochW.getScrnW()/2 - intro.getWidth()/2 , SpiochW.getScrnH()/2 - intro.getHeight()/2 );
        SDL_RenderPresent( SpiochRenderer );
        ++i;
    }

    intro.free();

}

void intro()
{
    btt btt1;
    DBtexture door;
    btt1.loadbtt( "PNG/Candle_btt.png" );
    door.LFF( "PNG/door.png" );
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