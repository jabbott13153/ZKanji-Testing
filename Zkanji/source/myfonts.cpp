//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------
#ifdef _DEBUG
static TGlobalTest tester1(114);
#endif

TStringList *jfonts;
TStringList *rfonts;
wchar_t *SystemFont = NULL;

int systemfontindex = -1;
int radicalfontindex = -1;

int MsgBox(UnicodeString message, UnicodeString caption, int buttons); //from zdefines.h not included here

int __stdcall fontproc(const ENUMLOGFONTEX *lpelfe, const NEWTEXTMETRICEX *lpntme, ULONG FontType, LPARAM lParam)
{
    if (lpelfe->elfLogFont.lfFaceName[0] == '@')
        return 1;
    if (lParam == (int)jfonts && !wcscmp(lpelfe->elfLogFont.lfFaceName, L"Terminal"))
        return 1;
    if (wcschr(lpelfe->elfLogFont.lfFaceName, '?'))
        return 1;
    if (!wcscmp(lpelfe->elfLogFont.lfFaceName, L"Segoe UI"))
    {
        delete[] SystemFont;
        SystemFont = new wchar_t[wcslen(lpelfe->elfLogFont.lfFaceName) + 1];
        wcscpy(SystemFont, lpelfe->elfLogFont.lfFaceName);
    }
    TStringList *list = (TStringList*)lParam;
    list->AddObject(lpelfe->elfLogFont.lfFaceName, (TObject*)lpelfe->elfLogFont.lfCharSet);
    return 1;
}
//---------------------------------------------------------------------------
void initfonts()
{
    TLogFont lf;
    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = SHIFTJIS_CHARSET;

    jfonts = new TStringList;
    rfonts = new TStringList;
    jfonts->Sorted = true;
    rfonts->Sorted = true;
    jfonts->Duplicates = dupIgnore;
    rfonts->Duplicates = dupIgnore;

    HDC h = GetDC(0);
    if (!EnumFontFamiliesExW(h, &lf, (int(__stdcall *)(const tagLOGFONTW*, const tagTEXTMETRICW*, ULONG, LPARAM))&fontproc, (unsigned long)jfonts, 0))
        MsgBox(IntToStr((int)GetLastError()).c_str(), L"Error reading japanese fonts", MB_OK);
    lf.lfCharSet = CHINESEBIG5_CHARSET;
    if (!EnumFontFamiliesExW(h, &lf, (int(__stdcall *)(const tagLOGFONTW*, const tagTEXTMETRICW*, ULONG, LPARAM))&fontproc, (unsigned long)jfonts, 0))
        MsgBox(IntToStr((int)GetLastError()).c_str(), L"Error reading chinese fonts", MB_OK);
    lf.lfCharSet = GB2312_CHARSET;
    if (!EnumFontFamiliesExW(h, &lf, (int(__stdcall *)(const tagLOGFONTW*, const tagTEXTMETRICW*, ULONG, LPARAM))&fontproc, (unsigned long)jfonts, 0))
        MsgBox(IntToStr((int)GetLastError()).c_str(), L"Error reading chinese fonts", MB_OK);

    (systemfontindex = jfonts->IndexOf("MS Gothic")) >= 0 || (systemfontindex = jfonts->IndexOf("MS PGothic")) >= 0 || (systemfontindex = jfonts->IndexOf("VL Gothic")) >= 0 || (systemfontindex = jfonts->IndexOf("VL PGothic")) >= 0 || (systemfontindex = jfonts->IndexOf("MS Mincho")) >= 0 || (systemfontindex = jfonts->IndexOf("MS PMincho")) >= 0 || (systemfontindex = jfonts->IndexOf("Arial Unicode")) >= 0 || (systemfontindex = jfonts->IndexOf("Arial Unicode MS")) >= 0 || (systemfontindex = jfonts->IndexOf("IPAGothic")) >= 0 || (systemfontindex = jfonts->IndexOf("IPAMincho")) >= 0 || (systemfontindex = jfonts->IndexOf("Sazanami Gothic")) >= 0 || (systemfontindex = jfonts->IndexOf("Droid Sans Fallback")) >= 0;
    if (systemfontindex < 0 && jfonts->Count)
        systemfontindex = 0;

    (radicalfontindex = jfonts->IndexOf("SimSun")) >= 0 || (radicalfontindex = jfonts->IndexOf("MS Mincho")) >= 0 || (radicalfontindex = jfonts->IndexOf("MS PGothic")) >= 0 || (radicalfontindex = jfonts->IndexOf("IPAMincho")) >= 0 || (radicalfontindex = jfonts->IndexOf("Sazanami Gothic")) >= 0 || (radicalfontindex = jfonts->IndexOf("Droid Sans Fallback")) >= 0;
    /*f->Name = "SimSun";
     f->Charset = GB2312_CHARSET;
     if (f->Name != "SimSun") {
     f->Name = "MS Mincho";
     if (f->Name != "MS Mincho") {
     f->Name = "MS PMincho";
     if (f->Name != "MS PMincho") {
     f->Name = "IPA Mincho";
     if (f->Name != "IPA Mincho")
     f->Name = "Sazanami Mincho";
     }
     }
     f->Charset = SHIFTJIS_CHARSET;
     }
     */
    if (radicalfontindex < 0 && jfonts->Count)
        radicalfontindex = 0;

    memset(&lf, 0, sizeof(lf));
    lf.lfCharSet = DEFAULT_CHARSET;
    if (!EnumFontFamiliesExW(h, &lf, (int(__stdcall *)(const tagLOGFONTW*, const tagTEXTMETRICW*, ULONG, LPARAM))&fontproc, (unsigned long)rfonts, 0))
        MsgBox(IntToStr((int)GetLastError()).c_str(), L"Error reading general fonts", MB_OK);
    ReleaseDC(0, h);
}
//---------------------------------------------------------------------------
void freefonts()
{
    delete jfonts;
    delete rfonts;
    delete[] SystemFont;
    SystemFont = NULL;
}
//---------------------------------------------------------------------------
HFONT create_localfont(TFont *Font)
{
    tagLOGFONTW lf;

    lf.lfHeight = Font->Height;
    lf.lfWidth = 0;
    lf.lfEscapement = Font->Orientation;
    lf.lfOrientation = Font->Orientation;
    if (Font->Style.Contains(fsBold))
        lf.lfWeight = FW_BOLD;
    else
        lf.lfWeight = FW_NORMAL;
    lf.lfItalic = Font->Style.Contains(fsItalic);
    lf.lfUnderline = Font->Style.Contains(fsUnderline);
    lf.lfStrikeOut = Font->Style.Contains(fsStrikeOut);
    lf.lfCharSet = Font->Charset;

    wcscpy(lf.lfFaceName, Font->Name.c_str());
    lf.lfCharSet = Font->Charset;
    lf.lfQuality = ANTIALIASED_QUALITY; //DEFAULT_QUALITY; // CLEARTYPE_QUALITY;
    if (lf.lfOrientation)
        lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    else
        lf.lfOutPrecision = OUT_DEFAULT_PRECIS;

    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    switch (Font->Pitch)
    {
    case fpVariable:
        lf.lfPitchAndFamily = VARIABLE_PITCH;
        break;
    case fpFixed:
        lf.lfPitchAndFamily = FIXED_PITCH;
        break;
    default:
        lf.lfPitchAndFamily = DEFAULT_PITCH;
        break;
    }
    return CreateFontIndirectW(&lf);
}
//---------------------------------------------------------------------------
HFONT create_localfont(UnicodeString name, TFontCharset charset, int height, TFontStyles style)
{
    tagLOGFONTW lf;

    lf.lfHeight = height;
    lf.lfWidth = 0;
    lf.lfEscapement = 0; //Font->Orientation;
    lf.lfOrientation = 0; //Font->Orientation;
    if (style.Contains(fsBold))
        lf.lfWeight = FW_BOLD;
    else
        lf.lfWeight = FW_NORMAL;
    lf.lfItalic = style.Contains(fsItalic);
    lf.lfUnderline = style.Contains(fsUnderline);
    lf.lfStrikeOut = style.Contains(fsStrikeOut);

    wcscpy(lf.lfFaceName, name.c_str());
    lf.lfCharSet = charset;
    lf.lfQuality = ANTIALIASED_QUALITY; //DEFAULT_QUALITY; // CLEARTYPE_QUALITY;
    if (lf.lfOrientation)
        lf.lfOutPrecision = OUT_TT_ONLY_PRECIS;
    else
        lf.lfOutPrecision = OUT_DEFAULT_PRECIS;

    lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    /*switch (Font->Pitch) {
     case fpVariable: lf.lfPitchAndFamily = VARIABLE_PITCH; break;
     case fpFixed: lf.lfPitchAndFamily = FIXED_PITCH; break;
     default:*/lf.lfPitchAndFamily = DEFAULT_PITCH; /*break;
     }*/
    return CreateFontIndirectW(&lf);
}
//---------------------------------------------------------------------------
void SetRadicalFont(TFont *f)
{
    if (radicalfontindex < 0)
    {
        f->Name = "Arial";
        f->Charset = DEFAULT_CHARSET;
        return;
    }
    f->Name = jfonts->Strings[radicalfontindex];
    f->Charset = (TFontCharset)jfonts->Objects[radicalfontindex];
}
//---------------------------------------------------------------------------
void SetSystemFont(TFont *f)
{
    if (systemfontindex < 0)
    {
        f->Name = "Arial";
        f->Charset = DEFAULT_CHARSET;
        return;
    }
    f->Name = jfonts->Strings[systemfontindex];
    f->Charset = (TFontCharset)jfonts->Objects[systemfontindex];
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(115);
#endif
