//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "listcontrols.h"
#include "wordobj.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(96);
#endif

//The classes in this unit are only accessible by TWordListObjBase ancestor classes

//---------------------------------------------------------------------------
TCustomListControlObj::TCustomListControlObj(TWordListObjBase *aowner) : fowner(aowner)
{
    checkboximages = new TDrawnCheckbox();
    radioboximages = new TDrawnRadiobox();
}
//---------------------------------------------------------------------------
TCustomListControlObj::~TCustomListControlObj()
{
    delete checkboximages;
    delete radioboximages;
}
//---------------------------------------------------------------------------
void TCustomListControlObj::drawcheckbox(TCanvas *c, int x, int y, TDrawnControlboxState state)
{
    checkboximages->draw(c, x, y, state);
}
//---------------------------------------------------------------------------
void TCustomListControlObj::drawcheckbox(TCanvas *c, int x, int y, TDrawnControlboxState state, TColor bk)
{
    checkboximages->draw(c, x, y, state, bk);
}
//---------------------------------------------------------------------------
void TCustomListControlObj::drawradiobox(TCanvas *c, int x, int y, TDrawnControlboxState state)
{
    radioboximages->draw(c, x, y, state);
}
//---------------------------------------------------------------------------
void TCustomListControlObj::drawradiobox(TCanvas *c, int x, int y, TDrawnControlboxState state, TColor bk)
{
    radioboximages->draw(c, x, y, state, bk);
}
//---------------------------------------------------------------------------
int TCustomListControlObj::get_checkboxwidth()
{
    return checkboximages->width;
}
//---------------------------------------------------------------------------
int TCustomListControlObj::get_checkboxheight()
{
    return checkboximages->height;
}
//---------------------------------------------------------------------------
int TCustomListControlObj::get_radioboxwidth()
{
    return radioboximages->width;
}
//---------------------------------------------------------------------------
int TCustomListControlObj::get_radioboxheight()
{
    return radioboximages->height;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
TListCheckboxObj::TListCheckboxObj(TWordListObjBase *aowner, TListCustomControlChanged acallback) : inherited(aowner), columns(false), fcallback(acallback), mouserow(-1), mouseix(-1), gcallback(NULL), mousehover(false), buttondown(false), fmulticheck(false), fmultichecksel(false), fchecked(NULL), fenabled(NULL)
{
}
//---------------------------------------------------------------------------
TListCheckboxObj::TListCheckboxObj(TWordListObjBase *aowner, TListCustomControlChanged acallback, TListVirtualCheckControlGetter agcallback) : inherited(aowner), columns(false), fcallback(acallback), gcallback(agcallback), mouserow(-1), mouseix(-1), mousehover(false), buttondown(false), fmulticheck(false), fmultichecksel(false), fchecked(NULL), fenabled(NULL)
{
}
//---------------------------------------------------------------------------
TListCheckboxObj::~TListCheckboxObj()
{
    for (unsigned int ix = 0; ix < columns.TrueCount && fchecked; ++ix)
    {
        delete[] fchecked[ix];
        delete[] fenabled[ix];
    }
    delete[] fchecked;
    delete[] fenabled;
}
//---------------------------------------------------------------------------
bool TListCheckboxObj::get_column(byte col)
{
    if (gcallback)
        return gcallback(INT_MAX, col, ccwvBoxColumn);
    return columns.Items[col];
}
//---------------------------------------------------------------------------
void TListCheckboxObj::set_column(byte col, bool val)
{
    if (gcallback)
        return;

    bool c = columns.Items[col];
    if (c == val)
        return;

    bool **ctmp = fchecked;
    bool **etmp = fenabled;
    if (val || columns.TrueCount > 1)
    {
        try
        {
            fchecked = new bool*[columns.TrueCount + (val ? 1 : -1)];
            fenabled = new bool*[columns.TrueCount + (val ? 1 : -1)];
            bool done = false;
            for (unsigned int ix = 0; ix < columns.TrueCount; ++ix)
            {
                if (columns.TruePos[ix] < (unsigned int)col)
                {
                    fchecked[ix] = ctmp[ix];
                    fenabled[ix] = etmp[ix];
                }
                else if (columns.TruePos[ix] >= (unsigned int)col && !done)
                {
                    done = true;
                    if (!val)
                    {
                        delete[] ctmp[ix];
                        delete[] etmp[ix];
                    }
                    else
                    {
                        if (owner->count)
                        {
                            fchecked[ix] = new bool[owner->count];
                            memset(fchecked + ix, 0, sizeof(bool)*owner->count);
                            fenabled[ix] = new bool[owner->count];
                            memset(fenabled + ix, 1, sizeof(bool)*owner->count);
                        }
                        else
                        {
                            fchecked[ix] = NULL;
                            fenabled[ix] = NULL;
                        }

                        fchecked[ix + 1] = ctmp[ix];
                        fenabled[ix + 1] = etmp[ix];
                    }
                }
                else
                {
                    fchecked[ix + (val ? 1 : -1)] = ctmp[ix];
                    fenabled[ix + (val ? 1 : -1)] = etmp[ix];
                }

            }
            if (!done && val)
            {
                if (owner->count)
                {
                    fchecked[columns.TrueCount] = new bool[owner->count];
                    memset(fchecked[columns.TrueCount], 0, sizeof(bool)*owner->count);
                    fenabled[columns.TrueCount] = new bool[owner->count];
                    memset(fenabled[columns.TrueCount], 1, sizeof(bool)*owner->count);
                }
                else
                {
                    fchecked[columns.TrueCount] = NULL;
                    fenabled[columns.TrueCount] = NULL;
                }
            }
        }
        catch (...)
        {
            ;
        }
    }
    delete[] ctmp;
    delete[] etmp;

    if (!val)
        columns.ClearIndex(col);
    else
        columns.Items[col] = true;

    if (!columns.TrueCount)
    {
        fchecked = NULL;
        fenabled = NULL;
    }

    if (mouseix != -1 && !columns.ItemSet(mouseix))
        mouseix = -1;
}
//---------------------------------------------------------------------------
void TListCheckboxObj::changed(int count, int lastcount)
{
    if (gcallback)
        return;

    if (count == lastcount)
        THROW(L"?!");

    if (!columns.TrueCount)
        return;

    if (count)
    {
        bool **ctmp = fchecked;
        bool **etmp = fenabled;
        fchecked = new bool*[columns.TrueCount];
        fenabled = new bool*[columns.TrueCount];
        for (unsigned int ix = 0; ix < columns.TrueCount; ++ix)
        {
            fchecked[ix] = new bool[count];
            fenabled[ix] = new bool[count];
            if (lastcount)
            {
                memcpy(fchecked[ix], ctmp[ix], sizeof(bool)*min(count, lastcount));
                memcpy(fenabled[ix], etmp[ix], sizeof(bool)*min(count, lastcount));
                delete[] ctmp[ix];
                delete[] etmp[ix];
            }
            if (count > lastcount)
            {
                memset(fchecked[ix] + lastcount, 0, sizeof(bool)*(count - lastcount));
                memset(fenabled[ix] + lastcount, 1, sizeof(bool)*(count - lastcount));
            }
        }
        delete[] ctmp;
        delete[] etmp;
    }
    else
    {
        if (columns.TrueCount)
        {
            for (unsigned int ix = 0; ix < columns.TrueCount; ++ix)
            {
                delete[] fchecked[ix];
                delete[] fenabled[ix];
            }
            delete[] fchecked;
            delete[] fenabled;
        }
        fchecked = NULL;
        fenabled = NULL;
    }

    if (mouserow >= count)
        mouserow = -1;
    if (mouseix != -1 && !columns.ItemSet(mouseix))
        mouseix = -1;
}
//---------------------------------------------------------------------------
bool TListCheckboxObj::get_checked(unsigned int row, byte ix)
{
    if (gcallback)
        return gcallback(row, ix, ccwvChecked);

    if (row >= (unsigned int)owner->count || !columns.ItemSet(ix))
        THROW(L"Checkbox position out of range!");

    return fchecked[columns.Pos[ix]][row];
}
//---------------------------------------------------------------------------
bool TListCheckboxObj::get_enabled(unsigned int row, byte ix)
{
    if (gcallback)
        return gcallback(row, ix, ccwvEnabled);

    if (row >= (unsigned int)owner->count || !columns.ItemSet(ix))
        THROW(L"Checkbox position out of range!");
    return fenabled[columns.Pos[ix]][row];
}
//---------------------------------------------------------------------------
void TListCheckboxObj::set_checked(unsigned int row, byte ix, bool val)
{
    if (row >= (unsigned int)owner->count || (!gcallback && !columns.ItemSet(ix)) || (gcallback && !gcallback(INT_MAX, ix, ccwvBoxColumn)))
        THROW(L"Checkbox position out of range!");

    if ((gcallback && gcallback(row, ix, ccwvChecked) == val) || (!gcallback && fchecked[ix][row] == val))
        return;

    int ix2 = gcallback ? ix : columns.Pos[ix];
    if (!gcallback)
        fchecked[ix2][row] = val;
    if (fcallback && ((!gcallback && fenabled[ix2][row]) || (gcallback && gcallback(INT_MAX, ix, ccwvEnabled))))
    {
        fcallback(row, ix, val);
        if (!gcallback && fchecked[ix2][row] != val)
        {
            fchecked[ix2][row] = val;
            return;
        }
    }
    owner->invalidatecell(ix, row);
}
//---------------------------------------------------------------------------
void TListCheckboxObj::set_enabled(unsigned int row, byte ix, bool val)
{
    if (gcallback)
    {
        owner->invalidatecell(ix, row);
        return;
    }

    if (row >= (unsigned int)owner->count || !columns.ItemSet(ix))
        THROW(L"Checkbox position out of range!");

    int ix2 = columns.Pos[ix];
    if (fenabled[ix2][row] == val)
        return;

    fenabled[ix2][row] = val;
    owner->invalidatecell(ix, row);
}
//---------------------------------------------------------------------------
void TListCheckboxObj::draw(TCanvas *c, int col, int row, TRect &r)
{
    if ((!gcallback && !columns.Items[col]) || (gcallback && !gcallback(row, col, ccwvBoxColumn)))
        return;

    TRect cell = r;
    int leftmargin = 2 * scaleres;
    HRGN rgn = CreateRectRgn(cell.left, cell.top, min(cell.right, cell.left + checkboxwidth + leftmargin), cell.bottom);
    HRGN oldrgn;
    if (!GetClipRgn(c->Handle, oldrgn))
        oldrgn = 0;
    SelectClipRgn(c->Handle, rgn);
    DeleteObject(rgn);
    int t = cell.top + (cell.Height() - checkboxheight) / 2;
    c->FillRect(Rect(cell.left + leftmargin, cell.top, cell.left + checkboxwidth + leftmargin, t));
    c->FillRect(Rect(cell.left, cell.top, cell.left + leftmargin, cell.bottom));

    int col2 = columns.Pos[col];
    bool enabl = !gcallback ? fenabled[col2][row] : gcallback(row, col, ccwvEnabled);
    bool check = !gcallback ? fchecked[col2][row] : gcallback(row, col, ccwvChecked);
    bool hover = mouserow == row && mouseix == col && mousehover;
    bool down = hover && buttondown;

    TDrawnControlboxState s;
    if (!enabl)
    {
        s = check ? dcsDisabledChecked : dcsDisabled;
    }
    else if (!check)
    {
        if (down)
            s = dcsEmptyDown;
        else if (hover)
            s = dcsEmptyHovered;
        else
            s = dcsEmpty;
    }
    else
    {
        if (down)
            s = dcsCheckedDown;
        else if (hover)
            s = dcsCheckedHovered;
        else
            s = dcsChecked;
    }

    drawcheckbox(c, cell.left + leftmargin, t, s, c->Brush->Color);
    c->FillRect(Rect(cell.left + leftmargin, t + checkboxheight, cell.left + checkboxwidth + leftmargin, cell.bottom));

    r.left = min(r.right, r.left + checkboxwidth + leftmargin);
    SelectClipRgn(c->Handle, oldrgn);
    if (oldrgn)
        DeleteObject(oldrgn);
}
//---------------------------------------------------------------------------
bool TListCheckboxObj::mouseincontrol()
{
    return mouserow >= 0 && mouseix >= 0 && ((!gcallback && fenabled[columns.Pos[mouseix]][mouserow]) || (gcallback && gcallback(mouserow, mouseix, ccwvEnabled)));
}
//---------------------------------------------------------------------------
void TListCheckboxObj::mousedown(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y)
{
    buttondown = true;
    if (!mouseincontrol())
        return;

    owner->invalidatecell(acol, arow);
}
//---------------------------------------------------------------------------
void TListCheckboxObj::mousecontrolpos(int col, int row, int x, int y, int &cix, int &crow)
{
    cix = -1;
    crow = -1;
    if (col < 0 || row < 0 || (!gcallback && !columns.Items[col]) || (gcallback && !gcallback(row, col, ccwvBoxColumn)))
        return;

    int leftmargin = 2 * scaleres;
    TRect cell = owner->cellbox(col, row);
    if (owner->vertline)
        cell.right--;
    if (owner->horzline)
        cell.bottom--;
    int t = cell.top + (cell.Height() - checkboxheight) / 2;
    if (x < cell.left + leftmargin || cell.left + checkboxwidth + leftmargin <= x || y < t || y >= t + checkboxheight)
        return;

    int col2 = gcallback ? col : columns.Pos[col];
    if ((!gcallback && !fenabled[col2][row]) || (gcallback && !gcallback(row, col, ccwvEnabled)))
        return;
    crow = row;
    cix = col;
}
//---------------------------------------------------------------------------
void TListCheckboxObj::mousemove(int acol, int arow, TShiftState Shift, int x, int y)
{
    int newix;
    int newrow;
    mousecontrolpos(acol, arow, x, y, newix, newrow);
    if (!buttondown)
    {
        if (newix == mouseix && newrow == mouserow)
        {
            if (newix < 0 || newrow < 0)
                mousehover = false;
            return;
        }
        swap(newrow, mouserow);
        swap(newix, mouseix);

        mousehover = mouserow >= 0 && mouseix >= 0;
        if (mousehover)
        {
            owner->invalidatecell(mouseix, mouserow);
        }
        if (newrow >= 0 && newix >= 0)
        {
            owner->invalidatecell(newix, newrow);
        }
    }
    else
    {
        if (mouseix < 0 || mouserow < 0)
            return;

        if (mousehover != (newix == mouseix && newrow == mouserow))
        {
            mousehover = (newix == mouseix && newrow == mouserow);
            owner->invalidatecell(mouseix, mouserow);
        }
    }
}
//---------------------------------------------------------------------------
void TListCheckboxObj::mouseup(int acol, int arow, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (!buttondown)
        return;

    buttondown = false;
    if (!mousehover || (!gcallback && columns.TrueCount == 0))
        return;

    int acol2 = !gcallback ? columns.Pos[acol] : acol;

    if (owner->multipleselected && owner->selected[arow] && (fmulticheck || fmultichecksel))
    {
        bool check = !gcallback ? !fchecked[acol2][arow] : !gcallback(arow, acol, ccwvChecked);
        for (int seqix = 0; seqix < owner->selseqcount; ++seqix)
        {
            const TSequenceItem &seq = owner->selseq[seqix];
            for (int rowy = seq.pos; rowy < seq.pos + seq.len; ++rowy)
            {
                unsigned int mx = fmultichecksel ? 0 : acol;
                int mx2 = !gcallback ? columns.Pos[mx] : mx;
                do
                {
                    if ((!gcallback && fenabled[mx2][rowy] && fchecked[mx2][rowy] != check) || (gcallback && gcallback(rowy, mx, ccwvEnabled) && gcallback(rowy, mx, ccwvChecked) != check))
                    {
                        if (!gcallback)
                            fchecked[mx2][rowy] = check;
                        if (fcallback)
                        {
                            fcallback(rowy, mx, check);
                            if (!gcallback && fchecked[mx2][rowy] != check)
                                fchecked[mx2][rowy] = check;
                            else
                                owner->invalidatecell(mx, rowy);
                        }
                        else
                            owner->invalidatecell(mx, rowy);
                    }
                    ++mx;
                    if (!gcallback)
                    {
                        if (columns.Count > mx)
                            mx2 = columns.Pos[mx];
                    }
                    else
                        ++mx2;
                }
                while (fmultichecksel ? mx < columns.Count : false);
            }
        }
    }
    else
    {
        if (!gcallback)
            fchecked[acol2][arow] = !fchecked[acol2][arow];
        if (fcallback)
        {
            if (!gcallback)
            {
                bool checked = fchecked[acol2][arow];
                fcallback(arow, acol, checked);
                if (fchecked[acol2][arow] != checked)
                {
                    fchecked[acol2][arow] = checked;
                    return;
                }
            }
            else
            {
                bool checked = !gcallback(arow, acol2, ccwvChecked);
                fcallback(arow, acol, checked);
            }
        }
        owner->invalidatecell(acol, arow);
    }
}
//---------------------------------------------------------------------------
void TListCheckboxObj::mouseleave()
{
    if (mouseix >= 0 && mouserow >= 0)
    {
        int oldix = mouseix;
        int oldrow = mouserow;
        mouseix = -1;
        mouserow = -1;
        owner->invalidatecell(oldix, oldrow);
    }
}
//---------------------------------------------------------------------------
void TListCheckboxObj::deactivated()
{
    if (buttondown)
    {
        buttondown = false;
        if (mousehover)
            owner->invalidatecell(mouseix, mouserow);
    }
}
//---------------------------------------------------------------------------
void TListCheckboxObj::updatecoordinates(int acol, int arow, int &x, int &y)
{
    if (acol < 0 || arow < 0 || ((!gcallback && !columns.Items[acol]) || (gcallback && !gcallback(INT_MAX, acol, ccwvBoxColumn))))
        return;

    x -= 2 * scaleres + checkboxwidth;
    if (x < 0)
        y = -1;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


#ifdef _DEBUG
static TGlobalTest tester2(97);
#endif

