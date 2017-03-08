//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "kanatest.h"
#include "romajizer.h"
#include "settings.h"
#include "gengraph.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(68);
#endif

TfKanaTest *fKanaTest;
//---------------------------------------------------------------------------
int TfKanaTest::testlen = 48;

wchar_t *syllables[104] = {
    L"か", L"き", L"く", L"け", L"こ", L"さ", L"し", L"す", L"せ", L"そ",
    L"た", L"ち", L"つ", L"て", L"と", L"な", L"に", L"ぬ", L"ね", L"の",
    L"は", L"ひ", L"ふ", L"へ", L"ほ", L"ま", L"み", L"む", L"め", L"も",
    L"や", L"ゆ", L"よ", L"ら", L"り", L"る", L"れ", L"ろ",
    L"わ", L"を", L"が", L"ぎ", L"ぐ", L"げ", L"ご",
    L"ざ", L"じ", L"ず", L"ぜ", L"ぞ", L"だ", L"ぢ", L"づ", L"で", L"ど",
    L"ば", L"び", L"ぶ", L"べ", L"ぼ", L"ぱ", L"ぴ", L"ぷ", L"ぺ", L"ぽ",

    L"きゃ", L"きゅ", L"きょ", L"しゃ", L"しゅ", L"しょ", L"ちゃ", L"ちゅ", L"ちょ",
    L"ひゃ", L"ひゅ", L"ひょ", L"にゃ", L"にゅ", L"にょ", L"ぎゃ", L"ぎゅ", L"ぎょ",
    L"じゃ", L"じゅ", L"じょ", L"ぢゃ", L"ぢゅ", L"ぢょ", L"びゃ", L"びゅ", L"びょ",
    L"ぴゃ", L"ぴゅ", L"ぴょ", L"りゃ", L"りゅ", L"りょ",
    L"あ", L"い", L"う", L"え", L"お", L"ん"
};
wchar_t *roomaji[104] = {
    L"KA", L"KI", L"KU", L"KE", L"KO", L"SA", L"SHI", L"SU", L"SE", L"SO",
    L"TA", L"CHI", L"TSU", L"TE", L"TO", L"NA", L"NI", L"NU", L"NE", L"NO",
    L"HA", L"HI", L"FU", L"HE", L"HO", L"MA", L"MI", L"MU", L"ME", L"MO",
    L"YA", L"YU", L"YO", L"RA", L"RI", L"RU", L"RE", L"RO",
    L"WA", L"WO", L"GA", L"GI", L"GU", L"GE", L"GO",
    L"ZA", L"JI", L"ZU", L"ZE", L"ZO", L"DA", L"DI(JI)", L"DU(ZU)", L"DE", L"DO",
    L"BA", L"BI", L"BU", L"BE", L"BO", L"PA", L"PI", L"PU", L"PE", L"PO",

    L"KYA", L"KYU", L"KYO", L"SHA", L"SHU", L"SHO", L"CHA", L"CHU", L"CHO",
    L"HYA", L"HYU", L"HYO", L"NYA", L"NYU", L"NYO", L"GYA", L"GYU", L"GYO",
    L"JA", L"JU", L"JO", L"DYA(JA)", L"DYU(JU)", L"DYO(JO)", L"BYA", L"BYU", L"BYO",
    L"PYA", L"PYU", L"PYO", L"RYA", L"RYU", L"RYO",
    L"A", L"I", L"U", L"E", L"O", L"N"
};
//---------------------------------------------------------------------------
__fastcall TfKanaTest::TfKanaTest(TComponent* Owner) : mistakecnt(0), TBaseForm(Owner)
{
    cnt = sizeof(syllables) / sizeof(syllables[0]);

    int ys = pb->ClientHeight / 10;
    TCanvas *c = pb->Canvas;
    c->Font->Size = double(ys) * 0.67 / scaleres;
    fsize = c->Font->Size;
    c->Font->Name = settings.kanafont;
    c->Font->Charset = settings.kanacharset;

    TSize s;
    GetTextExtentPoint32W(c->Handle, L"　", 1, &s);
    spacew = s.cx;

    GetTextExtentPoint32W(c->Handle, L"ヒョ", 2, &s);
    textw = s.cx;
    texth = s.cy;

    Width = Width - ClientWidth + (textw + spacew) * 8 + spacew;
    Height += (20 * scaleres + texth * ((testlen + 1) / 8) * 2.l) - pb->ClientHeight;

    lbType->Left = pbType->Left - lbType->Width - 8 * scaleres;
    lbError->Font->Name = settings.romajifont;
}
//---------------------------------------------------------------------------
int TfKanaTest::ShowModalEx(const Bits &hira, const Bits &kata)
{
    hiracnt = 0;
    katacnt = 0;
    for (int i = 0; i < cnt; i++)
    {
        if (hira[i])
            hiracnt++;
        if (kata[i])
            katacnt++;
    }
    wbuff = new wchar_t*[hiracnt + katacnt];
    rbuff = new wchar_t*[hiracnt + katacnt];
    gen = new int[testlen];
    misses = new bool[testlen];
    int r = mrCancel;
    bmp = new TBmp;
    bmp->SetSize(pb->ClientWidth, pb->ClientHeight);
    try
    {
        int pos = 0;
        for (int i = 0; i < cnt && hiracnt; i++)
        {
            if (hira[i])
            {
                wbuff[pos] = syllables[i];
                rbuff[pos++] = roomaji[i];
            }
        }
        for (int i = 0; i < cnt && katacnt; i++)
        {
            if (kata[i])
            {
                wbuff[pos] = syllables[i];
                rbuff[pos++] = roomaji[i];
            }
        }

        btnGen->Click();
        r = ShowModal();
    }
    catch (...)
    {
        ;
    }
    delete bmp;
    delete[] misses;
    delete[] gen;
    delete[] wbuff;
    delete[] rbuff;

    return r;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::btnGenClick(TObject *Sender)
{
    t->Enabled = false;
    donewrong = false;
    donegood = false;
    typed = "";
    testpos = 0;
    mistakes = 0;
    memset(misses, 0, sizeof(bool)*testlen);
    for (int i = 0; i < testlen; i++)
    {
        do
        {
            gen[i] = Random(hiracnt + katacnt);
        }
        while (i && hiracnt + katacnt > 4 && gen[i] == gen[i - 1]);
    }
    update();
    lbType->Visible = true;
    pbType->Visible = true;
    lbDone->Visible = false;
    lbMis->Visible = false;
    if (Visible)
        pbType->SetFocus();
    pbType->Invalidate();

    tTime->Enabled = true;

    starttime = GetTickCount();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::pbPaint(TObject *Sender)
{
    if (updatepb)
    {
        UnicodeString str;

        TCanvas *c = bmp->Canvas;
        c->FillRect(pb->ClientRect);

        TSize s;

        int x = spacew;
        int y = 10 * scaleres;

        TRect r = Rect(0, 0, 0, 0);
        SetTextAlign(c->Handle, TA_LEFT | TA_BASELINE);
        for (int i = 0; i < testlen; i++)
        {
            c->Font->Size = fsize;
            c->Font->Color = testpos == i ? clRed : misses[i] ? clMaroon : clBlack;
            if (misses[i])
                c->Font->Style = TFontStyles() << fsBold;
            else
                c->Font->Style = TFontStyles();
            c->Font->Name = settings.kanafont;
            c->Font->Charset = settings.kanacharset;
            str = gen[i] >= hiracnt ? ToKatakana(wbuff[gen[i]]).c_str() : wbuff[gen[i]];
            GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);
            if (i && !(i % 8))
            {
                x = spacew;
                y += texth * 2.1;
            }

            r = Rect(x - 2, y - 2, x + textw + 2, y + texth + 2);
            ExtTextOutW(c->Handle, r.Left + (r.Width() - s.cx) / 2, y + texth*0.82, ETO_CLIPPED, &r, str.c_str(), str.Length(), NULL);

            if (testpos > i)
            {
                c->Font->Size = fsize * 0.65;
                c->Font->Name = settings.romajifont;
                c->Font->Charset = DEFAULT_CHARSET;
                str = gen[i] < hiracnt ? AnsiLowerCase(rbuff[gen[i]]).c_str() : rbuff[gen[i]];
                GetTextExtentPoint32W(c->Handle, str.c_str(), str.Length(), &s);

                r = Rect(x - spacew / 2, y + texth, x + textw + spacew / 2, y + texth * 2 + 2);
                ExtTextOutW(c->Handle, r.Left + (r.Width() - s.cx) / 2, y + texth*1.62, ETO_CLIPPED, &r, str.c_str(), str.Length(), NULL);
            }

            x += spacew + textw;
        }
    }

    pb->Canvas->Draw(0, 0, bmp);
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::pbTypePaint(TObject *Sender)
{
    TCanvas *c = pbType->Canvas;
    c->Brush->Color = clBtnFace;
    c->FillRect(pbType->ClientRect);
    c->Pen->Color = pbType->Focused() ? clBtnText : coloravg(clBtnText, clBtnFace, 0.4);

    c->Pen->Width = 2 * scaleres;

    if (donegood || donewrong)
        c->Font->Color = donewrong ? clRed : clGreen;
    else
        c->Font->Color = clBlack;

    TRect r = pbType->ClientRect;
    r.Bottom -= 4;
    int y = lbType->Top - pbType->Top;

    int x = 0;
    while (x < pbType->ClientWidth)
    {
        c->MoveTo(x, pbType->ClientHeight - 2*scaleres);
        c->LineTo(x + 10*scaleres, pbType->ClientHeight - 2*scaleres);
        x += 16 * scaleres;
    }
    ExtTextOutW(c->Handle, 0, y, ETO_CLIPPED, &r, typed.c_str(), typed.Length(), 0);
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::FormPaint(TObject *Sender)
{
    DrawCaptionBar(this, NULL, bv1, false);
    //DrawPillowPanel(this,bv2,bv3,true);
    DrawCloseBar(this, bv4, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::pbTypeKeyPress(TObject *Sender, wchar_t &Key)
{
    if (Key == 0x08 && typed.Length()) // Backspace key.
    {
        typed.SetLength(typed.Length() - 1);
        pbType->Invalidate();
    }
    else if ((Key >= L'a' && Key <= L'z') || (Key >= L'A' && Key <= L'Z'))
    {
        if (Key >= L'A' && Key <= L'Z')
            Key = Key - L'A' + L'a';
        typed += wchar_t(Key);
        pbType->Invalidate();
    }
    int dummy;
    UnicodeString str = ToKana(typed, dummy);
    bool check = false;

    if (typed != L'n' || !wcscmp(wbuff[gen[testpos]], L"ん"))
    {
        for (int ix = 1; ix <= str.Length() && !check; ++ix)
            if (KANA(str[ix]))
                check = true;
    }

    if (check)
    {
        lbError->Visible = false;

        if (ToKana(typed, dummy) == wbuff[gen[testpos]])
        {
            typed = AnsiLowerCase(rbuff[gen[testpos]]);
            if (typed.Pos(L'('))
                typed.SetLength(typed.Pos(L'(') - 1);
            testpos++;
            donegood = true;
            t->Enabled = true;
            mistakes = 0;
            update();

            if (testpos == testlen)
            {
                lbType->Visible = false;
                pbType->Visible = false;
                lbDone->Visible = true;
                if (mistakecnt)
                {
                    lbMis->Caption = UnicodeString("Mistakes: ") + mistakecnt;
                    lbMis->Left = max(lbMis->Left, lbDone->Left + lbDone->Width + 8 * scaleres);
                    lbMis->Visible = true;
                }

                mistakecnt = 0;
                tTime->Enabled = false;
                btnGen->SetFocus();
            }
        }
        else
        {
            misses[testpos] = true;
            mistakes++;
            if (mistakes == 2)
            {
                lbError->Caption = (gen[testpos] >= hiracnt ? ToKatakana(UnicodeString(wbuff[gen[testpos]])) : UnicodeString(wbuff[gen[testpos]])) + " = " + rbuff[gen[testpos]];
                lbError->Visible = true;
                btnGen->Click();
            }
            else
            {
                mistakecnt++;
                donewrong = true;
                t->Enabled = true;
                update();
            }
        }
    }

    Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::pbTypeKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
    if (donegood || donewrong)
    {
        t->Enabled = false;
        typed = "";
        donegood = false;
        donewrong = false;
        pbType->Invalidate();
    }

    if ((/*Key == VK_BACK ||*/ Key == VK_DELETE) && typed.Length())
    {
        typed.SetLength(typed.Length() - 1);
        pbType->Invalidate();
    }
//    else if (Key >= 'a' && Key <= 'z' || Key >= 'A' && Key <= 'Z')
//    {
//        if (Key >= 'A' && Key <= 'Z')
//            Key = Key - 'A' + 'a';
//        typed += wchar_t(Key);
//        pbType->Invalidate();
//    }
//    int dummy;
//    UnicodeString str = ToKana(typed, dummy);
//    bool check = false;
//
//    if (typed != 'n' || !wcscmp(wbuff[gen[testpos]], L"ん"))
//    {
//        for (int ix = 1; ix <= str.Length() && !check; ++ix)
//            if (KANA(str[ix]))
//                check = true;
//    }
//
//    if (check)
//    {
//        lbError->Visible = false;
//
//        if (ToKana(typed, dummy) == wbuff[gen[testpos]])
//        {
//            typed = AnsiLowerCase(rbuff[gen[testpos]]);
//            if (typed.Pos(L'('))
//                typed.SetLength(typed.Pos(L'(') - 1);
//            testpos++;
//            donegood = true;
//            t->Enabled = true;
//            mistakes = 0;
//            update();
//
//            if (testpos == testlen)
//            {
//                lbType->Visible = false;
//                pbType->Visible = false;
//                lbDone->Visible = true;
//                if (mistakecnt)
//                {
//                    lbMis->Caption = UnicodeString("Mistakes: ") + mistakecnt;
//                    lbMis->Left = max(lbMis->Left, lbDone->Left + lbDone->Width + 8 * scaleres);
//                    lbMis->Visible = true;
//                }
//
//                mistakecnt = 0;
//                tTime->Enabled = false;
//                btnGen->SetFocus();
//            }
//        }
//        else
//        {
//            misses[testpos] = true;
//            mistakes++;
//            if (mistakes == 2)
//            {
//                lbError->Caption = (gen[testpos] >= hiracnt ? ToKatakana(UnicodeString(wbuff[gen[testpos]])) : UnicodeString(wbuff[gen[testpos]])) + " = " + rbuff[gen[testpos]];
//                lbError->Visible = true;
//                btnGen->Click();
//            }
//            else
//            {
//                mistakecnt++;
//                donewrong = true;
//                t->Enabled = true;
//                update();
//            }
//        }
//    }

    if (!Shift.Contains(ssAlt))
        Key = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::pbTypeEnter(TObject *Sender)
{
    pbType->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::pbTypeExit(TObject *Sender)
{
    pbType->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::tTimer(TObject *Sender)
{
    t->Enabled = false;
    typed = "";
    donegood = false;
    donewrong = false;
    pbType->Invalidate();
}
//---------------------------------------------------------------------------
void __fastcall TfKanaTest::tTimeTimer(TObject *Sender)
{
    int ctime = GetTickCount() - starttime;
    int tsec = ctime / 1000;
    int tmin = tsec / 60;
    tsec -= tmin * 60;
    if (lbMin->Caption != (tmin < 10 ? "0" : "") + UnicodeString(min(99, tmin)) + ":")
        lbMin->Caption = (tmin < 10 ? "0" : "") + UnicodeString(min(99, tmin)) + ":";
    if (lbSec->Caption != (tsec < 10 ? "0" : "") + UnicodeString(min(59, tsec)))
        lbSec->Caption = (tsec < 10 ? "0" : "") + UnicodeString(min(59, tsec));
}
//---------------------------------------------------------------------------
void TfKanaTest::update()
{
    updatepb = true;
    pb->Invalidate();
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(69);
#endif


