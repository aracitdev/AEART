#ifndef PIXEL_H_INCLUDED
#define PIXEL_H_INCLUDED

class Pixel
{
public:

    Pixel(char I, SDL_Color C)
    {
        Set(I, C);
    }

    Pixel()
    {
        Image='\0';
    }

    SDL_Color GetColor(void)
    {
        return Color;
    }

    char GetImage(void)
    {
        return Image;
    }

    void Set(char I, SDL_Color C)
    {
        Image=I;
        Color=C;
    }

    void Update(TTF_Font* F)
    {
        if(Image!='\0')
            S=TTF_RenderGlyph_Solid(F, Image, Color);
        else
            S=0;
    }

    void SetColor(SDL_Color C)
    {
        Color=C;
    }

    SDL_Surface* S;
private:

    char Image;
    SDL_Color Color;
};

#endif // PIXEL_H_INCLUDED
