//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <DateUtils.hpp>
#include "wordstudytest.h"
#include "studylist.h"
#include "settings.h"
#include "kanjiedit.h"
#include "recognizer.h"
#include "repetition.h"
#include "gengraph.h"
#include "romajizer.h"
#include "sitemanager.h"
#include "listimport.h"
#include "words.h"
#include "groupadd.h"
#include "kanjiinfo.h"
#include "collection.h"
#include "sodpopup.h"
#include "mouseovers.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(198);

// Make compiler happy by using variables declared elsewhere:
static bool Dummy_function()
{
    return OneHour * OneMinute * OneSecond * OneMillisecond > 10;
}
#endif

TfTest *fTest;

extern bool hasrecdata;
//---------------------------------------------------------------------------
UnicodeString __fastcall intervalstring(double interval)
{
    int h = interval / OneHour;
    interval -= h * OneHour;
    int m = interval / OneMinute;
    interval -= m * OneMinute;
    int s = interval / OneSecond;
    while (h > 99)
        h -= 100;
    UnicodeString htime = (UnicodeString)(h < 10 ? L"0" : L"") + h;
    UnicodeString mtime = (UnicodeString)(m < 10 ? L"0" : L"") + m;
    UnicodeString stime = (UnicodeString)(s < 10 ? L"0" : L"") + s;
    return htime + L":" + mtime + L":" + stime;
}
//---------------------------------------------------------------------------
__fastcall TfTest::TfTest(TComponent* Owner) : gindex(-1), answered(false), answering(false), mobj(NULL), TBaseForm(Owner)
{
    //RescaleStatusbar(sbInfo);

    fSOD = new TfSOD(Application);

    TPanel *panels[] = { pInput1, pInput2, pInput3, pAnswer };
    for (int ix = 0; ix < 4; ++ix)
    {
        panels[ix]->Top = ClientHeight - panels[ix]->Height - sbInfo->Height;
        panels[ix]->Visible = false;
    }
    edkanji = new TKanjiEdit(pbKanji, true, true, NULL, &kcallback);

    checklimit = profilesettings.kwarnafter || profilesettings.kwarnitems;
    if (checklimit)
        resetlimits();

    lb0->Visible = profilesettings.klabelinterval;
    lb1->Visible = profilesettings.klabelinterval;
    lb2->Visible = profilesettings.klabelinterval;
    lb3->Visible = profilesettings.klabelinterval;

    if (sitelist->Count)
    {
        miSites->Visible = true;
        for (int ix = 0; ix < sitelist->Count; ++ix)
        {
            TMenuItem *mi = new TMenuItem(miSites);
            mi->Caption = sitelist->Items[ix]->name;
            mi->Tag = ix;
            mi->OnClick = siteClick;
            miSites->Add(mi);
        }
    }

    mobj = new TMouseOvers(this, doMOPrepare, (TMouseOversMeasureItemProc)NULL, doMODraw, doMOChange);
    mobj->Visible = false;

    fRecognize->callback = &resetsuspendtime;

    AppWindow = true;
}
//---------------------------------------------------------------------------
__fastcall TfTest::~TfTest()
{
    fRecognize->resetpos = recognizereset;
    if (!recognizereset)
        fRecognize->SetDimensions(recognizerect.Left, recognizerect.Top, recognizerect.Right);
    fRecognize->callback = NULL;
    delete mobj;

    delete edkanji;

    delete fSOD;
    fSOD = 0;
}
//---------------------------------------------------------------------------
int TfTest::ShowModalEx(TWordStudyList *astudy)
{
    study = astudy;
    due = study->testdue;
    newcnt = study->testnew;
    tested = 0;
    if (study->statcount)
    {
        const TDayStat &stat = study->daystat[study->statcount - 1];
        if (stat.day == get_day(Now()))
            tested = stat.testcount;
    }
    failed = study->faileddue;

    return ShowModal();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::FormShow(TObject *Sender)
{
    recognizereset = fRecognize->resetpos;
    fRecognize->resetpos = false;
    recognizerect = Rect(fRecognize->Left, fRecognize->Top, fRecognize->Width, fRecognize->Height);
    if (recognizereset)
        edkanji->PlaceRecognize(rpBelow);

    start = Now();
    tTime->Enabled = true;
    Application->ProcessMessages();
    PostMessage(Handle, WM_Z_INIT, 0, 0);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::siteClick(TObject *Sender)
{
    TMenuItem *mi = ((TMenuItem*)Sender);
    LaunchSiteSearch(study->testkanji, mi->Tag, sitelist);
    btnPause->Click();
}
//---------------------------------------------------------------------------
void TfTest::z_init(TMessage &msg)
{
    initnext();
}
//---------------------------------------------------------------------------
void TfTest::z_ncactivate(TWMNCActivate &msg)
{
    HWND h = GetForegroundWindow();
    int wparam = (*(TMessage*) &msg).WParam;
    if (AppActive && msg.Active == false && fSOD && fSOD->Visible && (h == Handle || h == fSOD->Handle))
        wparam = true;

    msg.Result = DefWindowProc(Handle, WM_NCACTIVATE, wparam, (*(TMessage*) &msg).LParam);

}
//---------------------------------------------------------------------------
void TfTest::resetlimits()
{
    itemlimit = profilesettings.kwarnitemscount;
    timestart = Now();
}
//---------------------------------------------------------------------------
void TfTest::initnext()
{
    resetsuspendtime();
    mobj->Visible = false;

    if (checklimit && (profilesettings.kwarnafter && (double(Now()) -double(timestart)) / OneMinute >= profilesettings.kwarnaftertime || profilesettings.kwarnitems && !itemlimit))
    {
        tTime->Enabled = false;
        pLimit->Visible = true;
        pPause->Visible = false;
        pAnswer->Visible = false;
        pOptions->Visible = false;
        pInput1->Visible = false;
        pInput2->Visible = false;
        pInput3->Visible = false;
        pHint->Visible = false;
        lbFailed->Visible = false;
        lbNew->Visible = false;
        lbKanji->Visible = false;
        lbKana->Visible = false;
        lbMeaning->Visible = false;
        bv->Visible = false;
        lbKanjiQ->Visible = false;
        lbKanaQ->Visible = false;
        lbMeaningQ->Visible = false;
        answering = false;
        Invalidate();
        return;
    }
    else if (pLimit->Visible)
    {
        pLimit->Visible = false;
        pPause->Visible = true;
        lbKanji->Visible = true;
        lbKana->Visible = true;
        lbMeaning->Visible = true;
        bv->Visible = true;
        if (checklimit && profilesettings.kwarnafter)
        {
            start = Now();
        }
        tTime->Enabled = true;
    }

    update_info();

    ETA = study->testestimate();
    sbInfo->Panels->Items[5]->Text = intervalstring(ETA);

    study->startround();
    q = study->testquestion();
    fhint = study->testanswer();
    newitem = study->newquestion();
    firsttime = study->firstquestion();
    faileditem = !firsttime && study->failedquestion();
    miQuestion->Visible = !study->testalladded;

    lbNew->Visible = newitem;
    lbFailed->Visible = faileditem;

    TColor col = coloravg((TColor)colorsettings.background, (TColor)0x909090, 0.6);

    lbKanji->Font->Color = (TColor)colorsettings.maintext;
    lbKana->Font->Color = (TColor)colorsettings.maintext;
    lbMeaning->Font->Color = (TColor)colorsettings.maintext;

    if (q == qtKanji)
        fsecondary = fhint == qtKana ? qtMeaning : qtKana;
    else if (q == qtMeaning)
        fsecondary = fhint == qtKana ? qtKanji : qtKana;
    else if (q == qtKana)
        fsecondary = fhint == qtKanji ? qtMeaning : qtKanji;

    if (fhint == qtKanji)
    {
        UnicodeString k = study->testkanji;
        bool nokanji = true;
        for (int ix = k.Length(); ix > 0 && nokanji; --ix)
            nokanji = !KANJI(k[ix]);
        if (nokanji)
            fhint = fsecondary;
    }

    pHint->Visible = fhint != fsecondary;
    sbHint->Enabled = has_kanji(study->testkanji.c_str());

    if (q == qtKanji)
    {
        lbMeaningQ->Visible = false;
        lbKanjiQ->Visible = true;
        lbKanaQ->Visible = false;
        if (fhint == qtKana)
            lbMeaning->Font->Color = col;
        else
            lbKana->Font->Color = col;
    }
    else if (q == qtKana)
    {
        lbMeaningQ->Visible = false;
        lbKanaQ->Visible = true;
        lbKanjiQ->Visible = false;
        if (fhint == qtMeaning)
            lbKanji->Font->Color = col;
        else
            lbMeaning->Font->Color = col;
    }
    else if (q == qtMeaning)
    {
        lbMeaningQ->Visible = true;
        lbKanaQ->Visible = false;
        lbKanjiQ->Visible = false;
        if (fhint == qtKanji)
            lbKana->Font->Color = col;
        else
            lbKanji->Font->Color = col;
    }

    if (q == qtKanji && profilesettings.ktypekanji)
    {
        edkanji->Text = L"";
        pInput3->Visible = true;
        sbRecognize->Visible = true;
        pInput1->Visible = false;
        pInput2->Visible = false;
        edkanji->EnableRecognizer(sbRecognize, false, false, false, 1, true, false, rpDefault);
        edkanji->AcceptKanji = true;
        fRecognize->Reset();

        pbKanji->SetFocus();

        if (sbRecognize->Enabled)
        {
            sbRecognize->Down = true;
            sbRecognize->Click();
        }
    }
    else if (q == qtKana && profilesettings.ktypekana)
    {
        edkanji->Text = L"";
        pInput3->Visible = true;
        sbRecognize->Visible = false;
        pInput1->Visible = false;
        pInput2->Visible = false;
        edkanji->DisableRecognizer();
        edkanji->AcceptKanji = false;

        pbKanji->SetFocus();
    }
    else if (q == qtMeaning && profilesettings.ktypemeaning)
    {
        edRomaji->Text = L"";
        pInput2->Visible = true;
        pInput1->Visible = false;
        pInput3->Visible = false;

        edRomaji->SetFocus();
    }
    else
    {
        pInput1->Visible = true;
        pInput2->Visible = false;
        pInput3->Visible = false;

        btnShowAnswer->SetFocus();
    }

    pAnswer->Visible = false;
    pOptions->Visible = false;

    if (q == qtKanji)
    {
        UnicodeString str = study->testkanji;
        int x = bv->Left + 10;
        int y = lbKanji->Top;
        int len = str.Length();
        mobj->reset(len);
        TSize s;
        TCanvas *c = Canvas;

        int cnt;
        int *dist = NULL;
        try
        {
            dist = new int[str.Length() + 1];
            doMOPrepare(c, 0, false);
            GetTextExtentExPointW(c->Handle, str.c_str(), str.Length(), 99999999, &cnt, dist, &s);
            dist[str.Length()] = s.cx;

            for (int ix = 0; ix < cnt; ++ix)
            {

                mobj->add(Rect(x, y, x + dist[ix] - (ix ? dist[ix - 1] : 0), y + s.cy), (int)str[ix + 1], KANJI(str[ix + 1]));
                x += dist[ix] - (ix ? dist[ix - 1] : 0);
                if (ix < cnt - 1 && x + dist[ix + 1] - dist[ix] > ClientWidth - 15 - pOptions->Width)
                {
                    y += s.cy;
                    x = bv->Left + 10;
                }
            }

        }
        catch (...)
        {
            mobj->reset(0);
        }
        delete[] dist;
    }

    if (study->hasundo)
    {
        set_undofields(study->lastanswer);
        pUndo->BringToFront();
        pUndo->Visible = true;
    }

    answering = false;
    if (Visible)
        Invalidate();

#ifdef AUTOTEST
    if (due > 1)
    {
        skip();
        Application->ProcessMessages();
        if (Random(1000) < 650)
            PostMessage(Handle, WM_Z_TMP, 0, 0);
        else
            PostMessage(Handle, WM_Z_TMP, 0, 1);
    }
#endif
}
//---------------------------------------------------------------------------
void TfTest::update_info()
{
    sbInfo->Panels->Items[1]->Text = (UnicodeString)due + L"(" + newcnt + L")"; //due
    sbInfo->Panels->Items[3]->Text = (UnicodeString)tested + L"(" + failed + L")"; //tested
}
//---------------------------------------------------------------------------
void TfTest::set_undofields(TRepetitionAnswer answer)
{
    pmChange->Tag = answer;
    sbChange->Caption = answer_text(answer);
    undogoodinterval = study->undo_goodinterval;
    undoeasyinterval = study->undo_easyinterval;

    update_info();
}
//---------------------------------------------------------------------------
const wchar_t* TfTest::answer_text(TRepetitionAnswer answer)
{
    switch (answer)
    {
    case raUncertain:
        return L"Retry";
    case raHard:
        return L"Incorrect";
    case raGood:
        return L"Correct";
    case raEasy:
        return L"Easy";
    default:
        THROW(L"No such answer!");
    };
}
//---------------------------------------------------------------------------
#ifdef AUTOTEST
void __fastcall TfTest::tmpmsg(TMessage &msg)
{
    if (msg.LParam == 0)
        btn2->Click();
    else
        btn1->Click();
}
#endif
//---------------------------------------------------------------------------
void TfTest::kcallback()
{
    resetsuspendtime();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::sbRecognizeClick(TObject *Sender)
{
    resetsuspendtime();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::edRomajiKeyPress(TObject *Sender, wchar_t &Key)
{
    resetsuspendtime();

    if (Key == VK_RETURN)
    {
        btnRomajiAccept->Click();
        Key = 0;
    }
    if (Key == VK_ESCAPE)
        skip();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::edRomajiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (Key == VK_RETURN)
        Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfTest::pbKanjiKeyPress(TObject *Sender, wchar_t &Key)
{
    if (!pInput3->Visible)
        return;

    if (Key == 0x0D) // Enter key.
    {
        btnKanjiAccept->Click();
        Key = 0;
    }
    if (Key == 0x1B) // Escape key.
    {
        skip();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfTest::pbKanjiKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (!pInput3->Visible)
        return;

    if (Key == VK_RETURN)
    {
        //btnKanjiAccept->Click();
        Key = 0;
    }
    if (Key == VK_ESCAPE)
    {
        //skip();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnRomajiAcceptClick(TObject *Sender)
{
    accept(edRomaji->Text);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnKanjiAcceptClick(TObject *Sender)
{
    accept(edkanji->Text);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnShowAnswerClick(TObject *Sender)
{
    skip();
}
//---------------------------------------------------------------------------
void TfTest::accept(const UnicodeString &aanswer)
{
    resetsuspendtime();

    entered = aanswer;

    fRecognize->Hide();
    sbRecognize->Down = false;

    lbAnswer->Visible = aanswer.Length();
    pbAnswer->Visible = aanswer.Length();

    lbWordTypes->Caption = study->testwordtype();

    int r2 = MAX(1.0, (int)study->testinterval(raGood));
    int r3 = MAX(1.0, (int)study->testinterval(raEasy));

    if (profilesettings.klabelinterval)
    {
        lb2->Caption = generate_intervalstring(r2);
        lb3->Caption = generate_intervalstring(r3);
    }

    pUndo->Visible = false;
    pAnswer->Visible = true;
    if (q == qtKanji)
        mobj->Visible = true;

    pOptions->Visible = true;
    lbFailed->Visible = false;
    lbNew->Visible = false;

    pInput1->Visible = false;
    pInput2->Visible = false;
    pInput3->Visible = false;

    lbMeaningQ->Visible = false;
    lbKanjiQ->Visible = false;
    lbKanaQ->Visible = false;

    sbHint->Click();
}
//---------------------------------------------------------------------------
void TfTest::skip()
{
    accept("");
}
//---------------------------------------------------------------------------
void __fastcall TfTest::FormPaint(TObject *Sender)
{
    if (answering)
        return;

    TCanvas *c = Canvas;

    if (pLimit->Visible || pSuspended->Visible)
    {
        DrawPillowPanel(this, NULL, bv1);
        DrawPillowPanel(this, bv1, bv2, true);
        DrawPillowPanel(Canvas, Rect(0, ChildRect(bv2).Bottom, ClientWidth, sbInfo->Top));
        return;
    }

    int inner = ClientHeight - pInput1->Height - sbInfo->Height;

    int bdwidth = 4;

    c->Brush->Color = COLORDEC(faileditem ? clRed : newitem ? 0xae7b4f : 0xAFAFAF, 0x282828);
    for (int ix = 0; ix < 4; ++ix)
    {
        c->FrameRect(Rect(ix, ix, ClientWidth - ix, ClientHeight - ix - sbInfo->Height));
        c->Brush->Color = COLORINC(c->Brush->Color, 0x282828);
    }

    c->Brush->Color = (TColor)clWhite;
    c->FillRect(Rect(bdwidth, bdwidth, ClientWidth - bdwidth, inner));

    drawhinttext(fhint);
    if (!pHint->Visible && fsecondary != fhint)
        drawhinttext(fsecondary);
    if (pAnswer->Visible)
        drawhinttext(q);

    DrawGrad(c, c->Brush->Color, COLORDEC(c->Brush->Color, 0x101010), Rect(bdwidth, inner, ClientWidth - bdwidth, ClientHeight - bdwidth - sbInfo->Height));
}
//---------------------------------------------------------------------------
void __fastcall TfTest::sbHintClick(TObject *Sender)
{
    pHint->Visible = false;
    lbKanji->Font->Color = (TColor)colorsettings.maintext;
    lbKana->Font->Color = (TColor)colorsettings.maintext;
    lbMeaning->Font->Color = (TColor)colorsettings.maintext;

    resetsuspendtime();
    Invalidate();
}
//---------------------------------------------------------------------------
void TfTest::drawhinttext(TQuestionType type)
{
    TCanvas *c = Canvas;

    UnicodeString tmp;
    int top;

    int inner = ClientHeight - pInput1->Height - sbInfo->Height;
    TRect r = Rect(bv->Left + 10, 0, ClientWidth - 15, 1);

    c->Brush->Color = (TColor)clWhite;
    c->Font->Color = (TColor)clBlack;
    if (type == qtKanji)
    {
        c->Font->Name = settings.kanjifont;
        c->Font->Charset = settings.kanjicharset;
        c->Font->Size = 24;
        r.Right = pOptions->Left - 5;
        tmp = study->testkanji;
        top = lbKanji->Top;
    }
    else if (type == qtKana)
    {
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = 24;
        r.Right = pOptions->Left - 5;
        tmp = study->testkana;
        top = lbKana->Top;
    }
    else if (type == qtMeaning)
    {
        c->Font->Name = settings.romajifont;
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 18;
        tmp = study->testmeaning;
        top = lbMeaning->Top;
    }

    if (q == type)
    {
        bool sametext;
        if (q == qtMeaning)
            sametext = entered.Trim().LowerCase() == tmp.Trim().LowerCase();
        else
            sametext = Hiraganize(entered.Trim().LowerCase().c_str()) == Hiraganize(tmp.Trim().LowerCase().c_str());
        c->Font->Color = coloravg(c->Font->Color, entered.Length() && sametext ? clGreen : clRed, 0.3);
    }

    do
    {
        DrawTextW(c->Handle, tmp.c_str(), tmp.Length(), &r, DT_CALCRECT | DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK);
        OffsetRect(&r, 0, top);
        if (type == qtMeaning && r.Bottom > inner - 4 && c->Font->Size > 10)
        {
            c->Font->Size -= 2;
            r.bottom = r.top = 0;
        }
        else
            r.Bottom = min((int)r.Bottom, inner - 4);
    }
    while (r.Bottom == r.Top && c->Font->Size > 6);

    if (r.Bottom == r.Top)
    {
        r.Top = top;
        r.Bottom = type == qtKanji ? lbKana->Top : type == qtKana ? lbMeaning->Top : pAnswer->Top;
    }
    else
        r.Bottom = min((int)r.Bottom, type == qtKanji ? lbKana->Top : type == qtKana ? lbMeaning->Top : pAnswer->Top);

    if (type != qtKanji)
    {
        DrawTextW(c->Handle, tmp.c_str(), tmp.Length(), &r, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS);
    }
    else
    {
        int x = r.Left;
        int y = r.Top;

        UnicodeString tmp = study->testkanji;

        if (q == qtKanji)
            mobj->draw(x, y);
        else
        {
            int cnt;
            TSize s;
            int *dist = NULL;
            try
            {
                dist = new int[tmp.Length() + 1];
                GetTextExtentExPointW(c->Handle, tmp.c_str(), tmp.Length(), 99999999, &cnt, dist, &s);
                dist[tmp.Length()] = s.cx;
                int skipx = 0, y = r.Top;

                for (int ix = 0; ix < cnt; ++ix)
                {
                    if (ix && ix < cnt - 1 && dist[ix - 1] - skipx >= r.Width())
                    {
                        skipx = dist[ix - 1];
                        y += s.cy;
                    }

                    if (y >= r.Bottom)
                        break;

                    ExtTextOutW(c->Handle, r.Left + (ix ? dist[ix - 1] - skipx : 0), y, ETO_CLIPPED, &r, tmp.c_str() + ix, 1, NULL);

                }
            }
            catch (...)
            {
                ;
            }
            delete[] dist;
        }
    }
}
//---------------------------------------------------------------------------
void __fastcall TfTest::FormKeyPress(TObject *Sender, wchar_t &Key)
{
    if ((pInput1->Visible && Key == 0x0D) || (Key == L' ' && (pInput1->Visible || pInput2->Visible || pInput3->Visible) && !pbKanji->Focused() && dynamic_cast<TEdit*>(ActiveControl) == NULL && dynamic_cast<TButton*>(ActiveControl) == NULL && dynamic_cast<TBitBtn*>(ActiveControl) == NULL) )
    {
        btnShowAnswer->Click();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfTest::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    resetsuspendtime();

    if (pLimit->Visible)
    {
        if (Key == '1')
            btnLimit1->Click();
        if (Key == '2')
            btnLimit2->Click();
        if (Key == '3')
            btnLimit3->Click();
        return;
    }
    if (pSuspended->Visible)
    {
        if (Key == '1')
            btnResumeSuspend->Click();
        if (Key == '3')
            btnQuitSuspend->Click();
        return;
    }

    if (Shift.Contains(ssCtrl) && Key == 'H' && pHint->Visible && sbHint->Enabled)
        sbHint->Click();
    else if (pAnswer->Visible)
    {
        if (Key == '0')
            btn0->Click();
        if (Key == '1')
            btn1->Click();
        if (Key == '2')
            btn2->Click();
        if (Key == '3')
            btn3->Click();
    }
    else if (pInput1->Visible && Key == VK_RETURN || Key == VK_SPACE && !pbKanji->Focused())
    {
        //btnShowAnswer->Click();
        Key = 0;
    }
}
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;
void __fastcall TfTest::pbAnswerPaint(TObject *Sender)
{
    TCanvas *c = pbAnswer->Canvas;
    c->Brush->Color = (TColor)clWhite;
    c->Brush->Style = bsClear;
    c->Font->Color = (TColor)colorsettings.maintext;
    if (q == qtKanji)
    {
        c->Font->Name = settings.kanjifont;
        c->Font->Charset = settings.kanjicharset;
        c->Font->Size = 16;
    }
    else if (q == qtKana)
    {
        c->Font->Name = settings.kanafont;
        c->Font->Charset = settings.kanacharset;
        c->Font->Size = 16;
    }
    else if (q == qtMeaning)
    {
        c->Font->Name = settings.romajifont;
        c->Font->Charset = DEFAULT_CHARSET;
        c->Font->Size = 12;
    }
    if (!ThemesAreTurnedOn)
    {
        c->Brush->Style = bsSolid;
        c->FillRect(pbAnswer->ClientRect);
    }
    else
        DrawGrad(c, c->Brush->Color, COLORDEC(c->Brush->Color, 0x101010), Rect(-pbAnswer->Left, -pbAnswer->Top, pbAnswer->Left + pbAnswer->Width, pAnswer->Height - pbAnswer->Top));
    TRect r = pbAnswer->ClientRect;
    InflateRect(&r, -2, -2);
    r.Bottom = r.Top;

    DrawTextW(c->Handle, entered.c_str(), entered.Length(), &r, DT_CALCRECT | DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS);

    if (r.Height() <= pbAnswer->ClientHeight - 4)
        OffsetRect(&r, 0, (pbAnswer->ClientHeight - 4 - r.Height()) / 2);
    else
    {
        if (r.Bottom > pbAnswer->ClientHeight - 2)
            c->Font->Size = 10;
        r.Bottom = pbAnswer->ClientHeight - 2;
    }

    SetBkMode(c->Handle, TRANSPARENT);
    DrawTextW(c->Handle, entered.c_str(), entered.Length(), &r, DT_LEFT | DT_NOPREFIX | DT_TOP | DT_WORDBREAK | DT_END_ELLIPSIS);
    SetBkMode(c->Handle, OPAQUE);
    c->Pen->Color = cl3DDkShadow;
    c->MoveTo(0, pbAnswer->ClientHeight - 1);
    c->LineTo(pbAnswer->ClientWidth, pbAnswer->ClientHeight - 1);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btn1Click(TObject *Sender)
{
    answering = true;

    if (fSOD && fSOD->Visible)
        fSOD->Hide();
    mobj->Visible = false;

    TButton *btn = (TButton*)Sender;
    study->answertest((TRepetitionAnswer)btn->Tag);

    answered = true;

    undotested = tested;
    undofailed = failed;
    undodue = due;
    undofirsttime = firsttime;
    undofaileditem = faileditem;

    if (firsttime)
        tested++;
    if (btn->Tag <= 1 && firsttime)
        failed++;
    if (btn->Tag > 1)
    {
        due--;
        if (faileditem)
            failed--;
    }

    if (newitem)
        newcnt--;

    if (checklimit && profilesettings.kwarnitems)
        itemlimit--;

    if (!due)
        ModalResult = mrOk;
    else
        initnext();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::tTimeTimer(TObject *Sender)
{
    double n = (double)Now();
    sbInfo->Panels->Items[7]->Text = intervalstring(n - (double)start); //time passed

    if (profilesettings.ksuspendafter != stNoSuspend)
    {
        double measurelimit;
        switch (profilesettings.ksuspendafter)
        {
        case st1:
            measurelimit = OneMinute;
            break;
        case st1_5:
            measurelimit = OneMinute * 1.5;
            break;
        case st2:
            measurelimit = OneMinute * 2;
            break;
        case st3:
            measurelimit = OneMinute * 3;
            break;
        case st5:
            measurelimit = OneMinute * 5;
            break;
        default:
            measurelimit = 9999;
        }

        if (n - (double)lastinput > measurelimit)
            suspend();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnLimit2Click(TObject *Sender)
{
    checklimit = false;
    initnext();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnLimit1Click(TObject *Sender)
{
    resetlimits();
    initnext();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnOptionsClick(TObject *Sender)
{
    TPoint p = ClientToScreen(Point(pOptions->Left, pOptions->Top + pOptions->Height));
    pmOptions->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::miQuestionClick(TObject *Sender)
{
    TWordIndexList *l = NULL;
    try
    {
        l = new TWordIndexList(study->collection/*,false*/);
        l->Add(study->testwordindex);
        ListImport(l);
    }
    catch (...)
    {
        ;
    }
    delete l;

    if (study->testalladded)
        miQuestion->Visible = false;
}
//---------------------------------------------------------------------------
void __fastcall TfTest::miGroupClick(TObject *Sender)
{
    WordToGroup(study->collection, study->testwordindex, gindex);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnResumeSuspendClick(TObject *Sender)
{
    resume();
}
//---------------------------------------------------------------------------
template<typename T>
void TfTest::csuspend(T *c)
{
    if (c->Visible)
    {
        c->Tag = 1;
        c->Visible = false;
    }
}
//---------------------------------------------------------------------------
template<typename T>
void TfTest::cresume(T *c)
{
    if (c->Tag == 1)
    {
        c->Tag = 0;
        c->Visible = true;
    }
}
//---------------------------------------------------------------------------
void TfTest::suspend()
{
    if (pLimit->Visible)
        return;

    tTime->Enabled = false;

    csuspend(lbFailed);
    csuspend(lbNew);
    csuspend(lbKanji);
    csuspend(lbKanjiQ);
    csuspend(lbKana);
    csuspend(lbKanaQ);
    csuspend(lbMeaning);
    csuspend(lbMeaningQ);
    csuspend(pAnswer);
    csuspend(pInput1);
    csuspend(pInput2);
    csuspend(pInput3);
    csuspend(pHint);
    csuspend(pOptions);
    csuspend(bv);
    csuspend(pUndo);
    csuspend(pPause);

    study->suspenditem();
    start = double(Now()) -double(start);
    timestart = double(Now()) -double(timestart);

    btnOptions->Enabled = false;
    pSuspended->Visible = true;

    if (fSOD && fSOD->Visible)
        fSOD->Hide();
    mobj->Tag = mobj->Visible ? 1 : 0;
    if (mobj->Visible)
        mobj->Visible = false;

    Invalidate();
}
//---------------------------------------------------------------------------
void TfTest::resume()
{
    cresume(lbFailed);
    cresume(lbNew);
    cresume(lbKanji);
    cresume(lbKanjiQ);
    cresume(lbKana);
    cresume(lbKanaQ);
    cresume(lbMeaning);
    cresume(lbMeaningQ);
    cresume(pAnswer);
    cresume(pInput1);
    cresume(pInput2);
    cresume(pInput3);
    cresume(pHint);
    cresume(pOptions);
    cresume(bv);
    cresume(pUndo);
    cresume(pPause);

    pSuspended->Visible = false;
    btnOptions->Enabled = true;

    if (mobj->Tag)
        mobj->Visible = true;

    Invalidate();

    study->resumeitem();
    start = Now() - start;
    timestart = Now() - timestart;

    resetsuspendtime();
    tTime->Enabled = true;

    if (pbKanji->Parent->Visible)
    {
        pbKanji->SetFocus();
        if (edkanji->AcceptKanji && sbRecognize->Enabled)
        {
            sbRecognize->Down = true;
            sbRecognize->Click();
        }
    }
    else if (edRomaji->Parent->Visible)
    {
        edRomaji->SetFocus();
    }
    else if (btnShowAnswer->Parent->Visible)
    {
        btnShowAnswer->SetFocus();
    }

}
//---------------------------------------------------------------------------
void TfTest::resetsuspendtime()
{
    lastinput = Now();
}
//---------------------------------------------------------------------------
void __fastcall TfTest::FormMouseLeave(TObject *Sender)
{
    if (!mobj->Visible)
        return;

    TPoint p;
    GetCursorPos(&p);
    HWND h = ChildWindowFromPointEx(GetDesktopWindow(), p, CWP_SKIPINVISIBLE | CWP_SKIPDISABLED);

    if (fSOD && h == fSOD->Handle)
        mobj->lock();
}
//---------------------------------------------------------------------------
void TfTest::dcallback()
{
    mobj->unlock();
}
//---------------------------------------------------------------------------
void TfTest::doMOPrepare(TCanvas *c, int index, bool selected)
{
    c->Font->Name = settings.kanjifont;
    c->Font->Charset = settings.kanjicharset;
    c->Font->Size = 24;

    c->Pen->Style = psSolid;
    c->Pen->Width = 1;
    c->Pen->Color = clBlack;
}
//---------------------------------------------------------------------------
void TfTest::doMODraw(TCanvas *c, const TRect &r, int index, int data, bool selected)
{
    if (!fSOD)
        return;

    bool above = ClientToScreen(Point(0, r.Top)).y - fSOD->Height < 2;

    if (selected && ThemesAreTurnedOn)
    {
        TColor c1 = COLORDEC(clWhite, 0x050505), c2 = COLORDEC(clWhite, 0x202020);
        if (above)
            swap(c1, c2);
        GradientFillCanvas(c, c1, c2, r, gdVertical);
        c->Brush->Style = bsClear;
    }
    else
        c->Brush->Style = bsSolid;

    wchar_t ch = (wchar_t)data;
    ExtTextOutW(c->Handle, r.Left, r.Top, ETO_CLIPPED, &r, &ch, 1, NULL);

    if (selected)
    {
        c->MoveTo(r.Left, r.Top);
        c->LineTo(r.Left, r.Bottom);
        c->MoveTo(r.Right - 1, r.Top);
        c->LineTo(r.Right - 1, r.Bottom);
        if (!above)
        {
            c->MoveTo(r.Left, r.Bottom - 1);
            c->LineTo(r.Right - 1, r.Bottom - 1);
        }
        else
        {
            c->MoveTo(r.Left, r.Top);
            c->LineTo(r.Right - 1, r.Top);
        }
    }
}
//---------------------------------------------------------------------------
void TfTest::doMOChange(const TRect &r, int index, int data, bool selected)
{
    if (!fSOD)
        return;

    if (!selected)
    {
        if (!fSOD->Visible)
            return;

        fSOD->Hide();
        return;
    }

    TPoint p = ClientToScreen(Point(0, 0));
    TRect r2 = r;
    OffsetRect(r2, p.x, p.y);

    fSOD->ShowEx(this, r2, dcallback, (wchar_t)data);
}
//---------------------------------------------------------------------------
void TfTest::z_pos(TWMWindowPosChanging &msg)
{
    msg.Result = DefWindowProc(Handle, WM_WINDOWPOSCHANGING, (*(TMessage*)(&msg)).WParam, (*(TMessage*)(&msg)).LParam);
    if (!fSOD || !fSOD->Visible || fSOD->Activating || fSOD->ComponentState.Contains(csLoading))
        return;

    msg.WindowPos->hwndInsertAfter = fSOD->Handle;
    msg.WindowPos->flags = msg.WindowPos->flags & ~(SWP_NOOWNERZORDER | SWP_NOZORDER);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::miRetryClick(TObject *Sender)
{
    resetsuspendtime();

    TMenuItem *mi = (TMenuItem*)Sender;
    TRepetitionAnswer newanswer = (TRepetitionAnswer)mi->Tag;

    study->undo(newanswer);

    tested = undotested;
    failed = undofailed;
    due = undodue;

    //if (newanswer != raUncertain) {
    if (undofirsttime)
        tested++;
    if ((newanswer == raHard || newanswer == raUncertain) && undofirsttime)
        failed++;
    if (newanswer != raHard && newanswer != raUncertain)
    {
        due--;
        if (undofaileditem)
            failed--;
    }
    //}

    set_undofields(newanswer);

    bool oldenabled = tTime->Enabled;
    tTime->Enabled = false;

    MsgBox(L"The study data for the previous item has been updated.", L"Message", MB_OK);

    resetsuspendtime();
    tTime->Enabled = oldenabled;
}
//---------------------------------------------------------------------------
void __fastcall TfTest::sbChangeClick(TObject *Sender)
{
    resetsuspendtime();

    TPoint p = Point(0, sbChange->Height + 4);
    p = sbChange->ClientToScreen(p);

    if (pmChange->Tag == raUncertain)
        miRetry->Visible = false;
    else
    {
        miRetry->Visible = true;
        miRetry->Caption = (UnicodeString)L"Retry" + (profilesettings.klabelinterval ? L" (Few minutes)" : L"");
    }
    if (pmChange->Tag == raHard)
        miHard->Visible = false;
    else
    {
        miHard->Visible = true;
        miHard->Caption = (UnicodeString)L"Incorrect" + (profilesettings.klabelinterval ? L" (Few minutes)" : L"");
    }
    if (pmChange->Tag == raGood)
        miGood->Visible = false;
    else
    {
        miGood->Visible = true;
        miGood->Caption = (UnicodeString)L"Correct" + (profilesettings.klabelinterval ? (UnicodeString)L" (" + generate_intervalstring(undogoodinterval) + L")" : (UnicodeString)L"");
    }
    if (pmChange->Tag == raEasy)
        miEasy->Visible = false;
    else
    {
        miEasy->Visible = true;
        miEasy->Caption = (UnicodeString)L"Easy" + (profilesettings.klabelinterval ? (UnicodeString)L" (" + generate_intervalstring(undoeasyinterval) + L")" : (UnicodeString)L"");
    }

    pmChange->Popup(p.x, p.y);
}
//---------------------------------------------------------------------------
void __fastcall TfTest::btnPauseClick(TObject *Sender)
{
    suspend();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(199);
#endif



