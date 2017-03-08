//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#pragma warn -8022
#include <GdiPlus.h>
#pragma warn .8022

#include "zdefines.h"
#include <Math.h>
#include "gdipbmp.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

using namespace Gdiplus;

#ifdef _DEBUG
static TGlobalTest tester1(46);

// Make compiler happy by using variables declared elsewhere:
static bool Dummy_function()
{
    if (FlatnessDefault > 0)
        return false;
    GenericSansSerifFontFamily = NULL;
    GenericSerifFontFamily     = NULL;
    GenericMonospaceFontFamily = NULL;

    memset(GenericSansSerifFontFamilyBuffer, 0, sizeof(FontFamily));
    memset(GenericSerifFontFamilyBuffer, 0, sizeof(FontFamily));
    memset(GenericMonospaceFontFamilyBuffer, 0, sizeof(FontFamily));

    memset(GenericTypographicStringFormatBuffer, 0, sizeof(StringFormat));
    memset(GenericDefaultStringFormatBuffer, 0, sizeof(StringFormat));

    return true;
}
#endif

namespace GdipHelpers
{
  /*helper functions*/
    PointF pf(TDoublePoint p)
    {
        return PointF(p.x, p.y);
    }

    Color gdic(int c)
    {
        return Gdiplus::Color((c & 0xff), (c & 0xff00) >> 8, (c & 0xff0000) >> 16);
    }
}
/*----------------*/
TGdiToolSettingsData::TGdiToolSettingsData() : used(false)
{
    ;
}
TGdiToolSettingsData::TGdiToolSettingsData(const TGdiToolSettingsData &orig) : used(orig.used)
{
    ;
}
/*----------------*/
TGdiLineSettingsData::TGdiLineSettingsData() : color(0), width(1.0), endwidth(-1), x_scale(-1), y_scale(-1), TGdiToolSettingsData()
{
    ;
}
TGdiLineSettingsData::TGdiLineSettingsData(int color, float width, float endwidth, float x_scale, float y_scale) : color(color), width(width), endwidth(endwidth), x_scale(x_scale), y_scale(y_scale), TGdiToolSettingsData()
{
    ;
}
TGdiLineSettingsData::TGdiLineSettingsData(const TGdiLineSettingsData &orig) : color(orig.color), width(orig.width), endwidth(orig.endwidth), x_scale(orig.x_scale), y_scale(orig.y_scale), TGdiToolSettingsData(orig)
{
    ;
}
TGdiLineSettingsData& TGdiLineSettingsData:: operator = (const TGdiLineSettings & settings)
{
    if (settings.usecolor)
        color = settings.data.color;
    if (settings.usewidth)
        width = settings.data.width;
    if (settings.useendwidth)
        endwidth = settings.data.endwidth;
    if (settings.usex_scale)
        x_scale = settings.data.x_scale;
    if (settings.usey_scale)
        y_scale = settings.data.y_scale;

    return *this;
}
/*----------------*/
TGdiLineSettings::TGdiLineSettings(bool usecolor, int color, bool usewidth, float width, bool useendwidth, float endwidth, bool usex_scale, float x_scale, bool usey_scale, float y_scale) : usecolor(usecolor), usewidth(usewidth), useendwidth(useendwidth), usex_scale(usex_scale), usey_scale(usey_scale),
        data(color, width, endwidth, x_scale, y_scale), TGdiToolSettings()
{
    ;
}
/*----------------*/
TGdiFillSettingsData::TGdiFillSettingsData() : color(0), TGdiToolSettingsData()
{
    ;
}
TGdiFillSettingsData::TGdiFillSettingsData(int color) : color(color), TGdiToolSettingsData()
{
    ;
}
TGdiFillSettingsData::TGdiFillSettingsData(const TGdiFillSettingsData &orig) : color(orig.color), TGdiToolSettingsData(orig)
{
    ;
}
TGdiFillSettingsData& TGdiFillSettingsData:: operator = (const TGdiFillSettings & settings)
{
    if (settings.usecolor)
        color = settings.data.color;

    return *this;
}
/*----------------*/
TGdiFillSettings::TGdiFillSettings(bool usecolor, int color) : usecolor(usecolor),
        data(color), TGdiToolSettings()
{
    ;
}
/*----------------*/
TGdiLineSettings GdiLineWidth(float width)
{
    return TGdiLineSettings(false, 0, true, width, false, 0, false, 0, false, 0);
}
TGdiLineSettings GdiLineWidth(float width, float endwidth)
{
    return TGdiLineSettings(false, 0, true, width, true, endwidth, false, 0, false, 0);
}
TGdiLineSettings GdiLineColor(int color)
{
    return TGdiLineSettings(true, color, false, 0, false, 0, false, 0, false, 0);
}
TGdiLineSettings GdiLineScale(float x_scale, float y_scale)
{
    return TGdiLineSettings(false, 0, false, 0, false, 0, true, x_scale, true, y_scale);
}
TGdiLineSettings GdiLine(int color, float width)
{
    return TGdiLineSettings(true, color, true, width, false, 0, false, 0, false, 0);
}
TGdiLineSettings GdiLine(int color, float width, float endwidth)
{
    return TGdiLineSettings(true, color, true, width, true, endwidth, false, 0, false, 0);
}
TGdiLineSettings GdiLine(int color, float width, float x_scale, float y_scale)
{
    return TGdiLineSettings(true, color, true, width, false, 0, true, x_scale, true, y_scale);
}
TGdiLineSettings GdiLine(int color, float width, float endwidth, float x_scale, float y_scale)
{
    return TGdiLineSettings(true, color, true, width, true, endwidth, true, x_scale, true, y_scale);
}
TGdiFillSettings GdiFill(int color)
{
    return TGdiFillSettings(true, color);
}
/*----------------*/

TGdiSettings& TGdiSettings:: operator << (const TGdiOptions o)
{
    switch (o)
    {
    case goAntialias:
        this->antialias = (TGdiAntialias)o;
        break;
    case goAntialiasHi:
        this->antialias = (TGdiAntialias)o;
        break;
    }

    return *this;
}

TGdiSettings& TGdiSettings:: operator >> (const TGdiOptions o)
{
    switch (o)
    {
    case goAntialias:
        if (this->antialias == gaAntialias)
            this->antialias = gaNone;
        break;
    case goAntialiasHi:
        if (this->antialias == gaAntialiasHi)
            this->antialias = gaNone;
        break;
    }

    return *this;
}

TGdiSettings& TGdiSettings:: operator << (const TGdiLineSettings &line)
{
    this->line = line;
    this->line.used = true;
    return *this;
}

TGdiSettings& TGdiSettings:: operator << (const TGdiFillSettings &fill)
{
    this->fill = fill;
    this->fill.used = true;
    return *this;
}
//---------------------------------------------------------------------------

int TGdiPlus::instances = 0;
ULONG TGdiPlus::token = NULL;

TGdiPlus::TGdiPlus()
{
    instances++;
    if (instances > 1)
        return;

    GdiplusStartupInput sin;
    memset(&sin, 0, sizeof(GdiplusStartupInput));
    sin.GdiplusVersion = 1;
    if (GdiplusStartup(&token, &sin, NULL) != Ok)
        THROW(L"Couldn't create GDI+ object!");
}
TGdiPlus::~TGdiPlus()
{
    instances--;
    if (!instances)
        GdiplusShutdown(token);

    token = NULL;
}

void TGdiPlus::setup(void* graphics, const TGdiSettings &settings)
{
    using namespace GdipHelpers;
    Gdiplus::Graphics *g = (Gdiplus::Graphics*)graphics;

    if (MatchVersion(6, 0) && settings.antialias == gaAntialiasHi)
        g->SetSmoothingMode(SmoothingMode(SmoothingModeAntiAlias + 1));
    else if (settings.antialias == gaAntialias || settings.antialias == gaAntialiasHi)
        g->SetSmoothingMode(SmoothingModeAntiAlias);
}

void TGdiPlus::setuppen(void* pen, const TGdiSettings &settings)
{
    using namespace GdipHelpers;
    Gdiplus::Pen *p = (Gdiplus::Pen*)pen;

    p->SetStartCap(LineCapRound);
    p->SetEndCap(LineCapRound);

    if (settings.line.x_scale >= 0 || settings.line.y_scale >= 0)
    {
        float xs = settings.line.x_scale;
        float ys = settings.line.y_scale;
        if (xs < 0)
            xs = 1;
        if (ys < 0)
            ys = 1;
        p->ScaleTransform(xs, ys);
    }
}

void TGdiPlus::Line(TDoublePoint from, TDoublePoint to, const TGdiSettings &settings)
{
    using namespace GdipHelpers;
    Gdiplus::Graphics g(settings.dc);

    setup(&g, settings);

    if (settings.line.endwidth >= 0 || settings.line.x_scale >= 0 || settings.line.y_scale >= 0)
    {
        Gdiplus::GraphicsPath path;
        float a = 90; //start angle of first arc

        float w = settings.line.width;
        float ew = settings.line.endwidth;
        float xs = settings.line.x_scale;
        float ys = settings.line.y_scale;

        if (ew < 0)
            ew = w;
        if (xs < 0)
            xs = 1;
        if (ys < 0)
            ys = 1;

        float h = w * ys;
        float eh = ew * ys;
        w *= xs;
        ew *= xs;

        if (from.y != to.y)
        {
            if (from.x == to.x)
                a = 180;
            else
            {
                a = (180.0 / (M_PI / atan(fabs(to.y - from.y) / fabs(to.x - from.x)))) + 90;
            }
        }
        if (to.x < from.x)
            a = -a;
        if (to.y < from.y)
            a = 180 - a;

        path.AddArc(from.x - w / 2, from.y - h / 2, w, h, a, 180);
        path.AddArc(to.x - ew / 2, to.y - eh / 2, ew, eh, -180 + a, 180);
        path.CloseFigure();

        Gdiplus::SolidBrush b(gdic(settings.line.color));
        g.FillPath(&b, &path);
        return;
    }

    Gdiplus::Pen p(gdic(settings.line.color), settings.line.width);
    setuppen(&p, settings);
/*
  p.SetStartCap(LineCapRound);
  p.SetEndCap(LineCapRound);
*/
    g.DrawLine(&p, pf(from), pf(to));
}

void TGdiPlus::Bezier(TDoublePoint from, TDoublePoint from_control, TDoublePoint to_control, TDoublePoint to, const TGdiSettings &settings)
{
    using namespace GdipHelpers;
    Gdiplus::Graphics g(settings.dc);
    setup(&g, settings);

    Gdiplus::Pen p(gdic(settings.line.color), settings.line.width);
    setuppen(&p, settings);
/*
  p.SetStartCap(LineCapRound);
  p.SetEndCap(LineCapRound);
*/
    g.DrawBezier(&p, pf(from), pf(from_control), pf(to_control), pf(to));
}

void TGdiPlus::Circle(TDoublePoint center, float radius, const TGdiSettings &settings)
{
    Ellipse(center, radius, radius, settings);
}

void TGdiPlus::Ellipse(TDoublePoint center, float horz_radius, float vert_radius, const TGdiSettings &settings)
{
    using namespace GdipHelpers;
    Gdiplus::Graphics g(settings.dc);
    setup(&g, settings);

    if (!settings.line.used || settings.fill.used)
    {
        Gdiplus::SolidBrush b(gdic(settings.fill.color));
        g.FillEllipse(&b, float(center.x - horz_radius), float(center.y - vert_radius), horz_radius*2, vert_radius*2);
    }

    if (!settings.fill.used || settings.line.used)
    {
        Gdiplus::Pen p(gdic(settings.line.color), settings.line.width);
        setuppen(&p, settings);

        g.DrawEllipse(&p, float(center.x - horz_radius), float(center.y - vert_radius), horz_radius*2, vert_radius*2);
    }
}

void TGdiPlus::Square(TDoublePoint lefttop, float size, const TGdiSettings &settings)
{
    Rectangle(lefttop, size, size, settings);
}

void TGdiPlus::Rectangle(TDoublePoint lefttop, float width, float height, const TGdiSettings &settings)
{
    using namespace GdipHelpers;
    Gdiplus::Graphics g(settings.dc);
    setup(&g, settings);

    if (!settings.line.used || settings.fill.used)
    {
        Gdiplus::SolidBrush b(gdic(settings.fill.color));
        g.FillRectangle(&b, float(lefttop.x), float(lefttop.y), width, height);
    }

    if (!settings.fill.used || settings.line.used)
    {
        Gdiplus::Pen p(gdic(settings.line.color), settings.line.width);
        setuppen(&p, settings);

        g.DrawRectangle(&p, float(lefttop.x), float(lefttop.y), width, height);
    }
}

#ifdef _DEBUG
static TGlobalTest tester2(47);
#endif


