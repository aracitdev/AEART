#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED
#include <vector>
#include <fstream>
#include "Pixel.h"

int CountLines(std::string filename)
{
    std::ifstream file;
    file.open(filename.c_str());
    if(!file.is_open())
    {
        std::cout <<"Failed to load the file " <<filename <<"\n";
        return -1;
    }
    std::ifstream f(filename.c_str());
    std::string line;
    int i;
    for (i = 0; std::getline(f, line); ++i)
    ;
    std::cout <<i;
    return i;
}

void WriteColor(std::ofstream* file, SDL_Color c)
{
    uint8_t b[4]={c.r, c.g, c.b, c.a}; // use an unsigned type like uint8_t... don't use char
    /*file->read( reinterpret_cast<char*>(b), 4 );*/

    uint32_t v = b[0] |
            (b[1] << 8) |
            (b[2] << 16) |
            (b[3] << 24);

    //cout << v; // now prints 511 regardless of system endianness

    // to write:

    b[0] = (v & 0xFF);
    b[1] = ((v >> 8) & 0xFF);
    b[2] = ((v >> 16) & 0xFF);
    b[3] = ((v >> 24) & 0xFF);

    file->write( reinterpret_cast<char*>(b), 4 );
}

void ReadColor(std::ifstream* file, SDL_Color& c)
{
    uint8_t b[4];
    file->read(reinterpret_cast<char*>(b),4);
    uint32_t v = b[0] |
                    (b[1]<<8) |
                    (b[2]<<16)|
                    (b[2]<<24);
    c.r=b[0];
    c.g=b[1];
    c.b=b[2];
    c.a=b[3];
}

class Buffer
{
public:
    Buffer()
    {
        Resize(1,1);
    }

    Buffer(int x, int y)
    {
        Resize(x,y);
    }

    void Clear(Pixel P)
    {
        for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
            for(unsigned int countery=0; countery<PixelSet[counterx].size(); countery++)
            PixelSet[counterx][countery]=P;
    }

    void Clear(void)
    {
        SDL_Color White={255,255,255,0};
        Clear(Pixel(' ',White));
    }

    void Resize(int x, int y)
    {
        PixelSet.resize(x);
        for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
            PixelSet[counterx].resize(y);
    }

    Pixel GetPixel(int x, int y)
    {
        if(x>=0 && y>=0 && x<PixelSet.size() && y<PixelSet[0].size())
            return PixelSet[x][y];
        else
            return Pixel();
    }

    bool SetPixel(int x, int y, Pixel P)
    {
        if(x<0 || x>=(int)PixelSet.size() || y<0 || y>=(int)PixelSet[x].size())
            return false;
        PixelSet[x][y]=P;
        return true;
    }

    void Destroy(void)
    {
        PixelSet.clear();
    }

    bool Copy(Buffer* B, SDL_Rect Pos, SDL_Rect DrawPos)
    {
        if(!B)
            return false;
        for(int counterx=Pos.x; counterx<Pos.w; counterx++)
            for(int countery=Pos.y; countery<Pos.h; counterx++)
            SetPixel(counterx+DrawPos.x, countery+DrawPos.y, B->GetPixel(counterx,countery));
        return true;
    }

    Coord GetSize(void)
    {
        return Coord(PixelSet.size(), PixelSet[0].size());
    }

    void Update(TTF_Font* F)
    {
        for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
            for(unsigned int countery=0; countery<PixelSet[counterx].size(); countery++)
            PixelSet[counterx][countery].Update(F);
    }

    bool LoadRawFromFile(std::string filename)
    {
        int Lines=CountLines(filename);
        std::cout <<filename<<" Has " <<Lines <<" Lines\n";
        if(Lines<=0)
            return false;
        std::ifstream file;
        file.open(filename.c_str());
        if(!file.is_open())
            return false;
        SDL_Color Black={0,0,0,0};
        std::cout <<"Opened " <<filename <<"\n";
        std::vector<std::vector<char> > CharSet;
        CharSet.resize(Lines);
        for(unsigned int countery=0; countery<CharSet.size(); countery++)
        {
            char currentc=' ';
            for(int counterx=0; currentc!='\0' && currentc!='\n'; counterx++)
            {
                if(!file.get(currentc))
                    currentc='\n';
                if(currentc!='\n'&&currentc!='\0')
                {
                    CharSet[countery].push_back(currentc);
                }
            }
        }
        std::cout <<"Loading finished.\n";
        Standardize(&CharSet);
        std::cout <<"Standardized.\n";

        Resize(CharSet[0].size(), CharSet.size());

        for(unsigned int countery=0; countery<CharSet.size(); countery++)
            for(unsigned int counterx=0; counterx<CharSet[countery].size(); counterx++)
                PixelSet[counterx][countery]=Pixel(CharSet[countery][counterx], Black);
        CharSet.clear();
        std::cout <<"Coppied.\n";
        return true;
    }

    bool SaveRaw(std::string filename)
    {
        std::ofstream file;
        file.open(filename.c_str());
        if(!file.is_open())
            return false;
        for(unsigned int countery=0; countery<PixelSet[0].size(); countery++)
        {
            for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
                file<<PixelSet[counterx][countery].GetImage();
            if(countery!=PixelSet.size()-1)
                file<<'\n';
        }
        return true;
    }

    bool OpenImage(std::string filename)
    {
        std::ifstream file;
        file.open(filename.c_str(), std::ios::binary | std::ios::in);
        if(!file.is_open())
            return false;
        std::string textfilename;
        getline(file, textfilename);
        std::cout <<"Text file " <<textfilename<<"\n";
        if(!LoadRawFromFile(textfilename))
            return false;
        for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
            for(unsigned int countery=0; countery<PixelSet[counterx].size(); countery++)
            {
                SDL_Color c;
                ReadColor(&file, c);
                PixelSet[counterx][countery].SetColor(c);
            }
        return true;
    }

    bool SaveImage(std::string filename, std::string text)
    {
        std::ofstream file;
        file.open(filename.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        if(!file.is_open())
            return false;
        file<<text<<"\n";
        if(!SaveRaw(text))
            return false;
        for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
        {
            for(unsigned int countery=0; countery<PixelSet[counterx].size(); countery++)
            {
                WriteColor(&file, PixelSet[counterx][countery].GetColor());
            }
        }
        return true;
    }

    void Standardize(std::vector<std::vector<char> > *CharSet)
    {
        int sz=GetGreatestX(CharSet);
        for(unsigned int countery=0; countery<CharSet->size(); countery++)
            CharSet->at(countery).resize(sz);
    }

    int GetGreatestX(std::vector<std::vector<char> > *CharSet)
    {
        if(!CharSet)
            return -1;
        unsigned int g=0;
        for(unsigned int countery=0; countery<CharSet->size(); countery++)
            if(CharSet->at(countery).size()>g)
                g=CharSet->at(countery).size();
        return g;
    }

    void SetColor(SDL_Color c)
    {
        for(unsigned int counterx=0; counterx<PixelSet.size(); counterx++)
            for(unsigned int countery=0; countery<PixelSet[counterx].size(); countery++)
            PixelSet[counterx][countery].SetColor(c);
    }

private:

    std::vector< std::vector<Pixel> > PixelSet;
};


#endif // BUFFER_H_INCLUDED
