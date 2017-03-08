//---------------------------------------------------------------------------

#ifndef romajizerH
#define romajizerH
//---------------------------------------------------------------------------

extern byte consonantcolumn[];

// Turn the hepburn romanized text to "simplified" (sha->sixya, shi->si etc.).
// Spaces is the number of characters occupied in the new string from the old one.
// Like ja->zixya means j{1,2}a{1,3}, while shi->si is sh->s{2,1}i->i{1,1}).
// The spaces array must be at least three times the length of s.
//AnsiString __fastcall TranslateRomaji(AnsiString s, byte *spaces = NULL);
UnicodeString __fastcall TranslateRomaji(UnicodeString str, byte *spaces = NULL);

// Turn kana string to simplified romaji.
UnicodeString __fastcall Romanize(const wchar_t *c);

// Turn kana string to learner's Hepburn romaji (eg. ou for long wovel).
UnicodeString __fastcall RomanizeEx(const wchar_t *c);

// Make a kana text full hiragana (convert katakana).
UnicodeString __fastcall Hiraganize(const wchar_t *c);

// Return the hiragana equialent of a katakana character, or the character itself if it's not katakana.
wchar_t __fastcall HiraganaCh(const wchar_t *c, int ix);

bool __fastcall KanaMatch(const wchar_t *c1, int pos1, const wchar_t *c2, int pos2);

// Convert passed s string to kana equivalent.
// Used = number of characters converted and skipped in the original text.
UnicodeString __fastcall ToKana(UnicodeString str, int &used);

// Convert hiragana text to katakana.
UnicodeString __fastcall ToKatakana(UnicodeString w);

UnicodeString __fastcall TrimRomanized(const UnicodeString &str);
UnicodeString __fastcall RemoveUseless(const UnicodeString &str);


#endif
