#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
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
    std::string name;
    size_t namelng;
    bool alive;
    int scena;
    Postac();
    void newcharacter();
    void characterdead();
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
            fscrn = false;
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
            name = "Bin";
            namelng = name.size();
            SDL_RWwrite( postac, &namelng, sizeof(int), 1);
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
        SDL_RWread( postac, &namelng, sizeof(int), 1);
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
/*
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
*/

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


int main( int argc, char* argd[] )
{
    bool end = false;
    if( !init() )
    {
        printf( "Failed to initialize " );
    }
    else
    {
        SDL_Event e;
        DBtexture text;
        text.setFont(40);
        while (!end)
        {
            while( SDL_PollEvent( &e ) != 0 )
            {
                if( e.type == SDL_QUIT )
                {
                    end = true;
                }
            }

            SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
            SDL_RenderClear( SpiochRenderer );
            text.fintxt( "wersja spiocha do testowania modyfikacji", tcolor, SpiochW.getScrnW()/2 - SpiochW.getScrnW()/4, SpiochW.getScrnH()/2, 255 );
            text.fintxt( player.name, tcolor, 10, 10, 255 );
            SDL_RenderPresent( SpiochRenderer );
        }
    } 
    close();
    return 0;
}