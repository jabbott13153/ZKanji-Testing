//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordgrid.h"
#include "settings.h"
#include "alphaimagelist.h"
#include "minmax.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(180);
#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static int seqitemsrcproc(TSequenceItem **item)
{
    return (*item)->pos;
}
int TSequenceList::find_seq(int ix)
{
    int six = logsearch<TSequenceItem*>(list.List, 0, list.Count - 1, ix, seqitemsrcproc);
    if (six < 0)
    {
        six = -1 - six;
        if (six != 0 && list[six - 1]->pos + list[six - 1]->len > ix)
            --six;
    }
    return six;
}
//---------------------------------------------------------------------------
bool TSequenceList::get_item(int ix)
{
    int six = find_seq(ix);
    return six != list.Count && ix >= list[six]->pos;
}
//---------------------------------------------------------------------------
void TSequenceList::set_item(int ix, bool val)
{
    int six = find_seq(ix);
    set_item2(six, ix, val);
}
//---------------------------------------------------------------------------
void TSequenceList::set_item2(int six, int ix, bool val)
{
    if (val == (six != list.Count && ix >= list[six]->pos))
        return;

    if (!val)
    {
        TSequenceItem *s = list[six];
        if (s->pos == ix || s->pos + s->len - 1 == ix)
        {
            --s->len;
            if (s->len != 0)
            {
                if (s->pos == ix)
                    ++s->pos;
            }
            else
                list.Delete(six);
            return;
        }
        TSequenceItem *s2 = new TSequenceItem;
        s2->pos = ix + 1;
        s2->len = s->pos + s->len - ix - 1;
        s->len = ix - s->pos;
        list.Insert(six + 1, s2);
    }
    else
    {
        if (!list.Count || ((six == 0 || list[six - 1]->pos + list[six - 1]->len < ix) && (six == list.Count || list[six]->pos > ix + 1)) )
        {
            TSequenceItem *s2 = new TSequenceItem;
            s2->pos = ix;
            s2->len = 1;
            list.Insert(six, s2);
        }
        else if ((six != list.Count && list[six]->pos == ix + 1) != (six != 0 && ix == list[six - 1]->pos + list[six - 1]->len))
        {
            if (six != 0 && list[six - 1]->pos + list[six - 1]->len == ix)
                ++list[six - 1]->len;
            else
            {
                --list[six]->pos;
                ++list[six]->len;
            }
        }
        else
        {
            list[six - 1]->len += list[six]->len + 1;
            list.Delete(six);
        }
    }
}
//---------------------------------------------------------------------------
int TSequenceList::get_seqcnt()
{
    return list.Count;
}
//---------------------------------------------------------------------------
TSequenceItem* TSequenceList::get_seq(int ix)
{
    return list[ix];
}
//---------------------------------------------------------------------------
void TSequenceList::SetSequence(int start, int count, bool set)
{
    if (count < 0 || start < 0)
        THROW(L"Sequence must be positive.");

    int firstseq = find_seq(start);

    if ((!set && (firstseq == list.Count || start + count <= list[firstseq]->pos)) || (set && firstseq != list.Count && start >= list[firstseq]->pos && start + count <= list[firstseq]->pos + list[firstseq]->len))
        return;

    if (set)
    {
        if (((firstseq != list.Count && start + count < list[firstseq]->pos) || firstseq == list.Count) &&
             (firstseq == 0 || list[firstseq - 1]->pos + list[firstseq - 1]->len < start))
        {
            TSequenceItem *s2 = new TSequenceItem;
            s2->pos = start;
            s2->len = count;
            list.Insert(firstseq, s2);
            return;
        }

        if (firstseq != 0 && list[firstseq - 1]->pos + list[firstseq - 1]->len == start)
            --firstseq;
        TSequenceItem *s = list[firstseq];
        if (s->pos <= start)
        {
            if (start + count <= s->pos + s->len)
                return;
            s->len += count - (s->pos + s->len - start);
        }
        else
        {
            if (start + count <= s->pos + s->len)
            {
                s->len = s->pos + s->len - start;
                s->pos = start;
                return;
            }

            s->pos = start;
            s->len = count;
        }

        int lastseq = firstseq + 1;
        TSequenceItem *s2 = NULL;
        for ( ; lastseq < list.Count; ++lastseq)
        {
            s2 = list[lastseq];
            if (s2->pos + s2->len > s->pos + s->len)
                break;
        }
        if (lastseq < list.Count && s2->pos <= s->pos + s->len)
            s->len = s2->pos + s2->len - s->pos;
        else
            --lastseq;

        if (firstseq + 1 < list.Count)
            list.DeleteRange(firstseq + 1, min(lastseq - firstseq, list.Count - firstseq - 1));
    }
    else // set == false
    {
        TSequenceItem *s = list[firstseq];
        if (s->pos <= start)
        {
            s->len = start - s->pos;
            if (s->len)
                ++firstseq;
        }

        int lastseq = firstseq;
        TSequenceItem *s2 = NULL;
        for ( ; lastseq < list.Count; ++lastseq)
        {
            s2 = list[lastseq];
            if (s2->pos + s2->len > start + count)
                break;
        }
        if (lastseq == list.Count || s2->pos >= start + count)
            --lastseq;
        else
        {
            s2->len -= start + count - s2->pos;
            s2->pos = start + count;
            --lastseq;
        }

        if (firstseq < list.Count)
            list.DeleteRange(firstseq, min(lastseq - firstseq + 1, list.Count - firstseq));
    }
}
//---------------------------------------------------------------------------
void TSequenceList::Clear()
{
    list.Clear();
}
//---------------------------------------------------------------------------
void TSequenceList::Limit(int limit)
{
    SetSequence(limit, INT_MAX - limit, false);
}
//---------------------------------------------------------------------------
bool TSequenceList::Toggle(int ix)
{
    int six = find_seq(ix);
    bool newval = six == list.Count || (ix < list[six]->pos && (six == 0 || list[six - 1]->pos + list[six - 1]->len <= ix));
    set_item2(six, ix, newval);
    return newval;
}
//---------------------------------------------------------------------------
int TSequenceList::Next(int ix)
{
    int six = find_seq(ix);
    if (six == list.Count)
        return -1;
    if (list[six]->pos > ix)
        return list[six]->pos;
    return ix;
}
//---------------------------------------------------------------------------
int TSequenceList::Prev(int ix)
{
    int six = find_seq(ix);
    if (six == 0 && (list.Count == 0 || list[0]->pos > ix))
        return -1;
    if (six == list.Count || list[six]->pos > ix)
        return list[six - 1]->pos + list[six - 1]->len - 1;
    return ix;
}
//---------------------------------------------------------------------------
int TSequenceList::SequenceLength(int ix)
{
    int six = find_seq(ix);
    if (six == list.Count || list[six]->pos > ix)
        return 0;
    return list[six]->len;
}
//---------------------------------------------------------------------------
bool TSequenceList::Single()
{
    return list.Count == 1 && list[0]->len == 1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TWordListBase::TWordListBase(TZPaintBox *abox, int arowheight, bool aheader, TFuncPtr afunc) :
        ffunc(afunc), fpos(-1), fsellength(0), /*frowselect(false), fmultiselect(false),*/ fseltype(wlsNoSelection), fmulsel(wlmsContinuous),
        fresize(NULL), fdefsize(NULL), fcolhover(NULL), fsizedcolumn(-1), fsizing(false),
        fvertline(true), fhorzline(true), fdarkensel(true), fallownopos(false), fheaderbutton(false),
        fheaderbuttondown(false), fresizing(true), fheaderbuttoncolor((TColor)0x88d0ff),
        fheaderbuttonhover(false), fheaderbuttoncolumn(-1), fheaderpressproc(NULL),
        fheaderselectproc(NULL), fheadprepare(NULL), fcolselected(-1), fordered(false),
        arrowimg(NULL), fheaderbuttoncolumnorder(hbdNone), fmultirightclick(false), inherited(abox)
{
    header = aheader;
    rowsize = arowheight;
    headersize = arowheight;
}
//---------------------------------------------------------------------------
TWordListBase::~TWordListBase()
{
    delete[] fresize;
    delete[] fdefsize;
    delete[] fcolhover;
    delete arrowimg;
}
//---------------------------------------------------------------------------
void TWordListBase::set_cols(int acolcnt, int *acolwidths, int *acoldefaultwidths, bool *acolvisible, int acount)
{
    fcolselected = -1;

    set_cols(acolcnt, acolwidths, acolvisible, rowsize, headersize, acount, header);

    if (fdefsize)
        memcpy(fdefsize, acoldefaultwidths ? acoldefaultwidths : acolwidths, sizeof(int)*colcount);
}
//---------------------------------------------------------------------------
int TWordListBase::get_colsize(byte col)
{
    return inherited::colsize[col];
}
//---------------------------------------------------------------------------
void TWordListBase::set_colsize(byte col, int val)
{
    inherited::colsize[col] = val;
    fdefsize[col] = inherited::colsize[col];
}
//---------------------------------------------------------------------------
int TWordListBase::get_defsize(byte col)
{
    if (col >= colcount)
        THROW(L"Column out of range!");
    return fdefsize[col];
}
//---------------------------------------------------------------------------
void TWordListBase::set_defsize(byte col, int val)
{
    if (col >= colcount)
        THROW(L"Column out of range!");
    fdefsize[col] = MAX(4, val * scaleres);
}
//---------------------------------------------------------------------------
bool TWordListBase::get_colresize(byte col)
{
    if (col >= colcount)
        THROW(L"Column out of range for getting resize value!");
    if (!fresize)
        return false;
    return fresize[col];
}
//---------------------------------------------------------------------------
void TWordListBase::set_colresize(byte col, bool val)
{
    if (col >= colcount)
        THROW(L"Column out of range for setting resize value!");
    if (!fresize)
    {
        if (val)
        {
            fresize = new bool[colcount];
            memset(fresize, 0, sizeof(bool)*colcount);
            DoColResizing(true);
            fresizing = true;
        }
        else
            return;
    }

    fresize[col] = val;
}
//---------------------------------------------------------------------------
void TWordListBase::set_colresizing(bool val)
{
    if (fsizedcolumn >= 0)
    {
        fsizedcolumn = -1;
        fbox->Cursor = crDefault;
    }

    if (!val)
    {
        delete[] fresize;
        fresize = NULL;
    }
    else
    {
        if (!fresizing)
            fresize = new bool[colcount];
        memset(fresize, true, sizeof(bool)*colcount);
    }

    DoColResizing(val);
    fresizing = val;
}
//---------------------------------------------------------------------------
bool TWordListBase::get_colhover(byte col)
{
    if (col >= colcount)
        THROW(L"Column out of range for getting column hover!");
    return fcolhover[col];
}
//---------------------------------------------------------------------------
void TWordListBase::set_colhover(byte col, bool val)
{
    if (col >= colcount)
        THROW(L"Column out of range for setting column hover!");
    if (fcolhover[col] == val)
        return;

    fcolhover[col] = val;

    if (fcolselected == val)
        fcolselected = -1;
    if (fheaderbuttoncolumn == val)
        fheaderbuttoncolumn = -1;

    invalidateheadercell(col);
}
//---------------------------------------------------------------------------
void TWordListBase::set_vertline(bool val)
{
    if (fvertline == val)
        return;
    fvertline = val;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListBase::set_horzline(bool val)
{
    if (fhorzline == val)
        return;
    fhorzline = val;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListBase::set_darkensel(bool val)
{
    if (fdarkensel == val)
        return;
    fdarkensel = val;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListBase::set_headerbutton(bool val)
{
    if (fheaderbutton == val)
        return;
    fheaderbutton = val;
    generate_arrows();
    if (!val)
    {
        fheaderbuttonhover = false;
        fheaderbuttoncolumn = -1;
        fheaderbuttondown = false;
        fheaderbuttoncolumnorder = hbdNone;
        fbox->Invalidate();
        delete arrowimg;
        arrowimg = NULL;
    }
    else
    {
        if (val && headersize < rowsize + 6 && fordered)
            headersize = rowsize + 6;
        else
            fbox->Invalidate();
    }
}
//---------------------------------------------------------------------------
void TWordListBase::set_headerbuttoncolor(TColor color)
{
    if (fheaderbuttoncolor == color)
        return;
    fheaderbuttoncolor = color;
    delete arrowimg;
    arrowimg = NULL;
    generate_arrows();
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListBase::set_ordered(bool val)
{
    if (val == fordered)
        return;
    fordered = val;
    generate_arrows();
    if (fheaderbutton)
    {
        if (val && inherited::headersize != rowsize + 6)
            inherited::headersize = rowsize + 6;
        else
            fbox->Invalidate();
    }
}
//---------------------------------------------------------------------------
int TWordListBase::get_rowsize()
{
    return inherited::rowsize;
}
//---------------------------------------------------------------------------
void TWordListBase::set_rowsize(int val)
{
    if (!fheaderbutton || !fordered)
        inherited::rowsize = val;
    else
        set_cols(0, NULL, NULL, val, val + 6, -1, true, true);
}
//---------------------------------------------------------------------------
int TWordListBase::get_headersize()
{
    return inherited::headersize;
}
//---------------------------------------------------------------------------
void TWordListBase::set_headersize(int val)
{
    if (!fheaderbutton || !fordered)
        inherited::headersize = val;
    else
        THROW(L"Cannot set header size of a list with ordered header buttons!");
}
//---------------------------------------------------------------------------
void TWordListBase::generate_arrows()
{
    if (!fordered || !fheaderbutton)
    {
        delete arrowimg;
        arrowimg = NULL;
        return;
    }
    if (arrowimg)
        return;

    byte *bits = NULL;

    try
    {
        bits = new byte[11 * 5 * 4 * 2];
        memset(bits, 0, sizeof(byte)*11*5*4*2);

        //0x88d0ff fheaderbuttoncolor
        //0x97faf3 inner
        //0x99e0f1 innermid
        //0x5caee8 innerdark
        //0x0061da outerdark
        unsigned int inner = colormod(fheaderbuttoncolor, -12, 42, 15), innermid = colormod(fheaderbuttoncolor, -17, 16, 14), innerdark = colormod(fheaderbuttoncolor, -45, -54, -23), outerdark = colormod(fheaderbuttoncolor, -37, -111, -136);

        for (int iy = 0; iy < 5; ++iy)
        {
            int lo = 5 - iy;
            int hi = 5 + iy;
            byte *ln = bits + iy * 22 * 4;
            byte *ln2 = bits + (4 - iy) * 22 * 4 + 11 * 4;
            for (int ix = lo; ix <= hi; ++ix)
            {
                ln2[ix * 4 + 3] = ln[ix * 4 + 3] = 255; //alpha
                if (iy == 0 || iy == 4)
                {
                    ln2[ix * 4] = ln[ix * 4] = FROMBLUE(outerdark); //blue
                    ln2[ix * 4 + 1] = ln[ix * 4 + 1] = FROMGREEN(outerdark); //green
                    ln2[ix * 4 + 2] = ln[ix * 4 + 2] = FROMRED(outerdark); //red
                }
                else if (ix == lo || ix == hi)
                {
                    ln2[ix * 4] = ln[ix * 4] = FROMBLUE(innerdark); //blue
                    ln2[ix * 4 + 1] = ln[ix * 4 + 1] = FROMGREEN(innerdark); //green
                    ln2[ix * 4 + 2] = ln[ix * 4 + 2] = FROMRED(innerdark); //red
                }
                else if (ix == lo + 1 || ix == hi - 1)
                {
                    ln2[ix * 4] = ln[ix * 4] = FROMBLUE(innermid); //blue
                    ln2[ix * 4 + 1] = ln[ix * 4 + 1] = FROMGREEN(innermid); //green
                    ln2[ix * 4 + 2] = ln[ix * 4 + 2] = FROMRED(innermid); //red
                }
                else
                {
                    ln2[ix * 4] = ln[ix * 4] = FROMBLUE(inner); //blue
                    ln2[ix * 4 + 1] = ln[ix * 4 + 1] = FROMGREEN(inner); //green
                    ln2[ix * 4 + 2] = ln[ix * 4 + 2] = FROMRED(inner); //red
                }
            }
            ln2[(lo - 1) * 4 + 3] = ln[(lo - 1) * 4 + 3] = ln2[(hi + 1) * 4 + 3] = ln[(hi + 1) * 4 + 3] = 128; //alpha
            ln2[(lo - 1) * 4] = ln[(lo - 1) * 4] = ln2[(hi + 1) * 4] = ln[(hi + 1) * 4] = (byte)FROMBLUE(outerdark) / 2; //blue
            ln2[(lo - 1) * 4 + 1] = ln[(lo - 1) * 4 + 1] = ln2[(hi + 1) * 4 + 1] = ln[(hi + 1) * 4 + 1] = (byte)FROMGREEN(outerdark) / 2; //green
            ln2[(lo - 1) * 4 + 2] = ln[(lo - 1) * 4 + 2] = ln2[(hi + 1) * 4 + 2] = ln[(hi + 1) * 4 + 2] = (byte)FROMRED(outerdark) / 2; //red
        }

        arrowimg = new TAlphaImageList(bits, 11, 5, 2);
    }
    catch (...)
    {
        arrowimg = NULL;
        MsgBox("Error occurred while creating order indicator bitmap.", "Error", MB_OK);
    }
    delete[] bits;
}
//---------------------------------------------------------------------------
TRect TWordListBase::selectionbox()
{
    TRect r;

    if (!fpos < 0)
        return r;

    if (fseltype != wlsMultiSelect || fmulsel == wlmsContinuous)
    {
        r = rowbox(selstart);
        r.Bottom += rowsize * (sellength - 1);
        if (r.Bottom < (header ? headersize : 0) || r.Top >= height)
            return Rect(0, 0, 0, 0);
    }
    else
    {
        r = Rect(0, 999999, fbox->ClientWidth, 0);

        int end = fselection.Prev(fbox->VPos + (fbox->ClientHeight - (header ? headersize : 0) + rowsize - 1) / rowsize - 1);
        if (end < fbox->VPos)
            return Rect(0, 0, 0, 0);

        int start = fselection.Next(fbox->VPos);
        r.top = (start - fbox->VPos) * rowsize + (header ? headersize : 0);
        r.bottom = (end - fbox->VPos) * rowsize + (header ? headersize : 0) + rowsize;

//        while (++start < count && r.bottom < fbox->ClientHeight)
//        {
//            int next = fselection.Next(start);
//            if (next == -1 || r.bottom + (next - start) * rowsize >= fbox->ClientHeight)
//                break;
//            r.bottom += (next - start + 1) * rowsize;
//        }

        if (fhorzline)
            --r.top;
    }

    return r;
}
//---------------------------------------------------------------------------
void TWordListBase::invalidateselected()
{
    TRect r = selectionbox();
    InvalidateRect(fbox->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TWordListBase::invalidateheadercell(int col)
{
    if (col < 0 || col >= colcount)
        return;
    if (!colvisible[col])
        return;

    TRect r = cellbox(col, -1);
    r.Left = max((long)0, r.Left - 1);

    if (r.Right < 0 || r.Left >= width || r.Top >= height || r.Bottom < 0)
        return;
    InvalidateRect(fbox->Handle, &r, false);
}
//---------------------------------------------------------------------------
void TWordListBase::set_headprepproc(TWordListBasePrepareDrawHeaderProc val)
{
    fheadprepare = val;
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
/*void TWordListBase::set_rowselect(bool val)
{
    if (val == frowselect)
        return;

    if (!val)
    {
        invalidateselected();
        frowselect = false;
        fpos = -1;
        fsellength = 0;
    }
    else
    {
        fpos = (count >= 1 && !fallownopos ? 0 : -1);
        fsellength = 1;
        frowselect = true;
        invalidateselected();
    }
}
//---------------------------------------------------------------------------
void TWordListBase::set_multiselect(bool val)
{
    if (val == fmultiselect)
        return;

    if (!val && frowselect)
    {
        invalidateselected();
        fsellength = 1;
    }
    fmultiselect = val;
}*/
//---------------------------------------------------------------------------
void TWordListBase::set_seltype(TWordListSelectionType seltype)
{
    if (fseltype == seltype)
        return;

    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        fselection.Clear();

    if (seltype == wlsNoSelection)
    {
        invalidateselected();

        fpos = -1;
        fsellength = 0;
    }
    else if (seltype == wlsSingleSelect)
    {
        invalidateselected();

        if (fpos < 0)
            fpos = (count >= 1 && !fallownopos ? 0 : -1);
        if (fpos >= 0)
            fsellength = 1;
        else
            fsellength = 0;
    }
    else if (seltype == wlsMultiSelect)
    {
        invalidateselected();
        if (fpos < 0)
        {
            fpos = (count >= 1 && !fallownopos ? 0 : -1);
            if (fpos >= 0)
                fsellength = 1;
            else
                fsellength = 0;
        }

        if (fmulsel != wlmsContinuous && fpos >= 0)
        {
            fselection.Clear();
            fselection.Items[fpos] = true;
        }
    }

    fseltype = seltype;
    invalidateselected();
}
//---------------------------------------------------------------------------
void TWordListBase::set_mulsel(TWordListMultiSelection mulsel)
{
    if (fmulsel == mulsel)
        return;

    if (fseltype != wlsMultiSelect || ((fmulsel == wlmsContinuousRandom || fmulsel == wlmsRandom) && (mulsel == wlmsContinuousRandom || mulsel == wlmsRandom)))
    {
        fmulsel = mulsel;
        return;
    }

    if (fmulsel != wlmsContinuous)
        fselection.Clear();

    if (mulsel == wlmsContinuous)
    {
        invalidateselected();

        if (fpos >= 0)
            fsellength = fselection.SequenceLength(fpos);
        fselection.Clear();

        if (fpos < 0)
        {
            fpos = (count >= 1 && !fallownopos ? 0 : -1);
            if (fpos >= 0)
                fsellength = 1;
            else
                fsellength = 0;
        }
    }
    else
    {
        invalidateselected();
        if (fpos >= 0)
            fselection.SetSequence(fsellength < 0 ? fpos + fsellength + 1 : fpos, abs(fsellength), true);
        else if (fpos >= 0)
            fselection.Items[fpos] = true;
        fsellength = count && fpos >= 0 ? 1 : 0;
    }

    fmulsel = mulsel;
    invalidateselected();
}
//---------------------------------------------------------------------------
int TWordListBase::get_selstart()
{
    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        return fselection.SequenceCount == 0 ? -1 : fselection.Sequences[0]->pos;

    if (fsellength < 0)
        return fpos + fsellength + 1;
    return pos;
}
//---------------------------------------------------------------------------
int TWordListBase::get_sellength()
{
    return !count ? 0 : fseltype != wlsMultiSelect || fmulsel == wlmsContinuous ? abs(fsellength) : fselection.SequenceCount == 0 ? 0 : fselection.Sequences[fselection.SequenceCount - 1]->pos - fselection.Sequences[0]->pos + fselection.Sequences[fselection.SequenceCount - 1]->len;
}
//---------------------------------------------------------------------------
bool TWordListBase::get_selected(int row)
{
    if (fseltype == wlsNoSelection || fpos < 0)
        return false;
    if (fseltype == wlsSingleSelect)
        return row == fpos;
    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        return fselection.Items[row];

    row -= selstart;
    return row >= 0 && row < sellength;
}
//---------------------------------------------------------------------------
void TWordListBase::set_selected(int row, bool sel)
{
    if (fseltype != wlsMultiSelect || fmulsel == wlmsContinuous || row < 0 || row >= count || fselection.Items[row] == sel)
        return;

    invalidaterow(row);
    fselection.Items[row] = sel;
    if (sel && fpos != row)
        set_pos2(row, false);
    else
        DoCallback();
}
//---------------------------------------------------------------------------
bool TWordListBase::get_colreverse()
{
    if (fcolselected < 0)
        return false;
    return fheaderbuttoncolumnorder == hbdUp;
}
//---------------------------------------------------------------------------
/*void TWordListBase::set_colreverse(bool val)
 {
 if (fcolselected < 0)
 return;
 fheaderbuttoncolumnorder = val ? 3 : 1;

 }*/
//---------------------------------------------------------------------------
int TWordListBase::get_pos()
{
    if (!count)
        return -1;
    return fseltype != wlsNoSelection ? fpos : fbox->VPos;
}
//---------------------------------------------------------------------------
void TWordListBase::set_pos(int val)
{
    set_pos2(val, fseltype == wlsMultiSelect && (fmulsel == wlmsContinuous && sellength > 1 || fmulsel != wlmsContinuous));
}
//---------------------------------------------------------------------------
void TWordListBase::set_pos2(int val, bool resetsel)
{
    val = MIN(MAX((fseltype != wlsNoSelection && fallownopos) ? -1 : 0, val), count - 1);

    if (fseltype == wlsNoSelection)
    {
        fbox->VPos = MIN(val, fbox->VMax);
        return;
    }

    int scrollpos = fbox->VPos;
    int frows = (height - (header ? headersize : 0)) / rowsize;

    if (val <= scrollpos && val >= 0)
        scrollpos = val;
    else if (val >= scrollpos + frows && val >= 0)
        scrollpos = val - frows + 1;

    if (val == fpos && (!resetsel || sellength == 1))
    {
        if (fbox->VPos != scrollpos)
            fbox->VPos = scrollpos;
        return;
    }

    TRect r = selectionbox();
    //int sstart;
    //int slength;
    if (resetsel || (fsellength == 0 && val != -1 && val < count))
    {
        //sstart = selstart;
        //slength = sellength;
        fsellength = 1;

        if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        {
            fselection.Clear();
            fselection.Items[val] = true;
        }
    }

    //int oldpos = fpos;
    fpos = val;

    if (fbox->VPos != scrollpos)
        fbox->VPos = scrollpos;
    else
    {
        TRect r2 = selectionbox();
        if (r.bottom <= r.top)
            r = r2;
        if (r2.bottom <= r2.top)
            r2 = r;
        if (r.bottom > r.top && r2.bottom > r2.top)
        {
            r.left = min(r.left, r2.left);
            r.top = min(r.top, r2.top);
            r.right = max(r.right, r2.right);
            r.bottom = max(r.bottom, r2.bottom);
        }
        if (r.bottom > r.top)
            InvalidateRect(fbox->Handle, &r, false);

/*
        if (resetsel)
        {

            TRect r = rowbox(sstart);
            r.Top = MAX(header ? headersize : 0, (int)r.Top);
            r.Bottom = MIN(height, (int)r.Bottom + (slength - 1) * rowsize);
            if (r.Bottom >= (header ? headersize : 0) && r.Top < height)
                InvalidateRect(fbox->Handle, &r, false);

            if (fpos < sstart || fpos >= sstart + slength)
                invalidaterow(fpos);
        }
        else
        {
            if (fsellength < 0)
            {
                if (oldpos <= fpos && fpos + fsellength <= oldpos || fpos <= oldpos && oldpos + fsellength <= fpos)
                    invalidaterows(min(fpos, oldpos) + fsellength + 1, abs(fpos - oldpos) - fsellength);
                else
                {
                    invalidaterows(fpos + fsellength + 1, fsellength);
                    invalidaterows(oldpos + fsellength + 1, fsellength);
                }
            }
            else
            {
                if (oldpos <= fpos && fpos <= oldpos + fsellength || fpos <= oldpos && oldpos <= fpos + fsellength)
                    invalidaterows(min(fpos, oldpos), abs(fpos - oldpos) + fsellength);
                else
                {
                    invalidaterows(fpos, fsellength);
                    invalidaterows(oldpos, fsellength);
                }
            }
        }
*/
    }

    DoCallback();
}
//---------------------------------------------------------------------------
void TWordListBase::set_allownopos(bool val)
{
    if (fallownopos == val)
        return;
    fallownopos = val;
    if (!fallownopos && pos == -1 && count > 0)
        pos = 0;
}
//---------------------------------------------------------------------------
/*void TWordListBase::set_colselected(int col)
 {
 if (fcolselected == col && col >= 0 && col < colcount || fcolselected == -1 && (col < 0 || col >= colcount))
 return;
 int oldcol = fcolselected;
 if (oldcol >= 0)
 invalidateheadercell(oldcol);

 if (col < 0 || col >= colcount)
 fcolselected = -1;
 else
 fcolselected = col;
 invalidateheadercell(col);
 }*/
//---------------------------------------------------------------------------
void TWordListBase::SelectOrderColumn(int col, bool reverse)
{
    if (fcolselected == -1 && (col < 0 || col >= colcount) || fheaderbuttoncolumnorder != hbdNone && reverse == (fheaderbuttoncolumnorder == hbdUp) && fcolselected == col)
    {
        if (fcolselected < 0)
            fheaderbuttoncolumnorder = hbdNone;
        return;
    }
    fheaderbuttoncolumnorder = reverse ? hbdUp : hbdDown;

    int oldcol = fcolselected;
    if (oldcol >= 0)
        invalidateheadercell(oldcol);

    if (col < 0 || col >= colcount)
    {
        fcolselected = -1;
        fheaderbuttoncolumnorder = hbdNone;
    }
    else
        fcolselected = col;

    invalidateheadercell(col);
}
//---------------------------------------------------------------------------
void TWordListBase::ToggleSelected(int selstart, int sellength, bool selected)
{
    if (fseltype != wlsMultiSelect || fmulsel == wlmsContinuous)
        return;
    if (!selected)
        invalidaterows(sellength < 0 ? selstart + sellength + 1 : selstart, abs(sellength));
    fselection.SetSequence(sellength < 0 ? selstart + sellength + 1 : selstart, abs(sellength), selected);
    if (selected)
        invalidaterows(sellength < 0 ? selstart + sellength + 1 : selstart, abs(sellength));
}
//---------------------------------------------------------------------------
bool TWordListBase::get_multiplesel()
{
    if (fseltype != wlsMultiSelect)
        return false;
    if (fmulsel == wlmsContinuous)
        return abs(fsellength) > 1;
    return !fselection.Single();
}
//---------------------------------------------------------------------------
int TWordListBase::get_seqcnt()
{
    if (fseltype != wlsMultiSelect || fmulsel == wlmsContinuous)
        return 0;
    return fselection.SequenceCount;
}
//---------------------------------------------------------------------------
const TSequenceItem& TWordListBase::get_selseq(int ix)
{
    if (fseltype != wlsMultiSelect || fmulsel == wlmsContinuous)
        THROW(L"Cannot get the selection sequence of a grid without random selecting.");
    return *fselection.Sequences[ix];
}
//---------------------------------------------------------------------------
void TWordListBase::DoColResizing(bool newval)
{
}
//---------------------------------------------------------------------------
void TWordListBase::DoSetColumnCount(const byte &acolcount, const byte &oldcolcount)
{
    bool *btmp = fresize;
    int *stmp = fdefsize;
    bool *htmp = fcolhover;

    if (!acolcount)
    {
        fresize = NULL;
        delete[] btmp;
        fdefsize = NULL;
        delete[] stmp;
        fcolhover = NULL;
        delete[] htmp;
    }
    else
    {
        if (fresizing)
        {
            fresize = new bool[acolcount];
            if (MIN(acolcount, oldcolcount))
            {
                if (btmp)
                    memcpy(fresize, btmp, sizeof(bool)*MIN(acolcount, oldcolcount));
                else
                    memset(fresize, true, sizeof(bool)*MIN(acolcount, oldcolcount));
            }
            delete[] btmp;
            if (acolcount > oldcolcount)
                memset(fresize + oldcolcount, true, sizeof(bool)*(acolcount - oldcolcount));
        }

        fdefsize = new int[acolcount];
        if (stmp && MIN(acolcount, oldcolcount))
            memcpy(fdefsize, stmp, sizeof(int)*MIN(acolcount, oldcolcount));
        delete[] stmp;
        if (acolcount > oldcolcount)
            memset(fdefsize + oldcolcount, 0, sizeof(int)*(acolcount - oldcolcount));

        fcolhover = new bool[acolcount];
        if (htmp && MIN(acolcount, oldcolcount))
            memcpy(fcolhover, htmp, sizeof(bool)*MIN(acolcount, oldcolcount));
        delete[] htmp;
        if (acolcount > oldcolcount)
            memset(fcolhover + oldcolcount, true, sizeof(bool)*(acolcount - oldcolcount));
    }

    inherited::DoSetColumnCount(acolcount, oldcolcount);
}
//---------------------------------------------------------------------------
void TWordListBase::DoPrepareDraw(int col, int row)
{
    Canvas->Brush->Style = bsSolid;
    Canvas->Brush->Color = (TColor)colorsettings.background;
    Canvas->Pen->Mode = pmCopy;
    Canvas->Pen->Style = psSolid;
    Canvas->Pen->Width = 1;
    Canvas->Pen->Color = (TColor)colorsettings.gridline;

    SetTextAlign(Canvas->Handle, TA_BASELINE | TA_LEFT);
    Canvas->Font->Style = TFontStyles();
    Canvas->Font->Name = settings.romajifont;
    Canvas->Font->Charset = DEFAULT_CHARSET;
    Canvas->Font->Size = double(rowsize) * 0.625 / scaleres;
    Canvas->Font->Color = (TColor)colorsettings.maintext;

    if (selected[row])
        Canvas->Brush->Color = focused ? (TColor)colorsettings.highlight : coloravg((TColor)colorsettings.highlight, (TColor)colorsettings.background, 0.8);
}
//---------------------------------------------------------------------------
void TWordListBase::DoDraw(int col, int row, TRect r)
{
    //right line
    TColor orig = Canvas->Pen->Color;
    if (fvertline)
    {
        Canvas->Pen->Color = coloravg(orig, (TColor)colorsettings.background, 0.6);
        Canvas->MoveTo(r.Right - 1, r.Top);
        Canvas->LineTo(r.Right - 1, r.Bottom - (fhorzline ? 1 : 0));
        r.Right--;
        Canvas->Pen->Color = orig;
    }

    //bottom line
    if (fhorzline)
    {
        if (fdarkensel && focused && (selected[row] && !selected[row + 1] || !selected[row] && selected[row + 1]))
            Canvas->Pen->Color = coloravginv(orig, (TColor)colorsettings.background, 1.6);
        Canvas->MoveTo(r.Left, r.Bottom - 1);
        Canvas->LineTo(r.Right + 1, r.Bottom - 1);
        r.Bottom--;
        Canvas->Pen->Color = orig;
    }

    DoDrawCell(col, row, r);
}
//---------------------------------------------------------------------------
void TWordListBase::DoPrepareDrawHeader(int col)
{
    if (/*!fordered &&*/fheaderbuttonhover && fheaderbuttoncolumn == col)
        Canvas->Pen->Color = coloravg(clBtnFace, fheaderbuttoncolor, 0.3);
    else if (/*!fordered &&*/fcolselected == col)
        Canvas->Pen->Color = coloravg(clBtnFace, fheaderbuttoncolor, 0.7);
    else
        Canvas->Pen->Color = clBtnFace;
    Canvas->Pen->Mode = pmCopy;
    Canvas->Pen->Style = psSolid;
    Canvas->Pen->Width = 1;

    Canvas->Font->Name = SystemFont;
    Canvas->Font->Charset = DEFAULT_CHARSET;

    if (fheaderbutton && headersize == rowsize + 6 && fordered)
        Canvas->Font->Size = double(headersize - 6) * 0.5 / scaleres;
    else
        Canvas->Font->Size = double(headersize) * 0.5 / scaleres;
    Canvas->Font->Color = clBtnText;

    Canvas->Brush->Style = bsSolid;
    if (/*!fordered &&*/fheaderbuttonhover && fheaderbuttoncolumn == col)
        Canvas->Brush->Color = coloravg(clBtnFace, fheaderbuttoncolor, 0.3);
    else if (/*!fordered &&*/fcolselected == col)
        Canvas->Brush->Color = coloravg(clBtnFace, fheaderbuttoncolor, 0.7);
    else
        Canvas->Brush->Color = clBtnFace;

    if (fheadprepare)
        fheadprepare(col, fbox->Canvas);
}
//---------------------------------------------------------------------------
void TWordListBase::DoDrawHeader(int col, TRect r)
{
    headrect(col, r);
    DoDrawHead(col, r);
}
//---------------------------------------------------------------------------
void TWordListBase::DrawHeadGen(int col, TRect r, UnicodeString str, TAlignment alignment)
{
    TCanvas *c = Canvas;

    bool pressed = fheaderbuttondown && fheaderbuttonhover && col == fheaderbuttoncolumn;
    TColor cfont = c->Font->Color;
    TColor oldfont = cfont;

    if (fheaderbutton && fordered)
    {
        TColor cp = c->Pen->Color;
        c->Pen->Color = COLORINC(cp, 0x0f0f0f);
        c->MoveTo(r.Left, r.Top);
        c->LineTo(r.Left, r.Bottom);
        r.Left++;
        c->Pen->Color = cp;

        TColor c1 = COLORINC(c->Brush->Color, 0x0f0f0f), c2 = COLORDEC(c->Brush->Color, 0x060606);
        GradientFillCanvas(c, c1, c2, Rect(r.Left, r.Top, r.Right, r.Bottom), gdVertical);
        fixtextcolor(cfont, coloravg(c1, c2, 0.5));

        bool above = fheaderbuttoncolumnorder == hbdDown && fcolselected == col;
        bool below = fheaderbuttoncolumnorder == hbdUp && fcolselected == col;
        if (fheaderbuttondown && fheaderbuttonhover)
        {
            above = (!above || below) && fheaderbuttoncolumn == col;
            below = fheaderbuttoncolumnorder == hbdDown && fcolselected == col && fheaderbuttoncolumn == col;
        }

        if (above)
            arrowimg->draw(c, r.Left + (r.Width() - arrowimg->width) / 2, r.Top + 2 + (fheaderbuttondown ? 1 : 0), 0);
        else if (below)
            arrowimg->draw(c, r.Left + (r.Width() - arrowimg->width) / 2, r.Top + r.Height() - 2 - arrowimg->height + (fheaderbuttondown ? 1 : 0), 1);

    }
    else
    {

        if (!pressed)
        {
            TColor c1 = COLORINC(c->Brush->Color, 0x101010), c2 = COLORINC(c->Brush->Color, 0x202020), c3 = c->Brush->Color, c4 = COLORDEC(c->Brush->Color, 0x101010);

            GradientFillCanvas(c, c1, c2, Rect(r.Left, r.Top, r.Right, r.Top + r.Height()*0.4 + 1), gdVertical);
            GradientFillCanvas(c, c3, c4, Rect(r.Left, r.Top + r.Height()*0.4 + 1, r.Right, r.Bottom), gdVertical);

            fixtextcolor(cfont, coloravg(c1, c3, 0.5));
        }
        else
        {
            TColor c1 = COLORDEC(c->Brush->Color, 0x464646), c2 = COLORDEC(c->Brush->Color, 0x060606);
            GradientFillCanvas(c, c1, c2, Rect(r.Left + 2, r.Top + 2, r.Right - 1, r.Bottom), gdVertical);

            fixtextcolor(cfont, coloravg(c1, c2, 0.5));

            TColor cp = c->Pen->Color;
            c->Pen->Color = COLORDEC(cp, 0x5a5a5a);
            c->MoveTo(r.Left + 1, r.Top + 1);
            c->LineTo(r.Right - 1, r.Top + 1);
            c->Pen->Color = COLORDEC(cp, 0x707070);
            c->MoveTo(r.Left + 1, r.Top);
            c->LineTo(r.Right - 1, r.Top);

            c->Pen->Color = COLORDEC(c->Pen->Color, 0x101010);
            c->MoveTo(r.Left, r.Top);
            c->LineTo(r.Left + 1, r.Top);
            c->MoveTo(r.Right - 1, r.Top);
            c->LineTo(r.Right, r.Top);

            c->Pen->Color = cp;

            c1 = COLORDEC(c1, 0x111111);
            c2 = COLORDEC(c2, 0x141414);
            GradientFillCanvas(c, c1, c2, Rect(r.Left + 1, r.Top + 2, r.Left + 2, r.Bottom), gdVertical);

            c1 = COLORDEC(c1, 0x181818);
            c2 = COLORDEC(c2, 0x1a1a1a);
            GradientFillCanvas(c, c1, c2, Rect(r.Left, r.Top + 1, r.Left + 1, r.Bottom), gdVertical);
            GradientFillCanvas(c, c1, c2, Rect(r.Right - 1, r.Top + 1, r.Right, r.Bottom), gdVertical);

        }
    }

    c->Font->Color = cfont;

    TBrushStyle os = c->Brush->Style;

    int x = (alignment == taLeftJustify ? r.left + 4 : alignment == taRightJustify ? r.Right - 5 : r.Left + r.Width() / 2);
    SetTextAlign(c->Handle, TA_BASELINE | (alignment == taLeftJustify ? TA_LEFT : alignment == taRightJustify ? TA_RIGHT : TA_CENTER));
    c->Brush->Style = bsClear;
    if (fheaderbutton && fordered)
    {
        bool above = fheaderbuttoncolumnorder == hbdDown && fcolselected == col;
        bool below = fheaderbuttoncolumnorder == hbdUp && fcolselected == col;
        if (fheaderbuttondown && fheaderbuttonhover)
        {
            above = (!above || below) && fheaderbuttoncolumn == col;
            below = fheaderbuttoncolumnorder == hbdDown && fcolselected == col && fheaderbuttoncolumn == col;
        }

        ExtTextOutW(c->Handle, x, headersize * 0.72 + (above ? 2 : below ? -4 : 0), ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
    }
    else
        ExtTextOutW(c->Handle, x + (pressed ? 2 : 0), headersize * 0.8 + (pressed ? 1 : 0), ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
    c->Brush->Style = os;

    c->Font->Color = oldfont;
}
//---------------------------------------------------------------------------
/* static */ void TWordListBase::DrawHeadNormal(TCanvas *c, int col, TRect r, UnicodeString str, TAlignment alignment)
{
// Head Rect
    TColor cp = c->Pen->Color;

    //top
    c->Pen->Color = COLORDEC(cp, 0x282828);
    c->MoveTo(r.Left, r.Top);
    c->LineTo(r.Right, r.Top);
    r.Top++;

    //bottom
    r.Bottom--;
    c->Pen->Color = COLORDEC(cp, 0x303030);
    c->MoveTo(r.Left, r.Bottom);
    c->LineTo(r.Right, r.Bottom);

    c->Pen->Color = cp;

    //right
    TColor cb = cp;

    GradientFillCanvas(c, COLORDEC(cb, 0x202020), COLORDEC(cb, 0x282828), Rect(r.Right - 1, r.Top, r.Right, r.Bottom), gdVertical);

    r.Right--;

    GradientFillCanvas(c, COLORINC(cp, 0x000000), COLORINC(cp, 0x101010), Rect(r.Left, r.Top, r.Left + 1, r.Bottom), gdVertical);
    r.Left++;

// Head Gen
    TColor cfont = c->Font->Color;
    TColor oldfont = cfont;

    TColor c1 = COLORINC(c->Brush->Color, 0x101010), c2 = COLORINC(c->Brush->Color, 0x202020), c3 = c->Brush->Color, c4 = COLORDEC(c->Brush->Color, 0x101010);

    GradientFillCanvas(c, c1, c2, Rect(r.Left, r.Top, r.Right, r.Top + r.Height() * 0.4 + 1), gdVertical);
    GradientFillCanvas(c, c3, c4, Rect(r.Left, r.Top + r.Height() * 0.4 + 1, r.Right, r.Bottom), gdVertical);

    fixtextcolor(cfont, coloravg(c1, c3, 0.5));

    c->Font->Color = cfont;

    TBrushStyle os = c->Brush->Style;

    int x = (alignment == taLeftJustify ? r.left + 4 : alignment == taRightJustify ? r.Right - 5 : r.Left + r.Width() / 2);
    SetTextAlign(c->Handle, TA_BASELINE | (alignment == taLeftJustify ? TA_LEFT : alignment == taRightJustify ? TA_RIGHT : TA_CENTER));
    c->Brush->Style = bsClear;
    ExtTextOutW(c->Handle, x, r.Height() * 0.8, ETO_CLIPPED, &r, str.c_str(), str.Length(), 0);
    c->Brush->Style = os;

    c->Font->Color = oldfont;
}
//---------------------------------------------------------------------------
void TWordListBase::headrect(int col, TRect &r)
{
    TCanvas *c = Canvas;

    TColor cp = c->Pen->Color;

    if (fheaderbutton && fordered)
    {
        //top
        c->Pen->Color = COLORDEC(cp, 0x242424);
        c->MoveTo(r.Left, r.Top);
        c->LineTo(r.Right, r.Top);
        r.Top++;

        //bottom
        r.Bottom--;
        c->Pen->Color = COLORDEC(cp, 0x2d2d2d);
        c->MoveTo(r.Left, r.Bottom);
        c->LineTo(r.Right, r.Bottom);
        c->Pen->Color = cp;

        //right
        if (fheaderbuttoncolumn != -1 && mouserow == -1 && (visiblecolindex[fheaderbuttoncolumn] == visiblecolindex[col] + 1 || fheaderbuttoncolumn == col))
            cp = coloravg(clBtnFace, fheaderbuttoncolor, 0.3);
        else if (fcolselected == col || fcolselected >= 0 && colvisible[fcolselected] && visiblecolindex[fcolselected] == visiblecolindex[col] + 1 && (fheaderbuttoncolumn < 0 || fheaderbuttoncolumn == col || visiblecolindex[fheaderbuttoncolumn] != visiblecolindex[col]))
            cp = coloravg(clBtnFace, fheaderbuttoncolor, 0.7);
        GradientFillCanvas(c, COLORDEC(cp, 0x242424), COLORDEC(cp, 0x2d2d2d), Rect(r.Right - 1, r.Top - 1, r.Right, r.Bottom + 1), gdVertical);
        r.Right--;

        return;
    }

    //top
    c->Pen->Color = COLORDEC(cp, 0x282828);
    c->MoveTo(r.Left, r.Top);
    c->LineTo(r.Right, r.Top);
    r.Top++;

    //bottom
    r.Bottom--;
    c->Pen->Color = COLORDEC(cp, 0x303030);
    c->MoveTo(r.Left, r.Bottom);
    c->LineTo(r.Right, r.Bottom);

    c->Pen->Color = cp;

    //right
    TColor cb = cp;
    if (fheaderbutton && fheaderbuttoncolumn != -1 && mouserow == -1 && (visiblecolindex[fheaderbuttoncolumn] == visiblecolindex[col] + 1 || fheaderbuttoncolumn == col))
    {
        cb = coloravg(clBtnFace, fheaderbuttoncolor, 0.5);
        GradientFillCanvas(c, COLORDEC(cb, 0x303030), COLORDEC(cb, 0x383838), Rect(r.Right - 1, r.Top - 1, r.Right, r.Bottom + 1), gdVertical);
    }
    else if (fcolselected == col || fcolselected >= 0 && colvisible[fcolselected] && visiblecolindex[fcolselected] == visiblecolindex[col] + 1 && (fheaderbuttoncolumn < 0 || fheaderbuttoncolumn == col || visiblecolindex[fheaderbuttoncolumn] != visiblecolindex[col]))
    {
        cb = coloravg(clBtnFace, fheaderbuttoncolor, 0.8);
        GradientFillCanvas(c, COLORDEC(cb, 0x303030), COLORDEC(cb, 0x383838), Rect(r.Right - 1, r.Top - 1, r.Right, r.Bottom + 1), gdVertical);
    }
    else
        GradientFillCanvas(c, COLORDEC(cb, 0x202020), COLORDEC(cb, 0x282828), Rect(r.Right - 1, r.Top, r.Right, r.Bottom), gdVertical);

    r.Right--;

    if (!fheaderbuttonhover || !fheaderbuttondown || col != fheaderbuttoncolumn)
    {
        GradientFillCanvas(c, COLORINC(cp, 0x000000), COLORINC(cp, 0x101010), Rect(r.Left, r.Top, r.Left + 1, r.Bottom), gdVertical);
        r.Left++;
    }

}
//---------------------------------------------------------------------------
void TWordListBase::DoMouseMove(int acol, int arow, TShiftState shift, int x, int y)
{
    if (fsizing)
    {
        inherited::colsize[fsizedcolumn] = MAX(4, x - columnleft(fsizedcolumn) + fbox->HPos);
        return;
    }

    byte viscnt = visiblecolcount;
    byte col = visiblecolindex[acol];
    byte lastcol = realcolindex[viscnt - 1];

    if (!fheaderbuttondown && fresize && header && arow == -1)
    {
        if (col >= viscnt && x + fbox->HPos < columnleft(lastcol) + colsize[lastcol] + 4)
            fsizedcolumn = lastcol;
        else if (col > 0 && col < viscnt && x + fbox->HPos < columnleft(acol) + 4)
            fsizedcolumn = realcolindex[col - 1];
        else if (col < viscnt && x + fbox->HPos > columnleft(acol) + colsize[acol] - 3)
            fsizedcolumn = acol;
        else
            fsizedcolumn = -1;
        if (fsizedcolumn >= 0 && (!fresize[fsizedcolumn]))
            fsizedcolumn = -1;

        if (fsizedcolumn >= 0)
            fbox->Cursor = crHSplit;
        else
            fbox->Cursor = crDefault;
    }

    if (fheaderbutton && header)
    {
        if (fheaderbuttoncolumn != -1 && (fsizedcolumn >= 0 || arow != -1 || mousecol != acol))
        {
            int oldhover = fheaderbuttoncolumn;
            fheaderbuttoncolumn = -1;
            fheaderbuttonhover = false;
            invalidateheadercell(oldhover);
        }

        if (fsizedcolumn < 0 && arow == -1 && mousecol == acol && fcolhover[acol])
        {
            fheaderbuttoncolumn = mousecol;
            fheaderbuttonhover = true;
            TRect r = cellbox(acol, arow);
            invalidateheadercell(mousecol);
        }
    }

    if (arow >= 0 || arow < -1)
    {
        fbox->Cursor = crDefault;
        fsizedcolumn = -1;
        inherited::DoMouseMove(acol, arow, shift, x, y);
        return;
    }

    inherited::DoMouseMove(acol, arow, shift, x, y);
}
//---------------------------------------------------------------------------
void TWordListBase::DoMouseDown(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y)
{
    if (fsizedcolumn >= 0)
    {
        if (button == mbLeft)
        {
            fsizing = true;
            if (shift.Contains(ssDouble))
            {
                inherited::colsize[fsizedcolumn] = fdefsize[fsizedcolumn];
                fsizing = false;
                ReleaseCapture();
            }
        }
        else
            fsizedcolumn = -1;
    }
    else if (fseltype != wlsNoSelection && (button == mbLeft || button == mbRight && (!fmultirightclick || !selected[arow])) && arow < count && (arow >= 0 || arow < -1 && fallownopos && !shift.Contains(ssShift)))
    {
        if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        {
            if (button == mbLeft && (shift.Contains(ssCtrl) || (fmulsel == wlmsRandom && allownopos)))
            {
                if (!allownopos && fpos == arow && fselection.Single())
                {
                    inherited::DoMouseDown(acol, arow, button, shift, x, y);
                    return;
                }
                bool toggled = fselection.Toggle(arow);
                if (toggled)
                {
                    //fpos = arow;
                    //DoCallback();
                    if (arow != fpos)
                        set_pos2(arow, false);
                    else
                    {
                        invalidaterow(fpos);
                        DoCallback();
                    }
                }
                else if (fpos == arow)
                {
                    int bpos = fselection.Next(arow);
                    if (bpos < 0)
                        bpos = fselection.Prev(arow);
                    set_pos2(bpos, false);
                    //DoCallback();
                    invalidaterow(arow);
                }
                else
                {
                    DoCallback();
                    invalidaterow(arow);
                }
            }
            else if (button == mbLeft && fmulsel == wlmsContinuousRandom && shift.Contains(ssShift))
            {
                invalidateselected();
                fselection.Clear();
                fselection.SetSequence(arow < fpos ? arow : fpos, abs(arow - fpos) + 1, true);
                //set_pos2(arow, false);
                DoCallback();
                invalidateselected();
            }
            else
                set_pos2(arow, true);
        }
        else
        {
            int ss = fsellength < 0 ? fpos + fsellength + 1 : fpos + fsellength - 1;
            invalidateselected();

            bool resetsel = false;
            if (button == mbLeft && fseltype == wlsMultiSelect && shift.Contains(ssShift))
                fsellength = arow < ss ? ss - arow + 1 : ss - arow - 1;
            else if (fsellength > 1 || fsellength < -1)
                resetsel = true;

            if (abs(fsellength) > 1)
            {
                TRect r = rowbox(fsellength > 0 ? arow + 1 : arow + fsellength + 2);
                r.Bottom += (abs(fsellength) - 2) * rowsize;
                r.Top = MAX((int)r.Top, (header ? headersize : 0));
                r.Bottom = MIN((int)r.Bottom, height);
                if (r.Top < height && r.Bottom >= (header ? headersize : 0))
                    InvalidateRect(fbox->Handle, &r, false);
            }
            set_pos2(arow, resetsel);
        }
    }
    else if (fheaderbutton && button == mbLeft && fheaderbuttonhover && acol == fheaderbuttoncolumn)
    {
        fheaderbuttondown = true;
        invalidateheadercell(fheaderbuttoncolumn);
    }

    inherited::DoMouseDown(acol, arow, button, shift, x, y);
}
//---------------------------------------------------------------------------
void TWordListBase::DoMouseUp(int acol, int arow, TMouseButton button, TShiftState shift, int x, int y)
{
    if (fsizing)
    {
        DoMouseMove(acol, arow, shift, x, y);
        fsizing = false;
    }
    if (fheaderbuttondown)
    {
        fheaderbuttondown = false;
        if (fheaderbuttoncolumn != -1)
        {
            if (fheaderbuttonhover)
            {
                int oldcol = fcolselected;
                if (fordered)
                    fheaderbuttoncolumnorder = fheaderbuttoncolumnorder == hbdDown && fcolselected == acol ? hbdUp : hbdDown;
                fcolselected = acol;
                if (fheaderpressproc)
                    fheaderpressproc(oldcol);
                if (oldcol != acol)
                {
                    invalidateheadercell(oldcol);
                    invalidateheadercell(acol);
                    if (fheaderselectproc)
                        fheaderselectproc(oldcol, acol);
                }
            }
            invalidateheadercell(fheaderbuttoncolumn);
        }
    }

    inherited::DoMouseUp(acol, arow, button, shift, x, y);
}
//---------------------------------------------------------------------------
void TWordListBase::DoMouseLeave()
{
    if (fheaderbutton && fheaderbuttonhover && fheaderbuttoncolumn >= 0)
    {
        int oldhover = fheaderbuttoncolumn;
        fheaderbuttonhover = false;
        fheaderbuttoncolumn = -1;
        invalidateheadercell(oldhover);
    }

    inherited::DoMouseLeave();
}
//---------------------------------------------------------------------------
void TWordListBase::DoKeyPress(wchar_t &Key)
{
    if (Key == 0x01 && fseltype == wlsMultiSelect) // Ctrl-A
        SelectAll();
    if (Key == 0x04 && fseltype == wlsMultiSelect && abs(sellength) > 1) // Ctrl-D
        DeselectAll();

    inherited::DoKeyPress(Key);
}
//---------------------------------------------------------------------------
void TWordListBase::DoKeyDown(WORD &Key, TShiftState Shift)
{
    switch (Key)
    {
    case VK_LEFT:
        fbox->HPos -= fbox->HorzScrollBar->SmallChange;
        break;
    case VK_RIGHT:
        fbox->HPos += fbox->HorzScrollBar->SmallChange;
        break;
    case VK_UP:
        if (fseltype == wlsMultiSelect && fmulsel == wlmsRandom && fallownopos)
            fbox->VPos -= fbox->VertScrollBar->SmallChange;
        else if (fseltype != wlsNoSelection)
            pos = MAX(0, MIN(fpos, selstart) - 1);
        else
            pos = MAX(0, fbox->VPos - 1);
        break;
    case VK_DOWN:
        if (fseltype == wlsMultiSelect && fmulsel == wlmsRandom && fallownopos)
            fbox->VPos += fbox->VertScrollBar->SmallChange;
        else if (fseltype != wlsNoSelection)
            pos = MAX(fpos, selstart + sellength - 1) + 1;
        else
            pos = fbox->VPos + 1;
        break;
    case VK_PRIOR:
        if (fseltype == wlsMultiSelect && fmulsel == wlmsRandom && fallownopos)
            fbox->VPos -= fbox->VertScrollBar->LargeChange;
        else if (fseltype != wlsNoSelection)
            pos = MAX(0, MIN(fpos, selstart) - fbox->VertScrollBar->LargeChange);
        else
            pos = pos - fbox->VertScrollBar->LargeChange;
        break;
    case VK_NEXT:
        if (fseltype == wlsMultiSelect && fmulsel == wlmsRandom && fallownopos)
            fbox->VPos += fbox->VertScrollBar->LargeChange;
        else if (fseltype != wlsNoSelection)
            pos = MAX(fpos, selstart + sellength - 1) + fbox->VertScrollBar->LargeChange;
        else
            pos = pos + fbox->VertScrollBar->LargeChange;
        break;
    case VK_HOME:
        if (fseltype == wlsMultiSelect && fmulsel == wlmsRandom && fallownopos)
            fbox->VPos = 0;
        else
            pos = 0;
        break;
    case VK_END:
        if (fseltype == wlsMultiSelect && fmulsel == wlmsRandom && fallownopos)
            fbox->VPos = count;
        else
            pos = count - 1;
        break;
//    case 'A':
//        if (Shift.Contains(ssCtrl) && fseltype == wlsMultiSelect)
//            SelectAll();
//        break;
//    case 'D':
//        if (Shift.Contains(ssCtrl) && fseltype == wlsMultiSelect && abs(sellength) > 1)
//            DeselectAll();
//        break;
    }

    inherited::DoKeyDown(Key, Shift);
}
//---------------------------------------------------------------------------
void TWordListBase::SelectAll()
{
    Update(0, count);
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListBase::DeselectAll()
{
    Update(fallownopos ? -1 : pos, fallownopos ? 0 : 1);
    fbox->Invalidate();
}
//---------------------------------------------------------------------------
void TWordListBase::Update(int row)
{
    int cnt = count;
    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        fselection.Limit(cnt);
    update_count(cnt); // Updates count in lists.

    if (row >= cnt)
        row = cnt - 1;

    count = cnt;

    if (pos != row)
        pos = row;
    else
    {
        fbox->Invalidate();
        DoCallback();
    }
}
//---------------------------------------------------------------------------
void TWordListBase::Update(int aselstart, int asellength)
{
    int cnt = count;
    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        fselection.Limit(cnt);
    update_count(cnt); // Updates count in lists.

    if (aselstart >= cnt)
        aselstart = cnt;

    if (!fallownopos && aselstart < 0)
    {
        aselstart = 0;
        asellength = 1;
    }

    if ((!fallownopos || aselstart >= 0) && aselstart + 1 + asellength < 0)
        asellength = -aselstart - 1;
    else if (aselstart + asellength > count)
        aselstart = cnt - asellength;

    count = cnt;

    //fpos += (fsellength < 0 ? fsellength+1 : 0); // keep here, needed for oldpos<->fpos comparison in set_pos2

    invalidateselected();
    if ((fseltype != wlsMultiSelect || fmulsel == wlmsContinuous) && fseltype != wlsNoSelection)
        fsellength = asellength;
    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
    {
        fselection.Clear();
        if (aselstart >= 0)
            fselection.SetSequence(asellength < 0 ? aselstart + asellength + 1 : aselstart, abs(asellength), true);
    }
    set_pos2(aselstart, false);
    DoCallback();
}
//---------------------------------------------------------------------------
void TWordListBase::DoCallback()
{
    if (ffunc)
        ffunc();
}
//---------------------------------------------------------------------------
void TWordListBase::DoActivate(bool focused)
{
    if (focused)
        invalidateselected();

    inherited::DoActivate(focused);
}
//---------------------------------------------------------------------------
void TWordListBase::DoDeactivate(bool focused)
{
    if (fheaderbuttondown)
    {
        fheaderbuttondown = false;
        invalidateheadercell(fheaderbuttoncolumn);
    }

    if (focused)
        invalidateselected();

    fsizing = false;

    inherited::DoDeactivate(focused);
}
//---------------------------------------------------------------------------
void TWordListBase::DoSetCount(int &acount, int &oldcount)
{
    if ((fseltype != wlsNoSelection || !fallownopos) && fpos < 0)
    {
        if (fpos >= acount)
            fpos = acount - 1;
        if (abs(fsellength) != 1)
            fsellength = 1;
        if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        {
            fselection.Clear();
            if (fpos >= 0)
                fselection.Items[fpos] = true;
        }
    }
    if (fseltype == wlsMultiSelect && fmulsel != wlmsContinuous)
        fselection.Limit(acount);

    inherited::DoSetCount(acount, oldcount);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(181);
#endif

