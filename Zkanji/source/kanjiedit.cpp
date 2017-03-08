//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <Themes.hpp>
#include <vsstyle.h>

#include "kanjiedit.h"
#include "romajizer.h"
#include "dropdown.h"
#include "myfonts.h"
#include "settings.h"
#include "module.h"
#include "recognizer.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(76);
#endif

bool HideInfoWindows(bool inkeydown = false);
bool InfoWindowVisible();

//---------------------------------------------------------------------------
TKanjiEditList::TKanjiEditList() : keditcharset(SHIFTJIS_CHARSET)
{
    ;
}
//---------------------------------------------------------------------------
TKanjiEditList::~TKanjiEditList()
{
    ;
}
//---------------------------------------------------------------------------

TKanjiEditList *kanjieditlist = NULL;

extern bool hasrecdata;

TKanjiEdit::TKanjiEdit(TZPaintBox *apb, bool acceptkanji, bool acceptgeneral, TDropDownList *adropdown, TFuncPtr acallback) :
        pb(apb), xpos(0), charwidth(0), fkanji(acceptkanji), fgeneral(acceptgeneral), bstate(dbsNormal), mousedown(false),
        fcallback(acallback), fdropdown(adropdown), ftext(L""), fpos(0), fmax(0), fselstart(0),
        fsellength(0), fselecting(false), fcollection(0), frecognizebutton(NULL),
        frecognizer(false), fdroppos(-1), defaultcontext(0)/*,context(0)*/
{
    if (!kanjieditlist)
    {
        kanjieditlist = new TKanjiEditList;
        if (systemfontindex >= 0)
        {
            kanjieditlist->keditfont = jfonts->Strings[systemfontindex];
            kanjieditlist->keditcharset = (TFontCharset)jfonts->Objects[systemfontindex];
        }
        else
        {
            kanjieditlist->keditfont = settings.kanafont;
            kanjieditlist->keditcharset = settings.kanacharset;
        }
    }
    kanjieditlist->Add(this);
    pb->Cursor = crIBeam;

    textarea = pb->ClientRect;
    textarea.Left += 1;
    textarea.Right -= 1;

    TThemeServices *t = ThemeServices();
    int htheme = (t && t->ThemesAvailable && t->ThemesEnabled);

    if (fdropdown)
    {
        btnwidth = GetSystemMetrics(SM_CXVSCROLL);
        textarea.Right -= btnwidth + (htheme ? 1 : 0);
    }

    foldbuttonclick = NULL;

    foldpaint = pb->OnPaint;
    foldenter = pb->OnEnter;
    foldexit = pb->OnExit;
    foldmouseleave = pb->OnMouseLeave;
    foldmousedown = pb->OnMouseDown;
    foldmouseup = pb->OnMouseUp;
    foldmousemove = pb->OnMouseMove;
    foldkeydown = pb->OnKeyDown;
    foldkeypress = pb->OnKeyPress;
    foldresize = pb->OnResize;

    ((TBaseForm*)pb->Owner)->AddEvent(this, Activate, eltActivate);
    ((TBaseForm*)pb->Owner)->AddEvent(this, Deactivate, eltDeactivate);
    ((TBaseForm*)pb->Owner)->AddEvent(this, formHide, eltHide);

    AppActivateEvent(this, &appdeactivate, false, true);

    pb->OnPaint = pbPaint;

    if (fdropdown)
        pb->OnNCPaint = (TNCPaintEvent) & pbNCPaint;

    pb->OnEnter = pbEnter;
    pb->OnExit = pbExit;
    pb->OnMouseLeave = pbMouseLeave;
    pb->OnMouseDown = pbMouseDown;
    pb->OnMouseUp = pbMouseUp;
    pb->OnMouseMove = pbMouseMove;
    pb->OnKeyDown = pbKeyDown;
    pb->OnKeyPress = pbKeyPress;
    pb->OnResize = pbResize;
    tBlink = new TTimer(apb);
    tBlink->Enabled = false;
    tBlink->Interval = GetCaretBlinkTime();
    tBlink->OnTimer = &DrawCaret;

    defaultcontext = ImmAssociateContext(pb->Handle, 0/*context*/);
}
//---------------------------------------------------------------------------
TKanjiEdit::~TKanjiEdit()
{
    AppActivateEvent(this, &appdeactivate, false, false);

    if (recognizing)
        fRecognize->Hide();

    if (defaultcontext)
        ImmAssociateContext(pb->Handle, defaultcontext);

    if (kanjieditlist)
        kanjieditlist->Remove(this);

    tBlink->Enabled = false;

    pb->OnEnter = foldenter;
    pb->OnExit = foldexit;
    pb->OnMouseLeave = foldmouseleave;
    pb->OnMouseDown = foldmousedown;
    pb->OnMouseUp = foldmouseup;
    pb->OnMouseMove = foldmousemove;
    pb->OnKeyDown = foldkeydown;
    pb->OnKeyPress = foldkeypress;
    pb->OnResize = foldresize;
    pb->OnPaint = foldpaint;

    ((TBaseForm*)pb->Owner)->RemoveEvents(this);

    delete tBlink;
}
//---------------------------------------------------------------------------
void TKanjiEdit::FocusKanjiEd()
{
    CountPos();
    tBlink->Enabled = false;
    if (!recognizing && !pb->Focused() && Visible && (!fdropdown || !fdropdown->Visible || fdropdown->Form != (TForm*)pb->Owner))
        pb->SetFocus();
    pb->Refresh();
    tBlink->Enabled = true;
    tBlink->OnTimer(tBlink);
}
//---------------------------------------------------------------------------
void TKanjiEdit::set_max(unsigned short val)
{
    if (val >= fmax)
    {
        fmax = val;
        return;
    }

    fmax = val;

    DeleteHalf();
    if (ftext.Length() > val)
        ftext.SetLength(fmax);

    fselecting = false;
    if (pos > fmax)
        pos = fmax;
    else
        DeselectAll();
}
//---------------------------------------------------------------------------
void TKanjiEdit::Insert(UnicodeString c)
{
    DeleteHalf();
    DeleteSelected();

    if (fmax)
        c.SetLength(MIN(fmax - ftext.Length(), c.Length()));

    ftext = ftext.SubString(1, fpos) + c + ftext.SubString(fpos + 1, ftext.Length());
    pos += c.Length();

    FocusKanjiEd();

    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbMouseLeave(TObject *Sender)
{
    if (bstate != dbsNormal)
    {
        pb->Cursor = crIBeam;
        bstate = dbsNormal;
        DrawDropButton();
    }
    if (foldmouseleave)
        foldmouseleave(Sender);
}
//---------------------------------------------------------------------------
void TKanjiEdit::DrawDropButton()
{
    if (!fdropdown)
        return;

    HDC hdc = GetWindowDC(pb->Handle);
    TRect r;
    TPoint p = pb->ClientToScreen(Point(0, 0));
    GetWindowRect(pb->Handle, &r);
    OffsetRect(r, -p.x, -p.y);

    DrawNC(hdc, r, false);
    ReleaseDC(pb->Handle, hdc);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbNCPaint(TObject *Sender, HDC hdc, TRect r)
{
    DrawNC(hdc, r, true);
}
//---------------------------------------------------------------------------
void TKanjiEdit::DrawNC(HDC hdc, TRect r, bool frame)
{
    if (!fdropdown)
        return;

    int xs = -r.Left;
    int ys = -r.Top;
    OffsetRect(&r, -r.Left, -r.Top);

    TThemeServices *t = ThemeServices();
    int htheme = (t && t->ThemesAvailable && t->ThemesEnabled);

    if (!htheme)
    {
        if (frame)
            DrawEdge(hdc, &r, EDGE_SUNKEN, BF_RECT);

        r.bottom -= ys;
        r.right -= xs;
        r.left = r.right - btnwidth;
        r.top = ys;

        DrawFrameControl(hdc, &r, DFC_SCROLL, DFCS_SCROLLCOMBOBOX | (bstate == dbsPressed ? DFCS_FLAT : 0));
    }
    else
    {
        HRGN ohrg = 0, hrg;
        if (GetClipRgn(hdc, ohrg) <= 0)// Error or no cliprgn.
                    ohrg = 0;

        if (frame)
        {
            ExcludeClipRect(hdc, xs, ys, r.right - xs - btnwidth + 1, r.bottom - ys);
        }
        else
        {
            hrg = CreateRectRgn(r.right - xs + 1 - btnwidth, ys - 1, r.right - xs + 1, r.bottom - ys + 1);
            SelectClipRgn(hdc, hrg);
            DeleteObject(hrg);
        }

        TThemedElementDetails tcb;
        tcb.Element = teComboBox;
        if (frame)
        {
            tcb.Part = CP_BORDER;
            tcb.State = CBB_NORMAL;
            t->DrawElement(hdc, tcb, r, &r);
        }

        r = Rect(r.right - xs + 1 - btnwidth, ys - 1, r.right - xs + 1, r.bottom - ys + 1);
        int dropstates[3] = { CBXS_NORMAL, CBXS_HOT, CBXS_PRESSED };

        tcb.Part = CP_DROPDOWNBUTTON;
        tcb.State = dropstates[(int)bstate];
        t->DrawElement(hdc, tcb, r, &r);

        SelectClipRgn(hdc, ohrg);
        if (ohrg)
            DeleteObject(ohrg);
    }
}
//---------------------------------------------------------------------------
void TKanjiEdit::RestoreWndProc()
{
    if (defaultcontext)
        ImmAssociateContext(pb->Handle, defaultcontext);
}
//---------------------------------------------------------------------------
void TKanjiEdit::SetWndProc()
{
    ImmAssociateContext(pb->Handle, 0);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbPaint(TObject *Sender)
{
    TThemeServices *t = ThemeServices();
    int htheme = (t && t->ThemesAvailable && t->ThemesEnabled);

    TRect r = pb->ClientRect;
    if (fdropdown)
    {
        r.right -= btnwidth;
        if (htheme)
            r.right++;
    }
    TCanvas *c = pb->Canvas;
    c->Brush->Color = clWindow;
    c->Font->Name = kanjieditlist->keditfont;
    c->Font->Charset = kanjieditlist->keditcharset;
    SetTextAlign(c->Handle, TA_CENTER | TA_BASELINE);
    int len = ftext.Length();

    TSize s;
    GetTextExtentPoint32W(c->Handle, L"\x3081", 1, &s);
    charwidth = s.cx;

    if (!len)
    {
        c->FillRect(r);
        DrawDropButton();
        if (foldpaint)
            foldpaint(Sender);
        return;
    }
    c->FillRect(Rect(0, 0, textarea.left, pb->ClientHeight));
    c->FillRect(Rect(0, 0, textarea.right, 1));
    c->FillRect(Rect(0, r.bottom - 2, textarea.right, pb->ClientHeight));

    int i = xpos / charwidth; // First visible character.
    int x = r.left = -xpos % charwidth + textarea.left;
    r.right = MIN(textarea.right, r.left + charwidth);
    r.left = textarea.left;
    r.top++;
    r.bottom -= 1;

    for (; i < len && x < textarea.right; i++)
    {
        c->Brush->Color = (i >= MIN(fselstart, pos) && i < MIN(fselstart, pos) + fsellength ? clHighlight : clWindow);
        c->Font->Color = (c->Brush->Color == clHighlight ? clHighlightText : pb->Font->Color);
        ExtTextOutW(c->Handle, x + charwidth / 2, r.Height()*0.95, ETO_CLIPPED | ETO_OPAQUE, &r, ftext.c_str() + i, 1, 0);
        x += charwidth;
        r.left = x;
        r.right = MIN(textarea.right, r.left + charwidth);
    }
    c->Brush->Color = clWindow;
    c->FillRect(Rect(MIN(x, (int)textarea.right), 0, pb->ClientWidth - (fdropdown ? btnwidth - (htheme ? 1 : 0) : 0), pb->ClientHeight));

    DrawDropButton();

    if (foldpaint)
        foldpaint(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::DrawCaret(TObject *Sender)
{
    if ((!recognizing || !fRecognize->Active) && !Focused() && (!fdropdown || !fdropdown->Visible || fdropdown->Form != (TForm*)pb->Owner))
        return;
    // Draw caret to position.
    pb->Canvas->Pen->Color = clWhite;
    pb->Canvas->Pen->Mode = pmXor;
    pb->Canvas->MoveTo(pos*charwidth + 2 - xpos, 1);
    pb->Canvas->LineTo(pos*charwidth + 2 - xpos, pb->ClientHeight - 2);
    pb->Canvas->LineTo(pos*charwidth + 3 - xpos, pb->ClientHeight - 2);
    pb->Canvas->LineTo(pos*charwidth + 3 - xpos, 0);
}
//---------------------------------------------------------------------------
void TKanjiEdit::dropButtonClick(int index, UnicodeString w, void *data)
{
    Text = w;
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (ptext.Length())
        return;

    if (Button == mbLeft)
        mousedown = true;

    TThemeServices *t = ThemeServices();
    int htheme = (t && t->ThemesAvailable && t->ThemesEnabled);

    if (Button == mbLeft && fdropdown && x >= pb->ClientWidth - pb->ClientHeight + (htheme ? 3 : 0))
    {
        TPoint p = pb->ClientToScreen(Point((pb->ClientWidth - pb->Width) / 2, (pb->ClientHeight - pb->Height) / 2));
        TRect r = Rect(p, Point(p.x + pb->Width, p.y + pb->Height - 1));
        if (Text.Trim().Length())
            fdropdown->Add(Text.Trim(), NULL);

        while (!AppActive)
            Application->ProcessMessages();

        fdropdown->Popup(r, dropButtonClick, (TBaseForm*)pb->Owner, true);
        bstate = dbsPressed;
        DrawDropButton();
    }
    else if (Button == mbLeft && ftext.Length())
    {
        if (Shift.Contains(ssDouble))
        {
            SelectAll();
            fselecting = false;
            FocusKanjiEd();
        }
        else
        {
            pos = MIN((double)ftext.Length(), (x + xpos + charwidth * 0.4) / charwidth);
            fselecting = true;
            FocusKanjiEd();
        }
    }

    if (foldmousedown)
        foldmousedown(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button == mbLeft)
    {
        mousedown = false;

        TThemeServices *t = ThemeServices();
        int htheme = (t && t->ThemesAvailable && t->ThemesEnabled);

        fselecting = false;
        if (pb->Cursor == crIBeam && fdropdown && x >= pb->ClientWidth - pb->ClientHeight + (htheme ? 3 : 0))
            pb->Cursor = crDefault;
        else if (bstate == dbsPressed)
        {
            bstate = dbsNormal;
            fdropdown->Capture();
            DrawDropButton();
        }
    }

    if (Button == mbRight)
    {
        TPoint p = pb->ClientToScreen(Point(x, y));
        wchar_t w = 0;
        int mpos = (x + xpos) / charwidth;
        if (ftext.Length() > mpos && mpos >= 0 && KANJI(ftext[mpos + 1]))
            w = ftext[mpos + 1];
        data->PopupKanji(p.x, p.y, this, w);
    }

    if (foldmouseup)
        foldmouseup(Sender, Button, Shift, x, y);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    TThemeServices *t = ThemeServices();
    int htheme = (t && t->ThemesAvailable && t->ThemesEnabled);

    if (fdropdown && !mousedown)
    {
        if (x >= pb->ClientWidth - pb->ClientHeight + (htheme ? 3 : 0))
        {
            if (bstate != dbsHover)
            {
                pb->Cursor = crDefault;
                bstate = dbsHover;
                DrawDropButton();
            }
        }
        else
        {
            if (bstate != dbsNormal || pb->Cursor != crIBeam)
            {
                pb->Cursor = crIBeam;
                bstate = dbsNormal;
                DrawDropButton();
            }
        }
    }

    if (!ptext.Length() && ftext.Length() && (!fdropdown || x < pb->ClientWidth - pb->ClientHeight + (htheme ? 3 : 0)))
    {
        int a = MAX(0, (int)MIN((double)ftext.Length(), (x + xpos + charwidth * 0.4) / charwidth));
        if (mousedown && pos != a)
            pos = a;
    }

    if (foldmousemove)
        foldmousemove(Sender, Shift, x, y);
}
//---------------------------------------------------------------------------
void TKanjiEdit::DeleteHalf()
{
    if (!ptext.Length())
        return;
    ftext = ftext.SubString(1, fpos - ptext.Length()) + ftext.SubString(fpos + 1, ftext.Length());
    pos -= ptext.Length();
    ptext = "";
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbKeyPress(TObject *Sender, wchar_t &Key)
{
    int used;
    bool dokeycallback = true;

    if ((Key >= L'a' && Key <= L'z') || (Key >= L'A' && Key <= L'Z') || Key == L'-')
    {
        fselecting = false;
        DeleteSelected();
//        UnicodeString c = (Key == VK_OEM_MINUS || Key == VK_SUBTRACT ? '-' : (char)Key);
//
//        if (!Shift.Contains(ssShift) && (GetKeyState(VK_CAPITAL) & 1) != 1)
//            c = c.LowerCase();

        if (fmax && !ptext.Length() && ftext.Length() == fmax)
            return;

        ptext = ptext + Key;

        UnicodeString k = ToKana(ptext, used);
        if ((ptext[ptext.Length()] == 'n' || ptext[ptext.Length()] == 'N') && (ptext.Length() == 1 || ptext[ptext.Length() - 1] != 'n' && ptext[ptext.Length() - 1] != 'N')) // Remove last 'n' from k.
        {
            k.SetLength(k.Length() - 1);
            used--;
        }
        if (used) // Translated to kana.
        {
            UnicodeString kk = k;
            if (fmax)
                kk.SetLength(MIN(fmax - (ftext.Length() - ptext.Length() + k.Length()), kk.Length()));

            ftext = ftext.SubString(1, fpos - ptext.Length() + 1) + kk + ptext.SubString(used + 1, ptext.Length()) + ftext.SubString(fpos + 1, ftext.Length());
            pos += kk.Length() - used + 1;
            ptext = ptext.SubString(used + 1, ptext.Length());
            FocusKanjiEd();
        }
        else
        {
            ftext = ftext.SubString(1, fpos) + Key + ftext.SubString(fpos + 1, ftext.Length());
            pos++;
            FocusKanjiEd();
        }
        if (fcallback)
            fcallback();
    }
    else if (Key == VK_RETURN) // Finalize input (for n).
    {
        if (ptext.Length())
        {
            UnicodeString k = ToKana(ptext, used); // Used is not used.

            UnicodeString kk = k;
            if (fmax)
                kk.SetLength(MIN(fmax - (ftext.Length() - ptext.Length() + k.Length()), kk.Length()));

            ftext = ftext.SubString(1, fpos - ptext.Length()) + kk + ftext.SubString(fpos + 1, ftext.Length());
            pos += kk.Length() - ptext.Length();
            ptext = "";
            FocusKanjiEd();
            if (fcallback)
                fcallback();
        }
        else if (ftext.Length() && fdropdown)
        {
            fdropdown->Add(ftext.Trim(), NULL);
            Text = L"";
        }
    }
    else if (Key == VK_ESCAPE)
    {
        dokeycallback = false;
        if (!ptext.IsEmpty())
        {
            DeleteHalf();
            FocusKanjiEd();
            if (fcallback)
                fcallback();
        }
        else if (recognizing)
            fRecognize->Hide();
        else if (InfoWindowVisible())
            HideInfoWindows();
        else
            dokeycallback = true;
    }
    else if (Key == L' ' && !ptext.Length())
    {
        if (!recognizing && fkanji && frecognizer && hasrecdata)
            ShowRecognize();
        else
            fRecognize->Hide();
    }
    else if (Key == 0x08 && (fpos || fsellength)) // Backspace key.
    {
        if (fsellength)
        {
            DeleteSelected();
        }
        else
        {
            if (ptext.Length())
                ptext = ptext.SetLength(ptext.Length() - 1);
            ftext = ftext.SubString(1, fpos - 1) + ftext.SubString(fpos + 1, ftext.Length());
            pos--;
        }
        FocusKanjiEd();
        if (!ptext.Length() && fcallback)
            fcallback();
    }

    if (ptext.Length())
        return;

    if (Key == 0x16) // Ctrl-V
        Paste();
    else if (Key == 0x03) // Ctrl-C
        Copy();
    else if (Key == 0x18) // Ctrl-X
    {
        Copy();
        DeleteSelected();
        if (fcallback)
            fcallback();
    }
    else if (Key == 0x01) // Ctrl-A
        SelectAll();
    else if (Key == 0x04) // Ctrl-D
        DeselectAll();

    if (foldkeypress && dokeycallback)
        foldkeypress(Sender, Key);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    //int used;
    bool dokeycallback = true;

//    if ((Key >= 'A' && Key <= 'Z' || Key == VK_OEM_MINUS || Key == VK_SUBTRACT) && /*!Shift.Contains(ssShift) &&*/ !Shift.Contains(ssCtrl) && !Shift.Contains(ssAlt))
/*
    {
        fselecting = false;
        DeleteSelected();
        UnicodeString c = (Key == VK_OEM_MINUS || Key == VK_SUBTRACT ? '-' : (char)Key);

        if (!Shift.Contains(ssShift) && (GetKeyState(VK_CAPITAL) & 1) != 1)
            c = c.LowerCase();

        if (fmax && !ptext.Length() && ftext.Length() == fmax)
            return;

        ptext = ptext + c;

        UnicodeString k = ToKana(ptext, used);
        if ((ptext[ptext.Length()] == 'n' || ptext[ptext.Length()] == 'N') && (ptext.Length() == 1 || ptext[ptext.Length() - 1] != 'n' && ptext[ptext.Length() - 1] != 'N')) // Remove last 'n' from k.
        {
            k.SetLength(k.Length() - 1);
            used--;
        }
        if (used) // Translated to kana.
        {
            UnicodeString kk = k;
            if (fmax)
                kk.SetLength(MIN(fmax - (ftext.Length() - ptext.Length() + k.Length()), kk.Length()));

            ftext = ftext.SubString(1, fpos - ptext.Length() + 1) + kk + ptext.SubString(used + 1, ptext.Length()) + ftext.SubString(fpos + 1, ftext.Length());
            pos += kk.Length() - used + 1;
            ptext = ptext.SubString(used + 1, ptext.Length());
            FocusKanjiEd();
        }
        else
        {
            ftext = ftext.SubString(1, fpos) + c + ftext.SubString(fpos + 1, ftext.Length());
            pos++;
            FocusKanjiEd();
        }
        if (fcallback)
            fcallback();
    }
    else if (Key == VK_RETURN) // Finalize input (for n).
    {
        if (ptext.Length())
        {
            UnicodeString k = ToKana(ptext, used); // Used is not used.

            UnicodeString kk = k;
            if (fmax)
                kk.SetLength(MIN(fmax - (ftext.Length() - ptext.Length() + k.Length()), kk.Length()));

            ftext = ftext.SubString(1, fpos - ptext.Length()) + kk + ftext.SubString(fpos + 1, ftext.Length());
            pos += kk.Length() - ptext.Length();
            ptext = "";
            FocusKanjiEd();
            if (fcallback)
                fcallback();
        }
        else if (ftext.Length() && fdropdown)
        {
            fdropdown->Add(ftext.Trim().c_str());
            Text = L"";
        }
    }
    else*/ if (Key == VK_ESCAPE)
    {
        dokeycallback = ptext.IsEmpty() && !recognizing && !InfoWindowVisible();
/*
        if (ptext.Length())
        {
            DeleteHalf();
            FocusKanjiEd();
            if (fcallback)
                fcallback();
        }
        else if (recognizing)
        {
            fRecognize->Hide();
        }
*/
    }
/*    else if (Key == VK_SPACE && !ptext.Length())
    {
        if (!recognizing && fkanji && frecognizer && hasrecdata)
            ShowRecognize();
        else
            fRecognize->Hide();
    }
    else if (Key == VK_BACK && (fpos || fsellength))
    {
        if (fsellength)
        {
            DeleteSelected();
        }
        else
        {
            if (ptext.Length())
                ptext = ptext.SetLength(ptext.Length() - 1);
            ftext = ftext.SubString(1, fpos - 1) + ftext.SubString(fpos + 1, ftext.Length());
            pos--;
        }
        FocusKanjiEd();
        if (!ptext.Length() && fcallback)
            fcallback();
    }
*/
    else if (Key == VK_DELETE && !ptext.Length())
    {
        if (Shift.Contains(ssShift))
        {
            Copy();
            DeleteSelected();
        }
        else
        {
            if (fsellength)
                DeleteSelected();
            else
                ftext = ftext.SubString(1, fpos) + ftext.SubString(fpos + 2, ftext.Length());
            FocusKanjiEd();
        }
        if (fcallback)
            fcallback();
    }

    if (!ptext.IsEmpty())
        return;

    if (Key == VK_LEFT || Key == VK_RIGHT || Key == VK_HOME || Key == VK_END)
    {
        if (!fselecting && Shift.Contains(ssShift))
        {
            fselecting = true;
            fselstart = pos;
            fsellength = 0;
        }
        else if (fselecting && !Shift.Contains(ssShift))
            fselecting = false;
    }
    if (fdropdown && fdropdown->Count && (Key == VK_DOWN || Key == VK_UP) && Shift.Contains(ssCtrl))
    {
        bool found = false;
        for (int ix = 0; ix < fdropdown->Count; ++ix)
        {
            if (Text == fdropdown->Strings[ix])
            {
                fdroppos = ix;
                found = true;
                break;
            }
        }
        if (!found)
        {
            fdropdown->Add(Text.Trim(), NULL);
            fdroppos = 0;
        }

        if (fdroppos < 0 && Key == VK_UP)
            fdroppos = 0;

        if (Key == VK_DOWN)
        {
            fdroppos++;
            if (fdroppos >= fdropdown->Count)
                fdroppos = 0;
        }
        else if (Key == VK_UP)
        {
            fdroppos--;
            if (fdroppos < 0)
                fdroppos = fdropdown->Count - 1;
        }
        if (Text != fdropdown->Strings[fdroppos])
        {
            Text = fdropdown->Strings[fdroppos];
        }
    }
    switch (Key)
    {
    case (VK_LEFT):
        pos--;
        break;
    case (VK_RIGHT):
        pos++;
        break;
    case (VK_HOME):
        pos = 0;
        break;
    case (VK_END):
        pos = ftext.Length();
        break;
/*
    case 'V':
        if (Shift.Contains(ssCtrl))
            Paste();
        break;
    case 'C':
        if (Shift.Contains(ssCtrl))
            Copy();
        break;
    case 'X':
        if (Shift.Contains(ssCtrl))
        {
            Copy();
            DeleteSelected();
            if (fcallback)
                fcallback();
        }
        break;
    case 'A':
        if (Shift.Contains(ssCtrl))
            SelectAll();
        break;
    case 'D':
        if (Shift.Contains(ssCtrl))
            DeselectAll();
        break;
*/
    case VK_INSERT:
        if (Shift.Contains(ssShift))
            Paste();
        else if (Shift.Contains(ssCtrl))
            Copy();
        break;
    }
    if (foldkeydown && dokeycallback)
        foldkeydown(Sender, Key, Shift);
}
//---------------------------------------------------------------------------
void TKanjiEdit::DeselectAll()
{
    DeleteHalf();
    fselecting = false;
    fselstart = pos;
    fsellength = 0;

    pb->Invalidate();

    if (!tBlink->Enabled)
        return;

    tBlink->Enabled = false;
    pb->Invalidate();
    tBlink->Enabled = pb->Focused();
    if (tBlink->Enabled)
        tBlink->OnTimer(tBlink);
}
//---------------------------------------------------------------------------
void TKanjiEdit::SelectAll()
{
    DeleteHalf();
    fselecting = true;
    fselstart = 0;
    pos = ftext.Length();
}
//---------------------------------------------------------------------------
void TKanjiEdit::Paste()
{
    UnicodeString clip;
    if (!GetClipText(clip, 128))
        return;
    int dummy;
    clip = ToKana(clip, dummy);
    UnicodeString copy = L"";
    for (int i = 1; i <= clip.Length(); i++)
    {
        if (clip[i] == 0x000d || clip[i] == 0x000a)//new line
                    break;
        if (HIRAGANA(clip[i]) || KATAKANA(clip[i]) || clip[i] == KDASH || clip[i] == KDASHB || ((KANJI(clip[i]) || clip[i] == KURIKAESHI) && fkanji) || (UNICODE_J(clip[i]) && fgeneral))
            copy += clip[i];
    }
    if (copy.Length())
        Insert(copy);
}
//---------------------------------------------------------------------------
void TKanjiEdit::Copy()
{
    if (!fsellength)
        return;
    SetClipText(get_seltext());
}
//---------------------------------------------------------------------------
void TKanjiEdit::set_pos(int i)
{
    int a = MAX(0, MIN(ftext.Length(), i));

    if (fselecting)
    {
        if (a == fselstart)
            fsellength = 0;
        else if (a < fselstart)
            fsellength = fselstart - a;
        else
            fsellength = a - fselstart;
    }
    else if (!fselecting)
    {
        fsellength = 0;
        fselstart = a;
    }

    fpos = a;

    CountPos();

    if (!tBlink->Enabled)
        return;
    tBlink->Enabled = false;
    pb->Refresh();
    tBlink->Enabled = pb->Focused();
    if (tBlink->Enabled)
        tBlink->OnTimer(tBlink);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbExit(TObject *Sender)
{
    mousedown = false;

    if (recognizing && !fRecognize->Active && fRecognize->Visible)
        fRecognize->Hide();

    bool did = ptext.Length();
    DeleteHalf();

    if ((!fdropdown || !fdropdown->Visible || fdropdown->Form != (TForm*)pb->Owner) && !recognizing)
    {
        tBlink->Enabled = false;
        pb->Invalidate();
    }
    if (fdropdown && Text.Trim().Length())
        fdropdown->Add(Text.Trim(), NULL);

    if (did && fcallback)
        fcallback();
    if (foldexit)
        foldexit(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbEnter(TObject *Sender)
{
    FocusKanjiEd();
    if (foldenter)
        foldenter(Sender);
}
//---------------------------------------------------------------------------
void TKanjiEdit::appdeactivate(void *caller)
{
    mousedown = false;
    fselecting = false;
    tBlink->Enabled = false;
    pb->Invalidate();
    if (fdropdown)
        fdropdown->Add(CleanText.Trim(), NULL);
}
//---------------------------------------------------------------------------
void TKanjiEdit::Deactivate(void *caller)
{
    mousedown = false;
    fselecting = false;
    bool did = ptext.Length();
    DeleteHalf();
    if ((!fdropdown || !fdropdown->Visible || fdropdown->Form != (TForm*)pb->Owner) && !recognizing)
    {
        tBlink->Enabled = false;
        pb->Invalidate();
    }
    if (fdropdown)
        fdropdown->Add(Text.Trim(), NULL);
    if (did && fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void TKanjiEdit::Activate(void *caller)
{
    tBlink->Enabled = pb->Focused();
    if (tBlink->Enabled)
        pb->Refresh();
}
//---------------------------------------------------------------------------
void TKanjiEdit::formHide(void *caller)
{
    if (recognizing)
        fRecognize->Hide();
}
//---------------------------------------------------------------------------
void TKanjiEdit::set_text(UnicodeString str)
{
    if (fmax)
        str.SetLength(MIN(fmax, str.Length()));
    ptext = "";
    ftext = str;
    pos = ftext.Length();
    fselecting = false;
    fsellength = 0;
    if (Visible)
    {
        pb->Invalidate();
        if (fcallback)
            fcallback();
    }
}
//---------------------------------------------------------------------------
void TKanjiEdit::DeleteSelected()
{
    fselecting = false;
    if (!fsellength)
        return;
    ftext = ftext.SubString(1, SelStart - 1) + ftext.SubString(SelStart + fsellength, ftext.Length());
    pos = SelStart - 1;
}
//---------------------------------------------------------------------------
void TKanjiEdit::Delete()
{
    if (!fsellength)
        return;
    DeleteSelected();
    if (fcallback)
        fcallback();
}
//---------------------------------------------------------------------------
void TKanjiEdit::CountPos()
{
    if (!charwidth)
        return; // There was no time to measure the width.
    int x = fpos * charwidth - xpos; // Include caret width.
    if (x < 0)
    {
        xpos = MAX(0.0, xpos + x - charwidth * MAX(1.0, ((textarea.Width() / charwidth) / 2.3)));
    }
    else if (x + 2 > textarea.Width())
    {
        xpos = MIN(ftext.Length() * charwidth - textarea.Width() + 2, (int)(xpos + x - textarea.Width() + charwidth * MAX(1.0, ((textarea.Width() / charwidth) / 2.3))));
    }
    else
    {
        xpos = MAX(0, MIN(xpos, ftext.Length() * charwidth - textarea.Width() + 2));
    }
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::pbResize(TObject *Sender)
{
    textarea = pb->ClientRect;
    textarea.left += 1;
    textarea.right -= (fdropdown ? btnwidth + 1 : 1);

    CountPos();
    TRect r = Rect(0, 0, pb->Width, pb->Height);
    RedrawWindow(pb->Handle, &r, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_NOCHILDREN);
    if (tBlink->Enabled)
        tBlink->OnTimer(tBlink);
    if (foldresize)
        foldresize(Sender);
}
//---------------------------------------------------------------------------
bool TKanjiEdit::get_visible()
{
    TControl *p = pb;
    while (p)
    {
        if (!p->Visible)
            return false;
        p = p->Parent;
    }
    return true;
}
//---------------------------------------------------------------------------
int TKanjiEdit::get_selstart()
{
    return (fsellength ? MIN(fpos, fselstart) : fpos) + 1;
}
//---------------------------------------------------------------------------
UnicodeString TKanjiEdit::get_text()
{
    return ftext;
}
//---------------------------------------------------------------------------
UnicodeString TKanjiEdit::get_cleantext()
{
    if (!ptext.Length())
        return ftext;
    return ftext.SubString(1, fpos - ptext.Length()) + ftext.SubString(fpos + 1, ftext.Length());
}
//---------------------------------------------------------------------------
UnicodeString TKanjiEdit::get_seltext()
{
    if (!fsellength)
        return L"";

    return ftext.SubString(SelStart, fsellength);
}
//---------------------------------------------------------------------------
bool TKanjiEdit::CanInput()
{
    return !ptext.Length();
}
//---------------------------------------------------------------------------
bool TKanjiEdit::get_rec()
{
    return fRecognize && fRecognize->Visible && fRecognize->editor == this;
}
//---------------------------------------------------------------------------
void TKanjiEdit::ShowRecognize()
{
    if (!fkanji || !frecognizer || !hasrecdata)
        return;

    fRecognize->Hide();

    pb->SetFocus();
    fRecognize->ShowEx(this, frecognizebutton, frecognize_autohide, frecognize_showhint, frecognize_allowerror, frecognize_cntlimit, frecognize_generalchar, frecognize_showpopup, frecognize_position);
}
//---------------------------------------------------------------------------
extern bool hasrecdata;
void TKanjiEdit::EnableRecognizer(TSpeedButton *arecognizebutton, bool aautohide, bool ashowhint, bool aallowerror, int arecognizecntlimit, bool arecognizegeneral, bool arecognizeshowpopup, TRecognizePosition arecognizeposition)
{
    if (!arecognizebutton)
        THROW(L"Fix!");

    if (frecognizer)
        DisableRecognizer();

    frecognizer = true;
    frecognizebutton = arecognizebutton;
    frecognizebutton->Hint = hasrecdata ? "Write a Japanese letter with a mouse or stylus. [SPACE]" : "No kanji stroke data found! Download the full zkanji package and update your current version!";
    frecognize_autohide = aautohide;
    frecognize_showhint = ashowhint;
    frecognize_allowerror = aallowerror;
    frecognize_generalchar = arecognizegeneral;
    frecognize_cntlimit = arecognizecntlimit;
    frecognize_position = arecognizeposition;
    frecognize_showpopup = arecognizeshowpopup;
    frecognizebutton->Enabled = hasrecdata;

    foldbuttonclick = arecognizebutton->OnClick;
    arecognizebutton->OnClick = OnRecognizeClick;
}
//---------------------------------------------------------------------------
void TKanjiEdit::UpdateRecognizer()
{
    if (!frecognizer)
        return;
    if (!frecognizebutton)
        THROW(L"Fix!");

    frecognizebutton->Enabled = hasrecdata;
    frecognizebutton->Hint = hasrecdata ? "Write a Japanese letter with a mouse or stylus. [SPACE]" : "No kanji stroke data found! Download the full zkanji package and update your current version!";
}
//---------------------------------------------------------------------------
void TKanjiEdit::DisableRecognizer()
{
    if (!frecognizebutton)
        return;

    frecognizebutton->OnClick = foldbuttonclick;
    frecognizebutton->Enabled = !hasrecdata ? true : frecognizebutton->Enabled;
    frecognizebutton->Hint = "";
    foldbuttonclick = NULL;

    frecognizer = false;
}
//---------------------------------------------------------------------------
void __fastcall TKanjiEdit::OnRecognizeClick(TObject *Sender)
{
    if (foldbuttonclick)
        foldbuttonclick(Sender);

    TSpeedButton *sbRecognize = (TSpeedButton*)Sender;
    if (sbRecognize->Down)
        ShowRecognize();
    else
        fRecognize->Hide();
}
//---------------------------------------------------------------------------
void TKanjiEdit::PlaceRecognize(TRecognizePosition arecognizeposition)
{
    fRecognize->Place(this, arecognizeposition);
}
//---------------------------------------------------------------------------
void TKanjiEdit::StopRecognize()
{
    tBlink->Enabled = pb->Focused();
    if (tBlink->Enabled)
        tBlink->OnTimer(tBlink);
    else
        pb->Invalidate();
}
//---------------------------------------------------------------------------
bool TKanjiEdit::Focused()
{
    return Visible && pb->Owner && (((TForm*)pb->Owner)->Active || ((TForm*)pb->Owner)->Parent && ((TForm*)((TForm*)pb->Owner)->Parent)->Active) && pb->Focused();
}
//---------------------------------------------------------------------------
void TKanjiEdit::SetFocus()
{
    ((TForm*)pb->Owner)->SetFocus();
    FocusKanjiEd();
}
//---------------------------------------------------------------------------
int TKanjiEdit::get_left()
{
    return pb->Left;
}
//---------------------------------------------------------------------------
int TKanjiEdit::get_top()
{
    return pb->Top;
}
//---------------------------------------------------------------------------
int TKanjiEdit::get_width()
{
    return pb->Width;
}
//---------------------------------------------------------------------------
int TKanjiEdit::get_height()
{
    return pb->Height;
}
//---------------------------------------------------------------------------
TRect TKanjiEdit::get_sbounds()
{
    TPoint p = pb->ClientToScreen(Point(0, 0));
    return Rect(p.x, p.y, p.x + pb->Width - 1, p.y + pb->Height - 1);
}
//---------------------------------------------------------------------------
void TKanjiEdit::ToHistory()
{
    if (fdropdown)
        fdropdown->Add(CleanText.Trim(), NULL);
}
//---------------------------------------------------------------------------
HWND TKanjiEdit::get_handle()
{
    return pb->Handle;
}
//---------------------------------------------------------------------------
bool TKanjiEdit::HandleAllocated()
{
    return pb->HandleAllocated();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(77);
#endif
