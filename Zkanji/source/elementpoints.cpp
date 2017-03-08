//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>
#include "elementpoints.h"
#include "kanjielement.h"
#include "recobj.h"
#include "bits.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(36);
#endif

//---------------------------------------------------------------------------
TPointListEx::TPointListEx() : frect(0, 0, -1, -1), flen(-1), lengths(NULL), rads(NULL), sections(NULL), radsum(NULL), sectrads(NULL)
{

}
//---------------------------------------------------------------------------
TPointListEx::TPointListEx(TPointListEx *copy) : frect(0, 0, -1, -1), flen(-1), lengths(NULL), rads(NULL), sections(NULL), radsum(NULL), sectrads(NULL)
{
    assign(copy);
}
//---------------------------------------------------------------------------
TPointListEx::~TPointListEx()
{
    Clear();
}
//---------------------------------------------------------------------------
void TPointListEx::Clear()
{
    reset();
    inherited::Clear();
}
//---------------------------------------------------------------------------
int TPointListEx::add(TDoublePoint *p)
{
    return add(*p);
}
//---------------------------------------------------------------------------
int TPointListEx::add(TDoublePoint const & p)
{
    reset(false);

    if (frect.Left > frect.Right && !fnear(frect.Left, frect.Right) || frect.Top > frect.Bottom && !fnear(frect.Top, frect.Bottom))
        frect = DoubleRect(p.x, p.y, p.x, p.y);
    else
        frect = frect.Union(DoubleRect(p.x, p.y, p.x, p.y), false);

    return Add(p);
}
//---------------------------------------------------------------------------
void TPointListEx::del(int ix)
{
    bool resetrect = Items[ix]->x - tinyval <= frect.Left || Items[ix]->x + tinyval > frect.Right || Items[ix]->y - tinyval <= frect.Top || Items[ix]->y + tinyval > frect.Bottom;

    reset(resetrect);
    if (resetrect)
    {
        frect = DoubleRect(Items[0]->x, Items[0]->y, Items[0]->x, Items[0]->y);
        for (int ix = 1; ix < Count; ++ix)
            frect = frect.Union(DoubleRect(Items[ix]->x, Items[ix]->y, Items[ix]->x, Items[ix]->y), false);
    }

    Delete(ix);
}
//---------------------------------------------------------------------------
void TPointListEx::reset(bool resetrect)
{
    delete[] lengths;
    lengths = 0;
    delete[] rads;
    rads = 0;
    delete radsum;
    radsum = 0;
    delete[] sectrads;
    sectrads = 0;
    delete[] sections;
    sections = 0;
    fsectcnt = 0;
    flen = -1;
    if (resetrect)
        frect = DoubleRect(0, 0, -1, -1);
}
//---------------------------------------------------------------------------
void TPointListEx::draw(TCanvas *c, TRect const & r)
{

    double mulx = (double)max(1, r.Width() * 0.9) / max(1, frect.Width());
    double muly = (double)max(1, r.Height() * 0.9) / max(1, frect.Height());
    double mul = min(mulx, muly);

    int posx = (r.Width() * 0.9 - (frect.Width() * mul)) / 2 + r.Width() * 0.05;
    int posy = (r.Height() * 0.9 - (frect.Height() * mul)) / 2 + r.Height() * 0.05;

    c->MoveTo(r.Left + (Items[0]->x - frect.Left)*mul + posx, r.Top + (Items[0]->y - frect.Top)*mul + posy);
    for (int ix = 1; ix < Count; ++ix)
        c->LineTo(r.Left + (Items[ix]->x - frect.Left)*mul + posx, r.Top + (Items[ix]->y - frect.Top)*mul + posy);
}
//---------------------------------------------------------------------------
TRect TPointListEx::get_dim()
{
  //return frect.rect();
    TRect r = frect.rect();
    if (r.Left == r.Right)
        r.Right++;
    if (r.Top == r.Bottom)
        r.Bottom++;

    return r;
}
//---------------------------------------------------------------------------
double TPointListEx::get_lens(int ix)
{
    if (!lengths)
    {
        lengths = new double[Count - 1];
        for (int ix = 0; ix < Count - 1; ++ix)
            lengths[ix] = -1;
    }
    if (lengths[ix] < 0)
        lengths[ix] = Len(*Items[ix], *Items[ix + 1]);

    return lengths[ix];
}
//---------------------------------------------------------------------------
TRad TPointListEx::get_rad(int ix)
{
    if (!rads)
    {
        rads = new TRad[Count - 1];
        for (int ix = 0; ix < Count - 1; ++ix)
            rads[ix] = Rad(*Items[ix], *Items[ix + 1]);
    }

    return rads[ix];
}
//---------------------------------------------------------------------------
TRad TPointListEx::get_radsum()
{
    if (!radsum)
    {
        radsum = new TRad;
        if (Count > 2 && *Items[0] != *Items[Count - 1] || Count == 2 && *Items[0] != *Items[1])
            * radsum = Rad(*Items[0], *Items[Count - 1]);
        else
            *radsum = 0;
    }

    return *radsum;
}
//---------------------------------------------------------------------------
double TPointListEx::get_len()
{
    if (flen >= 0)
        return flen;

    flen = 0;
    for (int ix = 0; ix < Count - 1; ++ix)
        flen += Lengths[ix];

    return flen;
}
//---------------------------------------------------------------------------
void TPointListEx::generate_sections()
{
    sections = new byte[Count - 1];
    fsectcnt = 1;
    for (int ix = 0; ix < Count - 1; ++ix)
    {
        if (ix && (Radian[ix] - Radian[ix - 1]).Abs() > M_PI_2*0.8)
            fsectcnt++;
        sections[ix] = fsectcnt - 1;
    }
}
//---------------------------------------------------------------------------
int TPointListEx::get_sect(int ix)
{
    if (!sections)
        generate_sections();

    return sections[ix];
}
//---------------------------------------------------------------------------
int TPointListEx::get_sectcnt()
{
    if (!sections)
        generate_sections();
    return fsectcnt;
}
//---------------------------------------------------------------------------
int TPointListEx::get_sectsect(int ix)
{
    if (!sections)
        generate_sections();

    if (ix < 0 || ix >= fsectcnt)
        THROW(L"Section index out of range in section section!");

    int res = 0;
    for (int j = 0; j < Count - 1; ++j)
    {
        if (sections[j] == ix)
            res++;
        else if (sections[j] > ix)
            break;
    }

    return res;
}
//---------------------------------------------------------------------------
double TPointListEx::get_sectlen(int six)
{
    if (!sections)
        generate_sections();

    if (six < 0 || six >= fsectcnt)
        THROW(L"Section out of range");

    for (int ix = 0; ix < Count - 1; ++ix)
        if (Sections[ix] == six)
        {
            double l = 0;
            while (ix < Count - 1 && Sections[ix] == six)
                l += Lengths[ix++];
            return l;
        }

    return -1;
}
//---------------------------------------------------------------------------
TRad TPointListEx::get_sectrad(int six)
{
    if (!sections)
        generate_sections();

    if (six < 0 || six >= fsectcnt)
        THROW(L"Section out of range");

    if (!sectrads)
    {
        sectrads = new TRad[SectionCount];
        int sect = 0;
        int ix = 0;
        TPoint startp = *Items[0], endp;
        while (ix < Count)
        {
            if (ix == Count - 1 || Sections[ix] != sect)
            {
                endp = *Items[ix];
                sectrads[sect] = Rad(startp, endp);
                sect++;
                startp = endp;
            }
            ix++;
        }
    }

    return sectrads[six];
}
//---------------------------------------------------------------------------
void TPointListEx::splitpoints(TDoublePoint *pt, int cnt)
{
    if (!cnt)
        return;
    pt[0] = *Items[0];
    if (cnt == 1)
        return;

    double sect = Length / (cnt - 1);
    double current = sect;
    int pos = 1;
    int apos = 0;

    double l;
    while (apos < Count - 1 && pos < cnt - 1)
    {
        l = Lengths[apos];

        TDoublePoint dif = (*Items[apos + 1]) - (*Items[apos]);
        while (current <= l)
        {
            double mul = current / l;
            pt[pos] = DoublePoint(Items[apos]->x + dif.x * mul, Items[apos]->y + dif.y * mul);
            pos++;
            current += sect;
        }

        current -= l;
        apos++;
    }

/*while (apos < Count-1) {
    double l = Lengths[apos];
    if (l == current) {
      while ((double)pos/cnt < (double)apos/Count) {
        pt[pos] = pt[pos-1];
        pos++;
      }
    } else {
      TDoublePoint dif = (*Items[apos+1])-(*Items[apos]);
      while (current < l) {
        double mul = current / l;
        pt[pos] = Point(Items[apos]->x+dif.x*mul,Items[apos]->y+dif.y*mul);
        pos++;
        current += sect;
      }
    }
    current -= l;
    apos++;
  }
*/
    while (pos < cnt - 1)
    {
        pt[pos] = pt[pos - 1];
        pos++;
    }
    pt[cnt - 1] = *Items[Count - 1];
}
//---------------------------------------------------------------------------
void TPointListEx::split(TPointListEx *a, TPointListEx *b, double pos)
{
    double sectlen = Length * pos;
    TDoublePoint last = *Items[0];
    int alast = 1;
    double l = 0;

    do
    {
        double sl = Lengths[alast - 1];

        if (l + sl > sectlen)
        {
            double mul = (sectlen - l) / sl;

            last = DoublePoint(last.x + (double)(Items[alast]->x - last.x) * mul, last.y + (double)(Items[alast]->y - last.y) * mul);

            a->Clear();
            b->Clear();

            for (int ix = 0; ix < alast; ++ix)
                a->add(*Items[ix]);
            a->add(last);
            b->add(last);
            for (int ix = alast; ix < Count; ++ix)
                b->add(*Items[ix]);

            break;
        }

        last = *Items[alast];
        l += sl;
        if (alast == Count - 1)
            THROW(L"We have ran out of points!");
        alast++;
    }
    while (true);
}
//---------------------------------------------------------------------------
void TPointListEx::assign(TDoublePoint *pt, int cnt)
{
    Clear();
    while (Count < cnt)
        add(pt[Count]);
}
//---------------------------------------------------------------------------
void TPointListEx::assign(TPointListEx *list)
{
    Clear();
    while (Count < list->Count)
        add(*list->Items[Count]);

    if (list->flen >= 0)
        flen = list->flen;

    if (list->lengths)
    {
        lengths = new double[Count - 1];
        memcpy(lengths, list->lengths, sizeof(double)*(Count - 1));
    }
}
//---------------------------------------------------------------------------
void TPointListEx::offset(double deltax, double deltay)
{
    if (!Count)
        return;
    for (int ix = 0; ix < Count; ++ix)
    {
        Items[ix]->x += deltax;
        Items[ix]->y += deltay;
    }
    frect = frect.Offset(deltax, deltay);
}
//---------------------------------------------------------------------------
void TPointListEx::resize(double newwidth, double newheight)
{
    resize(Rect(0, 0, newwidth - 1, newheight - 1));
}
//---------------------------------------------------------------------------
void TPointListEx::resize(const TDoubleRect &r)
{
    double xmul = max(1.0, r.Width()) / max(1.0, frect.Width());
    double ymul = max(1.0, r.Height()) / max(1.0, frect.Height());
    double mul = min(xmul, ymul);
    if (flen >= 0)
        flen *= mul;
    double xdif = frect.Left;
    double ydif = frect.Top;
    frect = Rect(0, 0, -1, -1);
    for (int ix = 0; ix < Count; ++ix)
    {
        Items[ix]->x -= xdif;
        Items[ix]->y -= ydif;
        Items[ix]->x *= mul;
        Items[ix]->y *= mul;
        Items[ix]->x += r.Left;
        Items[ix]->y += r.Top;

        if (frect.Left > frect.Right && !fnear(frect.Left, frect.Right) || frect.Top > frect.Bottom && !fnear(frect.Top, frect.Bottom))
            frect = DoubleRect(Items[ix]->x, Items[ix]->y, Items[ix]->x, Items[ix]->y);
        else
            frect = frect.Union(DoubleRect(Items[ix]->x, Items[ix]->y, Items[ix]->x + 1, Items[ix]->y + 1), false);

        if (ix < Count - 1 && lengths && lengths[ix] > 0)
            lengths[ix] *= mul;
    }
}
//---------------------------------------------------------------------------
void TPointListEx::dissect(TDoublePoint *p, int cnt, TIntegerList *l)
{
    const int divver = 201;

    const double lenmul = 0.037 / ((double)cnt / divver);
    const double summul = 0.8 * ((double)cnt / divver);
    const double segmul = 0.4;
    const double steepness = 1.46;

    splitpoints(p, cnt);
    double seg = Length / (cnt - 1);
    double dx = p[1].x - p[0].x;
    double dy = p[1].y - p[0].y;
    double dsum = 0;
    double len = seg;
    int last = -1;

    for (int ix = 2; ix < cnt; ++ix)
    {
        double nx = p[ix].x - p[ix - 1].x;
        double ny = p[ix].y - p[ix - 1].y;

        double di = fabs(dx - nx) + fabs(dy - ny);

        if (last == -1 && di > seg * steepness)
        {
            last = ix - 1;
            dsum = di;
        }
        else if (last >= 0 && dsum > len * lenmul)
        {
            l->Add(last);
            dx = nx;
            dy = ny;
            len = 0;
            last = -1;
            dsum = 0;
        }
        else if (last >= 0)
            dsum = dsum * summul + di;

        len += seg;
        if (last >= 0 && dsum < (ix - last - 1) * seg * segmul)
            last = -1;
    }
}
//---------------------------------------------------------------------------
double TPointListEx::compare(TPointListEx *list)
{
    double res = 0;

    int n = list->Count - 1;
    int m = Count - 1;

    int nlen = list->Length / 3;
    int mlen = Length / 3;

    int steps = 1;

    int ns = -1, ms = -1;

    int mw = n, mh = m;
    int mmw = 0, mmh = 0;

    double wextra = 0;

    double *matrix = NULL;
    double *minimatrix = NULL;
    try
    {
        enum TWhichSection { wsNone, wsNStart, wsNEnd, wsMStart, wsMEnd, wsNMStart, wsNMEnd };
        TWhichSection ws = wsNone;
        double wval = 1;

        {
            double ntmp1 = list->SectionLength[0] / (nlen * 3);
            double ntmp2 = list->SectionLength[list->SectionCount - 1] / (nlen * 3);
            double mtmp1 = SectionLength[0] / (mlen * 3);
            double mtmp2 = SectionLength[SectionCount - 1] / (mlen * 3);

            if (list->SectionCount > 1 && list->SectionLength[0] < min(2000, nlen * 0.6) && list->SectionLength[1] > nlen * 0.6 && ntmp1 < ntmp2)
            {
                ws = wsNStart;
                wval = ntmp1;

                wextra = nlen * 0.3;

                mmw = list->SectionSections[0];
                mmh = SectionSections[0];
                mw = n - mmw;
            }
            else if (list->SectionCount > 1 && list->SectionLength[list->SectionCount - 1] < min(2000, nlen * 0.6) && list->SectionLength[list->SectionCount - 2] > nlen * 0.6 && ntmp2 < ntmp1)
            {
                ws = wsNEnd;
                wval = ntmp2;

                wextra = nlen * 0.3;

                mmw = list->SectionSections[list->SectionCount - 1];
                mmh = SectionSections[SectionCount - 1];
                mw = n - mmw;
            }

            if (SectionCount > 1 && SectionLength[0] < min(2000, mlen * 0.6) && SectionLength[1] > mlen * 0.6 && mtmp1 < mtmp2)
            {
                if (ws == wsNone)
                {
                    ws = wsMStart;
                    wextra = mlen * 0.3;
                }
                else if (ws == wsNStart)
                {
                    ws = wsNMStart;
                    wextra = min(wextra, mlen * 0.3);
                }
                else
                {
                    ws = wsNone;
                    mw = n;
                    mh = m;
                }

                wval = min(wval, mtmp1);

                if (ws != wsNone)
                {
                    mmw = list->SectionSections[0];
                    mmh = SectionSections[0];
                    mh = m - mmh;
                }
            }
            else if (SectionCount > 1 && SectionLength[SectionCount - 1] < min(2000, mlen * 0.6) && SectionLength[SectionCount - 2] > mlen * 0.6 && mtmp2 < mtmp1)
            {
                if (ws == wsNone)
                {
                    ws = wsMEnd;
                    wextra = mlen * 0.3;
                }
                else if (ws == wsNEnd)
                {
                    ws = wsNMEnd;
                    wextra = min(wextra, mlen * 0.3);
                }
                else
                {
                    ws = wsNone;
                    mw = n;
                    mh = m;
                }

                wval = min(wval, mtmp2);

                if (ws != wsNone)
                {
                    mmw = list->SectionSections[list->SectionCount - 1];
                    mmh = SectionSections[SectionCount - 1];
                    mh = m - mmh;
                }
            }

            if (ws != wsNone)
            {
                try
                {
                    int mmx = (ws == wsNEnd || ws == wsMEnd || ws == wsNMEnd) ? n - mmw : 0;
                    int mmy = (ws == wsNEnd || ws == wsMEnd || ws == wsNMEnd) ? m - mmh : 0;

                    minimatrix = new double[mmw * mmh];
                    for (int ix = 0; ix < mmw * mmh; ++ix)
                    {
                        TRad mnsrad = list->Radian[ix % mmw + mmx];
                        TRad mmsrad = Radian[ix / mmw + mmy];

                        double diff = (mnsrad - mmsrad).Abs() * 800
        /**wval*/;

                        int mmxpos = (ix % mmw);
                        int mmypos = (ix / mmw);
                        int mmix = mmypos * mmw + mmxpos;

                        minimatrix[mmix] = diff;
                        if (mmxpos && mmypos)
                            minimatrix[mmix] += min(min(minimatrix[mmix - 1], minimatrix[mmix - mmw]), minimatrix[mmix - mmw - 1]);
                        else if (!mmxpos && mmypos)
                            minimatrix[mmix] += minimatrix[mmix - mmw];
                        else if (mmix)
                            minimatrix[mmix] += minimatrix[mmix - 1];

                    }

          //backtracking
                    int msteps = 1;
                    int x = mmw - 1, y = mmh - 1;
                    while (x > 0 || y > 0)
                    {
                        msteps++;
                        if (x && y)
                        {
                            if (minimatrix[(x - 1) + (y - 1)*mmw] < minimatrix[x + (y - 1)*mmw] && minimatrix[(x - 1) + (y - 1)*mmw] < minimatrix[(x - 1) + y*mmw])
                                x-- , y--;
                            else if (minimatrix[x + (y - 1)*mmw] < minimatrix[(x - 1) + y*mmw])
                                y--;
                            else
                                x--;
                        }
                        else if (x)
                            x--;
                        else
                            y--;
                    }

//wextra = minimatrix[mmw*mmh-1];
//wextra = (wextra/double(msteps))*3.0;

//if (wextra < 1000) {
                    if ((minimatrix[mmw * mmh - 1] / double(msteps)) < 333.333)
                    {
                        wextra = 0;
                        ws = wsNone;
                        mw = n;
                        mh = m;
                    }

                }
                catch (...)
                {
                    ;
                }
                delete[] minimatrix;
            }
            else
                wextra = 0;

        }

        matrix = new double[mw * mh];

        for (int ix = 0; ix < n * m; ++ix)
        {
            if (ns != list->Sections[ix % n])
                ns = list->Sections[ix % n];
            if (ms != Sections[ix / n])
                ms = Sections[ix / n];

      //skip extra
            if (!ns && (ws == wsNStart || ws == wsNMStart) || !ms && (ws == wsMStart || ws == wsNMStart) || ns == list->SectionCount - 1 && (ws == wsNEnd || ws == wsNMEnd) || ms == SectionCount - 1 && (ws == wsMEnd || ws == wsNMEnd))
            {
                continue;
            }

            TRad nsrad = list->Radian[ix % n];
            TRad msrad = Radian[ix / n];

            int mxpos = (ix % n) - ((ws == wsNStart || ws == wsNMStart) ? list->SectionSections[0] : 0);
            int mypos = (ix / n) - ((ws == wsMStart || ws == wsNMStart) ? SectionSections[0] : 0);

            int mix = mypos * mw + mxpos;

            matrix[mix] = (nsrad - msrad).Abs() * 800;

            byte mhorz = msrad.Horizontal(), nhorz = nsrad.Horizontal();
            byte mvert = msrad.Vertical(), nvert = nsrad.Vertical();
            byte mdiag = msrad.Diagonal(), ndiag = nsrad.Diagonal();
            double d = 1.0;

            if ((ndiag && mdiag && ndiag != mdiag || mvert && nvert && mvert != nvert || mhorz && nhorz && mhorz != nhorz || ndiag && (mhorz || mvert) || mdiag && (nhorz || nvert)))
                d *= 1.3;
            else if (list->SectionLength[ns] < nlen*0.6 && SectionLength[ms] < mlen*0.6 && min(list->SectionLength[ns], SectionLength[ms]) / max(list->SectionLength[ns], SectionLength[ms]) > 0.8)
                d *= 0.5;
            matrix[mix] *= d;

            if (mxpos && mypos)
                matrix[mix] += min(min(matrix[mix - 1], matrix[mix - mw]), matrix[mix - mw - 1]);
            else if (!mxpos && mypos)
                matrix[mix] += matrix[mix - mw];
            else if (mix)
                matrix[mix] += matrix[mix - 1];
        }

        res = matrix[mw * mh - 1];

    //"backtrack" to see how many steps were needed to get here

        int x = mw - 1, y = mh - 1;
        while (x > 0 || y > 0)
        {
            steps++;
            if (x && y)
            {
                if (matrix[(x - 1) + (y - 1)*mw] < matrix[x + (y - 1)*mw] && matrix[(x - 1) + (y - 1)*mw] < matrix[(x - 1) + y*mw])
                    x-- , y--;
                else if (matrix[x + (y - 1)*mw] < matrix[(x - 1) + y*mw])
                    y--;
                else
                    x--;
            }
            else if (x)
                x--;
            else
                y--;
        }

//#define DEBUG_COMPARE 1
#ifdef DEBUG_COMPARE
        FILE *f = _wfopen(L"d:\\comparedebug.txt", L"w");
        UnicodeString str = L"";
        for (int ix = 0; ix < mw * mh; ++ix)
        {
            if (ix)
            {
                if (!(ix % mw))
                    str += L"\r\n";
                else
                    str += L"\t";
            }
            str += IntToStr(matrix[ix]);
        }
        fputs(str.c_str(), f);
        fclose(f);
#endif

    }
    catch (...)
    {
        ;
    }
    delete[] matrix;

    double sdif = double(min(m, n)) / double(max(m, n));
    if (sdif < 0.7)
    {
        wextra += min(20000, 3200 * (0.7 / sdif));
    }

    res = (res / double(steps)) * 10.0 + wextra;

    return res;
}
//---------------------------------------------------------------------------
double TPointListEx::get_pos(int pt)
{
    if (!Length || !pt)
        return 0;

    double d = 0;
    for (int ix = 0; ix < pt; ++ix)
        d += Lengths[ix];

    return d / Length;
}
//---------------------------------------------------------------------------
double TPointListEx::backtrackcompare(TPointListEx *list, TIntegerList *points)
{
    double res = 0;

    int n = list->Count;
    int m = Count;

    if (m > n)
        THROW(L"Main list must have less points to compare!");

    double *matrix = NULL;
    try
    {
        matrix = new double[n * m];
        for (int ix = 0; ix < n * m; ++ix)
        {
            double posdif = 0;

            int xm = ix / n;
            int xn = ix % n;
            TRad rad1 = !xm ? Radian[0] - Radian[1] : xm == Count - 1 ? Radian[xm - 2] - Radian[xm - 1] : Radian[xm - 1] - Radian[xm];
            TRad rad2 = !xn ? list->Radian[0] - list->Radian[1] : xn == list->Count - 1 ? list->Radian[xn - 2] - list->Radian[xn - 1] : list->Radian[xn - 1] - list->Radian[xn];
            double raddif = (rad1 - rad2).Abs() * 3000;
            double radabs = ((xm < Count - 1 ? Radian[xm] : Radian[xm - 1]) - (xn < list->Count - 1 ? list->Radian[xn] : list->Radian[xn - 1])).Abs() * 3000;

            matrix[ix] = raddif + radabs + posdif;
            if (ix > n && (ix % n))
                matrix[ix] += min(min(matrix[ix - 1], matrix[ix - n]), matrix[ix - n - 1]);
            else if (!(ix % n) && ix >= n)
                matrix[ix] += matrix[ix - n];
            else if (ix)
                matrix[ix] += matrix[ix - 1];
        }

        res = matrix[n * m - 1];

        backtrack(matrix, m, n, points);
    }
    catch (...)
    {
        ;
    }
    delete[] matrix;

    return res;
}
//---------------------------------------------------------------------------
void TPointListEx::backtrack(double *matrix, int m, int n, TIntegerList *points)
{
    int npos = n - 1;
    int mpos = m - 1;
    points->Add(npos);

    double minval = 9999999;
    int minpt = n - 1;

    int oldm = mpos;

    while (npos && mpos)
    {
        double mdec = mpos > 1 ? matrix[(mpos - 2) * n + npos - 1] : 0;
        double ndec = npos > 1 ? matrix[npos + (mpos - 1) * n - 2] : 0;
        double mndec = npos > 1 && mpos > 1 ? matrix[(mpos - 2) * n + npos - 2] : 0;

        double mindif = min(min(mpos > 1 ? mdec : 9999999, npos > 1 ? ndec : 9999999), npos > 1 && mpos > 1 ? mndec : 9999999);
        double dif = matrix[npos + (mpos - 1) * n - 1] - (mindif < 9999999 ? mindif : 0);

        if (oldm != mpos)
        {
            points->Insert(0, minpt);
            minval = dif;
            minpt = npos - 1;
            oldm = mpos;
        }
        else if (minval > dif)
        {
            minval = dif;
            minpt = npos - 1;
        }

        if (mpos == 1 && npos == 1 || (mpos > 1 && mndec < mdec) && (npos > 1 && mndec < ndec))
        {
            --npos;
            --mpos;
        }
        else if (npos > 1 && (mpos == 1 || ndec < mdec))
            --npos;
        else if (mpos > 1 && (npos == 1 || mdec < ndec))
            --mpos;
        else
        { //equal distance in every direction
            double nd = (double)npos / n;
            double md = (double)mpos / m;
            double nnd = (double)(npos - 1) / n;
            double mmd = (double)(mpos - 1) / m;

            if (max(mmd, nnd) / min(mmd, nnd) < max(mmd, nd) / min(mmd, nd) && max(mmd, nnd) / min(mmd, nnd) < max(md, nnd) / min(md, nnd))
            {
                --npos;
                --mpos;
            }
            else if (max(mmd, nd) / min(mmd, nd) < max(md, nnd) / min(md, nnd))
                --mpos;
            else
                --npos;
        }
    }
    points->Insert(0, NULL);

    for (int ix = 1; ix < points->Count - 1 && points->Items[ix] < n - 1; ++ix)
    {
        if (points->Items[ix] == points->Items[ix + 1])
        {
            for (int iy = ix + 1; iy < points->Count - 1 && points->Items[ix] == points->Items[iy]; ++iy)
                ++points->Items[iy];
        }
    }
    for (int ix = points->Count - 1; ix > 1 && points->Items[ix] > 0; --ix)
    {
        if (points->Items[ix] == points->Items[ix - 1])
        {
            for (int iy = ix - 1; iy > 0 && points->Items[ix] == points->Items[iy]; --iy)
                --points->Items[iy];
        }
    }
}
//---------------------------------------------------------------------------
double TPointListEx::divisionlength(int first, int last)
{
    if (first < 0 || last > Count - 1)
        THROW(L"Section part out of index!");

    double res = 0;
    for (int ix = first; ix < last; ++ix)
        res += Lengths[ix];

    return res;
}
//---------------------------------------------------------------------------
double TPointListEx::arcsize(int first, int last)
{
    double sizea = 0;
    double sizeb = 0;

    TDoublePoint f = *Items[first];
    TDoublePoint l = *Items[last];

    if (f == l)
        return 0;
    for (int ix = first + 1; ix < last - 1; ++ix)
    {
        TDoublePoint i = *Items[ix];
        double dist = zgenericlinedistance(f, l, i);

        if (zgenericlineside(f, l, i))
            sizea += dist;
        else
            sizeb += dist;
    }

    return ((sizeb - sizea) / divisionlength(first, last)) * 1000;
}
//---------------------------------------------------------------------------
TPointListEx::TDoublePointPair TPointListEx::get_pointpair(int ix)
{
    if (ix < 0 || ix >= Count - 1)
        THROW(L"Section index out of range.");

    TDoublePointPair pair;
    pair.p1 = *Items[ix];
    pair.p2 = *Items[ix + 1];

    return pair;
}
TPointListEx::TIntersections TPointListEx::intersections(TPointListEx *list, double threshold)
{
    TIntersections result;
    memset(&result, 0, sizeof(TIntersections));
    result.startpoint.distance = 99999;
    result.endpoint.distance = 99999;
    result.liststartpoint.distance = 99999;
    result.listendpoint.distance = 99999;
    result.nearness = 99999;
    double d;
    double len = 0;
    double llen;

    for (int ix = 0; ix < Count - 1; ++ix)
    {
        llen = 0;
        for (int iy = 0; iy < list->Count - 1; ++iy)
        {
            bool add = true;
            if (ix == 0)
            {
                d = zlinedistance(list->Pairs[iy][0], list->Pairs[iy][1], *Items[0], true);
                if (d < result.startpoint.distance)
                {
                    result.startpoint.position = llen + zpointposition(list->Pairs[iy][0], list->Pairs[iy][1], *Items[0], &d);
                    result.startpoint.distance = d;
                }
                if (d < threshold)
                    add = false;
            }
            if (ix == Count - 2)
            {
                d = zlinedistance(list->Pairs[iy][0], list->Pairs[iy][1], *Items[Count - 1], true);
                if (d < result.endpoint.distance)
                {
                    result.endpoint.position = llen + zpointposition(list->Pairs[iy][0], list->Pairs[iy][1], *Items[Count - 1], &d);
                    result.endpoint.distance = d;
                }
                if (d < threshold)
                    add = false;
            }
            if (iy == 0)
            {
                d = zlinedistance(Pairs[ix][0], Pairs[ix][1], *list->Items[0], true);
                if (d < result.liststartpoint.distance)
                {
                    result.liststartpoint.position = len + zpointposition(Pairs[ix][0], Pairs[iy][1], *list->Items[0], &d);
                    result.liststartpoint.distance = d;
                }
                if (d < threshold)
                    add = false;
            }
            if (iy == list->Count - 2)
            {
                d = zlinedistance(Pairs[ix][0], Pairs[ix][1], *list->Items[list->Count - 1], true);
                if (d < result.listendpoint.distance)
                {
                    result.listendpoint.position = len + zpointposition(Pairs[ix][0], Pairs[ix][1], *list->Items[list->Count - 1], &d);
                    result.listendpoint.distance = d;
                }
                if (d < threshold)
                    add = false;
            }

            TDoublePoint pt;
            int li = zlineintersect(Pairs[ix][0], Pairs[ix][1], list->Pairs[iy][0], list->Pairs[iy][1], pt);
            if (li == 1 || li == 2)
            {
                d = zpointdistance(Pairs[ix][0], pt);
                if (len + d > threshold && fabs(Length - len - d) > threshold)
                    result.intersection[result.count].pos1 = len + d;
                d = zpointdistance(list->Pairs[iy][0], pt);
                if (llen + d > threshold && fabs(list->Length - llen - d) > threshold)
                    result.intersection[result.count].pos2 = llen + d;

                if (add)
                {
                    result.count++;
                    result.nearness = 0;
                }
            }

            if (result.nearness)
            {
                result.nearness = min(result.nearness, zlinedistance(list->Pairs[iy][0], list->Pairs[iy][1], Pairs[ix][0], true));
                result.nearness = min(result.nearness, zlinedistance(list->Pairs[iy][0], list->Pairs[iy][1], Pairs[ix][1], true));
                result.nearness = min(result.nearness, zlinedistance(Pairs[ix][0], Pairs[ix][1], list->Pairs[iy][0], true));
                result.nearness = min(result.nearness, zlinedistance(Pairs[ix][0], Pairs[ix][1], list->Pairs[iy][1], true));
            }
            llen += list->Lengths[iy];
        }
        len += Lengths[ix];
    }

    return result;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
struct TRadPointElem
{
    TPoint p;
    TRad rad;
    double len;
};
double PtDist(const TRadPointElem &a, const TPoint &elem)
{
    TRad rm = a.rad - Rad(a.p, elem);
    return rm.Sin() * Len(a.p, elem);
}
  //---------------------------------------------------------------------------
double PtDist(const TPoint &a, const TPoint &b, const TPoint &elem)
{
    if (a == b)
        return Len(a, elem);
    TRadPointElem tmp;
    tmp.p = a;
    tmp.rad = Rad(a, b);
    tmp.len = Len(a, b);
    return PtDist(tmp, elem);
}
  //---------------------------------------------------------------------------
void ProcessStroke(TPointListEx *fpoints, TPointListEx *result, bool from_mouse)
{
    if (!fpoints->Count)
        THROW(L"No point in data!");

    double BRD_DIST = from_mouse ? 72.5 : 52.2;

    TGapList<bool> *moved = 0;
    typedef TTemplateList<TRadPointElem, true> TRadPointList;
    TRadPointList *points = new TRadPointList;
    try
    {
        moved = new TGapList<bool>(false);

        for (int ix = 0; ix < fpoints->Count; ++ix)
        {
            if (points->Count && points->Items[points->Count - 1]->p == fpoints->Items[ix]->point())
                continue;

            TRadPointElem *elem = new TRadPointElem;
            memset(elem, 0, sizeof(TRadPointElem));
            elem->p = *fpoints->Items[ix];
            points->Add(elem);

            if (points->Count > 1)
            {
                TRadPointElem *e = points->Items[points->Count - 2];
                e->len = Len(e->p, elem->p);
                e->rad = Rad(e->p, elem->p);
            }
        }

    /*remove dots that are near an imaginary straight line close to the drawn shape*/

        bool changed = true;
    /*1. removal: weld dots that are too close to each other*/
        while (changed && points->Count > 2)
        {
            int m = -1;
            double mdist = BRD_DIST * 100;
            for (int ix = 0; ix < points->Count - 2; ++ix)
                if (points->Items[ix]->len < mdist)
                {
                    mdist = points->Items[ix]->len;
                    m = ix;
                }

            if (mdist > BRD_DIST * 3 || m == points->Count - 2)
                break;

            changed = true;

            double balance; //0-1 position between m and m+1 dots
            if (!m)
                balance = 0;
            else
            {
                double nextrad = (points->Items[m]->rad - points->Items[m + 1]->rad).Abs();
                double thisrad = (points->Items[m - 1]->rad - points->Items[m]->rad).Abs();
                balance = 0.5 + (nextrad - thisrad) / ((2 * M_PI) / 3);

                if (TRad(thisrad) >= TRad(M_PI_2 * 0.83))
                {
                    if (m > 1 && (points->Items[m - 2]->rad - points->Items[m - 1]->rad).Abs() < M_PI_4 || nextrad < M_PI_4)
                        balance = 0;
                }

            }

            balance = min(max(balance, 0.0), 1.0);

            points->Items[m]->p = Point(points->Items[m]->p.x + (double)(points->Items[m + 1]->p.x - points->Items[m]->p.x) * balance, points->Items[m]->p.y + (double)(points->Items[m + 1]->p.y - points->Items[m]->p.y) * balance);

            points->Delete(m + 1);

            if (m + 1 < points->Count)
            {
                if (points->Items[m]->p == points->Items[m + 1]->p)
                    points->Delete(m + 1);
                points->Items[m]->rad = m + 1 < points->Count ? Rad(points->Items[m]->p, points->Items[m + 1]->p) : TRad(0.0);
                points->Items[m]->len = m + 1 < points->Count ? Len(points->Items[m]->p, points->Items[m + 1]->p) : 0;
            }
            if (m)
            {
                if (points->Items[m - 1]->p == points->Items[m]->p)
                    points->Delete(m);
                points->Items[m - 1]->rad = m < points->Count ? Rad(points->Items[m - 1]->p, points->Items[m]->p) : TRad(0.0);
                points->Items[m - 1]->len = m < points->Count ? Len(points->Items[m - 1]->p, points->Items[m]->p) : 0;
            }
        }

/*2. removal: remove dots that lie between the line of dot[ix]<->dot[iy] (ix+1 < iy)*/
        changed = true;
        while (changed)
        {
            changed = false;

            moved->Clear();
            for (int ix = 0; ix < points->Count - 2; ++ix)
            {
                double max_dist = BRD_DIST;
                if (min(points->Items[ix]->len, points->Items[ix + 1]->len) > BRD_DIST * 2)
                    max_dist *= max(0.5, min(points->Items[ix]->len, points->Items[ix + 1]->len) / (BRD_DIST * 16));
                if (PtDist(points->Items[ix]->p, points->Items[ix + 2]->p, points->Items[ix + 1]->p) < max_dist / 2)
                {
                    moved->Items[ix + 1] = true; //mark as removable
                    ++ix;
                    changed = true;
                }
            }
      //remove points that were marked
            if (changed)
            {
                for (int ix = points->Count - 2; ix >= 1; --ix)
                {
                    if (!moved->Items[ix])
                        continue;
                    points->Delete(ix);
                    points->Items[ix - 1]->rad = Rad(points->Items[ix - 1]->p, points->Items[ix]->p);
                    points->Items[ix - 1]->len = Len(points->Items[ix - 1]->p, points->Items[ix]->p);
                }
            }

        }

        delete moved;
        moved = 0;
    }
    catch (...)
    {
        delete points;
        points = 0;
        delete moved;
        moved = 0;
        throw;
    }

    if (points->Count <= 1)
    {
        if (fpoints->Count)
        {
            points->Clear();
            TRadPointElem *elem = new TRadPointElem;
            memset(elem, 0, sizeof(TRadPointElem));
            elem->p = *fpoints->Items[0];
            points->Add(elem);

            elem = new TRadPointElem;
            memset(elem, 0, sizeof(TRadPointElem));
            elem->p = Point(fpoints->Items[0]->x + 25, fpoints->Items[0]->y + 25);
            points->Add(elem);
        }
        else
        {
            delete points;
            THROW(L"No points left in stroke!");
        }
    }

    result->clear();
    for (int ix = 0; ix < points->Count; ++ix)
        result->add(points->Items[ix]->p);

    delete points;
}
//---------------------------------------------------------------------------

void StrokePointsBezier(TDoublePoint p1, TDoublePoint c1, TDoublePoint c2, TDoublePoint p2, TPointListEx *pts)
{
    int steps = Len(p1, p2) / 40;
    if (!steps)
        steps = 1;

    TDoublePoint pp1, pp2, pp3;

    TDoublePoint old = p1;

    int firststep = 0;
    int laststep = steps - 1;

    for (int i = firststep - 1; i <= laststep; i++)
    {
        double s = ((double)i + 1) / (double)steps;

        pp3.x = pow(1.0 - s, 3) * p1.x + 3 * s * pow(1 - s, 2) * c1.x + 3 * pow(s, 2) * (1 - s) * c2.x + pow(s, 3) * p2.x;
        pp3.y = pow(1.0 - s, 3) * p1.y + 3 * s * pow(1 - s, 2) * c1.y + 3 * pow(s, 2) * (1 - s) * c2.y + pow(s, 3) * p2.y;

        if (round(old.x) != round(pp3.x) || round(old.y) != round(pp3.y))
            if (firststep != i + 1)
                pts->add(round(pp3.x), round(pp3.y));
        old = pp3;
    }
}
//---------------------------------------------------------------------------
void GetElementStrokePoints(const TRect &r, TKanjiElement *e, int variant, int stroke, TPointListEx *pts)
{
    if (e->variantcount <= variant)
        return;
    TKanjiVariant *v = e->variants + variant;
    if (v->strokecount <= stroke)
        return;

    if (!v->standalone)
    {
        bool docenter = (e->pattern >= epEbl && e->pattern <= epElr);
        int ix;
        for (ix = 0; ix < 4 && e->parts[ix] >= 0; ++ix)
        {
            TKanjiElement *pe = elements->Items[e->parts[ix]];
            TKanjiVariant *pv = pe->variants + v->positions[ix].variant;

            int ps = pv->strokecount;
            if (docenter && pv->standalone && pv->centrepoint)
                ps = pv->centrepoint - 1;
            if (stroke - ps < 0)
            {
                TRect r2;
                TPartPosition *ppos = v->positions + ix;

                r2.Left = (double)r.Left + (double)(ppos->x - (double)pv->x * ppos->width / max(1, pv->width)) * r.Width() / 10000.0;
                r2.Top = (double)r.Top + (double)(ppos->y - (double)pv->y * ppos->height / max(1, pv->height)) * r.Height() / 10000.0;
                r2.Right = (double)r2.Left + (double)ppos->width * r.Width() / max(1, pv->width);
                r2.Bottom = (double)r2.Top + (double)ppos->height * r.Height() / max(1, pv->height);
                GetElementStrokePoints(r2, pe, v->positions[ix].variant, stroke, pts);
                return;
            }
            else
                stroke -= ps;
        }

    //nothing found before centrepoint
        for (--ix; ix >= 0; --ix)
        {
            TKanjiElement *pe = elements->Items[e->parts[ix]];
            TKanjiVariant *pv = pe->variants + v->positions[ix].variant;
            if (pv->standalone && pv->centrepoint)
            {
                int ps = pv->strokecount - pv->centrepoint + 1;
                if (stroke - ps < 0)
                {
                    TRect r2;
                    TPartPosition *ppos = v->positions + ix;
                    r2.Left = (double)r.Left + (double)(ppos->x - (double)pv->x * ppos->width / max(1, pv->width)) * r.Width() / 10000.0;
                    r2.Top = (double)r.Top + (double)(ppos->y - (double)pv->y * ppos->height / max(1, pv->height)) * r.Height() / 10000.0;
                    r2.Right = (double)r2.Left + (double)ppos->width * r.Width() / max(1, pv->width);
                    r2.Bottom = (double)r2.Top + (double)ppos->height * r.Height() / max(1, pv->height);
                    GetElementStrokePoints(r2, pe, v->positions[ix].variant, stroke + pv->centrepoint - 1, pts);
                    return;
                }
                else
                    stroke -= ps;
            }
        }

    }
    else
    {
        TElementStroke *s = v->strokes + stroke;
        TDoublePoint pp, pt, ptc1, ptc2;
        double wdelta = (double)r.Width() / 10000.0;
        double hdelta = (double)r.Height() / 10000.0;

        TElementPoint *p = &s->points[0];
        pt.x = wdelta * (double)p->x + r.left;
        pt.y = hdelta * (double)p->y + r.top;

        pts->add(pt);
        pp = pt;
        for (int ix = 1; ix < s->pointcount; ++ix)
        {
            p = &s->points[ix];
            if (p->type == ptCurve)
            {
                pt.x = wdelta * (double)p->x + r.left;
                pt.y = hdelta * (double)p->y + r.top;
                ptc1.x = wdelta * (double)p->c1x + r.left;
                ptc1.y = hdelta * (double)p->c1y + r.top;
                ptc2.x = wdelta * (double)p->c2x + r.left;
                ptc2.y = hdelta * (double)p->c2y + r.top;
                StrokePointsBezier(pp, ptc1, ptc2, pt, pts);
            }
            else
            {
                pt.x = wdelta * (double)p->x + r.left;
                pt.y = hdelta * (double)p->y + r.top;
                pts->add(pt);
            }
            pp = pt;
        }

    }
}
//---------------------------------------------------------------------------
/*Positions:
 *                 0
 *                 4
 *                 8
 *                12
 * 1  5  9  13          14 10 6  2
 *                15
 *                11
 *                 7
 *                 3
*/
void ProcessDimensions(Bits &bit, TRect orig, TRect comp, const TRect &fullsize)
{

    int hivalue = 1000000;
    int lovalue = fullsize.Width() < 5000 && fullsize.Height() < 5000 ? 150 : 300;

    int loxo = orig.Width() > 1000 ? 0 : orig.Width() > 300 ? 50 : lovalue;
    int loyo = orig.Height() > 1000 ? 0 : orig.Height() > 300 ? 50 : lovalue;
    int loxc = comp.Width() > 1000 ? 0 : comp.Width() > 300 ? 50 : lovalue;
    int loyc = comp.Height() > 1000 ? 0 : comp.Height() > 300 ? 50 : lovalue;

    InflateRect(&orig, loxo, loyo);
    InflateRect(&comp, loxc, loyc);

//int mxw = max(orig.Width(),comp.Width());
//int mxh = max(orig.Height(),comp.Height());
    int mnw = min(orig.Width(), comp.Width());
    int mnh = min(orig.Height(), comp.Height());
    int ow = orig.Width();
    int oh = orig.Height();

    double d = ((double)max(fullsize.Width(), fullsize.Height()) / 6.0);

    bit.checkall(16, false);

    TRect dummy, r;

    r = Rect(-hivalue, -hivalue, hivalue, orig.Top - max(d * 0.63, oh * 0.10));
    if (IntersectRect(dummy, comp, r))
        bit[0] = true;
    r = Rect(-hivalue, -hivalue, orig.Left - max(d * 0.63, ow * 0.10), hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[1] = true;
    r = Rect(orig.Right + max(d * 0.63, ow * 0.10), -hivalue, hivalue, hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[2] = true;
    r = Rect(-hivalue, orig.Bottom + max(d * 0.63, oh * 0.10), hivalue, hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[3] = true;

/*
  if (comp.Bottom+max(d*0.5,lovalue) < orig.Top)
    bit[0] = true;
  if (comp.Top-max(d*0.5,lovalue) > orig.Bottom)
    bit[3] = true;
  if (comp.Right+max(d*0.5,lovalue) < orig.Left)
    bit[1] = true;
  if (comp.Left-max(d*0.5,lovalue) > orig.Right)
    bit[2] = true;
*/
    r = Rect(-hivalue, orig.Top - max(d * 0.65, oh * 0.11), hivalue, orig.Top - min(d * 0.3, mnh * 0.5));
    if (IntersectRect(dummy, comp, r))
        bit[4] = true;
    r = Rect(orig.Left - max(d * 0.65, ow * 0.11), -hivalue, orig.Left - min(d * 0.3, mnw * 0.5), hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[5] = true;
    r = Rect(orig.Right + min(d * 0.3, mnw * 0.5), -hivalue, orig.Right + max(d * 0.65, ow * 0.11), hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[6] = true;
    r = Rect(-hivalue, orig.Bottom + min(d * 0.3, mnh * 0.5), hivalue, orig.Bottom + max(d * 0.65, oh * 0.11));
    if (IntersectRect(dummy, comp, r))
        bit[7] = true;

    r = Rect(-hivalue, orig.Top - min(d * 0.35, mnh * 0.55), hivalue, orig.Top + min(d, oh * 0.2));
    if (IntersectRect(dummy, comp, r))
        bit[8] = true;
    r = Rect(orig.Left - min(d * 0.35, mnw * 0.55), -hivalue, orig.Left + min(d, ow * 0.2), hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[9] = true;
    r = Rect(orig.Right - min(d, ow * 0.2), -hivalue, orig.Right + min(d * 0.35, mnw * 0.55), hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[10] = true;
    r = Rect(-hivalue, orig.Bottom - min(d, oh * 0.2), hivalue, orig.Bottom + min(d * 0.35, mnh * 0.55));
    if (IntersectRect(dummy, comp, r))
        bit[11] = true;

    r = Rect(-hivalue, orig.Top + min(d * 0.9, oh * 0.18), hivalue, orig.Top + oh * 0.55);
    if (IntersectRect(dummy, comp, r))
        bit[12] = true;
    r = Rect(orig.Left + min(d * 0.9, ow * 0.18), -hivalue, orig.Left + ow * 0.55, hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[13] = true;
    r = Rect(orig.Left + ow * 0.55, -hivalue, orig.Right - min(d * 0.9, ow * 0.18), hivalue);
    if (IntersectRect(dummy, comp, r))
        bit[14] = true;
    r = Rect(-hivalue, orig.Top + oh * 0.55, hivalue, orig.Bottom - min(d * 0.9, ow * 0.18));
    if (IntersectRect(dummy, comp, r))
        bit[15] = true;

/*
  if (comp.Bottom+max(d*0.95,mxh*0.09) < orig.Top)
    bit[0] = true;
  if (comp.Top-max(d*0.95,mxh*0.09) > orig.Bottom)
    bit[3] = true;
  if (comp.Right+max(d*0.95,mxw*0.09) < orig.Left)
    bit[1] = true;
  if (comp.Left-max(d*0.95,mxw*0.09) > orig.Right)
    bit[2] = true;


  r = Rect(-hivalue,orig.Top-max(d,mxh*0.11),hivalue,orig.Top-min(d/6.0,oh*0.1));
  if (IntersectRect(dummy,comp,r))
    bit[4] = true;
  r = Rect(orig.Left-max(d,mxw*0.11),-hivalue,orig.Left-min(d/6.0,ow*0.1),hivalue);
  if (IntersectRect(dummy,comp,r))
    bit[5] = true;
  r = Rect(orig.Right+min(d/6.0,ow*0.1),-hivalue,orig.Right+max(d,mxw*0.11),hivalue);
  if (IntersectRect(dummy,comp,r))
    bit[6] = true;
  r = Rect(-hivalue,orig.Bottom+min(d/6.0,oh*0.1),hivalue,orig.Bottom+max(d,mxh*0.11));
  if (IntersectRect(dummy,comp,r))
    bit[7] = true;


  r = Rect(-hivalue,orig.Top-min(d/5.0,oh*0.2),hivalue,orig.Top+min(2*d,oh*0.38));
  if (IntersectRect(dummy,comp,r))
    bit[8] = true;
  r = Rect(orig.Left-min(d/5.0,ow*0.2),-hivalue,orig.Left+min(2*d,ow*0.38),hivalue);
  if (IntersectRect(dummy,comp,r))
    bit[9] = true;
  r = Rect(orig.Right-min(2*d,ow*0.38),-hivalue,orig.Right+min(d/5.0,ow*0.2),hivalue);
  if (IntersectRect(dummy,comp,r))
    bit[10] = true;
  r = Rect(-hivalue,orig.Bottom-min(2*d,oh*0.38),hivalue,orig.Bottom+min(d/5.0,oh*0.2));
  if (IntersectRect(dummy,comp,r))
    bit[11] = true;

  r = Rect(-hivalue,orig.Top+min(2*d,oh*0.32),hivalue,orig.Top+oh*0.55);
  if (IntersectRect(dummy,comp,r))
    bit[12] = true;
  r = Rect(orig.Left+min(2*d,ow*0.32),-hivalue,orig.Left+ow*0.55,hivalue);
  if (IntersectRect(dummy,comp,r))
    bit[13] = true;
  r = Rect(orig.Left+ow*0.55,-hivalue,orig.Right-min(2*d,ow*0.32)+1,hivalue);
  if (IntersectRect(dummy,comp,r))
    bit[14] = true;
  r = Rect(-hivalue,orig.Top+oh*0.55,hivalue,orig.Bottom-min(2*d,oh*0.32)+1);
  if (IntersectRect(dummy,comp,r))
    bit[15] = true;
*/
}
//---------------------------------------------------------------------------
void DissectStroke(TPointListEx *src, TPointListEx *dest, int steps, bool checkarc)
{
    TDoublePoint *pp = NULL;
    TDoublePoint *pp_t = NULL;
    TIntegerList *l = NULL;
    TPointListEx *dummy = NULL;

    try
    {
        int checkpoints = steps * 60 + 1;
        pp = new TDoublePoint[checkpoints];
        pp_t = new TDoublePoint[steps + 1];
        dummy = new TPointListEx;

        l = new TIntegerList;
        l->Add(0);
        src->dissect(pp, checkpoints, l);
        l->Add(checkpoints - 1);

        if (checkarc)
        {
            for (int ix = 1; ix < l->Count; ++ix)
            {
                int fx = l->Items[ix - 1];
                int lx = l->Items[ix];

                if (lx - fx <= steps * 2 + 1)
                    continue;

                TDoublePoint pf = pp[fx];
                TDoublePoint pl = pp[lx];

                double len = 0, sidea = 0, sideb = 0;
                for (int iy = fx + 1; iy < lx; ++iy)
                {
                    if (zgenericlineside(pf, pl, pp[iy]))
                        sidea += zgenericlinedistance(pf, pl, pp[iy]);
                    else
                        sideb += zgenericlinedistance(pf, pl, pp[iy]);
                    len += Len(pp[iy - 1], pp[iy]);
                }
                len += Len(pp[lx - 1], pp[lx]);

                if (sidea && sideb && sidea / len > 0.05 && sideb / len >= 0.05)
                {
                    l->Insert(ix, (l->Items[ix] + l->Items[ix - 1]) / 2);
                    break;
                }
            }
        }

        dest->clear();

        for (int ix = 0; ix < l->Count - 1; ++ix)
        {
            dummy->clear();
            for (int iy = l->Items[ix]; iy <= l->Items[ix + 1]; ++iy)
                dummy->add(pp[iy]);

            dummy->splitpoints(pp_t, steps + 1);
            for (int iy = dest->Count ? 1 : 0; iy <= steps; ++iy)
                dest->add(pp_t[iy]);
        }

    }
    catch (...)
    {
        ;
    }
    delete[] pp;
    delete[] pp_t;
    delete l;
    delete dummy;

    for (int ix = dest->Count - 2; ix >= 0; --ix)
        if (*dest->Items[ix + 1] == *dest->Items[ix])
            dest->del(ix + 1);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(37);
#endif

