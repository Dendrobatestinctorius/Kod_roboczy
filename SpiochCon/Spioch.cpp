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
const SDL_Color wcolor = {255, 255, 255};

class Window
{
private:
    
    SDL_Window* SpiochWindow;
    

public:
    Window();
    bool init();
    SDL_Renderer* createRenderer();
    void setresolution( int scrnW, int scrnH );
    void setfullscrn( bool fs );
    void free();
    int SCREEN_W = 0;
    int SCREEN_H = 0;
    bool fscrn;
};

class DBtexture
{
public:
    int mWidth;
    int mHeight;
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
private:
    int getWidth();
    int getHeight();
    TTF_Font* DBfont;
    SDL_Texture* mtexture;

};

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
/*
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
*/

SDL_Renderer* SpiochRenderer = NULL;
Window SpiochW;


Window::Window()
{
    SpiochWindow = NULL;
    SCREEN_W = 1280;
    SCREEN_H = 720;
}

bool Window::init()
{
    SpiochWindow = SDL_CreateWindow( "Spioch Gra Fabularna", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN );
    if( SpiochWindow != NULL )
    {
        SDL_SetWindowFullscreen( SpiochWindow, SDL_TRUE );
        fscrn = true;
    }
    return SpiochWindow != NULL;

}

SDL_Renderer* Window::createRenderer()
{
    return SDL_CreateRenderer( SpiochWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
}

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
       // SDL_SetWindowDisplayMode( SpiochWindow,);
    }
    
    SDL_RenderPresent( SpiochRenderer );
}

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

void Window::free()
{
    if( SpiochWindow != NULL )
    {
        SDL_DestroyWindow( SpiochWindow );
    }
    SCREEN_H = 0;
    SCREEN_W = 0;
}

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

void DBtexture::fintxt( std::string textureText, SDL_Color textColor, int x, int y, Uint8 alpha)
{
    loadText( textureText, tcolor );
    setBlendMode( SDL_BLENDMODE_BLEND );
    setAplha( alpha );
    render(x, y);
}

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
        else if( x > ePos.x + exitspritesheet.mWidth )
        {
            inside = false;
        }
        else if( y < ePos.y )
        {
            inside = false;
        }
        else if( y > ePos.y + (exitspritesheet.mHeight/4) )
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

void btt::renderbtt( Uint8 alpha )
{
    exitspritesheet.setBlendMode( SDL_BLENDMODE_BLEND );
    exitspritesheet.setAplha( alpha );
    exitspritesheet.render( ePos.x, ePos.y, &bexitclip[exitcurrentsprite] );
}

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
            bexitclip[i].y = i * (exitspritesheet.mHeight/4);
            bexitclip[i].w = exitspritesheet.mWidth;
            bexitclip[i].h = exitspritesheet.mHeight/4;
        }
    }
}

void btt::endbtt()
{
    exitspritesheet.free();
}

bool init()
{
    bool success = true;
    if( SDL_Init( SDL_INIT_VIDEO /*|  SDL_INIT_AUDIO */ ) < 0)
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
 /*               if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                } */
            }
        }        
    }
    return success;
}
/*
sound::sound()
{
    mmusic = NULL;
}

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

void sound::endsnd()
{
    Mix_FreeMusic( mmusic );
    mmusic = NULL;
}
*/
void close()
{
    SDL_DestroyRenderer( SpiochRenderer );
    SpiochW.free();
    SpiochRenderer = NULL;
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void intro()
{
    DBtexture intro;
    intro.LFF( "PNG/intro.png" );
    int i = 0;
    while ( i <= 150 )
    {
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        intro.render( 0, 0 );
        SDL_RenderPresent( SpiochRenderer );
        ++i;
    }

    intro.free();

}

void title()
{
    btt btt1;
    DBtexture door;
    btt1.loadbtt("PNG/Candle_btt.png");
    door.LFF("PNG/door.png");
    SDL_Rect doorclip[2];
    for(int d = 0; d < 2; ++d)
    {
            doorclip[d].x = d * (door.mWidth/2);
            doorclip[d].y = 0;
            doorclip[d].w = door.mWidth/2;
            doorclip[d].h = door.mHeight;
    }
    bool quit = false;
    SDL_Event e;
    int i = 0;
    int a = 255;
    while(i < 960 )
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
                i += 5;
            }
            
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 0, 0, 0, 255 );
        SDL_RenderClear( SpiochRenderer );
        door.render( 0 - i, 0, &doorclip[0] );
        door.render( (door.mWidth/2) + i, 0, &doorclip[1] );
        btt1.setpos(SpiochW.SCREEN_W/2 - 51, SpiochW.SCREEN_H/2 - 51);
        btt1.renderbtt(a);
        SDL_RenderPresent( SpiochRenderer );
    }
    door.free();
    btt1.endbtt();
    
}

bool opcje();

bool menu()
{
    bool quit = false;
    bool opc = false;
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
            btt2.handleEvent( &e );

        }
        if( opc )
        {
            opc = opcje();
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        btt2.setpos(50, SpiochW.SCREEN_H/4);
        btt2.renderbtt(255);
        btt3.setpos( 50, SpiochW.SCREEN_H/4 + 80);
        btt3.renderbtt( 255 );
        btt1.setpos( SpiochW.SCREEN_W/2 - 50, SpiochW.SCREEN_H - 100);
        btt1.renderbtt(255);
        text.fintxt( "Zakończ", tcolor, SpiochW.SCREEN_W/2 - 40, SpiochW.SCREEN_H - 90, 255 );
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
    bool okno = false;
    bool fscr = false;
    btt btt1, btt2, btt3;
    DBtexture text;
    btt1.loadbtt("PNG/btt.png");
    btt2.loadbtt("PNG/btt.png");
    btt3.loadbtt("PNG/btt.png");
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
            if(SpiochW.fscrn)
            {
                okno = btt2.handleEvent( &e );
                if(okno)
                {
                    SpiochW.setfullscrn( false );
                    fscr = false;
                }
            }
            else
            {
                fscr = btt3.handleEvent( &e );
                if(fscr)
                {
                    SpiochW.setfullscrn( true );
                    okno = false;
                }
            }
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 192, 143, 255 );
        SDL_RenderClear( SpiochRenderer );
        
        if(SpiochW.fscrn)
        {
            btt2.setpos(50, SpiochW.SCREEN_H/4);
            btt2.renderbtt(255);
            text.fintxt("okno", tcolor, 55, SpiochW.SCREEN_H/4 + 10, 255);
        }
        else
        {
            btt3.setpos(50, SpiochW.SCREEN_H/4);
            btt3.renderbtt(255);
            text.fintxt("Pełny Ekran", tcolor, 55, SpiochW.SCREEN_H/4 + 10, 255);
        }
        btt1.setpos( SpiochW.SCREEN_W/2 - 50, SpiochW.SCREEN_H - 100);
        btt1.renderbtt(255);
        text.fintxt( "Wróć", tcolor, SpiochW.SCREEN_W/2 - 40, SpiochW.SCREEN_H - 90, 255 );
        SDL_RenderPresent( SpiochRenderer );
    }
    btt1.endbtt();
    btt2.endbtt();
    text.free();
    return false;
}
/*
bool scena1()
{
    
    bool bar1p = false;
    bool dajs = false;
    bool wprowadzenie = false;
    int f = 0;
    int i = 0;
    int w = 0;
    Uint8 a = 0;
    btt btt1, btt2, btt3, btt4;
    btt1.loadbtt("PNG/btt.png");
    btt2.loadbtt("PNG/chcebar.png");
    btt3.loadbtt("PNG/chcebar.png");
    btt4.loadbtt("PNG/chcebar.png");
    
    DBtexture text;
    text.setFont( 20 );
    
    
    bool quit = false;
    SDL_Event e;
    
    while( !wprowadzenie )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            
            if( !wprowadzenie )
            {
                 wprowadzenie = btt1.handleEvent( &e );
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
        if( i >= 50 )
        {
            text.fintxt( "Mój świat jest daleko. Czy może raczej dawno? Wyruszam dzisiaj po to by nie przepadły", tcolor, 100, 200, f );
            text.fintxt( "resztki tego co mnie z nim wiążą. Tak naprawdę nie wierzyłem w to, że cokolwiek zostało z tego", tcolor, 100, 225, f );
            text.fintxt( "co ukryłem w skryte wbudowanej w podstawę mojej trumny. Jednakże musiałem spróbować", tcolor, 100, 250, f);
            text.fintxt( "Dla siebie, by nie oszaleć od obcości Oktawii.", tcolor, 100, 275, f );
            if( f < 255 )
            {
                f += 3;
            }
        }
        if( i >= 180)
        {
            text.fintxt( "Te ponure rozmyślania przerwał mi Gyce.", tcolor, 100, 350, w );
            if( w < 255 )
            {
                w++;
            }
        }
        if( i >= 400 )
        {
            btt1.setpos(SpiochW.SCREEN_W/2 - 50, SpiochW.SCREEN_H/2 - 25 );
            btt1.renderbtt(a);
            text.fintxt(" Dalej", tcolor, SpiochW.SCREEN_W/2 - 40, SpiochW.SCREEN_H/2 - 20, a);
            if(a < 255 )
            {
                a += 17;
            }
        }
        SDL_RenderPresent( SpiochRenderer );
        ++i;
    }
    while( !quit )
    {
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( !quit )
            {
                quit = btt1.handleEvent( &e );
            }
            if( bar1p == false )
            {
                bar1p = btt2.handleEvent( &e );
            }
            btt3.handleEvent( &e );
            btt4.handleEvent( &e );
        }
        SDL_SetRenderDrawColor( SpiochRenderer, 214, 193, 143, 255);
        SDL_RenderClear( SpiochRenderer );
        btt1.setpos( SpiochW.SCREEN_W/2 - 50, SpiochW.SCREEN_H/2 - 25 );    
        btt1.renderbtt(255);
        text.fintxt("Zakończ", tcolor, SpiochW.SCREEN_W/2 - 40, SpiochW.SCREEN_H/2 - 20, 255 );
        if( bar1p == false )
        {
            btt2.setpos( 50, 660 );
            btt2.renderbtt(255);
            text.fintxt("Daj mi spokój", tcolor, 70, 680, 255 );
        }
        btt3.setpos( 50, 780 );
        btt3.renderbtt(255);
        btt4.setpos( 50, 900 );
        btt4.renderbtt(255);
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
    btt1.endbtt();
    btt2.endbtt();
    btt3.endbtt();
    btt4.endbtt();
    text.fontend();

    return true;
}
*/
int main( int argc, char* argd[] )
{
    bool end = false;
    if( !init() )
    {
        printf( "Failed to initialize " );
    }
    else
    {
        //sound tlo;
        //tlo.loadmusic( "Sound/music.mp3" );
        intro();
        while (!end)
        {
            
            //tlo.playsnd();
            title();
            end = menu();
        }
        //tlo.stopsnd();
        //tlo.endsnd();
    } 
    close();
    return 0;
}