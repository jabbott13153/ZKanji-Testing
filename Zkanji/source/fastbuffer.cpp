//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "fastbuffer.h"
#include "minmax.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(42);
#endif

//---------------------------------------------------------------------------
TFastBuffer::TFastBuffer(HDC dc, TRect r) : cliprect(Rect(0, 0, 0, 0)), fcompatible(false)
{
    bool getsize = r.Right <= r.Left || r.Bottom <= r.Top;
    HWND wnd = WindowFromDC(dc);
    if (wnd)
    {
        TRect rw;
        GetWindowRect(wnd, &rw);
        SendMessage(wnd, WM_NCCALCSIZE, FALSE, (long)((void*)&rw));
        fw = rw.Width();
        fh = rw.Height();
    }
    else
    {
        HGDIOBJ obj = GetCurrentObject(dc, OBJ_BITMAP);
        if (!obj)
            throw Exception(L"Couldn't retrieve BITMAP object from DC!");
        tagBITMAP tbmp;
        memset(&tbmp, 0, sizeof(tagBITMAP));
        if (!GetObject(obj, sizeof(tagBITMAP), &tbmp))
        {
            int err = GetLastError();
            throw Exception((UnicodeString)L"Couldn't retrieve DC from BITMAP object! (errno: " + err + L")");
        }
        fw = tbmp.bmWidth;
        fh = tbmp.bmHeight;
    }

    if (!getsize)
    {
        r.Left = max<int>(r.Left, 0);
        r.Top = max<int>(r.Top, 0);
        r.Right = min<int>(r.Right, fw);
        r.Bottom = min<int>(r.Bottom, fh);
        fw = r.Width();
        fh = r.Height();
    }

    initialize();

    if (!BitBlt(hdc, 0, 0, fw, fh, dc, r.Left, r.Top, SRCCOPY))
    {
        SelectObject(olddc, hbmp);
        DeleteObject(hbmp);
        DeleteDC(hdc);
        int err = GetLastError();
        throw Exception((UnicodeString)L"Last error was: " + err);
    }

}
//---------------------------------------------------------------------------
TFastBuffer::TFastBuffer(int awidth, int aheight) : cliprect(Rect(0, 0, 0, 0)), fcompatible(false)
{
    fw = awidth;
    fh = aheight;

    initialize();
}
//---------------------------------------------------------------------------
void TFastBuffer::initialize()
{
    tagBITMAPINFO tinfo;
    memset(&tinfo, 0, sizeof(tinfo));
    tinfo.bmiHeader.biSize = sizeof(tinfo);
    tinfo.bmiHeader.biWidth = fw;
    tinfo.bmiHeader.biHeight = -fh;
    tinfo.bmiHeader.biPlanes = 1;
    tinfo.bmiHeader.biBitCount = 32;
    tinfo.bmiHeader.biCompression = BI_RGB;
    tinfo.bmiHeader.biSizeImage = 0;

    hbmp = CreateDIBSection(NULL, &tinfo, DIB_RGB_COLORS, (void**) &bits, NULL, NULL);

    if (hbmp)
    {
        hdc = CreateCompatibleDC(NULL);
        olddc = (HDC)SelectObject(hdc, hbmp);
    }
    else
    {
        throw Exception(L"No DIB Section created!");
    }
}
//---------------------------------------------------------------------------
TFastBuffer::~TFastBuffer()
{
    SelectObject(olddc, hbmp);
    DeleteObject(hbmp);
    DeleteDC(hdc);
}
//---------------------------------------------------------------------------
int TFastBuffer::get_pixel(int x, int y)
{
    if (x < 0 || y < 0 || fw <= x || fh <= y)
        return 0;

    char *row = bits + y * 4 * fw;
    return *(int*)(row + x * 4);
}
//---------------------------------------------------------------------------
void TFastBuffer::set_pixel(int x, int y, int color)
{
    if (x < 0 || y < 0 || fw <= x || fh <= y || !IsRectEmpty(cliprect) && (x < cliprect.Left || x >= cliprect.Right || y < cliprect.Top || y >= cliprect.Bottom))
        return;

    char *row = bits + y * 4 * fw;
    *(int*)(row + x*4) = color;
}
//---------------------------------------------------------------------------
void TFastBuffer::get(int x, int y, byte &r, byte &g, byte &b)
{
    if (x < 0 || y < 0 || fw <= x || fh <= y)
    {
        r = g = b = 0;
        return;
    }

    char *row = bits + y * 4 * fw;
    b = row[x * 4];
    g = row[x * 4 + 1];
    r = row[x * 4 + 2];
}
//---------------------------------------------------------------------------
void TFastBuffer::set(int x, int y, byte r, byte g, byte b)
{
    if (x < 0 || y < 0 || fw <= x || fh <= y || !IsRectEmpty(cliprect) && (x < cliprect.Left || x >= cliprect.Right || y < cliprect.Top || y >= cliprect.Bottom))
        return;

    char *row = bits + y * 4 * fw;
    row[x * 4] = b;
    row[x * 4 + 1] = g;
    row[x * 4 + 2] = r;
}
//---------------------------------------------------------------------------
void TFastBuffer::blit(HDC dest, TPoint destpt, TRect src)
{
    HDC dc = hdc;

    if (src.Width() <= 0 || src.Height() <= 0)
        src = Rect(0, 0, fw, fh);
    else
    {
//OffsetRect(&r,-fleft,-ftop);
        src.Right = min<int>(src.Right, fw);
        src.Bottom = min<int>(src.Bottom, fh);
        src.Left = max<int>(src.Left, 0);
        src.Top = max<int>(src.Top, 0);
    }

    int cleft = src.Left;
    int ctop = src.Top;

    HBITMAP cbmp;
    HDC cdc;
    HDC colddc;
    if (fcompatible)
    {
        cbmp = CreateCompatibleBitmap(dest, src.Width(), src.Height());
        cdc = CreateCompatibleDC(dest);
        colddc = (HDC)SelectObject(cdc, cbmp);
        dc = cdc;
        cleft = 0;
        ctop = 0;
        BitBlt(cdc, 0, 0, src.Width(), src.Height(), hdc, src.Left, src.Top, SRCCOPY);
    }
    BitBlt(dest, destpt.x, destpt.y, /*fleft+src.Left,ftop+src.Top,*/src.Width(), src.Height(), dc, cleft, ctop, SRCCOPY);
    if (fcompatible)
    {
        SelectObject(colddc, cbmp);
        DeleteObject(cbmp);
        DeleteDC(cdc);
    }
}
//---------------------------------------------------------------------------
void TFastBuffer::clip(TRect r)
{
    if (r.Width() <= 0 || r.Height() <= 0)
    {
        cliprect = Rect(0, 0, 0, 0);
        SelectClipRgn(hdc, NULL);
        return;
    }

    cliprect.Right = min<int>(r.Right, fw);
    cliprect.Bottom = min<int>(r.Bottom, fh);
    cliprect.Left = max<int>(r.Left, 0);
    cliprect.Top = max<int>(r.Top, 0);

    HRGN rgn = CreateRectRgn(cliprect.Left, cliprect.Top, cliprect.Right, cliprect.Bottom);
    try
    {
        SelectClipRgn(hdc, rgn);
    }
    catch (...)
    {
        ;
    }
    DeleteObject(rgn);

}
//---------------------------------------------------------------------------
void TFastBuffer::noclip()
{
    clip();
}
//---------------------------------------------------------------------------
void TFastBuffer::draw(HDC src, int x, int y, int cx, int cy)
{
//x -= fleft;
//y -= ftop;
    cx = min(cx, fw - x);
    cy = min(cy, fh - y);
    int srcx = max(0, -x), srcy = max(0, -y);
    x = max(x, 0);
    y = max(y, 0);
    if (!BitBlt(hdc, x, y, cx, cy, src, srcx, srcy, SRCCOPY))
    {
        int err = GetLastError();
        if (err)
            THROW(L"There was an error");
    }
}
//---------------------------------------------------------------------------
void TFastBuffer::clear(int color)
{
    for (int y = 0; y < fh; ++y)
    {
        for (int x = 0; x < fw; ++x)
            set_pixel(x, y, color);
    }
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(43);
#endif
