//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Math.h>

#include "writeobj.h"
#include "kanjielement.h"
#include "elementpoints.h"
#include "angle.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(200);
#endif

//---------------------------------------------------------------------------
__fastcall TStrokeTester::TStrokeTester() : fstroke(0), felemindex(-1), strokes(new TTemplateList<TPointListEx, true>)
{
}
//---------------------------------------------------------------------------
__fastcall TStrokeTester::~TStrokeTester()
{
    delete strokes;
}
//---------------------------------------------------------------------------
void __fastcall TStrokeTester::set_elem(int ix)
{
    if (felemindex == ix)
        return;
    strokes->Clear();
    felemindex = ix;
    stroke = 0;
}
//---------------------------------------------------------------------------
void __fastcall TStrokeTester::set_stroke(int ix)
{
    if (felemindex < 0 || ix < 0 || ix >= elements->Items[felemindex]->variants[0].strokecount)
        THROW(L"Index for stroke out of range!");

    fstroke = ix;
    while (strokes->Count <= fstroke)
    {
        TPointListEx *s = new TPointListEx;
        GetElementStrokePoints(Rect(0, 0, 10000, 10000), elements->Items[felemindex], 0, strokes->Count, s);
        ProcessStroke(s, s, false);
        strokes->Add(s);
    }
}
//---------------------------------------------------------------------------
double proportionate(double val, double max, double min, double vmax, double vmin, bool limit = false);
bool __fastcall TStrokeTester::test(TPointListEx *list, TPointListEx *match, TPointListEx *strokelist, TPointListEx *orig, TStrings *errors)
{
    const int checksteps = 4;

    bool result = true;

    TPointListEx *base = NULL;
    TPointListEx *cmp = NULL;
    TDoublePoint *floatlist = NULL;
    TIntegerList *l = 0;
    try
    {
        base = new TPointListEx();
        base->assign(strokes->Items[fstroke]);
        if (!strokelist)
            strokelist = base;
        else
            strokelist->assign(base);

        cmp = new TPointListEx();
        ProcessStroke(list, cmp, false);

        bool independent = true;
        bool found[6];
        bool foundorig[6];

        //bool firsttry = true;
        for (int ix = 0; ix < fstroke; ++ix)
        {
            TPointListEx::TIntersections ins = strokelist->intersections(strokes->Items[ix], 100);
            TPointListEx::TIntersections inscmp = cmp->intersections(strokes->Items[ix], 100);
            memset(found, false, sizeof(bool)*6);
            memset(foundorig, false, sizeof(bool)*6);

            if (ins.nearness > 200 && inscmp.nearness > 200)
                continue;

            independent = false;

            if (ins.startpoint.distance <= 100)
            {
                if (inscmp.startpoint.distance > 400 || fabs(inscmp.startpoint.position - ins.startpoint.position) / strokes->Items[ix]->Length > 0.3 && fabs(inscmp.startpoint.position - ins.startpoint.position) > 500)
                {

                    result = false;
                    if (errors)
                        errors->Add("Bad start point 1");
                }
                else
                {
                    for (int iy = 0; iy < inscmp.count; ++iy)
                    {
                        if (inscmp.intersection[iy].pos1 <= 400 && (fabs(inscmp.intersection[iy].pos2 - ins.startpoint.position) / strokes->Items[ix]->Length < 0.3 || fabs(inscmp.intersection[iy].pos2 - ins.startpoint.position) < 500))
                            found[iy] = true;
                    }
                }
            }
            if (ins.liststartpoint.distance <= 100)
            {
                if (inscmp.liststartpoint.distance > 400 || fabs(inscmp.liststartpoint.position / cmp->Length - ins.liststartpoint.position / strokelist->Length) > 0.3 && fabs(inscmp.liststartpoint.position - ins.liststartpoint.position) > 500)
                {
                    result = false;
                    if (errors)
                        errors->Add("Bad start point 2");
                }
                else
                {
                    for (int iy = 0; iy < inscmp.count; ++iy)
                    {
                        if (inscmp.intersection[iy].pos2 <= 400 && (fabs(inscmp.intersection[iy].pos1 / cmp->Length - ins.liststartpoint.position / strokelist->Length) < 0.3 || fabs(inscmp.intersection[iy].pos1 - ins.liststartpoint.position) < 500))
                            found[iy] = true;
                    }
                }
            }
            if (ins.endpoint.distance <= 100)
            {
                if (inscmp.endpoint.distance > 400 || fabs(inscmp.endpoint.position - ins.endpoint.position) / strokes->Items[ix]->Length > 0.3 && fabs(inscmp.endpoint.position - ins.endpoint.position) > 500)
                {
                    result = false;
                    if (errors)
                        errors->Add("Bad end point 1");
                }
                else
                {
                    for (int iy = 0; iy < inscmp.count; ++iy)
                    {
                        if (cmp->Length - inscmp.intersection[iy].pos1 <= 400 && (fabs(inscmp.intersection[iy].pos2 - ins.endpoint.position) / strokes->Items[ix]->Length < 0.3 || fabs(inscmp.intersection[iy].pos2 - ins.endpoint.position) < 500))
                            found[iy] = true;
                    }
                }
            }
            if (ins.listendpoint.distance <= 100)
            {
                if (inscmp.listendpoint.distance > 400 || fabs(inscmp.listendpoint.position / cmp->Length - ins.listendpoint.position / strokelist->Length) > 0.3 && fabs(inscmp.listendpoint.position - ins.listendpoint.position) > 500)
                {
                    result = false;
                    if (errors)
                        errors->Add("Bad end point 2");
                }
                else
                {
                    for (int iy = 0; iy < inscmp.count; ++iy)
                    {
                        if (strokes->Items[ix]->Length - inscmp.intersection[iy].pos2 <= 400 && fabs(inscmp.intersection[iy].pos1 / cmp->Length - ins.liststartpoint.position / strokelist->Length) < 0.3 || fabs(inscmp.intersection[iy].pos1 - ins.liststartpoint.position) < 500)
                            found[iy] = true;
                    }
                }
            }

            for (int iy = 0; iy < ins.count; ++iy)
            {
                for (int iz = 0; iz < inscmp.count; ++iz)
                {
                    if ((fabs(ins.intersection[iy].pos1 / strokelist->Length - inscmp.intersection[iz].pos1 / cmp->Length) <= 0.2 || fabs(ins.intersection[iy].pos1 - inscmp.intersection[iz].pos1) <= 300) && (fabs(ins.intersection[iy].pos2 - inscmp.intersection[iz].pos2) / strokes->Items[ix]->Length <= 0.2 || fabs(ins.intersection[iy].pos2 - inscmp.intersection[iz].pos2) <= 300))
                    {
                        found[iz] = true;
                        foundorig[iy] = true;
                    }
                }
            }

            for (int iy = 0; iy < ins.count; ++iy)
            {
                if (!foundorig[iy])
                {
                    result = false;
                    if (errors)
                        errors->Add((UnicodeString)"Bad intersection 1, no match: " + iy);
                }
            }

            for (int iy = 0; iy < inscmp.count; ++iy)
            {
                if (!found[iy])
                {
                    result = false;
                    if (errors)
                        errors->Add((UnicodeString)"Bad intersection 2, no match: " + iy);
                }
            }
            /*if (result == false && firsttry) {
             firsttry = false;
             ix--;
             }*/
        }

        TRect r = strokelist->Dimension;
        TRect lr = list->Dimension;
        double xmul = max(1, proportionate(r.Width(), 10000, 2000, 3, 1));
        double ymul = max(1, proportionate(r.Height(), 10000, 2000, 3, 1));
        if (independent)
        {
            xmul *= 2.5;
            ymul *= 2.5;
        }
        if (abs(r.Left - lr.Left) > 900 * xmul || abs(r.Top - lr.Top) > 900 * ymul || abs(r.Bottom - lr.Bottom) > 900 * ymul || abs(r.Right - lr.Right) > 900 * xmul)
        {
            result = false;
            if (errors)
                errors->Add("Wrong position!");
        }
        if (abs(r.Width() - lr.Width()) > 1300 * xmul || abs(r.Height() - lr.Height()) > 1300 * ymul)
        {
            result = false;
            if (errors)
                errors->Add("Size difference!");
        }

        base->resize(10000, 10000);

        DissectStroke(base, base, checksteps, true);
        cmp->resize(10000, 10000);
        DissectStroke(cmp, cmp, checksteps*20, false);
        for (int ix = cmp->Count - 1; ix > 0; --ix)
            if (*cmp->Items[ix] == *cmp->Items[ix - 1])
                cmp->del(ix);

        l = new TIntegerList;
        base->backtrackcompare(cmp, l);
        if (l->Count != base->Count)
            THROW(L"Stroke does not match!");

        base->resize(r.Width(), r.Height());
        base->offset(r.Left, r.Top);
        if (orig)
            for (int ix = 0; ix < base->Count; ++ix)
                orig->add(base->Items[ix]);

        cmp->add(0, 0);
        cmp->add(cmp->Dimension.Width() - 1, cmp->Dimension.Height() - 1);
        cmp->resize(list->Dimension);
        //cmp->resize(list->Dimension.Width(),list->Dimension.Height());
        //cmp->offset(list->Dimension.Left,list->Dimension.Top);
        cmp->del(cmp->Count - 1);
        cmp->del(cmp->Count - 1);

        int last = -1;
        double sectionlen = 0;

        int ch = checksteps;
        for (int ix = 0; ix < l->Count; ++ix)
        {
            if (!(ix % ch) && ix < l->Count - 1)
                sectionlen = cmp->divisionlength(l->Items[ix], l->Items[ix + ch]);
            int z = l->Items[ix - (ix % ch)];
            if (ix % ch)
            {
                double len = (sectionlen / ch) * (ix % ch);
                double d = 0;
                z = last + 1;
                for (int iy = l->Items[ix - (ix % ch)]; iy < l->Items[ix - (ix % ch) + ch]; ++iy)
                {
                    if (d + cmp->Lengths[iy] > len)
                    {
                        z = iy;
                        if ((d + cmp->Lengths[iy]) - len < len - d)
                            z++;
                        break;
                    }
                    d += cmp->Lengths[iy];
                }
                z = min(z, l->Items[ix - (ix % ch) + ch] - (ch - (ix % ch)));
            }

            z = max(last + 1, z);
            match->add(cmp->Items[z]);

            if (ix && !(ix % ch))
            {
                double d = (Rad(*match->Items[ix - ch], *match->Items[ix]) - Rad(*base->Items[ix - ch], *base->Items[ix])).Abs();
                double size = base->divisionlength(ix - ch, ix);
                double dmax = size < 4000 ? proportionate(size, 4000, 0, 0.3, 0.8) : 0.3;

                double baserad = min((Rad(*base->Items[ix - ch], *base->Items[ix]) - Rad(DoublePoint(0, 0), DoublePoint(0, 1))).Abs(), (Rad(*base->Items[ix - ch], *base->Items[ix]) - Rad(DoublePoint(0, 0), DoublePoint(1, 0))).Abs());
                if (baserad < 0.0872)//be more strict with near horizontal or near vertical stroke parts
                            dmax /= 2.5;

                if (d > dmax)
                {
                    result = false;
                    if (errors)
                    {
                        errors->Add((UnicodeString)"d: " + d);
                        errors->Add((UnicodeString)"dmax: " + dmax);
                        if (!result)
                        {
                            errors->Add((UnicodeString)"size: " + size);
                            errors->Add((UnicodeString)"len: " + base->Length);
                        }
                    }
                }

                //check arcsize
                double m = match->arcsize(ix - ch, ix);
                double b = base->arcsize(ix - ch, ix);
                double mul = size < 4000 ? proportionate(size, 4000, 0, 1, 4) : 1;

                if (fabs(m) > 40 || fabs(b) > 40)
                {

                    if (sgn(m) != sgn(b) && fabs(m - b) > 175 * mul)
                    {
                        result = false;
                        if (errors)
                            errors->Add((UnicodeString)(ix / ch) + " Arc1: " + m + " - " + b);
                    }
                    else
                    {
                        double val1;
                        double val2;
                        if (sgn(m) == sgn(b))
                        {
                            m = fabs(m);
                            b = fabs(b);
                            val1 = min(m, b);
                            val2 = max(m, b);
                        }
                        else
                        {
                            m = fabs(m);
                            b = fabs(b);
                            val2 = max(m, b) + min(m, b);
                            val1 = 0;
                        }

                        if (val2 - val1 > max(230, val1 * 2))
                        {
                            result = false;
                            if (errors)
                                errors->Add((UnicodeString)(ix / ch) + " Arc2: " + val1 + " - " + val2);
                        }
                    }
                }

                for (int iy = ix - ch + 1; iy < ix; ++iy)
                {
                    double diff = (Rad(*match->Items[iy - 1], *match->Items[iy]) - Rad(*match->Items[iy], *match->Items[iy + 1])).Abs();
                    if (diff > 1)
                    {
                        result = false;
                        if (errors)
                            errors->Add((UnicodeString)(iy) + " Rad: " + diff);
                    }
                }
            }
            last = z;
        }

        double nrad = M_PI;

        for (int ix = ch; ix < match->Count; ix += ch)
        {
            bool pdouble = nrad < 1.3;
            bool ndouble = false;

            if (ix < base->Count - 1)
            {
                nrad = (Rad(*base->Items[ix - 1], *base->Items[ix]) - Rad(*base->Items[ix], *base->Items[ix + 1])).Abs();
                ndouble = nrad < 1.3;
            }

            int c1 = ix - (pdouble && !ndouble ? 2 : 1) * ch;
            int c2 = ix + (ndouble ? 1 : 0) * ch;

            if (c1 == 0 && c2 == base->Count - 1)
                continue;

            double bsectlen = base->divisionlength(c1, c2);
            double blen = base->Length - bsectlen;
            double msectlen = match->divisionlength(c1, c2);
            double mlen = match->Length - msectlen;

            double size = bsectlen / blen;
            double msize = msectlen / mlen;
            if (max(msize, size) / min(size, msize) > 1.8)
            {
                result = false;
                if (errors)
                {
                    errors->Add((UnicodeString)c1 + "*" + (UnicodeString)c2 + (pdouble ? "p" : ndouble ? "n" : ""));
                    errors->Add((UnicodeString)" SLen: " + size + " - " + msize + "\n*" + max(msize, size) / min(size, msize));
                }
            }
        }

    }
    catch (...)
    {
        delete base;
        delete cmp;
        delete l;
        delete[] floatlist;
        return false;
    }
    delete base;
    delete cmp;
    delete l;
    delete[] floatlist;

    return result;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(201);
#endif

