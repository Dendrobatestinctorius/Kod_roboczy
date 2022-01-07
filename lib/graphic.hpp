#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
//#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <string>

using namespace std;

//tekstura, manipulacja teksturą
class DBtexture
{
public:
    DBtexture();
    ~DBtexture();
    void setFont(int size);
    void fontend();
    bool LFF( std::string path, SDL_Renderer &SpiochRenderer);
    bool loadText( std::string textureText, SDL_Color textColor, SDL_Renderer &SpiochRenderer );
    void free();
    void setColor( Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode( SDL_BlendMode blending );
    void setAplha( Uint8 alpha );
    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE, SDL_Renderer &SpiochRenderer );
    void fintxt( std::string textureText, SDL_Color textColor, int x, int y, Uint8 alpha);
    int getWidth();
    int getHeight();
private:
    int mWidth;
    int mHeight;
    TTF_Font* DBfont;
    SDL_Texture* mtexture;

};

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
bool DBtexture::LFF( std::string path, SDL_Renderer &SpiochRenderer)
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
bool DBtexture::loadText( std::string textureText, SDL_Color textColor , SDL_Renderer &SpiochRenderer)
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
void DBtexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip, SDL_Renderer &SpiochRenderer)
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