//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "gengraph.h"
#include "baseform.h"
#include "settings.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(48);
#endif

extern bool ThemesAreTurnedOn;

//---------------------------------------------------------------------------
//Generic drawing functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
TRect __fastcall FormBevelRect(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel)
{
    TRect r;
    if (topbevel)
    {
        r = form->ChildRect(topbevel);
        r.Top += (topbevel->Height == 1 ? 1 : 2);
    }
    else
        r = Rect(0, 0, form->ClientWidth, form->ClientHeight);

    if (bottombevel)
        r.Bottom = form->ChildRect(bottombevel).Top;
    else
        r.Bottom = form->ClientHeight;

    r.Left = max((long)0, r.Left);
    r.Right = min((long)form->ClientWidth, r.Right);

    return r;
}
//---------------------------------------------------------------------------
TRect __fastcall HorzFormBevelRect(TBaseForm *form, TBevel *leftbevel, TBevel *rightbevel)
{
    TRect r;
    if (leftbevel)
    {
        r = form->ChildRect(leftbevel);
        r.Left += (leftbevel->Width == 1 ? 1 : 2);
    }
    else
        r = Rect(0, 0, form->ClientWidth, form->ClientHeight);

    if (rightbevel)
        r.Right = form->ChildRect(rightbevel).Left;
    else
        r.Right = form->ClientWidth;

    r.Top = max((long)0, r.Top);
    r.Bottom = min((long)form->ClientHeight, r.Bottom);

    return r;
}
//---------------------------------------------------------------------------
void __fastcall DrawButtonBar(TCanvas *c, const TRect &r, bool stretch)
{
    if (!ThemesAreTurnedOn)
        return;

    int half = stretch ? r.Height() * 0.5 : min(13, r.Height() * 0.46);
    GradientFillCanvas(c, COLORINC(clBtnFace, 0x101010), COLORINC(clBtnFace, 0x303030), Rect(r.Left, r.Top, r.Right, r.Top + half), gdVertical);
    GradientFillCanvas(c, clBtnFace, COLORDEC(clBtnFace, 0x161616), Rect(r.Left, r.Top + half, r.Right, r.Bottom), gdVertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawButtonBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel, bool stretch)
{
    TRect r = FormBevelRect(form, topbevel, bottombevel);
    DrawButtonBar(form->Canvas, r, stretch);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DrawPillowPanel(TCanvas *c, const TRect &r, bool dim, bool sunken)
{
    if (!ThemesAreTurnedOn)
        return;

    TColor c1 = sunken ? dim ? COLORDEC(clBtnFace, 0x060606) : COLORDEC(clBtnFace, 0x101010) : dim ? COLORINC(clBtnFace, 0x060606) : COLORINC(clBtnFace, 0x101010), c2 = sunken ? dim ? COLORINC(clBtnFace, 0x060606) : COLORINC(clBtnFace, 0x101010) : dim ? COLORDEC(clBtnFace, 0x060606) : COLORDEC(clBtnFace, 0x101010);
    GradientFillCanvas(c, c1, c2, r, gdVertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawPillowPanel(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel, bool dim, bool sunken)
{
    TRect r = FormBevelRect(form, topbevel, bottombevel);
    DrawPillowPanel(form->Canvas, r, dim, sunken);
}
//---------------------------------------------------------------------------
void __fastcall DrawHorzPillowPanel(TCanvas *c, const TRect &r, bool dim, bool sunken)
{
    if (!ThemesAreTurnedOn)
        return;

    TColor c1 = sunken ? dim ? COLORDEC(clBtnFace, 0x060606) : COLORDEC(clBtnFace, 0x101010) : dim ? COLORINC(clBtnFace, 0x060606) : COLORINC(clBtnFace, 0x101010), c2 = sunken ? dim ? COLORINC(clBtnFace, 0x060606) : COLORINC(clBtnFace, 0x101010) : dim ? COLORDEC(clBtnFace, 0x060606) : COLORDEC(clBtnFace, 0x101010);
    GradientFillCanvas(c, c1, c2, r, gdHorizontal);
}
//---------------------------------------------------------------------------
void __fastcall DrawHorzPillowPanel(TBaseForm *form, TBevel *leftbevel, TBevel *rightbevel, bool dim, bool sunken)
{
    TRect r = HorzFormBevelRect(form, leftbevel, rightbevel);
    DrawHorzPillowPanel(form->Canvas, r, dim, sunken);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DrawCaptionBar(TCanvas *c, const TRect &r, bool active)
{
    if (!ThemesAreTurnedOn)
        return;

    TColor base = clBtnFace;
    TColor modifier = (TColor)0xd0d0d0;
    TColor modifier2 = COLORDEC(modifier, 0x303030);
    if (active)
    {
        base = coloravg(base, (TColor)colorsettings.activeborder, 0.2);
        modifier = coloravg(modifier, (TColor)colorsettings.activeborder, 0.2);
    }
    GradientFillCanvas(c, coloravg(base, modifier, 0.5), coloravg(base, modifier, 0.7), Rect(r.Left, r.Top, r.Right, r.Top + r.Height()*0.1), gdVertical);
    c->Brush->Style = bsSolid;
    c->Brush->Color = coloravg(base, modifier, 0.7);
    c->FillRect(Rect(r.Left, r.Top + r.Height()*0.1, r.Right, r.Top + r.Height()*0.2));
    GradientFillCanvas(c, coloravg(base, modifier, 0.7), coloravg(base, modifier2, 0.6), Rect(r.Left, r.Top + r.Height()*0.2, r.Right, r.Bottom), gdVertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawCaptionBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel, bool active)
{
    TRect r = FormBevelRect(form, topbevel, bottombevel);
    DrawCaptionBar(form->Canvas, r, active);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DrawStatusBar(TCanvas *c, const TRect &r)
{
    if (!ThemesAreTurnedOn)
        return;

    GradientFillCanvas(c, COLORINC(clBtnFace, 0x080808), clBtnFace, r, gdVertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawStatusBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel)
{
    TRect r = FormBevelRect(form, topbevel, bottombevel);
    DrawStatusBar(form->Canvas, r);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DrawCloseBar(TCanvas *c, const TRect &r)
{
    if (!ThemesAreTurnedOn)
        return;

    GradientFillCanvas(c, COLORDEC(clBtnFace, 0x363636), COLORDEC(clBtnFace, 0x060606), Rect(r.Left, r.Top, r.Right, r.Top + r.Height()*0.15), gdVertical);
    GradientFillCanvas(c, COLORDEC(clBtnFace, 0x060606), COLORINC(clBtnFace, 0x101010), Rect(r.Left, r.Top + r.Height()*0.15, r.Right, r.Bottom), gdVertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawCloseBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel)
{
    TRect r = FormBevelRect(form, topbevel, bottombevel);
    DrawCloseBar(form->Canvas, r);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DrawTabBar(TCanvas *c, const TRect &r)
{
    if (!ThemesAreTurnedOn)
        return;

    GradientFillCanvas(c, COLORDEC(coloravg(clBtnFace, clSkyBlue, 0.9), 0x363636), COLORDEC(coloravg(clBtnFace, clSkyBlue, 0.9), 0x161616), r, gdVertical);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void __fastcall DrawGrad(TBaseForm *form, TColor c1, TColor c2, TBevel *topbevel, TBevel *bottombevel, bool vertical)
{
    DrawGrad(form->Canvas, c1, c2, FormBevelRect(form, topbevel, bottombevel), vertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawGrad(TCanvas *c, TColor c1, TColor c2, const TRect &r, bool vertical)
{
    if (!ThemesAreTurnedOn)
        return;

    GradientFillCanvas(c, c1, c2, r, vertical ? gdVertical : gdHorizontal);
}
//---------------------------------------------------------------------------
void __fastcall DrawGrad(TBaseForm *form, TColor c1, TColor c2, TColor c3, double mid, TBevel *topbevel, TBevel *bottombevel, bool vertical)
{
    DrawGrad(form->Canvas, c1, c2, c3, mid, FormBevelRect(form, topbevel, bottombevel), vertical);
}
//---------------------------------------------------------------------------
void __fastcall DrawGrad(TCanvas *c, TColor c1, TColor c2, TColor c3, double mid, const TRect &r, bool vertical)
{
    if (!ThemesAreTurnedOn)
        return;

    int m = (vertical ? r.Height() : r.Width()) * mid;
    GradientFillCanvas(c, c1, c2, Rect(r.Left, r.Top, vertical ? r.Right : (r.Left + m), vertical ? (r.Top + m) : r.Bottom), vertical ? gdVertical : gdHorizontal);
    GradientFillCanvas(c, c2, c3, Rect(vertical ? r.Left : (r.Left + m), vertical ? (r.Top + m) : r.Top, r.Right, r.Bottom), vertical ? gdVertical : gdHorizontal);
}

//---------------------------------------------------------------------------
//Clipping functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void __fastcall SetClipRect(TCanvas *c, const TRect &r)
{
    HRGN rgn = CreateRectRgnIndirect(&r);
    SelectClipRgn(c->Handle, rgn);
    DeleteObject(rgn);
}
//---------------------------------------------------------------------------
void __fastcall ClearClipRect(TCanvas *c)
{
    SelectClipRgn(c->Handle, NULL);
}
//---------------------------------------------------------------------------
void __fastcall ExcludeClipRect(TCanvas *c, const TRect &r)
{
    ExcludeClipRect(c->Handle, r.Left, r.Top, r.Right, r.Bottom);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(49);
#endif

