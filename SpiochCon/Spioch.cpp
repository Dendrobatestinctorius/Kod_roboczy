#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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
const SDL_Color tcolor = {0, 0, 0};
const SDL_Color gcolor = {0, 255, 0};

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
    int getWidth();
    int getHeight();
private:
    TTF_Font* DBfont;
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
    void loadexitb( std::string path );
    void render();
    void endext();

private:
    SDL_Point ePos;
    DBtexture exitspritesheet;
    buttonsprite exitcurrentsprite;
    const int bexitW = 100;
    const int bexitH = 50; 
    SDL_Rect bexitclip[BUTTON_SPRITE_TOTAL];
};

SDL_Window* SpiochWindow = NULL;
SDL_Renderer* SpiochRenderer = NULL;



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

void DBtexture::setFont(int size)
{
    DBfont = TTF_OpenFont( "Font/ComicSansMS3.ttf", size );
}

void DBtexture::fontend()
{
    TTF_CloseFont( DBfont );
    DBfont = NULL;
}

bool DBtexture::loadText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderUTF8_Solid( DBfont, textureText.c_str() , textColor );
	if( textSurface == NULL )
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        mtexture = SDL_CreateTextureFromSurface( SpiochRenderer, textSurface );
		if( mtexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	
	//Return success
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
    loadexitb( "PNG/btt.png" );
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
                    return true;
                    break;
            }
        }
    }
    return false;
}

void bexit::render()
{
    exitspritesheet.render( ePos.x, ePos.y, &bexitclip[exitcurrentsprite] );
}

void bexit::loadexitb( std::string path )
{
    if(!exitspritesheet.LFF( path ) )
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
    }
}

void bexit::endext()
{
    exitspritesheet.free();
}

class choicebar
{
public:
    choicebar();
    void setpos( int x, int y);
    bool handleEvent( SDL_Event* e);
    void loadchcbr( std::string path );
    void render();
    void chcend();

private:
    SDL_Point ePos;
    DBtexture chspritesheet;
    buttonsprite chcurrsheet;
    const int chcW = 800;
    const int chcH = 100; 
    SDL_Rect chbar[BUTTON_SPRITE_TOTAL];
};

choicebar::choicebar()
{
    ePos.x = 0;
    ePos.y = 0;
    chcurrsheet = BUTTON_SPRITE_MOUSE_OUT;
    loadchcbr( "PNG/chcebar.png");
}

void choicebar::setpos(int x, int y)
{
    ePos.x = x;
    ePos.y = y;
}

bool choicebar::handleEvent( SDL_Event* e )
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
        else if( x > ePos.x + chcW )
        {
            inside = false;
        }
        else if( y < ePos.y )
        {
            inside = false;
        }
        else if( y > ePos.y + chcH )
        {
            inside = false;
        }

        if( !inside )
        {
            chcurrsheet = BUTTON_SPRITE_MOUSE_OUT;
        }
        else
        {
            switch( e->type ){
                case SDL_MOUSEMOTION:
                    chcurrsheet = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    chcurrsheet = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                case SDL_MOUSEBUTTONUP:
                    chcurrsheet = BUTTON_SPRITE_MOUSE_UP;
                    return true;
                    break;
            }
        }
    }
    return false;
}

void choicebar::render()
{
    chspritesheet.render( ePos.x, ePos.y, &chbar[chcurrsheet] );
}

void choicebar::loadchcbr( std::string path )
{
    if(!chspritesheet.LFF( path ) )
    {
        printf( "Failed to load exit button texture\n" );
    }
    else{
        for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
        {
            chbar[i].x = 0;
            chbar[i].y = i * 100;
            chbar[i].w = chcW;
            chbar[i].h = chcH;
        }
    }
}

void choicebar::chcend()
{
    chspritesheet.free();
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
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
            }
        }        
    }
    return success;
}

void close()
{
    SDL_DestroyRenderer( SpiochRenderer );
    SDL_DestroyWindow( SpiochWindow );
    SpiochWindow = NULL;
    SpiochRenderer = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void intro()
{
    DBtexture intro;
    intro.LFF( "PNG/intro.png" );
    int f = 0;
    int i = 0;
    int w = 0;
    bool wprowadzenie = false;
    bexit btt;
    btt.setpos(910, 900);
    DBtexture text;
    text.setFont( 20 );
    SDL_Event e;
    while ( f < 150 )
    {
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        intro.render( 0, 0 );
        SDL_RenderPresent( SpiochRenderer );
        ++f;
    }
    while( wprowadzenie == false )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( i >= 150 )
            {
                wprowadzenie = btt.handleEvent( &e );
            }
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        text.loadText( "Wielki precel z pary wodnej wyglądał niezwykle majestatycznie gdy w świetle księżyca", tcolor);
        text.render(100, 20);
        text.loadText( "obserwowało się jak otacza całą Oktawie.  Zarazem bronił przed światem zewnętrznym jak i nie", tcolor );
        text.render(100, 45);
        text.loadText( "pozwalał się wydostać. Wiedziałem już, że i tak nie było po co się wydostawać. Chyba, że ktoś", tcolor );
        text.render(100, 70);
        text.loadText( "lubił patrzeć na niekończącą się powierzchnię stwardniałego waniliowego budyniu ze szczyptą", tcolor );
        text.render(100, 95);
        text.loadText( "cynamonu. Słońce właśnie wyłaniało się z zza wiecznej chmury oświetlając kolejno każdy z", tcolor );
        text.render(100, 120);
        text.loadText( "ośmiu pierścieni", tcolor);
        text.render(100, 145);
        if( i >= 90 )
        {
            text.loadText( "Mój świat jest daleko. Czy może raczej dawno? Wyruszam dzisiaj po to by nie przepadły", tcolor );
            text.render(100, 200);
            text.loadText( "resztki tego co mnie z nim wiążą. Tak naprawdę nie wierzyłem w to, że cokolwiek zostało z tego", tcolor );
            text.render(100, 225);
            text.loadText( "co ukryłem w skryte wbudowanej w podstawę mojej trumny. Jednakże musiałem spróbować", tcolor );
            text.render(100, 250);
            text.loadText( "Dla siebie, by nie oszaleć od obcości Oktawii.", tcolor );
            text.render(100, 275);
        }
        if( i >= 180)
        {
            text.loadText( "Te ponure rozmyślania przerwał mi Gyce.", tcolor );
            text.render(100, 350);
        }
        if( i >= 220 )
        {
            btt.render();
            text.loadText("Dalej", tcolor);
            text.render( 940, 910 );
        }
        SDL_RenderPresent( SpiochRenderer );
        ++i;
    }
    intro.free();
    text.free();
    text.fontend();
    btt.endext();
}

void scena1()
{
    bexit btexit;
    choicebar bar1, bar2, bar3;
    bool bar1p = false;
    bool dajs = false;
    DBtexture text;
    text.setFont( 20 );
    SDL_Color rcolor = {255, 0, 0};
    bool quit = false;
    SDL_Event e;
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            quit = btexit.handleEvent( &e );
            if( bar1p == false )
            {
                bar1p = bar1.handleEvent( &e );
            }
            bar2.handleEvent( &e );
            bar3.handleEvent( &e );
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255);
        SDL_RenderClear( SpiochRenderer );
        btexit.setpos( 1800, 1000 );    
        btexit.render();
        text.loadText("Zakończ", tcolor );
        text.render( 1810, 1010);
        if( bar1p == false )
        {
            bar1.setpos( 50, 660 );
            bar1.render();
            text.loadText("Daj mi spokój", tcolor );
            text.render(70, 680 );

        }
        bar2.setpos( 50, 780 );
        bar2.render();
        bar3.setpos( 50, 900 );
        bar3.render();
        text.loadText( "Gyce", gcolor );
        text.render( 50, 50);
        text.loadText(" – Jeżeli dobrze zrozumiałem zostawiłeś coś w kapliczce i teraz chcesz to odzyskać?", tcolor);
        text.render( 95, 50);
        if( bar1p == true )
        {
            text.loadText("Bin", rcolor );
            text.render( 50, 75 );
            text.loadText(" - Daj mi spokój", tcolor);
            text.render( 95, 75 );
            text.loadText("Gyce zapatrzył się na chmurę, nie reagując na moją niegrzeczność. Nie odszedł jednak", tcolor );
            text.render(50, 100 );
        }
        SDL_RenderPresent( SpiochRenderer );
    }
    bar1.chcend();
    bar2.chcend();
    bar3.setpos( 50, 900 );
    bar3.chcend();
    btexit.endext();
    text.fontend();
}

int main( int argc, char* argd[] )
{
    if( !init() )
    {
        printf( "Failed to initialize " );
    }
    else
    {
        intro();
        scena1();
    }
    close();
    return 0;
}