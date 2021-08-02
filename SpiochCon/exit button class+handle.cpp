#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

const int SCREEN_W = 1920;
const int SCREEN_H = 1080;

class DBtexture
{
public:
    DBtexture();
    ~DBtexture();
    bool LFF( std::string path);
    void free();
    void setColor( Uint8 red, Uint8 green, Uint8 blue);
    void setBlendMode( SDL_BlendMode blending );
    void setAplha( Uint8 alpha );
    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
    int getWidth();
    int getHeight();
private:
    SDL_Texture* mtexture;
    int mWidth;
    int mHeight;
};

class bexit
{
public:
    bexit();
    void setpos( int x, int y);
    bool handleEvent( SDL_Event* e);
    void loadexitb();
    void render();

private:
    SDL_Point ePos;
    buttonsprite exitcurrentsprite;
    const int bexitW = 300;
    const int bexitH = 200; 
    SDL_Rect bexitclip[BUTTON_SPRITE_TOTAL];
};
 class bar
 {
public:
    bar();
    void setpos( int x, int y);
    bool handleEvent()
private:
 };

SDL_Window* SpiochWindow = NULL;
SDL_Renderer* SpiochRenderer = NULL;

DBtexture exitspritesheet;
bexit btexit;

DBtexture::DBtexture()
{
    mtexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

DBtexture::~DBtexture()
{
    free();
}

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

bexit::bexit()
{
    ePos.x = 0;
    ePos.y = 0;
    exitcurrentsprite = BUTTON_SPRITE_MOUSE_OUT;
}

void bexit::setpos( int x, int y )
{
    ePos.x = x;
    ePos.y = y;
}

bool bexit::handleEvent( SDL_Event* e )
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
        else if( x > ePos.x + bexitW )
        {
            inside = false;
        }
        else if( y < ePos.y )
        {
            inside = false;
        }
        else if( y > ePos.y + bexitH )
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
                    return false;
                    break;
            }
        }
    }
    return true;
}

void bexit::render()
{
    exitspritesheet.render( ePos.x, ePos.y, &bexitclip[exitcurrentsprite] );
}

void bexit::loadexitb()
{
    if(!exitspritesheet.LFF( "PNG/ebt.png" ) )
    {
        printf( "Failed to load exit button texture\n" );
    }
    else{
        for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
        {
            bexitclip[i].x = 0;
            bexitclip[i].y = i * 50;
            bexitclip[i].w = bexitW;
            bexitclip[i].h = bexitH;
        }
        btexit.setpos( 1800, 1000 );
    }
}

bool init()
{
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0)
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
        SpiochWindow = SDL_CreateWindow( "Spioch Gra Fabularna", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_FULLSCREEN );
        if( SpiochWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %S\n", SDL_GetError() );
            success = false;
        }
        else
        {
            SpiochRenderer = SDL_CreateRenderer( SpiochWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
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
            }
        }        
    }
    return success;
}

void close()
{
    exitspritesheet.free();
    SDL_DestroyRenderer( SpiochRenderer );
    SDL_DestroyWindow( SpiochWindow );
    SpiochWindow = NULL;
    SpiochRenderer = NULL;
    IMG_Quit();
    SDL_Quit();
}

int main( int argc, char* argd[] )
{
    if( !init() )
    {
        printf( "Failed to initialize\n" );
    }
    else
    {
        btexit.loadexitb();
        bool quit = false;
        SDL_Event e;
        while( !quit )
        {
            while( SDL_PollEvent( &e ) != 0 )
            {
                if( e.type == SDL_QUIT )
                {
                    quit = true;
                }
                quit = btexit.handleEvent( &e );
            }
            SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255);
            SDL_RenderClear( SpiochRenderer );
            btexit.render();
            SDL_RenderPresent( SpiochRenderer );
        }
    }
    close();
    return 0;
}