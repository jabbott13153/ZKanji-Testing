//---------------------------------------------------------------------------

#ifndef fastbufferH
#define fastbufferH
//---------------------------------------------------------------------------

#include "angle.h"

class TFastBuffer
{
private:
    HDC src;

    char *bits;
    HBITMAP hbmp; //dib section
    HDC hdc; //dc created for drawing
    HDC olddc; //select this back when done

    TRect cliprect;

    bool fcompatible; //use compatible bitmap when blitting

   //dimensions of created DC
    int fw;
    int fh;

    void initialize();

    int get_pixel(int x, int y);
    void set_pixel(int x, int y, int color);
public:
    TFastBuffer(HDC dc, TRect r = Rect(0, 0, -1, -1));
    TFastBuffer(int awidth, int aheight);
    ~TFastBuffer();
    void get(int x, int y, byte &r, byte &g, byte &b);
    void set(int x, int y, byte r, byte g, byte b);

    void blit(HDC dest, TPoint destpt = Point(0, 0), TRect src = Rect(0, 0, -1, -1));

    void clip(TRect r = Rect(0, 0, -1, -1));
    void noclip();
    void draw(HDC src, int x, int y, int cx, int cy);

    void clear(int color);

    __property HDC Handle = { read = hdc };

    __property int Width = { read = fw };
    __property int Height = { read = fh };

    __property bool UseCompatible = { read = fcompatible, write = fcompatible };
    __property int Pixels[int x][int y] = { read = get_pixel, write = set_pixel };
};

#endif
