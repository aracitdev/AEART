#ifndef ARACITASCIISDL_H_INCLUDED
#define ARACITASCIISDL_H_INCLUDED

#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <iostream>
#include "coord.h"
#include "buffer.h"
#include "ltimer.h"

class AracitASCII
{
public:
    AracitASCII()
    {
        gWindow=NULL;
        gScreenSurface=NULL;
        WindowFont=NULL;
        BackColor={0x0ff,0xff,0xff, 0};
    }

    bool SetFont(std::string font, int PointSize, int Kerning)
    {
        WindowFont=TTF_OpenFont(font.c_str(), PointSize);
        if(!WindowFont)
            return false;
        TTF_SetFontKerning(WindowFont, Kerning);
        UpdateWindowSize(WindowSize.x, WindowSize.y);
        return true;
    }

    void UpdateWindowSize(int w, int h)
    {
        WindowSize.x=w; WindowSize.y=h;
        UpdateTextSize();
        SDL_SetWindowSize(gWindow, w*TextWidth, h*TextHeight);
        gScreenSurface=SDL_GetWindowSurface(gWindow);
    }

    bool Init(std::string title, Coord Size, std::string font, int PointSize, SDL_Color BC)
    {
        WindowSize=Size;

        if(SDL_Init(SDL_INIT_VIDEO)<0)
        {
            std::cout <<"Failed to initialize SDL video.\n";
            Shutdown();
            return false;
        }

        if(TTF_Init()<0)
        {
            std::cout <<"Failed to init ASCII graphics library.\n";
            Shutdown();
            return false;
        }

        if(!SetFont(font, PointSize, 0))
        {
            std::cout <<"Failed to load the window font "<<font<<"\n";
            Shutdown();
            return false;
        }

        std::cout <<"NOTE: It's HIGHLY suggested to use a MONOSPACE font.\n";

        UpdateTextSize();

        std::cout <<"Calculated font size W: " <<TextWidth <<" H: "<<TextHeight <<"\n";

        gWindow=SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Size.x*TextWidth, Size.y*TextHeight,
                                  SDL_WINDOW_SHOWN);

        if(!gWindow)
        {
            std::cout <<"Failed to create the SDL window.\n";
            Shutdown();
            return false;
        }

        gScreenSurface = SDL_GetWindowSurface( gWindow );

        BackColor=BC;

        return true;
    }

    bool Shutdown(void)
    {
        TTF_CloseFont(WindowFont);
        WindowFont=0;

        SDL_DestroyWindow(gWindow);
        gWindow=0;

        TTF_Quit();
        SDL_Quit();
        return true;
    }

    void UpdateBuffer(Buffer* B)
    {
        B->Update(WindowFont);
    }

    void Clear(void)
    {
        SDL_FillRect(gScreenSurface, NULL, SDL_MapRGB(gScreenSurface->format, BackColor.r, BackColor.g, BackColor.b));//0xffffff);
    }

    void Update(void)
    {
        SDL_UpdateWindowSurface(gWindow);
    }

    void Draw(Buffer* B, Coord Pos, SDL_Rect* cut)
    {
        bool n=false;
        if(cut==NULL)
        {
            n=true;
            cut=new SDL_Rect();
            cut->x=0; cut->y=0; cut->w=B->GetSize().x; cut->h=B->GetSize().y; ///Allows you to cut part of the image
        }
        Coord WindowBound=Coord(WindowSize.x*TextWidth, WindowSize.y*TextHeight);
        SDL_Rect SrcRect;
        SrcRect.w=TextWidth; SrcRect.h=TextHeight; SrcRect.x=0; SrcRect.y=0; ///Set Up Positioning on the source
        SDL_Rect DoneRect;
        bool xend=false;
        bool yend=false;
        for(int counterx=cut->x; counterx<cut->x+cut->w && !xend; counterx++)
            for(int countery=cut->y; countery<cut->h+cut->y && !yend; countery++)
        {
            //std::cout <<counterx <<" "<<countery<<"\n";
            if(B->GetPixel(counterx,countery).S)
            {
                DoneRect.x=((counterx-cut->x)+Pos.x)*TextWidth; DoneRect.w=TextWidth;
                DoneRect.y=((countery-cut->y)+Pos.y)*TextHeight; DoneRect.h=TextHeight;
                if(DoneRect.y>=WindowBound.y)
                    yend=true;
                else
                if(DoneRect.x>=WindowBound.x)
                    xend=true;
                else
                if(DoneRect.x>0 && DoneRect.y>0)
                    SDL_BlitSurface(B->GetPixel(counterx,countery).S, &SrcRect, gScreenSurface, &DoneRect);
            }
            yend=false;
        }
        if(n)
        {
            delete cut;
            cut=NULL;
        }
    }

    //The window we'll be rendering to
    SDL_Window* gWindow;

    //The surface contained by the window
    SDL_Surface* gScreenSurface;

private:

    TTF_Font* WindowFont;

    SDL_Color BackColor;

    int TextWidth, TextHeight;

    Coord WindowSize;

    void UpdateTextSize(void)
    {
        TTF_SizeText(WindowFont, "#", &TextWidth, &TextHeight);
    }
};


#endif // ARACITASCIISDL_H_INCLUDED
