#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED
#include "../AracitASCIISDL/AracitASCIISDL.h"

std::string Extension(std::string filename)
{
    std::size_t fnd=filename.find(".");
    return filename.substr(fnd);
}

class Engine
{
public:
    bool Init(int argc, char* args[])
    {
        White={0,255,255,255};
        Red={0,0,0,255};
        Black={0,0,0,0};
        Blue={0,255,0,0};
        Green={0,0,255,0};

        Fps=0;
        frame=0;
        fpsTimer.stop();

        if(!window.Init(std::string(""), Coord(100,100), "lucon.ttf", 4, White))
            return false;

        if(argc==1)
            return true;

        if(argc==2)
        {
            if(Extension(std::string(args[1]))==".ascii")
            {
                if(!Image.OpenImage(std::string(args[1])))
                    return false;
                else
                if(!Image.LoadRawFromFile(std::string(args[1])))
                    return false;
            }
        }

        return true;
    }

    bool Shutdown(void)
    {
        return true;
    }

    int Run(void)
    {
        while(1)
        {
            if(fpsTimer.getTicks()>1000)
            {
                Fps=frame;
                frame=0;
                std::cout <<Fps<<"\n";
                fpsTimer.stop();
                fpsTimer.start();
            }

            window.Clear();
            window.Draw(&Image, Coord(-40,-10), NULL);
            frame++;
            window.Update();
        }
        return 0;
    }

    AracitASCII window;
    Buffer Image;

    int frame;
    LTimer fpsTimer;
    int Fps;

    SDL_Color White;
    SDL_Color Red;
    SDL_Color Black;
    SDL_Color Blue;
    SDL_Color Green;
};

#endif // ENGINE_H_INCLUDED
