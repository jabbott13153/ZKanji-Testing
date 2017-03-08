//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "wordreview.h"
#include "words.h"
#include "wordgroupstudy.h"
#include "settings.h"
#include "collection.h"
#include "zkformats.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(192);
#endif

TfWReview *fWReview;
//---------------------------------------------------------------------------

bool lastrandom = false; // The last setting of the random 10-20-30 buttons.

void __fastcall TfWReview::WMSizing(TWMSizing &msg)
{
    ConstraintSizing(msg, 526*scaleres + (Width - ClientWidth), 146*scaleres + (Height - ClientHeight));
}
//---------------------------------------------------------------------------
__fastcall TfWReview::TfWReview(TComponent* Owner) : TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);
    cbGroup->MaxLength = group_name_length_limit;

    oldminusdisabled = false;

    flinesize = rowsizes(settings.dictfontsize) + 7 * scaleres;
    fromajisize = rowsizes(settings.dictfontsize) + 4 * scaleres;
    frowsize = fromajisize + flinesize * 2 + 2 * scaleres;

    fcompactsize = MAX(ilCheck->Height + 1, flinesize);

    pbList->VertScrollBar->SmallChange = frowsize / 2;

    TCanvas *c = pbList->Canvas;
    c->Font->Name = settings.romajifont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = (10 - settings.dictfontsize * 2) / scaleres;
    c->Font->Style = TFontStyles() << fsBold;
    TSize s;
    GetTextExtentPoint32W(c->Handle, L" 567.", 5, &s);
    fnumbersize = MAX((int)s.cx, ilCheck->Width);
    c->Font->Style = TFontStyles();

    fsel = -1;
    foncheck = 0;
    fmousecheck = 0;

    AppWindow = true;
}
//---------------------------------------------------------------------------
int TfWReview::ShowModalEx(TWordGroupStudy *agroup, TWordIndexList *alist, TWordStudyStateList *astats, TWordStatGlobalList *aglobals, int afirstnotwrong, int alastwrong, int around, bool alter, bool remove, int random)
{
    btnCompact->Down = settings.compactreview;
    fgroup = agroup;
    canalter = alter;
    canremove = remove;
    fround = around;
    Caption = Caption + fround;
    flist = alist;
    fstats = astats;
    fglobals = aglobals;
    ffnotw = afirstnotwrong;
    flw = alastwrong;
    selected = flist->Count - 1;
    btnRemove->Enabled = canremove;
    btnAll->Enabled = canremove;
    btnRandom10->Enabled = canremove;
    btnRandom20->Enabled = canremove;
    btnRandom30->Enabled = canremove;

    for (int i = 0; i < flist->collection->groups->Count; i++)
        cbGroup->Items->Add(flist->collection->groups->Names[i]);

    if (canremove)
    {
        switch (random)
        {
        case 1:
            btnRandom10->Down = true;
            btnRandom10Click(btnRandom10);
            break;
        case 2:
            btnRandom20->Down = true;
            btnRandom10Click(btnRandom20);
            break;
        case 3:
            btnRandom30->Down = true;
            btnRandom10Click(btnRandom30);
            break;
        default:
            if (lastrandom)
                btnAllClick(btnAll);
        }
    }

    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::FormShow(TObject *Sender)
{
    UpdateScore();
    count_sizes();
}
//---------------------------------------------------------------------------
void TfWReview::UpdateScore()
{
    sbInfo->Panels->Items[1]->Text = IntToStr(flist->Count);

    prob = 0;
    wrong = 0;
    skip = 0;

    for (int i = 0; i < fstats->Count; i++)
    {
        TWordStudyState ws = *fstats->Items[i];

        if (canalter && (ws.meaningerror || ws.kanjierror || ws.kanaerror))
            fgroup->SaveStat(ws, !(ws.meaningerror > 0 || ws.kanjierror > 0 || ws.kanaerror > 0));

        if (ws.score < -2)
            prob++;
        if (ws.meaningerror > 0 || ws.kanjierror > 0 || ws.kanaerror > 0)
            wrong++;
    }
    for (int i = 0; i < fstats->Count; i++)
    {
        if (fstats->Items[i]->skip != 0)
        {
            if (!wrong && i < ffnotw || wrong && i > flist->Count - flw)
                fstats->Items[i]->skip = 0;
            else
                skip++;
        }
    }
    sbInfo->Panels->Items[3]->Text = skip + (wrong ? flw - 1 : 0) + (wrong ? 0 : ffnotw);
    sbInfo->Panels->Items[5]->Text = wrong;
    sbInfo->Panels->Items[7]->Text = prob;

    btnGenerate->Tag = (wrong > 0 && canalter ? 1 : 0);
    cbGroup->Enabled = canalter;
    cbGroupChange(cbGroup);

    selected = selected; // Keep word in range.
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListResize(TObject *Sender)
{
    count_sizes();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::count_sizes()
{
    if (!flist)
        return;
    pbList->HorzScrollBar->LargeChange = MAX(20, pbList->ClientWidth - 20);
    if (!btnCompact->Down)
    {
        pbList->VMax = frowsize * flist->Count - 1;
        pbList->VertScrollBar->ThumbSize = pbList->ClientHeight;
        pbList->VertScrollBar->LargeChange = MAX(frowsize / 2, pbList->ClientHeight - frowsize / 2);
    }
    else
    {
        pbList->VMax = fcompactsize * flist->Count - 1;
        pbList->VertScrollBar->ThumbSize = pbList->ClientHeight;
        pbList->VertScrollBar->LargeChange = MAX(fcompactsize / 2, pbList->ClientHeight - fcompactsize / 2);
    }
}
//---------------------------------------------------------------------------
int TfWReview::realrow(int ix)
{
    return btnForward->Visible ? ix : flist->Count - 1 - ix;
}
//---------------------------------------------------------------------------
int TfWReview::mouserow(int y)
{
    int r;
    if (!btnCompact->Down)
        r = realrow((pbList->VPos + y) / frowsize);
    else
        r = realrow((pbList->VPos + y) / fcompactsize);

    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::DrawCheck(int ix)
{
    if (!(canalter || canremove) || ix < (canalter ? 0 : ffnotw) || ix > flist->Count - (wrong || !canalter ? flw : 1))
        return;

    int top;
    int compactdifftop = 0;
    int compactdiffbottom = 0;
    int left = 0;

    bool disableminus = (flist->Count - ((skip + (wrong ? flw - 1 : 0) + (wrong ? 0 : ffnotw))) <= 3);

    if (!btnCompact->Down)
        top = frowsize * (realrow(ix) + 1) - 2 - ilCheck->Height * 2 - pbList->VPos;
    else
    {
        compactdifftop = (fcompactsize - 1 - ilCheck->Height) / 2;
        compactdiffbottom = (fcompactsize - 1 - ilCheck->Height) - compactdifftop;
        top = fcompactsize * realrow(ix) - pbList->VPos + compactdifftop;
    }

    if (!btnCompact->Down && top < -ilCheck->Height * 2 || btnCompact->Down && top < -ilCheck->Height || top >= pbList->ClientHeight || ix > flist->Count || ix < 0)
        return;

    int state = (foncheck == 1 && ix == fmouserow ? fmousecheck == 1 ? 2 : 1 : 0);
    if (fstats->Items[ix]->meaningerror > 0 || fstats->Items[ix]->kanjierror > 0 || fstats->Items[ix]->kanaerror > 0)
        state += 3;

    pbList->Canvas->Brush->Color = clBlack;
    if (canalter && ix >= 0 && ix <= flist->Count - flw && (fstats->Items[ix]->meaningerror || fstats->Items[ix]->kanjierror || fstats->Items[ix]->kanaerror))
    {
        if (compactdifftop)
            pbList->Canvas->FillRect(Rect(left, top - compactdifftop, left + ilCheck->Width, top));
        ilCheck->Draw(pbList->Canvas, left, top, state, true);
        if (compactdiffbottom)
            pbList->Canvas->FillRect(Rect(left, top + ilCheck->Height, left + ilCheck->Width, top + ilCheck->Height + compactdiffbottom));
    }
    else
        pbList->Canvas->FillRect(Rect(left, top - compactdifftop, left + ilCheck->Width, top + ilCheck->Height + compactdiffbottom));

    if (btnCompact->Down)
        left += ilCheck->Width;
    else
        top += ilCheck->Height;

    state = (foncheck == 2 && ix == fmouserow ? fmousecheck == 2 ? 2 : 1 : 0) + 6;
    if (fstats->Items[ix]->skip < 0)
        state += 3;
    if (canremove && ix >= (wrong ? 0 : ffnotw) && ix <= flist->Count - flw)
    {
        if (compactdifftop)
            pbList->Canvas->FillRect(Rect(left, top - compactdifftop, left + ilCheck->Width, top));
        if (disableminus && !fstats->Items[ix]->skip)
            state = 12;
        ilCheck->Draw(pbList->Canvas, left, top, state, true);
        if (compactdiffbottom)
            pbList->Canvas->FillRect(Rect(left, top + ilCheck->Height, left + ilCheck->Width, top + ilCheck->Height + compactdiffbottom));
    }
    else
        pbList->Canvas->FillRect(Rect(left, top - compactdifftop, left + ilCheck->Width, top + ilCheck->Height + compactdiffbottom));
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListPaint(TObject *Sender)
{
    TWordStudyState ws;

    TCanvas *c = pbList->Canvas;
    int y = -pbList->VPos % (!btnCompact->Down ? frowsize : fcompactsize);
    int ix = mouserow(0);
    int x = -pbList->HPos;

    UnicodeString m;
    UnicodeString k;

    TRect r;
    TSize s, sm;
    int statsize = 10 - settings.dictfontsize * 2;

    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;
    c->Font->Style = TFontStyles();
    c->Font->Size = rowsizes(settings.dictfontsize) * 0.72 / scaleres;
    GetTextExtentPoint32W(c->Handle, L"感かんジ", 4, &s);
    int col4w = s.cx;

    c->Font->Name = settings.romajifont;
    c->Font->Charset = DEFAULT_CHARSET;
    c->Font->Size = statsize;
    int colw = fnumbersize;
    GetTextExtentPoint32W(c->Handle, L"Correct in a row:", 22, &s);
    int col2w = colw * 2 + s.cx;

    int statrow = (frowsize - 2) / 4;

    UnicodeString str;

    bool showcheck;
    bool correct;
    while (y < pbList->ClientHeight && (btnForward->Visible && ix < flist->Count || !btnForward->Visible && ix >= 0))
    {
        ws = *fstats->Items[ix];
        showcheck = (canalter || canremove) && ix >= (canalter ? 0 : ffnotw) && ix <= flist->Count - (wrong || !canalter ? flw : 1); //&& (fstats->Items[ix]->meaningerror || fstats->Items[ix]->kanjierror || fstats->Items[ix]->kanaerror);
        if (canalter)
        {
            correct = fstats->Items[ix]->meaningerror < 0 || fstats->Items[ix]->kanjierror < 0 || fstats->Items[ix]->kanaerror < 0;
            if (fstats->Items[ix]->meaningerror || fstats->Items[ix]->kanjierror || fstats->Items[ix]->kanaerror)
                fgroup->SaveStat(ws/*,fglobals->Items[ix]->state*/, correct);
        }

        if (!btnCompact->Down)
        {
            // Number:
            c->Font->Color = clWhite;
            c->Font->Style = TFontStyles() << fsBold;
            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = statsize;

            c->Brush->Color = clBlack;
            SetTextAlign(c->Handle, TA_RIGHT | TA_TOP);
            r = Rect(0, y, colw, y + frowsize - 2 - (showcheck ? ilCheck->Height * 2 : 0));
            ExtTextOutW(c->Handle, r.right, r.top + statsize, ETO_CLIPPED | ETO_OPAQUE, &r, (IntToStr(ix + 1) + ". ").c_str(), (IntToStr(ix + 1) + ": ").Length(), NULL);

            if (showcheck)
                DrawCheck(ix);

            // Statistics:
            c->Font->Style = TFontStyles();
            c->Font->Size = statsize;

            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
            {
                c->Brush->Color = clSilver;
                c->Font->Color = clGray;
            }
            else
            {
                c->Font->Color = clBlack;
                c->Brush->Color = (TColor)0xefffff;
            }

            if (fsel == ix)
                DARKEN(c->Brush->Color, (pbList->Focused() ? 0xe8e8e4 : 0xeeeeee));

            r = Rect(colw, y, colw + s.cx, y + statrow);
            ExtTextOutA(c->Handle, r.right, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, "Correct: ", 9, NULL);
            r = Rect(colw, y + statrow, colw + s.cx, y + statrow * 2);
            ExtTextOutA(c->Handle, r.right, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, "Incorrect: ", 11, NULL);
            r = Rect(colw, y + statrow * 2, colw + s.cx, y + statrow * 3);
            ExtTextOutA(c->Handle, r.right, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, "Correct in a row: ", 18, NULL);
            r = Rect(colw, y + statrow * 3, colw + s.cx, y + statrow * 4);
            ExtTextOutA(c->Handle, r.right, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, "Score: ", 7, NULL);

            SetTextAlign(c->Handle, TA_LEFT | TA_TOP);

            r = Rect(colw + s.cx, y, col2w - 1, y + statrow);
            str = ws.correct;
            ExtTextOutW(c->Handle, r.left, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);
            r = Rect(colw + s.cx, y + statrow, col2w - 1, y + statrow * 2);
            str = ws.incorrect;
            ExtTextOutW(c->Handle, r.left, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);
            r = Rect(colw + s.cx, y + statrow * 2, col2w - 1, y + statrow * 3);
            str = ws.chaincorrect;
            ExtTextOutW(c->Handle, r.left, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);
            r = Rect(colw + s.cx, y + statrow * 3, col2w - 1, y + statrow * 4);
            str = (int)ws.score;
            ExtTextOutW(c->Handle, r.left, r.top + 2, ETO_CLIPPED | ETO_OPAQUE, &r, str.c_str(), str.Length(), NULL);

            c->FillRect(Rect(colw, y + statrow*4, col2w - 1, y + frowsize - 2));
            c->Pen->Color = clSilver;
            c->MoveTo(col2w - 1, y);
            c->LineTo(col2w - 1, y + frowsize - 2);

            // Meaning:
            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
            {
                c->Brush->Color = clSilver;
                c->Font->Color = clGray;
            }
            else
            {
                c->Brush->Color = (ws.score < -2 ? (TColor)0xefffff : clWhite);
                c->Font->Color = (fstats->Items[ix]->meaningerror > 0 ? clRed : clBlack);
            }
            if (fsel == ix)
                DARKEN(c->Brush->Color, (pbList->Focused() ? 0xe8e8e4 : 0xeeeeee));

            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = rowsizes(settings.dictfontsize) * 0.625 / scaleres;

            m = flist->Meanings[ix];
            GetTextExtentPoint32W(c->Handle, m.c_str(), m.Length(), &sm);
            r = Rect(col2w, y, MAX(col2w, col2w + 16 + (int)sm.cx + x), y + fromajisize);

            SetTextAlign(c->Handle, TA_LEFT | TA_BOTTOM);

            ExtTextOutW(c->Handle, col2w + 8 + x, r.bottom - (fromajisize - sm.cy) / 2, ETO_CLIPPED | ETO_OPAQUE, &r, m.c_str(), m.Length(), NULL);

            c->Font->Name = settings.smallromajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = 7;
            c->Font->Style = TFontStyles();
            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
                c->Font->Color = clGray;
            else
                c->Font->Color = (TColor)0x00308040;
            m = "";
            for (int i = 0; i < WT_COUNT; i++)
                if (flist->Words[ix]->data[flist->Items[ix]->meaning].types & (1 << i))
                    m += ((UnicodeString)wtypetext[i]) + " ";
            m.Trim();
            r = Rect(MAX(col2w, col2w + 16 + (int)sm.cx + x), y, pbList->Width, y + fromajisize);
            ExtTextOutW(c->Handle, col2w + 16 + sm.cx + x, r.bottom - (fromajisize - sm.cy) / 2, ETO_CLIPPED | ETO_OPAQUE, &r, m.c_str(), m.Length(), NULL);

            SetTextAlign(c->Handle, TA_LEFT | TA_TOP);

            c->Pen->Color = clSilver;
            c->MoveTo(col2w, y + fromajisize - 1);
            c->LineTo(pbList->ClientWidth, y + fromajisize - 1);

            y += fromajisize;

            // Kanji:
            k = flist->Words[ix]->kanji;

            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = rowsizes(settings.dictfontsize) * 0.72 / scaleres;

            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
                c->Font->Color = clGray;
            else
                c->Font->Color = (fstats->Items[ix]->kanjierror > 0 ? clRed : clBlack);

            r = Rect(col2w, y, pbList->Width, y + flinesize);
            ExtTextOutW(c->Handle, r.left + 8 + x, r.top + 4, ETO_CLIPPED | ETO_OPAQUE, &r, k.c_str(), k.Length(), NULL);
            c->MoveTo(col2w, y + flinesize - 1);
            c->LineTo(pbList->ClientWidth, y + flinesize - 1);

            y += flinesize;

            // Kana:

            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
                c->Font->Color = clGray;
            else
                c->Font->Color = (fstats->Items[ix]->kanaerror > 0 ? clRed : clBlack);

            k = flist->Words[ix]->kana;
            r = Rect(col2w, y, pbList->Width, y + flinesize);
            ExtTextOutW(c->Handle, r.left + 8 + x, r.top + 4, ETO_CLIPPED | ETO_OPAQUE, &r, k.c_str(), k.Length(), NULL);

            y += flinesize;
        }
        else
        {
            // Number:
            c->Font->Color = clWhite;
            c->Font->Style = TFontStyles() << fsBold;
            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = statsize;

            GetTextExtentPoint32W(c->Handle, L" 567.", 5, &s);
            colw = MAX((int)s.cx, ilCheck->Width);

            c->Brush->Color = clBlack;
            SetTextAlign(c->Handle, TA_RIGHT | TA_TOP);
            r = Rect(ilCheck->Width * 2, y, ilCheck->Width * 2 + colw, y + fcompactsize - 1);
            ExtTextOutW(c->Handle, r.right, r.top + statsize, ETO_CLIPPED | ETO_OPAQUE, &r, (IntToStr(ix + 1) + ". ").c_str(), (IntToStr(ix + 1) + ": ").Length(), NULL);
            SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
            c->Font->Style = TFontStyles();

            if (showcheck)
                DrawCheck(ix);
            else
                c->FillRect(Rect(0, y, ilCheck->Width*2, y + fcompactsize - 1));

            colw += ilCheck->Width * 2;

            // Kanji:
            k = flist->Words[ix]->kanji;

            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            c->Font->Size = rowsizes(settings.dictfontsize) * 0.72 / scaleres;

            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
            {
                c->Brush->Color = clSilver;
                c->Font->Color = clGray;
            }
            else
            {
                c->Brush->Color = (ws.score < -2 ? (TColor)0xefffff : clWhite);
                c->Font->Color = (fstats->Items[ix]->kanjierror > 0 ? clRed : clBlack);
                if (ix % 2)
                    DARKEN(c->Brush->Color, 0xf8f8f8);
            }

            if (fsel == ix)
                DARKEN(c->Brush->Color, (pbList->Focused() ? 0xe8e8e4 : 0xeeeeee));

            r = Rect(colw, y, colw + col4w * 2 + x - 1, y + fcompactsize - 1);

            if (r.left <= r.right)
            {
                ExtTextOutW(c->Handle, colw + 8 + x, r.top + r.Height()*0.76, ETO_CLIPPED | ETO_OPAQUE, &r, k.c_str(), k.Length(), NULL);
                c->Pen->Color = clSilver;
                c->MoveTo(r.right, y);
                c->LineTo(r.right, y + fcompactsize);
            }

            // Kana:
            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
                c->Font->Color = clGray;
            else
                c->Font->Color = (fstats->Items[ix]->kanaerror > 0 ? clRed : clBlack);

            k = flist->Words[ix]->kana;
            r.left = MAX(colw, colw + x + col4w * 2);
            r.right = colw + x + col4w * 5 - 1;

            if (r.left <= r.right)
            {
                ExtTextOutW(c->Handle, colw + x + col4w*2 + 8, r.top + r.Height()*0.76, ETO_CLIPPED | ETO_OPAQUE, &r, k.c_str(), k.Length(), NULL);
                c->MoveTo(r.right, y);
                c->LineTo(r.right, y + fcompactsize);
            }

            SetTextAlign(c->Handle, TA_LEFT | TA_BOTTOM);

            // Meaning:
            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
                c->Font->Color = clGray;
            else
                c->Font->Color = (fstats->Items[ix]->meaningerror > 0 ? clRed : clBlack);

            c->Font->Name = settings.romajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = rowsizes(settings.dictfontsize) * 0.625 / scaleres;

            m = flist->Meanings[ix];
            GetTextExtentPoint32W(c->Handle, m.c_str(), m.Length(), &sm);

            r.left = colw + x + col4w * 5;
            r.right = r.left + sm.cx + 16;
            r.left = MAX((int)r.left, colw);

            if (r.left <= r.right)
                ExtTextOutW(c->Handle, colw + x + col4w*5 + 8, r.top + r.Height()*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, m.c_str(), m.Length(), NULL);

            r.left = MAX(colw, (int)r.right);
            r.right = pbList->ClientWidth;

            c->Font->Name = settings.smallromajifont;
            c->Font->Charset = DEFAULT_CHARSET;
            c->Font->Size = 7;
            c->Font->Style = TFontStyles();

            if (ix < (wrong ? 0 : ffnotw) || ix > flist->Count - (wrong ? flw : 1))
                c->Font->Color = clGray;
            else
                c->Font->Color = (TColor)0x00308040;

            m = "";
            for (int i = 0; i < WT_COUNT; i++)
                if (flist->Words[ix]->data[flist->Items[ix]->meaning].types & (1 << i))
                    m += ((UnicodeString)wtypetext[i]) + " ";
            m.Trim();

            ExtTextOutW(c->Handle, colw + x + col4w*5 + 16 + sm.cx, r.top + r.Height()*0.8, ETO_CLIPPED | ETO_OPAQUE, &r, m.c_str(), m.Length(), NULL);

            y += fcompactsize - 1;
        }

        c->Pen->Color = clWhite;
        c->MoveTo(0, y);
        c->LineTo(colw, y);
        if (!btnCompact->Down)
        {
            c->MoveTo(0, y + 1);
            c->LineTo(colw, y + 1);
        }
        c->Pen->Color = clBlack;
        c->MoveTo(colw, y);
        c->LineTo(pbList->ClientWidth, y);
        if (!btnCompact->Down)
        {
            c->MoveTo(colw, y + 1);
            c->LineTo(pbList->ClientWidth, y + 1);
        }

        if (btnForward->Visible)
            ix++;
        else
            ix--;
        if (!btnCompact->Down)
            y += 2;
        else
            y++;
    }

    if (y < pbList->ClientHeight)
    {
        c->Brush->Color = clWhite;
        c->FillRect(Rect(0, y, pbList->ClientWidth, pbList->ClientHeight));
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListMouseLeave(TObject *Sender)
{
    if (foncheck)
    {
        foncheck = 0;
        DrawCheck(fmouserow);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListMouseMove(TObject *Sender, TShiftState Shift, int x, int y)
{
    int check = foncheck;
    int oldrow = fmouserow;
    int row = mouserow(y);
    int yin = (y + pbList->VPos) % frowsize;
    if (!fmousecheck)
        fmouserow = row;
    foncheck = 0;

    if (row >= 0 && row <= flist->Count - flw && (!fmousecheck || row == fmouserow) && x >= 0 && x <= (!btnCompact->Down ? ilCheck->Width : ilCheck->Width * 2))
    {
        if (!btnCompact->Down)
        {
            if ((!fmousecheck || fmousecheck == 2) && yin > frowsize - ilCheck->Height - 2 && yin < frowsize - 2 && row >= (wrong ? 0 : ffnotw))
                foncheck = 2;
            else if ((!fmousecheck || fmousecheck == 1) && yin > frowsize - ilCheck->Height*2 - 2 && yin < frowsize - ilCheck->Height - 2)
                foncheck = 1;
        }
        else
        {
            if ((!fmousecheck || fmousecheck == 2) && x >= ilCheck->Width && x < ilCheck->Width*2 && row >= (wrong ? 0 : ffnotw))
                foncheck = 2;
            else if ((!fmousecheck || fmousecheck == 1) && x >= 0 && x < ilCheck->Width)
                foncheck = 1;
        }
    }

    if (foncheck == 2 && flist->Count - ((skip + (wrong ? flw - 1 : 0) + (wrong ? 0 : ffnotw))) <= 3 && !fstats->Items[fmouserow]->skip)
        foncheck = 0;

    if (oldrow != fmouserow || check != foncheck)
    {
        DrawCheck(oldrow);
        if (oldrow != fmouserow)
            DrawCheck(row);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::FormPaint(TObject *Sender)
{
    DrawButtonBar(this, NULL, b0);
    DrawPillowPanel(Canvas, Rect(0, b1->Top + b1->Height, ClientWidth, sbInfo->Top));
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::FormResize(TObject *Sender)
{
    TRect r = Rect(0, b1->Top + b1->Height, ClientWidth, sbInfo->Top);
    RedrawWindow(Handle, &r, NULL, RDW_INVALIDATE);
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int x, int y)
{
    if (Button != mbMiddle)
    {
        selected = mouserow(y);
        if (Button == mbLeft && foncheck)
        {
            fmousecheck = foncheck;
            fmouserow = mouserow(y);
            if (fmouserow > flist->Count - (wrong ? flw : 1))
                fmouserow = flist->Count - (wrong ? flw : 1);
            DrawCheck(selected);
            UpdateScore();
        }
    }
}
//---------------------------------------------------------------------------
void TfWReview::set_selected(int ix)
{
    if (ix < (wrong ? 0 : ffnotw))
        ix = (wrong ? 0 : ffnotw);

    if (ix > flist->Count - (wrong ? flw : 1))
        ix = flist->Count - (wrong ? flw : 1);

    if (fsel == ix)
        return;

    int oldsel = fsel;
    fsel = ix;
    int rix = realrow(ix);
    bool scrolled = false;

    if (!btnCompact->Down)
    {
        if (pbList->VPos > frowsize * rix)
        {
            pbList->VPos = frowsize * rix;
            scrolled = true;
        }
        else if (pbList->VPos + pbList->ClientHeight < frowsize * (rix + 1))
        {
            pbList->VPos = frowsize * (rix + 1) - pbList->ClientHeight;
            scrolled = true;
        }
    }
    else
    {
        if (pbList->VPos > fcompactsize * rix)
        {
            pbList->VPos = fcompactsize * rix;
            scrolled = true;
        }
        else if (pbList->VPos + pbList->ClientHeight < fcompactsize * (rix + 1))
        {
            pbList->VPos = fcompactsize * (rix + 1) - pbList->ClientHeight;
            scrolled = true;
        }
    }

    if (!scrolled)
    {
        InvalidateRow(oldsel);
        InvalidateRow(fsel);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == L' ' && canalter)
        Check(true);
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    int pg;
    switch (Key)
    {
    case VK_UP:
    case VK_DOWN:
        selected += (Key == VK_UP ? -1 : 1) * (btnForward->Visible ? 1 : -1);
        break;
    case VK_HOME:
    case VK_END:
        selected = abs((btnForward->Visible ? 0 : flist->Count - 1) - (Key == VK_END ? flist->Count - 1 : 0));
        break;
    case VK_PRIOR:
    case VK_NEXT:
        pg = MAX(1, (!btnCompact->Down ? (pbList->ClientHeight + frowsize - 1) / frowsize : (pbList->ClientHeight + fcompactsize - 1) / fcompactsize) - 1);
        selected += (Key == VK_PRIOR ? -pg : pg) * (btnForward->Visible ? 1 : -1);
        break;
    case VK_LEFT:
        pbList->HPos -= pbList->HorzScrollBar->SmallChange;
        break;
    case VK_RIGHT:
        pbList->HPos += pbList->HorzScrollBar->SmallChange;
        break;
    case VK_SPACE:
        Key = 0;
//        if (canalter)
//            Check(true);
        break;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y)
{
    if (Button == mbLeft && fmousecheck && foncheck)
    {
        fmousecheck = 0;
        if (canalter || canremove)
            Check(false);
    }
    else
        fmousecheck = 0;
}
//---------------------------------------------------------------------------
void TfWReview::Check(bool key)
{
    if ((foncheck == 1 || key) && canalter)
    {
        fstats->Items[(key ? fsel : fmouserow)]->meaningerror *= -1;
        fstats->Items[(key ? fsel : fmouserow)]->kanjierror *= -1;
        fstats->Items[(key ? fsel : fmouserow)]->kanaerror *= -1;
        InvalidateRow((key ? fsel : fmouserow));
    }
    else if (foncheck == 2 && canremove)
    {
        if (btnRandom10->Down || btnRandom20->Down || btnRandom30->Down)
        {
            btnRandom10->AllowAllUp = true;
            btnRandom10->Down = false;
            btnRandom20->Down = false;
            btnRandom30->Down = false;
        }
        fstats->Items[fmouserow]->skip = (fstats->Items[fmouserow]->skip < 0 ? 0 : -1);
        DrawCheck(fmouserow);
    }

    bool haswrong = wrong > 0;
    UpdateScore();
    selected = selected; // Looks stupid, but this is important to keep selected inside the accepted range.

    if (haswrong != (wrong > 0))
    {
        if (btnRandom10->Down || btnRandom20->Down || btnRandom30->Down)
            btnRandom10Click(btnRandom10->Down ? btnRandom10 : btnRandom20->Down ? btnRandom20 : btnRandom30);
        else
        {
            for (int i = 0; i <= ffnotw; i++)
                InvalidateRow(i);
            for (int i = flist->Count - flw + 1; i < flist->Count; i++)
                InvalidateRow(i);
        }
    }

    while (flist->Count - ((skip + (wrong ? flw - 1 : 0) + (wrong ? 0 : ffnotw))) < 3)
    {
        for (int ix = 0; ix < fstats->Count; ++ix)
        {
            if (fstats->Items[ix]->skip)
            {
                fstats->Items[ix]->skip = false;
                DrawCheck(ix);
                break;
            }
        }
        UpdateScore();
    }

    if (flist->Count - ((skip + (wrong ? flw - 1 : 0) + (wrong ? 0 : ffnotw))) <= 3)
    {
        for (int ix = 0; ix < fstats->Count; ++ix)
            if (!fstats->Items[ix]->skip)
                DrawCheck(ix);
        oldminusdisabled = true;
    }
    else if (oldminusdisabled)
    {
        oldminusdisabled = false;
        for (int ix = 0; ix < fstats->Count; ++ix)
            if (!fstats->Items[ix]->skip)
                DrawCheck(ix);
    }
}
//---------------------------------------------------------------------------
void TfWReview::InvalidateRow(int ix)
{
    TRect r;
    if (!btnCompact->Down)
    {
        r = Rect(0, 0, pbList->ClientWidth, frowsize);
        OffsetRect(&r, 0, realrow(ix)*frowsize - pbList->VPos);
    }
    else
    {
        r = Rect(0, 0, pbList->ClientWidth, fcompactsize);
        OffsetRect(&r, 0, realrow(ix)*fcompactsize - pbList->VPos);
    }
    if (r.Top >= pbList->ClientHeight || r.Bottom < 0)
        return;
    InvalidateRect(pbList->Handle, &r, false);
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::pbListEnter(TObject *Sender)
{
    pbList->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::btnAllClick(TObject *Sender)
{
    for (int i = 0; i < fstats->Count; i++)
        fstats->Items[i]->skip = 0;
    pbList->Refresh();
    btnRandom10->AllowAllUp = true;
    btnRandom10->Down = false;
    btnRandom20->Down = false;
    btnRandom30->Down = false;
    UpdateScore();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::btnRemoveClick(TObject *Sender)
{
    for (int i = 0; i < fstats->Count; i++)
        if (fstats->Items[i]->score > 20 && fstats->Items[i]->meaningerror <= 0 && fstats->Items[i]->kanjierror <= 0 && fstats->Items[i]->kanaerror <= 0)
            fstats->Items[i]->skip = -1;

    btnRandom10->AllowAllUp = true;
    btnRandom10->Down = false;
    btnRandom20->Down = false;
    btnRandom30->Down = false;
    pbList->Refresh();
    UpdateScore();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::btnGenerateClick(TObject *Sender)
{
    TWordGroup *g;
    TWordCollection *c = flist->collection;
    int wrong = 0;
    int i = RealIndex(cbGroup);

    MODON
    bool mod = false;
    if (i >= 0)
        g = c->groups->Items[i];
    else
    {
        g = (c->groups->Items[c->groups->Add(cbGroup->Text.Trim())]);
        cbGroup->Items->Add(cbGroup->Text.Trim());
        cbGroup->ItemIndex = cbGroup->Items->Count - 1;
        mod = true;
    }
    for (int i = 0; i < fstats->Count; i++)
    {
        if (fstats->Items[i]->meaningerror > 0 || fstats->Items[i]->kanjierror > 0 || fstats->Items[i]->kanaerror > 0)
        {
            TWord *w = c->words->Items[flist->Items[i]->index];
            TWordGroups *gs = w->group;
            while (gs)
            {
                if (gs->meaning == flist->Items[i]->meaning && gs->group == g)
                    break;
                gs = gs->next;
            }
            if (!gs) // No match.
            {
                wrong++;
                mod = true;
                g->AddWord(flist->Items[i]->index, flist->Items[i]->meaning);
            }
        }
    }
    if (mod)
        flist->collection->Modify(false);
    MODOFF
    if (wrong > 1)
        MsgBox((IntToStr(wrong) + " words were added to the group.").c_str(), "Message", MB_OK);
    else if (wrong == 1)
        MsgBox("One word was added to the group.", "Message", MB_OK);
    else
        MsgBox("No words were added to the group.", "Message", MB_OK);
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::cbGroupChange(TObject *Sender)
{
    btnGenerate->Enabled = btnGenerate->Tag > 0 && cbGroup->Text.Trim().Length() > 0;
}
//---------------------------------------------------------------------------
struct __TRandStat
{
    int state;
    int num;
    word round;
};
void __fastcall TfWReview::btnRandom10Click(TObject *Sender)
{
    UpdateScore();

    TSpeedButton *b = (TSpeedButton*)Sender;
    if (!b->Down)
        return;

    b->AllowAllUp = false;
    TWordStudyState *wss;
    TWordStudyState ws;
    int pmin = 100000;
    int rounddiff = 0;
    int rpos;
    int range = 0;
    int cur;
    int m = wrong ? 0 : ffnotw;
    int n = flist->Count - (wrong ? flw : 1);

    for (int i = 0; i < fstats->Count; i++)
    {
        wss = fstats->Items[i];
        wss->skip = 0;
        if (i < m || i > fstats->Count - flw)
            continue;
        rounddiff = MAX(rounddiff, fround - wss->round);
    }

    if (n - m + 1 > b->Tag)
    {
        TTemplateList<__TRandStat, true> *l = new TTemplateList<__TRandStat, true>;
        for (int i = m; i <= fstats->Count - flw; i++)
        {
            __TRandStat *st = new __TRandStat;

            wss = fstats->Items[i];
            ws = *wss;
            fgroup->SaveStat(ws/*, flist->Words[i]->stats[flist->Items[i]->meaning].state*/, wss->meaningerror < 0 || wss->kanjierror < 0 || wss->kanaerror < 0);

            pmin = MIN(pmin, ws.score);
            st->num = ws.score;
            st->state = i;
            st->round = ws.round;
            l->Add(st);
        }

        try
        {
            for (int i = 0; i < l->Count; i++)
            {
                cur = l->Items[i]->num - pmin + 1;
                cur *= (rounddiff - (fround - l->Items[i]->round) + 1);
                l->Items[i]->num = cur;
                if (cur <= 0)
                    THROW(L"???");
                range += cur;
            }

            while (l->Count + (!wrong ? flw - 1 : 0) > b->Tag)
            {
                rpos = Random(range);
                for (int i = 0; i < l->Count; i++)
                {
                    wss = fstats->Items[l->Items[i]->state];
                    cur = l->Items[i]->num;
                    rpos -= cur;
                    if (rpos < 0)
                    {
                        range -= cur;
                        wss->skip = -1;
                        l->Delete(i);
                        break;
                    }
                }
            }

        }
        catch (...)
        {
            ;
        }

        delete l;
    }
    pbList->Refresh();
    UpdateScore();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::FormClose(TObject *Sender, TCloseAction &Action)
{
    lastrandom = btnRandom10->Down || btnRandom20->Down || btnRandom30->Down;
    settings.compactreview = btnCompact->Down;
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::btnBackwardClick(TObject *Sender)
{
    if (btnForward->Visible)
    {
        btnBackward->Visible = true;
        btnForward->Visible = false;
    }
    else
    {
        btnForward->Visible = true;
        btnBackward->Visible = false;
    }

    bool scrolled = false;
    int scrollpos;

    if (realrow(fsel) * (!btnCompact->Down ? frowsize : fcompactsize) < pbList->VPos || (realrow(fsel) + 1) * (!btnCompact->Down ? frowsize : fcompactsize) - pbList->VPos >= pbList->ClientHeight)
    {
        scrolled = true;
        scrollpos = MAX(0.0, ((double)realrow(fsel) + 0.5) * (!btnCompact->Down ? frowsize : fcompactsize) - pbList->ClientHeight / 2);
    }

    if (scrolled)
        pbList->VertScrollBar->Position = scrollpos;
    else
        pbList->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::btnCompactClick(TObject *Sender)
{
    count_sizes();

    bool scrolled = false;
    int scrollpos;

    if (realrow(fsel) * (!btnCompact->Down ? frowsize : fcompactsize) < pbList->VPos || (realrow(fsel) + 1) * (!btnCompact->Down ? frowsize : fcompactsize) - pbList->VPos >= pbList->ClientHeight)
    {
        scrolled = true;
        scrollpos = MAX(0.0, ((double)realrow(fsel) + 0.5) * (!btnCompact->Down ? frowsize : fcompactsize) - pbList->ClientHeight / 2);
    }

    if (scrolled)
        pbList->VertScrollBar->Position = scrollpos;
    else
        pbList->Refresh();
}
//---------------------------------------------------------------------------
void __fastcall TfWReview::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == VK_RETURN && !btnContinue->Focused())
        btnContinue->Click();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(193);
#endif

