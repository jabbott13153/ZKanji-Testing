//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "alphaimagelist.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(2);
#endif

//---------------------------------------------------------------------------
__fastcall TAlphaImageList::TAlphaImageList(TImageList *bitmap, TImageList *alpha, int first, int cnt)
{
    if (first < 0 || first >= min(bitmap->Count, alpha->Count))
        THROW(L"First index out of range!");
    if (cnt <= 0 || first + cnt > min(bitmap->Count, alpha->Count))
        cnt = min(bitmap->Count - first, alpha->Count - first);

    fcount = cnt;

    if (bitmap->Width != alpha->Width || bitmap->Height != alpha->Height)
        THROW(L"Image lists must match in size!");

    TBmp *tmpimg = NULL;

    images = NULL;
    w = bitmap->Width;
    h = bitmap->Height;

    byte *bits = NULL;
    byte *bitsa = NULL;

    byte *copybits = NULL;

    HDC dc = 0;

    try
    {
        images = new TBmp;
        images->PixelFormat = pf32bit;
        images->SetSize(w*fcount, h);

        bits = new byte[4 * w * h];
        bitsa = new byte[4 * w * h];
        copybits = new byte[4 * w * count * h];
        memset(bits, 0, 4*w*h);
        memset(bitsa, 0, 4*w*h);
        memset(copybits, 0, 4*w*fcount*h);

        tmpimg = new TBmp;
        tmpimg->PixelFormat = pf32bit;
        tmpimg->SetSize(w, h);

        dc = GetDC(0);

        int r;

        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        for (int i = first; i < fcount + first; ++i)
        {

            bitmap->Draw(tmpimg->Canvas, 0, 0, i, true);
            r = GetDIBits(dc, tmpimg->Handle, 0, h, bits, &bi, DIB_RGB_COLORS);
            if (!r)
                THROW(L"Can't retrieve color information from imagelist bitmap!");

            alpha->Draw(tmpimg->Canvas, 0, 0, i, true);
            r = GetDIBits(dc, tmpimg->Handle, 0, h, bitsa, &bi, DIB_RGB_COLORS);
            if (!r)
                THROW(L"Can't retrieve alpha information from imagelist bitmap!");

            for (int iy = 0; iy < h; ++iy)
            {
                byte *ln = bits + iy * w * 4;
                byte *lna = bitsa + iy * w * 4;
                byte *copyln = copybits + i * w * 4 + iy * w * fcount * 4;
                for (int ix = 0; ix < w; ++ix)
                {
                    double alpha = (float)lna[ix * 4 + 1] / 255.0; //take alpha information from green channel
                    copyln[ix * 4] = max(0, min(255.0, ln[ix * 4] * alpha)); //blue channel
                    copyln[ix * 4 + 1] = max(0, min(255.0, ln[ix * 4 + 1] * alpha)); //green channel
                    copyln[ix * 4 + 2] = max(0, min(255.0, ln[ix * 4 + 2] * alpha)); //red channel
                    copyln[ix * 4 + 3] = lna[ix * 4 + 1]; //alpha channel
                }
            }

        }

        bi.bmiHeader.biWidth = w * fcount;

        SetDIBits(dc, images->Handle, 0, h, copybits, &bi, DIB_RGB_COLORS);

    }
    catch (...)
    {
        delete images;
        delete tmpimg;

        delete[] bits;
        delete[] bitsa;
        delete[] copybits;
        ReleaseDC(0, dc);
        throw;
    }

    delete tmpimg;

    delete[] bits;
    delete[] bitsa;
    delete[] copybits;
    ReleaseDC(0, dc);

    memset(&func, 0, sizeof(func));
    func.BlendOp = AC_SRC_OVER;
    func.BlendFlags = 0;
    func.SourceConstantAlpha = 0xff;
    func.AlphaFormat = AC_SRC_ALPHA;
}
//---------------------------------------------------------------------------
__fastcall TAlphaImageList::TAlphaImageList(byte *bits, int imgwidth, int imgheight, int imgcount)
{
    HDC dc = 0;

    w = imgwidth;
    h = imgheight;
    fcount = imgcount;

    images = NULL;
    try
    {
        images = new TBmp;
        images->PixelFormat = pf32bit;
        images->SetSize(w*fcount, h);

        BITMAPINFO bi;
        memset(&bi, 0, sizeof(bi));
        bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
        bi.bmiHeader.biWidth = w * fcount;
        bi.bmiHeader.biHeight = h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;

        dc = GetDC(0);
        int r = SetDIBits(dc, images->Handle, 0, h, bits, &bi, DIB_RGB_COLORS);
        if (!r)
            MsgBox((UnicodeString)L"Couldn't create indicator bitmap. " + GetLastError(), L"Error", MB_OK);
    }
    catch (...)
    {
        delete images;
        ReleaseDC(0, dc);
        throw;
    }
    ReleaseDC(0, dc);

    memset(&func, 0, sizeof(func));
    func.BlendOp = AC_SRC_OVER;
    func.BlendFlags = 0;
    func.SourceConstantAlpha = 0xff;
    func.AlphaFormat = AC_SRC_ALPHA;
}
//---------------------------------------------------------------------------
__fastcall TAlphaImageList::~TAlphaImageList()
{
    delete images;
}
//---------------------------------------------------------------------------
void __fastcall TAlphaImageList::draw(TCanvas *c, int x, int y, int index)
{
    AlphaBlend(c->Handle, x, y, w, h, images->Canvas->Handle, index*w, 0, w, h, func);
}
//---------------------------------------------------------------------------
void __fastcall TAlphaImageList::draw(TCanvas *c, int x, int y, int index, TColor bk)
{
    TBrushStyle os = c->Brush->Style;
    TColor oc = c->Brush->Color;

    c->Brush->Color = bk;
    c->Brush->Style = bsSolid;

    c->FillRect(Rect(x, y, x + w, y + h));

    c->Brush->Color = oc;
    c->Brush->Style = os;

    draw(c, x, y, index);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(3);
#endif

