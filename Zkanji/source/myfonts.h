//---------------------------------------------------------------------------

#ifndef myfontsH
#define myfontsH
//---------------------------------------------------------------------------

extern TStringList *jfonts;
extern TStringList *rfonts;
extern int systemfontindex;
extern int radicalfontindex;
extern wchar_t *SystemFont;
void initfonts();
void freefonts();

HFONT create_localfont(TFont *f);
HFONT create_localfont(UnicodeString name, TFontCharset charset, int height, TFontStyles style);

void SetRadicalFont(TFont *f);
void SetSystemFont(TFont *f);

#endif
