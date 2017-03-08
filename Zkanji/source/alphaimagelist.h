//---------------------------------------------------------------------------

#ifndef alphaimagelistH
#define alphaimagelistH
//---------------------------------------------------------------------------

class TAlphaImageList
{
private:
    TBmp *images; //generated images from bitmap and alpha information
    byte w; //width of a single image
    byte h; //height of a single image

    int fcount;

    BLENDFUNCTION func;
public:
    __fastcall TAlphaImageList(TImageList *bitmap, TImageList *alpha, int first = 0, int count = 0);
    __fastcall TAlphaImageList(byte *bits, int imgwidth, int imgheight, int imgcount);
    __fastcall ~TAlphaImageList();

    void __fastcall draw(TCanvas *c, int x, int y, int index);
    void __fastcall draw(TCanvas *c, int x, int y, int index, TColor bk);

    __property byte width = { read = w };
    __property byte height = { read = h };
    __property int count = { read = fcount };
};

#endif
