//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include "angle.h"
#include "gdipbmp.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(4);
#endif

extern TGdiPlus *gdi;

const double tinyval = 0.0001;
const double smallval = 0.001;

const double M_PI_14 = M_PI / 14.0;
const double M_PI_11 = M_PI / 11.0;
const double M_PI_11_2 = M_PI_2 - M_PI_11;
//const double M_PI_3 = M_PI / 3.0;
const double M_2PI = M_PI * 2.0;
const double M_3PI_2 = M_PI_2 * 3.0;

//---------------------------------------------------------------------------
TDoublePoint::TDoublePoint()
{
    ;
}
//---------------------------------------------------------------------------
TDoublePoint::TDoublePoint(const TDoublePoint &p)
{
    memcpy(this, &p, sizeof(TDoublePoint));
}
//---------------------------------------------------------------------------
TDoublePoint::TDoublePoint(const TPoint &p)
{
    x = p.x, y = p.y;
}
//---------------------------------------------------------------------------
TDoublePoint::TDoublePoint(double xx, double yy)
{
    x = xx, y = yy;
}
//---------------------------------------------------------------------------
TDoublePoint:: operator TPoint()
{
    return Point(x, y);
}
//---------------------------------------------------------------------------
TDoublePoint DoublePoint(double x, double y)
{
    TDoublePoint p;
    p.x = x;
    p.y = y;
    return p;
}
//---------------------------------------------------------------------------
bool operator == (TDoublePoint const &a, TDoublePoint const &b)
{
    return a.x == b.x && a.y == b.y;
}
//---------------------------------------------------------------------------
bool operator != (TDoublePoint const &a, TDoublePoint const &b)
{
    return a.x != b.x || a.y != b.y;
}
//---------------------------------------------------------------------------
TDoublePoint operator -(TDoublePoint const &a, TDoublePoint const &b)
{
    return DoublePoint(a.x - b.x, a.y - b.y);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TDoubleRect::TDoubleRect()
{
    ;
}
//---------------------------------------------------------------------------
TDoubleRect::TDoubleRect(const TDoubleRect &r)
{
    memcpy(this, &r, sizeof(TDoubleRect));
}
//---------------------------------------------------------------------------
TDoubleRect::TDoubleRect(const TDoublePoint &a, const TDoublePoint &b)
{
    Left = min(a.x, b.x), Top = min(a.y, b.y), Right = max(a.x, b.x), Bottom = max(a.y, b.y);
}
//---------------------------------------------------------------------------
TDoubleRect::TDoubleRect(double aleft, double atop, double aright, double abottom)
{
    Left = aleft, Top = atop, Right = aright, Bottom = abottom;
}
//---------------------------------------------------------------------------
double TDoubleRect::Width() const
{
    return Right - Left + 1;
}
//---------------------------------------------------------------------------
double TDoubleRect::Height() const
{
    return Bottom - Top + 1;
}
//---------------------------------------------------------------------------
TDoubleRect DoubleRect(double l, double t, double r, double b)
{
    return TDoubleRect(l, t, r, b);
}
//---------------------------------------------------------------------------
TDoubleRect TDoubleRect::Union(const TDoubleRect &r, bool checkempty) const
{
    if (checkempty)
    {
        if (r.Empty())
            return *this;
        if (Empty())
            return r;
    }

    return DoubleRect(min(r.Left, Left), min(r.Top, Top), max(r.Right, Right), max(r.Bottom, Bottom));
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TRad Rad(const TDoublePoint &a, const TDoublePoint &b)
{
    if (a == b)
        THROW(L"Cannot atan zero distance!");
    return atan2(a.y - b.y, a.x - b.x);
}
//---------------------------------------------------------------------------
TRad RadAvg(const TRad &a, const TRad &b, const double &ratio)
{
    double ad = a.data, bd = b.data;
    if (fabs(ad - bd) > M_PI)
    {
        if (ad > M_PI)
            ad -= 2 * M_PI;
        else
            bd -= 2 * M_PI;
    }
    TRad rad(ad*ratio + bd*(1 - ratio));

    return rad;
}
//---------------------------------------------------------------------------
double Len(const TDoublePoint &a, const TDoublePoint &b)
{
    return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
}
//---------------------------------------------------------------------------
TRad::TRad()
{
    data = 0;
}
//---------------------------------------------------------------------------
TRad::TRad(const TRad &r)
{
    data = r.data;
}
//---------------------------------------------------------------------------
TRad::TRad(double x)
{
    data = x;
    Normalize();
}
//---------------------------------------------------------------------------
TRad TRad::Normalize(const TRad &r) const
{
    TRad tmp = r;
    tmp.Normalize();
    return tmp;
}
//---------------------------------------------------------------------------
void TRad::Normalize()
{
    while (data < 0)
        data += 2 * M_PI;
    while (data >= 2 * M_PI)
        data -= 2 * M_PI;
}
//---------------------------------------------------------------------------
TRad TRad::Reverse()
{
    return Normalize(data - M_PI);
}
//---------------------------------------------------------------------------
bool TRad:: operator < (const TRad &r) const
{
    return fabs(data) < fabs(r.data);
}
//---------------------------------------------------------------------------
bool TRad:: operator > (const TRad &r) const
{
    return fabs(data) > fabs(r.data);
}
//---------------------------------------------------------------------------
bool TRad:: operator <= (const TRad &r) const
{
    return fabs(data) <= fabs(r.data);
}
//---------------------------------------------------------------------------
bool TRad:: operator >= (const TRad &r) const
{
    return fabs(data) >= fabs(r.data);
}
//---------------------------------------------------------------------------
TRad TRad:: operator -(const TRad &r) const
{
    TRad b = *this;
    return b -= r;
}
//---------------------------------------------------------------------------
TRad& TRad:: operator -= (const TRad &r)
{
    double b = data;
    double a = r.data;

    if (fabs(a - b) > M_PI)
    {
        if (a > M_PI)
            a -= 2 * M_PI;
        else
            b -= 2 * M_PI;
    }

    data = a - b;

    return *this;
}
//---------------------------------------------------------------------------
double TRad::Sin() const
{
    return sin(fabs(data));
}
//---------------------------------------------------------------------------
double TRad::Cos() const
{
    return cos(fabs(data));
}
//---------------------------------------------------------------------------
double TRad::Abs() const
{
    return fabs(data);
}
//---------------------------------------------------------------------------
byte TRad::Horizontal() const
{
//return data < M_PI_14 || data  > M_2PI-M_PI_14 || fabs(data-M_PI) < M_PI_14;
    if (fabs(data - M_PI) < M_PI_14)
        return 1; //rightwards
    if (data < M_PI_14 || data > M_2PI - M_PI_14)
        return 2; //leftwards

    return 0;
}
//---------------------------------------------------------------------------
byte TRad::Vertical() const
{
//return fabs(data - M_PI_2) < M_PI_14 || fabs(data - M_3PI_2)  < M_PI_14;
    if (fabs(data - M_PI_2) < M_PI_14)
        return 1; //upwards
    if (fabs(data - M_3PI_2) < M_PI_14)
        return 2; //downwards

    return 0;
}
//---------------------------------------------------------------------------
byte TRad::Diagonal() const
{
/*bool diag = data > M_PI_11 && data < M_PI_11_2 ||
              data-M_PI_2 > M_PI_11 && data-M_PI_2 < M_PI_11_2 ||
              data-M_PI > M_PI_11 && data-M_PI < M_PI_11_2 ||
              data-M_3PI_2 > M_PI_11 && data-M_3PI_2 < M_PI_11_2;
*/
    if (data > M_PI_11 && data < M_PI_11_2)
        return 1; //left and up
    if (data - M_PI_2 > M_PI_11 && data - M_PI_2 < M_PI_11_2)
        return 2; //right and up
    if (data - M_PI > M_PI_11 && data - M_PI < M_PI_11_2)
        return 3; //right and down
    if (data - M_3PI_2 > M_PI_11 && data - M_3PI_2 < M_PI_11_2)
        return 4; //left and down
    return 0;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
#define ZDOT_PIXEL_STEPS 17
double zdotwidths[17] = { 0.2, 0.31, 0.363, 0.424, 0.546, 0.62, 0.7, 0.79, 0.86, 0.98, 0.995, 1.12, 1.11, 1.1, 1.05, 0.973, 0.6 };

int zbeziersteps(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2, int bits)
{
    int dist = max((double)max(bits * 2, 10), int((zpointdistance(p1, p2) + zpointdistance(c1, c2)) / 8.0));
    if (bits > 1 && dist > bits)
    {
        int k = bits;
        while (k < dist)
            k *= 2;
        dist = k;
    }

    return dist;
}

TRect zline(HDC handle, double width, int color, TDoublePoint p1, TDoublePoint p2, TZStrokeType starttip, TZStrokeType endtip, int bits, int bit)
{
    TRect r = Rect(-1, -1, -1, -1);
    TDoublePoint pp1, pp2;

//TDoublePoint origin;

    if (bits == 1)
    {
        TGdiSettings settings = GdiSettings(handle) << goAntialias;
        if ((starttip != stNormalTip || endtip != stNormalTip) && starttip != stSingleDot)
        {
            double fullsize = width;
            double sw = starttip == stNormalTip ? fullsize : starttip == stStartThin ? max(fullsize * 0.7, 0.3) : max(fullsize * 0.3, 0.2);
            double ew = endtip == stNormalTip ? fullsize : endtip == stEndThin ? max(fullsize * 0.7, 0.3) : max(fullsize * 0.3, 0.2);

            settings << GdiLine(color, sw, ew);
        }
        else
        {
            settings << GdiLine(color, width);
        }
        gdi->Line(p1, p2, settings);
/*c->MoveTo(round(p1.x),round(p1.y));
    c->LineTo(round(p2.x),round(p2.y));
*/
        pp1 = p1;
        pp2 = p2;
    }
    else
    {
        bit = MAX(0, MIN(bits, bit));
        double dx = (p2.x - p1.x) / (double)bits;
        double dy = (p2.y - p1.y) / (double)bits;

        pp1 = DoublePoint(p1.x + dx * bit, p1.y + dy * bit);
        pp2 = DoublePoint(dx < 0 ? MAX(p1.x + dx * (bit + 1), p2.x) : MIN(p1.x + dx * (bit + 1), p2.x), dy < 0 ? MAX(p1.y + dy * (bit + 1), p2.y) : MIN(p1.y + dy * (bit + 1), p2.y));

        TGdiSettings settings = GdiSettings(handle) << goAntialias;
        if (starttip != stNormalTip || endtip != stNormalTip)
        {
//double wd = c->Pen->Width-0.5;
//double bd = 1.0/bits;
            double fullsize = width;
            double sw = starttip == stNormalTip ? fullsize : starttip == stStartThin ? max(fullsize * 0.7, 0.3) : max(fullsize * 0.3, 0.2);
            double ew = endtip == stNormalTip ? fullsize : endtip == stEndThin ? max(fullsize * 0.7, 0.3) : max(fullsize * 0.3, 0.2);

            double wd = sw - ew;
            double bd = 1.0 / bits;

            settings << GdiLine(color, sw - wd * bd * bit, sw - wd * bd * (bit + 1));
        }
        else
        {
            settings << GdiLine(color, width);
        }
        gdi->Line(pp1, pp2, settings);
//c->MoveTo(round(pp1.x),round(pp1.y));
//c->LineTo(round(pp2.x),round(pp2.y));

    }

    r = Rect(round(MIN(pp1.x, pp2.x)), round(MIN(pp1.y, pp2.y)), round(MAX(pp1.x, pp2.x)), round(MAX(pp1.y, pp2.y)));
    InflateRect(&r, width / 2, width / 2);
    return r;
}
//---------------------------------------------------------------------------
TRect zbezier(HDC handle, double width, int color, TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2, TZStrokeType starttip, TZStrokeType endtip, int bits, int bit)
{
    if (bits == 1 && starttip == stNormalTip && endtip == stNormalTip)
    {
        TGdiSettings settings = GdiSettings(handle) << goAntialias << GdiLine(color, width);

        gdi->Bezier(p1, c1, c2, p2, settings);

        TRect tmp = zbezierdimensions(p1, c1, c2, p2).rect();
        InflateRect(&tmp, width / 2, width / 2);
//c->MoveTo(p2.x,p2.y);
        return tmp;
    }

//c->MoveTo(round(p1.x),round(p1.y));

    TDoublePoint pp1, pp2, pp3;
    TRect r = Rect(-1, -1, -1, -1);

    TDoublePoint old = p1;

    int steps = zbeziersteps(p1, c1, c2, p2, bits);

    int firststep = 0;
    int laststep = steps - 1;

    if (bits > 1)
    {
        bit = MAX(0, MIN(bits - 1, bit));
        double st = ((double)steps / (double)bits);

        firststep = round((double)bit * st);
        laststep = round((double)(bit + 1) * st) - 1;
    }

    double fullsize = width;
    double sw = starttip == stNormalTip ? fullsize : starttip == stStartThin ? max(fullsize * 0.7, 0.3) : max(fullsize * 0.3, 0.2);
    double ew = endtip == stNormalTip ? fullsize : endtip == stEndThin ? max(fullsize * 0.7, 0.3) : max(fullsize * 0.3, 0.2);

    float w = sw;
    double wdec = (starttip == stNormalTip && endtip == stNormalTip) || steps <= 1 ? 0 : (sw - ew) / (steps - 1);
    w -= (firststep*wdec);

    for (int i = firststep - 1; i <= laststep; i++)
    {
        double s = ((double)i + 1) / (double)steps;

        pp3.x = pow(1.0 - s, 3) * p1.x + 3 * s * pow(1 - s, 2) * c1.x + 3 * pow(s, 2) * (1 - s) * c2.x + pow(s, 3) * p2.x;
        pp3.y = pow(1.0 - s, 3) * p1.y + 3 * s * pow(1 - s, 2) * c1.y + 3 * pow(s, 2) * (1 - s) * c2.y + pow(s, 3) * p2.y;

        if (round(old.x) != round(pp3.x) || round(old.y) != round(pp3.y))
        {
            if (firststep == i + 1)
            {
//c->MoveTo(round(pp3.x),round(pp3.y));
            }
            else
            {
//c->LineTo(round(pp3.x),round(pp3.y));
                TGdiSettings settings(handle);
                settings << goAntialias;
                if (starttip == stNormalTip && endtip == stNormalTip)
                {
                    settings << GdiLine(color, width);
                }
                else if (starttip == stSingleDot)
                {
                    double zdotindex1 = ((double)i / steps) * (ZDOT_PIXEL_STEPS - 1);
                    double zdotpos1 = fpart(zdotindex1);
                    double size1 = fullsize * (zdotwidths[intfloor(zdotindex1)] * (1 - zdotpos1)) + fullsize * (zdotwidths[intceil(zdotindex1)] * zdotpos1);

                    double zdotindex2 = ((double)(i + 1) / steps) * (ZDOT_PIXEL_STEPS - 1);
                    double zdotpos2 = fpart(zdotindex2);
                    double size2 = fullsize * (zdotwidths[intfloor(zdotindex2)] * (1 - zdotpos2)) + fullsize * (zdotwidths[intceil(zdotindex2)] * zdotpos2);

                    settings << GdiLine(color, size1, size2);
                }
                else
                {
                    settings << GdiLine(color, w, w - wdec);
                }

                gdi->Line(old, pp3, settings);
            }
        }

        if (firststep <= i)
        {
            TRect tmp = Rect(round(MIN(old.x, pp3.x)), round(MIN(old.y, pp3.y)), round(MAX(old.x, pp3.x)), round(MAX(old.y, pp3.y)));
            InflateRect(&tmp, width / 2, width / 2);
            if (r == Rect(-1, -1, -1, -1))
                r = tmp;
            else
                r = Rect(MIN(r.Left, tmp.Left), MIN(r.Top, tmp.Top), MAX(r.Right, tmp.Right), MAX(r.Bottom, tmp.Bottom));
        }

        old = pp3;

        w -= wdec;
    }

    return r;
}
//---------------------------------------------------------------------------
double zbezierlen(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2)
{
    TDoublePoint pp1, pp2, pp3;

    TDoublePoint old = p1;
    double len = 0;

    int steps = zbeziersteps(p1, c1, c2, p2);

    for (int i = 0; i < steps; i++)
    {
        double s = ((double)i + 1) / (double)steps;

        pp3.x = pow(1.0 - s, 3) * p1.x + 3 * s * pow(1 - s, 2) * c1.x + 3 * pow(s, 2) * (1 - s) * c2.x + pow(s, 3) * p2.x;
        pp3.y = pow(1.0 - s, 3) * p1.y + 3 * s * pow(1 - s, 2) * c1.y + 3 * pow(s, 2) * (1 - s) * c2.y + pow(s, 3) * p2.y;

        len += zpointdistance(pp3, old);
        old = pp3;
    }

    return len;
}
//---------------------------------------------------------------------------
TDoubleRect zbezierdimensions(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2)
{
    TDoublePoint pp1, pp2, pp3;
    TDoublePoint old = p1;

    TDoubleRect r = DoubleRect(99999, 99999, 0, 0);

    int steps = zbeziersteps(p1, c1, c2, p2);

    for (int i = 0; i < steps; i++)
    {
        double s = ((double)i + 1) / (double)steps;

        pp3.x = pow(1.0 - s, 3) * p1.x + 3 * s * pow(1 - s, 2) * c1.x + 3 * pow(s, 2) * (1 - s) * c2.x + pow(s, 3) * p2.x;
        pp3.y = pow(1.0 - s, 3) * p1.y + 3 * s * pow(1 - s, 2) * c1.y + 3 * pow(s, 2) * (1 - s) * c2.y + pow(s, 3) * p2.y;

        r.Left = round(MIN(old.x, pp3.x));
        r.Top = round(MIN(old.y, pp3.y));
        r.Right = round(MAX(old.x, pp3.x));
        r.Bottom = round(MAX(old.y, pp3.y));

        old = pp3;
    }

    return r;
}
//---------------------------------------------------------------------------
double zpointdistance(TDoublePoint p1, TDoublePoint p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}
//---------------------------------------------------------------------------
double zlinedistance(TDoublePoint l1, TDoublePoint l2, TDoublePoint p, bool endpoints)
{
    double dd, pdd, u1, u2;

    dd = l2.x == l1.x ? INFINITE : (l2.y - l1.y) / (l2.x - l1.x);

    if (dd != INFINITE && dd != 0)
    {
        pdd = -1.0 / dd;

        u1 = p.y + pdd * (l1.x - p.x);
        u2 = p.y + pdd * (l2.x - p.x);
    }

    if (dd == INFINITE && (p.y < MIN(l1.y, l2.y) || p.y > MAX(l1.y, l2.y)) || dd == 0 && (p.x < MIN(l1.x, l2.x) || p.x > MAX(l1.x, l2.x)) || dd != INFINITE && dd != 0 && (u1 < l1.y && u2 < l2.y || u1 > l1.y && u2 > l2.y))
    {
        if (!endpoints)
            return 99999;

        double di1 = zpointdistance(l1, p);
        double di2 = zpointdistance(l2, p);
        return MIN(di1, di2);
    }

    return fabs((l2.x - l1.x) * (l1.y - p.y) - (l1.x - p.x) * (l2.y - l1.y)) / sqrt(pow(l2.x - l1.x, 2) + pow(l2.y - l1.y, 2));
}
//---------------------------------------------------------------------------
double zgenericlinedistance(TDoublePoint l1, TDoublePoint l2, TDoublePoint p)
{
    if (l1 == l2)
        return zpointdistance(l1, p);
    return fabs((l2.x - l1.x) * (l1.y - p.y) - (l1.x - p.x) * (l2.y - l1.y)) / sqrt(pow(l2.x - l1.x, 2) + pow(l2.y - l1.y, 2));
}
//---------------------------------------------------------------------------
bool zgenericlineside(TDoublePoint l1, TDoublePoint l2, TDoublePoint p)
{
    if (fabs(l2.x - l1.x) >= fabs(l2.y - l1.y))
    {
        double midy = l1.y + ((p.x - l1.x) / (l2.x - l1.x)) * (l2.y - l1.y);
        if ((midy > p.y) == (l1.x < l2.x))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        double midx = l1.x + ((p.y - l1.y) / (l2.y - l1.y)) * (l2.x - l1.x);
        if ((midx > p.x) == (l1.y < l2.y))
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}
//---------------------------------------------------------------------------
TDoublePoint zlineintersect(TDoublePoint l1, TDoublePoint l2, TDoublePoint p)
{
    if (l1 == l2)
        return l1;

    double a = zgenericlinedistance(l1, l2, p/*,false,false*/);

    if (!a)
        return p;

    double d = zpointdistance(l1, p);
    double e = zpointdistance(l2, p);

    double len = zpointdistance(l1, l2);

    if (d > e)
    {
        double b = sqrt(pow(d, 2) - pow(a, 2));
        double per = b / len;
        return DoublePoint(l1.x + per*(l2.x - l1.x), l1.y + per*(l2.y - l1.y));
    }
    else
    {
        double b = sqrt(pow(e, 2) - pow(a, 2));
        double per = b / len;
        return DoublePoint(l2.x + per*(l1.x - l2.x), l2.y + per*(l1.y - l2.y));
    }
}
//---------------------------------------------------------------------------
int zlineintersect(TDoublePoint l1, TDoublePoint l2, TDoublePoint p1, TDoublePoint p2, TDoublePoint &result)
{
    double ml = fabs(l1.x - l2.x) < 0.000001 ? INFINITE : (l2.y - l1.y) / (l2.x - l1.x);
    double mp = fabs(p1.x - p2.x) < 0.000001 ? INFINITE : (p2.y - p1.y) / (p2.x - p1.x);

    if (fabs(ml - mp) < 0.0001)
    {
        if (zgenericlinedistance(l1, l2, p1) < 0.0001)
        {
            if ((min(l1.x, l2.x) - 0.0001 <= p1.x && max(l1.x, l2.x) + 0.0001 >= p1.x && min(l1.y, l2.y) - 0.0001 <= p1.y && max(l1.y, l2.y) + 0.0001 >= p1.y) || (min(p1.x, p2.x) - 0.0001 <= l1.x && max(p1.x, p2.x) + 0.0001 >= l1.x && min(p1.y, p2.y) - 0.0001 <= l1.y && max(p1.y, p2.y) + 0.0001 >= l1.y))
            {
                result = DoublePoint(((p1.x + p2.x) / 2 + (l1.x + l2.x) / 2) / 2, ((p1.y + p2.y) / 2 + (l1.y + l2.y) / 2) / 2);
                return 2; //lines on each other
            }
            else
            {
                result = DoublePoint(99999, 99999);
                return 3; //lines on each other but sections are not
            }
        }
        result = DoublePoint(99999, 99999);
        return -1; //lines are close to parallel and never intersect
    }

    if (ml == INFINITE)
        result = DoublePoint(l1.x, p1.y + (l1.x - p1.x) * mp);
    else if (ml == 0 && mp != INFINITE)
        result = DoublePoint(p1.x + (l1.y - p1.y) / mp, l1.y);
    else if (mp == INFINITE)
        result = DoublePoint(p1.x, l1.y + (p1.x - l1.x) * ml);
    else if (mp == 0)
        result = DoublePoint(l1.x + (p1.y - l1.y) / ml, p1.y);
    else
    {
//result = DoublePoint((l1.y-p1.y+p1.x*mp-l1.x*ml)/(mp-ml),(l1.x-p1.x+p1.y/mp-l1.y/ml)/(1/mp-1/ml));
        double e = l2.x - l1.x, f = p1.x - p2.x, g = l2.y - l1.y, h = p1.y - p2.y, p = p1.x - l1.x, q = p1.y - l1.y;
        double det = e * h - f * g;
        double t = (h * p - f * q) / det/*, u = (e*q-p*g)/det*/;
        result = DoublePoint(l1.x + t * e, l1.y + t * g);
/*if (result != DoublePoint(p1.x-u*f,p1.y-u*h))
      THROW(L"mathematician error.";*/
    }

    return (result.x >= min(l1.x, l2.x) - 0.0001 && result.x <= max(l1.x, l2.x) + 0.0001 && result.x >= min(p1.x, p2.x) - 0.0001 && result.x <= max(p1.x, p2.x) + 0.0001 && result.y >= min(l1.y, l2.y) - 0.0001 && result.y <= max(l1.y, l2.y) + 0.0001 && result.y >= min(p1.y, p2.y) - 0.0001 && result.y <= max(p1.y, p2.y) + 0.0001) ? 1 : 0;
}
//---------------------------------------------------------------------------
double zbezierdistance(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2, TDoublePoint p, double &where, bool endpoints)
{
    TDoublePoint next, old = p1;

    where = 0;
    double len = 0;
    double d = 99999;

    int steps = zbeziersteps(p1, c1, c2, p2) * 10;

    for (int i = 0; i < steps; i++)
    {
        double s = ((double)i + 1) / (double)steps;

        next.x = pow(1.0 - s, 3) * p1.x + 3 * s * pow(1 - s, 2) * c1.x + 3 * pow(s, 2) * (1 - s) * c2.x + pow(s, 3) * p2.x;
        next.y = pow(1.0 - s, 3) * p1.y + 3 * s * pow(1 - s, 2) * c1.y + 3 * pow(s, 2) * (1 - s) * c2.y + pow(s, 3) * p2.y;

        double zd = zlinedistance(old, next, p, false);
        if (zd < d)
        {
            d = zd;
            where = (double)i / steps;
        }
        len += zpointdistance(next, old);
        old = next;
    }

    if (endpoints)
    {
        double di = MIN(zpointdistance(p1, p), zpointdistance(p2, p));
        if (di < d)
            d = di;
    }

    return d;
}
//---------------------------------------------------------------------------
double zpointposition(TDoublePoint l1, TDoublePoint l2, TDoublePoint p, double *distance)
{
    double where;

    double zd = distance ? *distance : zlinedistance(l1, l2, p, false);
    if (zd == 99999)
        THROW(L"Point too far from line! No position can be determined!");

    if (l1.x == l2.x || l1.y == l2.y)
        where = zpointdistance(l1, DoublePoint(l1.x == l2.x ? l1.x : p.x, l1.y == l2.y ? l1.y : p.y));
    else
        where = sqrt(pow(zpointdistance(l1, p), 2) - pow(zd, 2));

    return where;
}
//---------------------------------------------------------------------------
void zbeziersplit(TDoublePoint p1, TDoublePoint &c1, TDoublePoint &c2, TDoublePoint p2, TDoublePoint &newpoint, TDoublePoint &newc1, TDoublePoint &newc2, double where)
{
    double cx, cy;

    cx = c1.x + where * (c2.x - c1.x);
    cy = c1.y + where * (c2.y - c1.y);
    c1.x = p1.x + where * (c1.x - p1.x);
    c1.y = p1.y + where * (c1.y - p1.y);
    c2.x = c2.x + where * (p2.x - c2.x);
    c2.y = c2.y + where * (p2.y - c2.y);
    newc1.x = c1.x + where * (cx - c1.x);
    newc1.y = c1.y + where * (cy - c1.y);
    newc2.x = cx + where * (c2.x - cx);
    newc2.y = cy + where * (c2.y - cy);
    newpoint.x = newc1.x + where * (newc2.x - newc1.x);
    newpoint.y = newc1.y + where * (newc2.y - newc1.y);

}
//---------------------------------------------------------------------------
TDoublePoint zpointaverage(const TDoublePoint &a, const TDoublePoint &b)
{
    return TDoublePoint((a.x + b.x) / 2, (a.y + b.y) / 2);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(5);
#endif


