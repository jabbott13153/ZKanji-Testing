//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "localeinfo.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(100);
#endif

TSystemLocaleSettings locale;

TSystemLocaleSettings::TSystemLocaleSettings() : inited(false)
{
}

TSystemLocaleSettings::~TSystemLocaleSettings()
{

}

void __fastcall TSystemLocaleSettings::init()
{
    if (inited)
        return;
    inited = true;

    wchar_t b[10];
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_INEGNUMBER, b, 10);
    fnumformat = b[0] - L'0';
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SDECIMAL, b, 10);
    fdecimal = b[0];
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SNEGATIVESIGN, b, 10);
    fnegsign = b[0];
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_STHOUSAND, b, 10);
    fgroupsep = b[0];
    GetLocaleInfoW(LOCALE_USER_DEFAULT, LOCALE_SGROUPING, b, 10);
    fgroupingcnt = 0;
    wchar_t *t = wcstok(b, L";");
    while (t)
    {
        fgrouping[fgroupingcnt] = _wtoi(t);
        t = wcstok(NULL, L";");
        if (fgrouping[fgroupingcnt] != 0)
            fgroupingcnt++;
        else
            fgroupingrepeat = true;
    }
}

char __fastcall TSystemLocaleSettings::get_decimal()
{
    if (!inited)
        init();

    return fdecimal;
}

char __fastcall TSystemLocaleSettings::get_neg()
{
    if (!inited)
        init();

    return fnegsign;
}

char __fastcall TSystemLocaleSettings::get_groupsep()
{
    if (!inited)
        init();

    return fgroupsep;
}

byte __fastcall TSystemLocaleSettings::get_format()
{
    if (!inited)
        init();

    return fnumformat;
}

UnicodeString __fastcall TSystemLocaleSettings::commalize(int i)
{
    if (!inited)
        init();

    UnicodeString s = "";
    UnicodeString s2 = IntToStr(abs(i));
//int d = 3;
    int k = s2.Length(); //(s2.Length()-1) / d;
    int pos = 0;
    int g = 0;

    while (pos < k && g < fgroupingcnt)
    {
        int d = fgrouping[g];
        int n = k - pos - d + 1;
        s = s2.SubString(max(1, n), d + (n < 1 ? n - 1 : 0)) + s;
        pos += d;
        if (pos < k)
            s = (UnicodeString)fgroupsep + s;

        if (g < fgroupingcnt - 1 || !fgroupingrepeat)
            g++;
    }
    if (pos < k)
        s = s2.SubString(1, k - pos) + s;

    if (i < 0)
        s = negativize(s);

    return s;
}

UnicodeString __fastcall TSystemLocaleSettings::negativize(UnicodeString str)
{
    switch (fnumformat)
    {
    case 0:
        return L"(" + str + L")";
    case 2:
        return (UnicodeString)fnegsign + L" " + str;
    case 3:
        return str + fnegsign;
    case 4:
        return str + L" " + fnegsign;
    case 1:
    default:
        return (UnicodeString)fnegsign + str;
    }
}

#ifdef _DEBUG
static TGlobalTest tester2(101);
#endif

