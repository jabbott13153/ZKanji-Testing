//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "studyroom.h"
#include "settings.h"
#include "kanjiedit.h"
#include "romajizer.h"
#include "myfonts.h"
#include "gengraph.h"
#include "wordgroupstudy.h";
#include "collection.h"
#include "recognizer.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(164);
#endif

TfWStudy *fWStudy = NULL;
//---------------------------------------------------------------------------

HFONT TfWStudy::Kfont = 0;
HFONT TfWStudy::Kfontb = 0;

__fastcall TfWStudy::TfWStudy(TComponent* Owner) : TBaseForm(Owner)
{
    pbKanji->Font->Name = settings.kanafont;
    pbKanji->Font->Charset = settings.kanacharset;
    pbKana->Font->Name = settings.kanafont;
    pbKana->Font->Charset = settings.kanacharset;

    pbMeaning->Font->Name = settings.romajifont;
    pbMeaning->Font->Charset = DEFAULT_CHARSET;

    pbKanji->Font->Size = pbKanji->Height * 0.66;
    pbKana->Font->Size = pbKana->Height * 0.63;

    fbtnmeaning = false;

    EraseBackground = false;

    edkanji = new TKanjiEdit(edKanji, false, false, NULL, (TFuncPtr)NULL);
    edkana = new TKanjiEdit(edKana, false, false, NULL, (TFuncPtr)NULL);

    edkanji->EnableRecognizer(sbKanji, false, false, false, 1, true, false, rpDefault);
    edkanji->AcceptKanji = true;

    AppWindow = true;
}
//---------------------------------------------------------------------------
__fastcall TfWStudy::~TfWStudy()
{
    fRecognize->resetpos = recognizereset;
    if (!recognizereset)
        fRecognize->SetDimensions(recognizerect.Left, recognizerect.Top, recognizerect.Right);

    delete edkanji;
    delete edkana;
    if (Kfont)
        DeleteObject(Kfont);
    if (Kfontb)
        DeleteObject(Kfontb);
    Kfont = 0;
    Kfontb = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::FormShow(TObject *Sender)
{
    recognizereset = fRecognize->resetpos;
    fRecognize->resetpos = false;
    recognizerect = Rect(fRecognize->Left, fRecognize->Top, fRecognize->Width, fRecognize->Height);
    if (recognizereset)
        edkanji->PlaceRecognize(rpBelow);

    fgroup->StartRound(true);
    NextWord();
}
//---------------------------------------------------------------------------
int TfWStudy::ShowModalEx(TWordGroupStudy *agroup)
{
    fgroup = agroup;

    fanswer = 1;

    // First leave everything empty.
    fshowmeaning = false;
    fshowkanji = false;
    fshowkana = false;
    fshown = false;

    fmeaning = "";
    fkanji = L"";
    fkana = L"";

    fchoices = fgroup->choices;
    ftimer = fgroup->timer > 0;

    if (!fchoices)
    {
        pbChoices->Visible = false;
        bChoices->Visible = false;
        lbChoices->Visible = false;
        b9->Visible = false;
        b8->Visible = false;

        int d = b7->Top - b8->Top;
        b7->Top -= d;
        b11->Top -= d;
        b12->Top -= d;
        pbInfo->Top -= d;
        pKana->Top -= d;
        pKanji->Top -= d;
        pButtons->Top -= d;
        btnSolve->Top -= d;
        btnSuspend->Top -= d;
        b12->Top = pbInfo->Top + pbInfo->Height;
    }
    ClientHeight = b12->Top + 6;

    Caption = fgroup->methodname;

    int r;
    MODON
    r = ShowModal();
    MODOFF

    btnSolve->Tag = 0;
    return r;
}
//---------------------------------------------------------------------------
void TfWStudy::NextWord()
{
    fmeaningcorrect = 0;
    fkanjicorrect = 0;
    fkanacorrect = 0;

    fselected = -1;
    fgroup->NextWord();

    pButtons->Visible = false;
    btnSuspend->Enabled = true;
    pKana->Visible = fgroup->typeanswer && !fgroup->showkana;
    pKanji->Visible = fgroup->typeanswer && !fgroup->showkanji;
    edkanji->Text = L"";
    edkana->Text = L"";
    btnSolve->Visible = !pKana->Visible && !pKanji->Visible;

    if (pKanji->Visible)
    {
        sbKanji->Down = true;
        sbKanji->Click();
    }

    btnSolve->Caption = "Reveal the answer";
    fshown = false;
    // First see what information should be shown.

    fshowmeaning = fgroup->showmeaning;
    fshowkanji = fgroup->showkanji;
    fshowkana = fgroup->showkana;

    int rnd;
    bool badrnd;
    if (fchoices)
    {
        memset(fchoice, 0xff, sizeof(int)*5);
        fchoice[0] = fgroup->currentindex;
        for (int i = 1; i < 5; i++)
        {
            do
            {
                rnd = fgroup->GetChoice();
                badrnd = false;
                for (int j = 0; j < i; j++)
                    if (fchoice[j] == rnd)
                    {
                        badrnd = true;
                        break;
                    }
            }
            while (badrnd);
            fchoice[i] = rnd;
        }
        // Swap one with the solution.
        rnd = Random(5);
        if (rnd > 0)
        {
            fchoice[0] -= fchoice[rnd];
            fchoice[rnd] += fchoice[0];
            fchoice[0] = fchoice[rnd] - fchoice[0];
        }
        fanswer = rnd;
    }

    lbTitleMeaning->Visible = fshowmeaning;
    lbTitleKanji->Visible = fshowkanji;
    lbTitleKana->Visible = fshowkana;

    fmeaning = fgroup->wordmeanings[fgroup->currentindex];
    fkanji = fgroup->wordkanjis[fgroup->currentindex];
    fkana = fgroup->wordkanas[fgroup->currentindex];

    fnokanji = true;
    for (int i = fkanji.Length(); i > 0 && fnokanji; i--)
        fnokanji = !KANJI(fkanji[i]);

    pbChoices->Invalidate();
    pbMeaning->Invalidate();
    pbKana->Invalidate();
    pbKanji->Invalidate();
    pbInfo->Invalidate();

    if (pKana->Visible)
        edKana->SetFocus();
    else if (pKanji->Visible)
    {
        edKanji->SetFocus();
        fRecognize->Reset();
    }

    if (ftimer)
    {
        ftimepos = 0;
        ftimemax = fgroup->timer * 1000;
        ticks = GetTickCount();
        tTimer->Enabled = true;
        pbTimer->Invalidate();
    }

    if (fbtnmeaning && !fshowmeaning)
    {
        btnMShow->Enabled = true;
        btnMShow->Visible = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::tTimerTimer(TObject *Sender)
{
    unsigned long tc = GetTickCount();
    if (tc < ticks) // The system is running for a few days straight.
                ftimepos += ((unsigned long) - 1) - ticks + tc;
    else
        ftimepos += tc - ticks;
    ticks = tc;

    if (ftimepos >= ftimemax)
    { // Time is up.
        ftimepos = ftimemax;
        fselected = 127;
        btnSolve->Click();
        // And do what we do when it's a failure.
    }
    pbTimer->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbTimerPaint(TObject *Sender)
{
    const TPoint pp[5] = { Point(0, 0), Point(pbTimer->Width - 1, 0), Point(pbTimer->Width - 1, pbTimer->Height - 1), Point(0, pbTimer->Height - 1), Point(0, 0) };

    TCanvas *c = pbTimer->Canvas;

    if (ftimer)
    {
        // Where are we now?
        int pos;
        double percent = ((double)ftimepos / ftimemax);
        pos = pbTimer->Height - ((double)pbTimer->Height) * percent;
        c->Brush->Color = clWhite; // White first
        c->FillRect(Rect(0, 0, pbTimer->Width, pos));
        c->Brush->Color = (percent < 0.9 ? (TColor)0x40ff10 : (TColor)0x0000ff);
        c->FillRect(Rect(0, pos, pbTimer->Width, pbTimer->Height));
    }
    else
    {
        c->Brush->Color = clScrollBar;
        c->FillRect(pbTimer->ClientRect);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbMeaningPaint(TObject *Sender)
{
    TCanvas *c = pbMeaning->Canvas;
    c->Brush->Color = clWhite;

    if (fshowmeaning || fshown && (fgroup->currentindex >= 0 || fmeaningcorrect))
    {
        if (fmeaningcorrect > 0)
            c->Font->Color = (TColor)0x08a600;
        else if (fmeaningcorrect < 0)
            c->Font->Color = clRed;
        else
            c->Font->Color = clBlack;

        c->Font->Style = fmeaningcorrect ? TFontStyles() << fsBold : TFontStyles();

        c->FillRect(pbMeaning->ClientRect);

        TSize s;
        c->Font->Size = pbMeaning->Height * 0.40;
        GetTextExtentPoint32W(c->Handle, fmeaning.c_str(), fmeaning.Length(), &s);
        if (s.cx >= pbMeaning->ClientWidth - 30)
            c->Font->Size = pbMeaning->Height * 0.33;

        TRect r = pbMeaning->ClientRect;
        r.Left += 15;
        r.Right -= 15;
        r.Top += 2;
        r.Bottom -= 2;

        DrawTextW(c->Handle, fmeaning.c_str(), fmeaning.Length(), &r, DT_LEFT | DT_TOP | DT_NOPREFIX | DT_WORDBREAK);
    }
    else
        c->FillRect(pbMeaning->ClientRect);
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbKanjiPaint(TObject *Sender)
{
    TCanvas *c = pbKanji->Canvas;
    c->Brush->Color = clWhite;
    if (fshowkanji || fshown && (fgroup->currentindex >= 0 || fkanjicorrect))
    {
        if (fkanjicorrect > 0)
            c->Font->Color = (TColor)0x08a600;
        else if (fkanjicorrect < 0)
            c->Font->Color = clRed;
        else
            c->Font->Color = clBlack;

        c->Font->Style = fkanjicorrect ? TFontStyles() << fsBold : TFontStyles();

        c->Font->Name = settings.kanjifont;
        c->Font->Charset = settings.kanjicharset;

        TSize s;
        UnicodeString w = fkanji;
        if (fgroup->hidekana && !fshowkana && !fshown)
            for (int i = 1; i <= w.Length(); i++)
                if (KANA(w[i]))
                    w[i] = L'*';

        HFONT oldf = NULL;
        if (settings.kanjiantialias)
        {
            oldf = c->Font->Handle;
            if (!c->Font->Style.Contains(fsBold) && !Kfont)
                Kfont = create_localfont(c->Font);
            if (c->Font->Style.Contains(fsBold) && !Kfontb)
                Kfontb = create_localfont(c->Font);
            SelectObject(c->Handle, c->Font->Style.Contains(fsBold) ? Kfontb : Kfont);
        }

        GetTextExtentPoint32W(c->Handle, w.c_str(), w.Length(), &s);
        TRect r = pbKanji->ClientRect;

        ExtTextOutW(c->Handle, 15, (pbKanji->Height - s.cy) / 2, ETO_CLIPPED | ETO_OPAQUE, &r, w.c_str(), w.Length(), NULL);
        if (oldf)
            SelectObject(c->Handle, oldf);
    }
    else
    {
        c->FillRect(pbKanji->ClientRect);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbKanaPaint(TObject *Sender)
{
    TCanvas *c = pbKana->Canvas;
    c->Brush->Color = clWhite;
    if (fshowkana || fshown && (fgroup->currentindex >= 0 || fkanacorrect))
    {
        if (fkanacorrect > 0)
            c->Font->Color = (TColor)0x08a600;
        else if (fkanacorrect < 0)
            c->Font->Color = clRed;
        else
            c->Font->Color = clBlack;

        c->Font->Style = fkanacorrect ? TFontStyles() << fsBold : TFontStyles();

        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;

        TSize s;
        int l = fkana.Length();
        GetTextExtentPoint32W(c->Handle, fkana.c_str(), l, &s);
        TRect r = pbKana->ClientRect;
        ExtTextOutW(c->Handle, 15, (pbKana->Height - s.cy) / 2, ETO_CLIPPED | ETO_OPAQUE, &r, fkana.c_str(), l, NULL);
    }
    else
    {
        c->FillRect(pbKana->ClientRect);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::btnSuspendClick(TObject *Sender)
{
    tTimer->Enabled = false;
    int r = fgroup->Suspend(fshown);
    if (r != mrCancel)
    {
        ModalResult = r;
        fgroup->collection->Modify(false);
    }
    else
    {
        ticks = GetTickCount();
        tTimer->Enabled = ftimer;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::btnSolveClick(TObject *Sender)
{
    fgroup->collection->Modify(false);

    if (fgroup->typeanswer && fselected == 127 && ((pKana->Visible && Romanize(fkana.c_str()) == Romanize(edkana->Text.Trim().c_str())) || (pKanji->Visible && ToKatakana(fkanji) == ToKatakana(edkanji->Text.Trim()) ) ) )
        fselected = fanswer;

    pKana->Visible = false;
    pKanji->Visible = false;

    btnMShow->Visible = false;
    btnMShow->Enabled = false;

    if (btnSolve->Tag == 0)
    {
        tTimer->Enabled = false;
        fshown = true;
        pbChoices->Invalidate();
        lbTitleMeaning->Visible = true;
        lbTitleKanji->Visible = true;
        lbTitleKana->Visible = true;

        if (fchoices || fselected >= 0)
        {
            btnSolve->Visible = true;
            btnSolve->Tag = 1;
            CorrectAnswer(fselected == fanswer);
            if (fgroup->LastWord())
            {
                btnSolve->Tag = 2;
                btnSolve->Caption = "Finish";
                btnSuspend->Enabled = false;
            }
            else if (fgroup->RoundLastWord())
            {
                btnSolve->Tag = 3;
                btnSolve->Caption = "Next round";
                btnSuspend->Enabled = false;
            }
            else if (!fchoices && !fgroup->typeanswer && fselected != 127)
            {
                NextWord();
                btnSolve->Tag = 0;
            }
            else
            {
                btnSolve->Caption = "Next";
                btnSuspend->Enabled = true;
            }
        }
        else
        {
            btnSolve->Visible = false;
            pButtons->Visible = true;
            btnSuspend->Enabled = false;
        }
        pbMeaning->Invalidate();
        pbKanji->Invalidate();
        pbKana->Invalidate();
    }
    else if (btnSolve->Tag == 1)
    {
        NextWord();
        btnSolve->Tag = 0;
    }
    else if (btnSolve->Tag == 2)  // Finish.
    {
        fgroup->FinishTest(); // Show statistics, change miss or hit status of words, reset test, etc.
        ModalResult = mrOk;
    }
    else if (btnSolve->Tag == 3) // For next round.
    {
        fgroup->StartRound(false);
        NextWord();
        btnSolve->Tag = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::btnCorrectClick(TObject *Sender)
{
    if (((TButton*)Sender)->Tag == 0)
        fselected = fanswer;
    else
        fselected = 127; // So it's a positive number in character.
    pButtons->Visible = false;
    btnSuspend->Enabled = true;
    btnSolve->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbChoicesPaint(TObject *Sender)
{
    if (!pbChoices->Visible || !fchoices)
        return;

    int lheight = pbChoices->Height / 5;
    int y = 1;
    TCanvas *c = pbChoices->Canvas;
    c->Brush->Color = clWhite;
    c->Font->Color = clBlack;
    int i = 0;

    c->Pen->Width = 1;
    c->Pen->Color = cl3DDkShadow;
    TRect r = pbChoices->ClientRect;
    //TPoint pps[] = { TPoint(0, 0), TPoint(pbChoices->ClientWidth - 1, 0), TPoint(pbChoices->ClientWidth - 1, pbChoices->ClientHeight - 1), TPoint(0, pbChoices->ClientHeight - 1), TPoint(0, 0) };
    //c->Polyline(pps, sizeof(pps) / sizeof(TPoint) - 1);
    //InflateRect(&r, 1, 1);
    c->FillRect(r);
    if (!fchoices || (fgroup->currentindex < 0 && !fmeaningcorrect && !fkanjicorrect && !fkanacorrect))
        return;

    c->Brush->Style = bsClear;

    // Paint the solution lines.
    TSize s, s2, s3;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Size = 12;
    s3.cx = 0;
    GetTextExtentPoint32W(c->Handle, L" 9999.", 6, &s3);

    while (i < 5)
    {
        y += lheight;

        if ((fselected >= 0 || fshown) && i == fanswer)
        {
            c->Font->Color = (TColor)0x108000;
            c->Font->Style = TFontStyles() << fsBold;
        }
        else
        {
            c->Font->Color = clBlack;
            c->Font->Style = TFontStyles();
        }

        SetTextAlign(c->Handle, TA_BASELINE | TA_RIGHT);
        // Index.
        c->Font->Name = settings.romajifont;
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 10;
        r = Rect(1, y - lheight, 5 * scaleres + s3.cx, y);
        GetTextExtentPoint32W(c->Handle, (IntToStr(i + 1) + ".").c_str(), IntToStr(i + 1).Length() + 1, &s);
        ExtTextOutW(c->Handle, r.right - 5 * scaleres, y - s.cy * 0.2 - 8 * scaleres, ETO_CLIPPED, &r, (IntToStr(i + 1) + ".").c_str(), IntToStr(i + 1).Length() + 1, NULL);

        SetTextAlign(c->Handle, TA_BASELINE | TA_LEFT);

        r = Rect(5 * scaleres + s3.cx, y - lheight, pbChoices->Width - 3 * scaleres, y);
        int x = r.left + 5 * scaleres;
        int index = fchoice[i];
        if (!fshowmeaning)
        {
            UnicodeString str;

            s2.cx = -2 * scaleres;
            s2.cy = 0;

            if (!fshowkana && !fnokanji)
            {

                c->Font->Name = settings.kanafont;
                c->Font->Charset = settings.kanacharset;
                c->Font->Size = 12;

                str = fgroup->wordkanas[index] + L", ";
                GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s2);
                r.Right = x + s2.cx + 2 * scaleres;
                ExtTextOutW(c->Handle, x, y - s2.cy * 0.2 - 8 * scaleres, ETO_CLIPPED, &r, str.c_str(), str.Length(), NULL);
                r.Left = r.Right;
                r.Right = pbChoices->Width - 3 * scaleres;
            }

            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = 11;

            str = fgroup->wordmeanings[index];
            GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
            ExtTextOutW(c->Handle, x + s2.cx + 2, y - s.cy * 0.2 - 8 * scaleres, ETO_CLIPPED, &r, str.c_str(), str.Length(), NULL);
            s.cx += s2.cx + 2 * scaleres;
            s.cy = MAX(s.cy, s2.cy);
        }
        else
        {
            UnicodeString str = L"";

            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = 12;

            if (!fshowkanji)
            {
                str = fgroup->wordkanjis[index];
                if (!fshowkana)
                    str += L" / ";
            }
            if (!fshowkana)
                str += fgroup->wordkanas[index];
            GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
            ExtTextOutW(c->Handle, x, y - 12 * scaleres, ETO_CLIPPED, &r, str.c_str(), str.Length(), NULL);
        }
        c->FillRect(Rect(pbChoices->Width - 3, 1, pbChoices->Width - 1, pbChoices->Height - 1));
        if (i == pbChoices->Tag) // Mouse is on this one.
        {
            c->MoveTo(5 * scaleres, y - 5 * scaleres);
            c->LineTo(MIN(pbChoices->Width - 3 * scaleres, 25 * scaleres + (int)s.cx), y - 5 * scaleres);
        }
        if (fselected >= 0 && fselected != fanswer && i == fselected)
        {
            TColor pc = c->Pen->Color;
            c->Pen->Color = clRed;
            c->Pen->Width = 2 * scaleres;
            c->MoveTo(5 * scaleres, y - 7 * scaleres);
            c->LineTo(MIN(pbChoices->Width - 3 * scaleres, 25 * scaleres + (int)s.cx), y - s.cy - 9 * scaleres);
            c->MoveTo(5 * scaleres, y - s.cy - 9 * scaleres);
            c->LineTo(MIN(pbChoices->Width - 3 * scaleres, 25 * scaleres + (int)s.cx), y - 7 * scaleres);
            c->Pen->Width = 1;
            c->Pen->Color = pc;
        }
        i++;
    }

    c->Brush->Style = bsSolid;
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbChoicesMouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
    if (fselected >= 0 || fshown)
        return;
    int lheight = pbChoices->Height / 5;
    if (pbChoices->Tag == Y / lheight)
        return;
    pbChoices->Tag = Y / lheight;
    pbChoices->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbChoicesMouseLeave(TObject *Sender)
{
    if (pbChoices->Tag >= 0)
    {
        pbChoices->Tag = -1;
        pbChoices->Invalidate();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbChoicesMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (fshown || !fchoices)
        return;

    fselected = pbChoices->Tag;
    pbChoicesMouseLeave(pbChoices);
    btnSolve->Click();

}
//---------------------------------------------------------------------------
void TfWStudy::CorrectAnswer(bool correct)
{
    fgroup->Answer(correct); //,!fshowmeaning, !fshowkanji, !fshowkana);

    if (!fshowmeaning)
        fmeaningcorrect = (correct ? 1 : -1);
    if (!fshowkanji)
        fkanjicorrect = (correct ? 1 : -1);
    if (!fshowkana)
        fkanacorrect = (correct ? 1 : -1);

    pbInfo->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (!edkanji->CanInput() || !edkana->CanInput())
        return;

    if (Key == L' ' && btnSolve->Visible) // Space key.
        btnSolve->Click();
    else if (Key == 0x1B && btnSuspend->Enabled) // Escape key.
        btnSuspend->Click();
    else if (Key == 0x0D) // Enter key.
    {
        if (pKana->Visible)
            btnAccept->Click();
        else if (pKanji->Visible)
            btnAccept2->Click();
        else if (btnSolve->Visible)
            btnSolve->Click();
    }
    else if (Key >= '1' && Key <= '5' && fchoices && !fshown)
    {
        pbChoices->Tag = Key - '1';
        pbChoicesMouseDown(pbChoices, mbLeft, TShiftState(), 0, 0);
    }
    else if (btnMShow->Visible && /*!Shift.Contains(ssAlt) &&*/ !pKana->Visible && !pKanji->Visible && (Key == L'M' || Key == L'm'))
        btnMShow->Click();
    if (pButtons->Visible)
    {
        if (Key == L'C' || Key == L'c')
            btnCorrect->Click();
        else if (Key == L'W' || Key == L'w')
            btnWrong->Click();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
/*
    if (!edkanji->CanInput())
        return;

    if (Key == VK_SPACE && btnSolve->Visible)
        btnSolve->Click();
    if (Key == VK_ESCAPE && btnSuspend->Enabled)
        btnSuspend->Click();
    if (Key == VK_RETURN)
        if (pKana->Visible)
            btnAccept->Click();
        else if (btnSolve->Visible)
            btnSolve->Click();
    if (Key >= '1' && Key <= '5' && fchoices && !fshown)
    {
        pbChoices->Tag = Key - '1';
        pbChoicesMouseDown(pbChoices, mbLeft, TShiftState(), 0, 0);
    }

    if (btnMShow->Visible && !Shift.Contains(ssAlt) && !pKana->Visible && Key == 'M')
        btnMShow->Click();
    if (pButtons->Visible)
    {
        if (Key == 'C')
            btnCorrect->Click();
        else if (Key == 'W')
            btnWrong->Click();
    }
*/
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::pbInfoPaint(TObject *Sender)
{
    pbInfo->Canvas->Brush->Style = bsClear;
    fgroup->DrawInfo(pbInfo);
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::FormCloseQuery(TObject *Sender, bool &CanClose)
{
    if (ModalResult == mrCancel)
    {
        if (btnSuspend->Enabled)
            btnSuspend->Click();
        CanClose = ModalResult != mrCancel;
    }
    else
        CanClose = true;
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::btnAcceptClick(TObject *Sender)
{
    fselected = 127;
    btnSolve->Click();
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::btnMShowClick(TObject *Sender)
{
    btnMShow->Visible = false;
    btnMShow->Enabled = false;
    lbTitleMeaning->Visible = true;
    fshowmeaning = true;
    pbMeaning->Invalidate();
}
//---------------------------------------------------------------------------
void TfWStudy::set_btnmeaning(bool val)
{
    fbtnmeaning = val;
}
//---------------------------------------------------------------------------
void __fastcall TfWStudy::FormPaint(TObject *Sender)
{
    b9->Top = lbChoices->Top + lbChoices->Height + 6 * scaleres;

    TRect r;
    r.Left = 0;
    r.Right = ClientWidth;

    DrawCaptionBar(this, NULL, b1, false);
    Canvas->Brush->Color = clWhite;
    Canvas->FillRect(Rect(0, b1->Top + (b1->Height == 1 ? 1 : 2), ClientWidth, b8->Top));

    if (fchoices)
    {
        DrawPillowPanel(this, b8, b9);
        DrawPillowPanel(this, b9, b7, false, true);
    }

    DrawPillowPanel(this, b7, b11, true);

    r.Top = b11->Top + 2;
    r.Bottom = b12->Top;
    Canvas->Brush->Color = clBtnFace;
    Canvas->FillRect(r);

    DrawPillowPanel(this, b12, NULL);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(165);
#endif



