//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "romajizer.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#define KANAVOWEL(c) (c == L'a' || c == L'i' || c == L'u' || c == L'e' || c == L'o')
#define KANACONSONANT(x) (x == L'k' || x == L'g' || x == L's' || x == L'z' || x == L't' || \
                          x == L'd' || x == L'm' || x == L'h' || x == L'b' || x == L'p' || \
                          x == L'r' || x == L'y' || x == L'w' || x == L'c' || x == L'n' || \
                          x == L'v' || x == L'f')

#ifdef _DEBUG
static TGlobalTest tester1(136);
#endif

wchar_t *kanatable[86] =
{
    L"xa", L"a", L"xi", L"i", L"xu", L"u", L"xe", L"e", L"xo", L"o", L"ka", L"ga", L"ki", L"gi", L"ku", //14
    L"gu", L"ke", L"ge", L"ko", L"go", L"sa", L"za", L"si", L"zi", L"su", L"zu", L"se", L"ze", L"so", L"zo", L"ta", //30
    L"da", L"ti", L"di", L"+", L"tu", L"du", L"te", L"de", L"to", L"do", L"na", L"ni", L"nu", L"ne", L"no", L"ha", //46
    L"ba", L"pa", L"hi", L"bi", L"pi", L"hu", L"bu", L"pu", L"he", L"be", L"pe", L"ho", L"bo", L"po", L"ma", L"mi", //62
    L"mu", L"me", L"mo", L"xya", L"ya", L"xyu", L"yu", L"xyo", L"yo", L"ra", L"ri", L"ru", L"re", L"ro", L"xwa", L"wa", //78
    L"whi", L"whe", L"wo", L"n'", L"vu", L"xka", L"xke"
};

wchar_t *kanatableU[86] =
{
    L"XA", L"A", L"XI", L"I", L"XU", L"U", L"XE", L"E", L"XO", L"O", L"KA", L"GA", L"KI", L"GI", L"KU", //14
    L"GU", L"KE", L"GE", L"KO", L"GO", L"SA", L"ZA", L"SI", L"ZI", L"SU", L"ZU", L"SE", L"ZE", L"SO", L"ZO", L"TA", //30
    L"DA", L"TI", L"DI", L"+", L"TU", L"DU", L"TE", L"DE", L"TO", L"DO", L"NA", L"NI", L"NU", L"NE", L"NO", L"HA", //46
    L"BA", L"PA", L"HI", L"BI", L"PI", L"HU", L"BU", L"PU", L"HE", L"BE", L"PE", L"HO", L"BO", L"PO", L"MA", L"MI", //62
    L"MU", L"ME", L"MO", L"XYA", L"YA", L"XYU", L"YU", L"XYO", L"YO", L"RA", L"RI", L"RU", L"RE", L"RO", L"XWA", L"WA", //78
    L"WHI", L"WHE", L"WO", L"N'", L"VU", L"XKA", L"XKE"
};

wchar_t *kanatable_ext[84] =
{
    L"a", L"a", L"i", L"i", L"u", L"u", L"e", L"e", L"o", L"o", L"ka", L"ga", L"ki", L"gi", L"ku", //14
    L"gu", L"ke", L"ge", L"ko", L"go", L"sa", L"za", L"shi", L"ji", L"su", L"zu", L"se", L"ze", L"so", L"zo", L"ta", //30
    L"da", L"chi", L"ji", L"+", L"tsu", L"zu", L"te", L"de", L"to", L"do", L"na", L"ni", L"nu", L"ne", L"no", L"ha", //46
    L"ba", L"pa", L"hi", L"bi", L"pi", L"fu", L"bu", L"pu", L"he", L"be", L"pe", L"ho", L"bo", L"po", L"ma", L"mi", //62
    L"mu", L"me", L"mo", L"ya", L"ya", L"yu", L"yu", L"yo", L"yo", L"ra", L"ri", L"ru", L"re", L"ro", L"wa", L"wa", //78
    L"wi", L"we", L"wo", L"n", L"vu"
};

wchar_t *kanatable_extU[84] =
{
    L"A", L"A", L"I", L"I", L"U", L"U", L"E", L"E", L"O", L"O", L"KA", L"GA", L"KI", L"GI", L"KU", //14
    L"GU", L"KE", L"GE", L"KO", L"GO", L"SA", L"ZA", L"SHI", L"JI", L"SU", L"ZU", L"SE", L"ZE", L"SO", L"ZO", L"TA", //30
    L"DA", L"CHI", L"JI", L"+", L"TSU", L"ZU", L"TE", L"DE", L"TO", L"DO", L"NA", L"NI", L"NU", L"NE", L"NO", L"HA", //46
    L"BA", L"PA", L"HI", L"BI", L"PI", L"FU", L"BU", L"PU", L"HE", L"BE", L"PE", L"HO", L"BO", L"PO", L"MA", L"MI", //62
    L"MU", L"ME", L"MO", L"YA", L"YA", L"YU", L"YU", L"YO", L"YO", L"RA", L"RI", L"RU", L"RE", L"RO", L"WA", L"WA", //78
    L"WI", L"WE", L"WO", L"N", L"VU"
};

char vowelcolumn[84] =
{
    0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 0, 0, 1, 1, 2, //14
    2, 3, 3, 4, 4, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 0, //30
    0, 1, 1, -1, 2, 2, 3, 3, 4, 4, 0, 1, 2, 3, 4, 0, //46
    0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 0, 1, //62
    2, 3, 4, 0, 0, 2, 2, 4, 4, 0, 1, 2, 3, 4, -1, 0, //78
    -1, -1, 4, -1, 3
};

byte consonantcolumn[84] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 101, 1, 101, 1, //14
    101, 1, 101, 1, 101, 2, 102, 2, 102, 2, 102, 2, 102, 2, 102, 3, //30
    103, 3, 103, 0, 3, 103, 3, 103, 3, 103, 4, 4, 4, 4, 4, 5, //46
    105, 205, 5, 105, 205, 5, 105, 205, 5, 105, 205, 5, 105, 205, 6, 6, //62
    6, 6, 6, 0, 7, 0, 7, 0, 7, 8, 8, 8, 8, 8, 9, 9, //78
    0, 0, 9, 4, 0
};

const int HiraganaBase = 0x3041;
const int HiraganaEnd = 0x3093;
const int KatakanaBase = 0x30A1;
const int KatakanaEnd = 0x30F6;
const int KanjiBase = 0x4E00;
const int KanjiEnd = 0x9FAF;

wchar_t kanavowel[5] = { L'あ', L'い', L'う', L'え', L'お' };

bool IsUpper(wchar_t c)
{
    return c >= L'A' && c <= L'Z';
}

int vowelpos(char c)
{ //return kana table index for a one character vowel
    switch (c)
    {
    case L'a':
    case L'A':
        return 1;
    case L'i':
    case L'I':
        return 3;
    case L'u':
    case L'U':
        return 5;
    case L'e':
    case L'E':
        return 7;
    case L'o':
    case L'O':
        return 9;
    default:
        return -1;
    }
}

wchar_t smallkata(wchar_t c)
{
    switch (c)
    {
    case L'a':
    case L'A':
        return L'ァ';
    case L'i':
    case L'I':
        return L'ィ';
    case L'u':
    case L'U':
        return L'ゥ';
    case L'e':
    case L'E':
        return L'ェ';
    case L'o':
    case L'O':
        return L'ォ';
    }
    THROW(L"?");
}

//return kana table index for i syllable with the given consonant
int isyllable(wchar_t c)
{
    wchar_t **table;
    if (IsUpper(c))
        table = kanatableU;
    else
        table = kanatable;

    for (int i = 10; i < 86; i++)
    {
        if (table[i][0] == c && (table[i][1] == 'i' || table[i][1] == 'I'))
            return i;
    }
    return -1;
}

//return small yu, yo and ya - not in this order
int smally(wchar_t c)
{
    switch (c)
    {
    case L'a':
    case L'A':
        return 66;
    case L'u':
    case L'U':
        return 68;
    case L'o':
    case L'O':
        return 70;
    default:
        return -1;
    }
}

bool IsKanaVowel(wchar_t c)
{
    return c == 'a' || c == 'A' || c == 'i' || c == 'I' || c == 'u' || c == 'U' || c == 'e' || c == 'E' || c == 'o' || c == 'O';
}

bool IsKanaConsonant(wchar_t c)
{
    return c == 'k' || c == 'K' || c == 'g' || c == 'G' || c == 's' || c == 'S' || c == 'z' || c == 'Z' || c == 't' || c == 'T' || c == 'd' || c == 'D' || c == 'm' || c == 'M' || c == 'h' || c == 'H' || c == 'b' || c == 'B' || c == 'p' || c == 'P' || c == 'r' || c == 'R' || c == 'y' || c == 'Y' || c == 'w' || c == 'W' || c == 'c' || c == 'C' || c == 'n' || c == 'N' || c == 'v' || c == 'V' || c == 'f' || c == 'F';
}

wchar_t ToKanaVowel(wchar_t c)
{
    switch (c)
    {
    case L'a':
        return L'あ';
    case L'A':
        return L'ア';
    case L'i':
        return L'い';
    case L'I':
        return L'イ';
    case L'u':
        return L'う';
    case L'U':
        return L'ウ';
    case L'e':
        return L'え';
    case L'E':
        return L'エ';
    case L'o':
        return L'お';
    case L'O':
        return L'オ';
    default:
        return L'　';
    }
}

wchar_t ToKatakanaVowel(wchar_t c)
{
    switch (c)
    {
    case L'a':
    case L'A':
        return L'ア';
    case L'i':
    case L'I':
        return L'イ';
    case L'u':
    case L'U':
        return L'ウ';
    case L'e':
    case L'E':
        return L'エ';
    case L'o':
    case L'O':
        return L'オ';
    default:
        return L'　';
    }
}

wchar_t ToSmallKanaVowel(wchar_t c)
{
    switch (c)
    {
    case L'a':
        return L'ぁ';
    case L'A':
        return L'ァ';
    case L'i':
        return L'ぃ';
    case L'I':
        return L'ィ';
    case L'u':
        return L'ぅ';
    case L'U':
        return L'ゥ';
    case L'e':
        return L'ぇ';
    case L'E':
        return L'ェ';
    case L'o':
        return L'ぉ';
    case L'O':
        return L'ォ';
    default:
        return L'　';
    }
}

wchar_t ToSmallKatakanaVowel(wchar_t c)
{
    switch (c)
    {
    case L'a':
    case L'A':
        return L'ァ';
    case L'i':
    case L'I':
        return L'ィ';
    case L'u':
    case L'U':
        return L'ゥ';
    case L'e':
    case L'E':
        return L'ェ';
    case L'o':
    case L'O':
        return L'ォ';
    default:
        return L'　';
    }
}

wchar_t KanaBase(wchar_t c, bool &iskata)
{
    if ((iskata = KATAKANA(c)) == true)
        return c - KatakanaBase;
    else
        return c - HiraganaBase;
}

//make a kana string from the input, but leave any parts that may be written later
//used = number of characters converted and skipped in the original text
UnicodeString __fastcall ToKana(UnicodeString str, int &used)
{
    used = 0;
    byte *changes = new byte[str.Length() * 3];
    UnicodeString s = TranslateRomaji(str, changes);
    wchar_t *result = new wchar_t[str.Length() * 3];
    int rpos = 0;

    int i = 1;
    int skipnum = 0;
    while (i <= s.Length())
    {
        if (JAPAN(s[i]))
        {
            used += skipnum + changes[i - 1];
            skipnum = 0;
            result[rpos++] = s[i];
            i++;
            continue;
        }

        if (!IsKanaVowel(s[i]) && !IsKanaConsonant(s[i]) && s[i] != '-')
        {
            skipnum += changes[i - 1];
            i++;
            continue;
        }

        if (s[i] == '-')
        {
            used += skipnum + changes[i - 1];
            i++;
            skipnum = 0;
            result[rpos++] = L'ー';
            continue;
        }

        if (IsKanaVowel(s[i]))
        {
            if (i > 1 && (s[i - 1] == 'x' || s[i - 1] == 'X'))
                result[rpos++] = ToSmallKanaVowel(s[i]);
            else
                result[rpos++] = ToKanaVowel(s[i]);
            used += skipnum + changes[i - 1];
            i++;
            skipnum = 0;
            continue;
        }

        //only for single N
        if ((s[i] == 'n' || s[i] == 'N') && i < s.Length() && s[i + 1] == '\'')
        {
            result[rpos++] = s[i] == 'n' ? L'ん' : L'ン';
            used += skipnum + changes[i - 1];
            i += 2;
            skipnum = 0;
            if (i <= s.Length() - 1 && (s[i] == 'n' || s[i] == 'N') && s[i + 1] == '\'')
            {
                used += changes[i - 1];
                i += 2;
            }
            continue;
        }

        if (i == s.Length() || i == s.Length() - 1 && (s[i + 1] == 'y' || s[i + 1] == 'Y' || s[i + 1] == 'x' || s[i + 1] == 'X'))
            break;

        if (s[i + 1] == 'y' || s[i + 1] == 'Y' || (s[i] == 't' || s[i] == 'T' || s[i] == 'd' || s[i] == 'D') && (s[i + 1] == 'h' || s[i + 1] == 'H'))
        {
            if (s[i] == 'y' || s[i] == 'Y' || s[i] == 'w' || s[i] == 'W')
            {
                skipnum += changes[i - 1];
                i++;
                continue;
            }
            if (i == s.Length() - 1 || !IsKanaVowel(s[i + 2]))
            {
                skipnum += changes[i - 1] + changes[i];
                i += 2;
                continue;
            }

            bool kata = IsUpper(s[i]) || IsUpper(s[i + 1]) || IsUpper(s[i + 2]);

            wchar_t syl;
            if (s[i] != 'f' && s[i] != 'F' && s[i] != 'w' && s[i] != 'W' && s[i] != 'v' && s[i] != 'V' && s[i + 1] != 'h' && s[i + 1] != 'H')
                syl = isyllable(s[i]) + (kata ? KatakanaBase : HiraganaBase);
            else
                syl = s[i] == 'f' && s[i + 1] == 'y' && islower(s[i + 2]) ? L'ふ' : s[i] == 'f' || s[i] == 'F' ? L'フ' : (s[i] == 'v' || s[i] == 'V') ? L'ヴ' : s[i] == 't' && s[i + 1] == 'h' && islower(s[i + 2]) ? L'て' : (s[i] == 'T' || s[i] == 't') ? L'テ' : s[i] == 'd' && s[i + 1] == 'h' && islower(s[i + 2]) ? L'で' : (s[i] == 'D' || s[i] == 'd') ? L'デ' : '*';
            if ((s[i + 2] == 'e' || s[i + 2] == 'E') && (s[i] == 's' || s[i] == 'S' || s[i] == 'z' || s[i] == 'Z' || s[i] == 't' || s[i] == 'T' || s[i] == 'd' || s[i] == 'D' || s[i] == 'f' || s[i] == 'F' || s[i] == 'v' || s[i] == 'V'))
            {
                if (syl != '*')
                    result[rpos++] = syl;
                result[rpos++] = kata ? L'ェ' : L'ぇ';
                skipnum += changes[i - 1] + changes[i] + changes[i + 1];
                i += syl != '*' ? 3 : 2;
            }
            else if (s[i + 2] == 'i' || s[i + 2] == 'I')
            {
                if (syl != '*')
                    result[rpos++] = syl;
                result[rpos++] = kata ? L'ィ' : L'ぃ';
                skipnum += changes[i - 1] + changes[i] + changes[i + 1];
                i += syl != '*' ? 3 : 2;
            }
            else if (s[i + 2] == 'e' || s[i + 2] == 'E')
            {
                if (syl != '*')
                    result[rpos++] = syl;
                skipnum += changes[i - 1] + changes[i];
                i += syl != '*' ? 2 : 1;
            }
            else
            {
                if (syl != '*')
                    result[rpos++] = syl;
                result[rpos++] = smally(s[i + 2]) + (kata ? KatakanaBase : HiraganaBase);
                skipnum += changes[i - 1] + changes[i] + changes[i + 1];
                i += syl != '*' ? 3 : 2;
            }
            used += skipnum;
            skipnum = 0;
            continue;
        }

        if ((s[i] == 'f' || s[i] == 'F') && IsKanaVowel(s[i + 1]) && s[i + 1] != 'u' && s[i + 1] != 'U')
        {
            result[rpos++] = L'フ';
            result[rpos++] = ToSmallKatakanaVowel(s[i + 1]);
            i += 2;
            used += 2;
            continue;
        }

        if (i < s.Length() - 1 && (s[i + 1] == 'x' || s[i + 1] == 'X') && (s[i] == 'v' || s[i] == 'V') && IsKanaVowel(s[i + 2]))
        {
            result[rpos++] = L'ヴ';
            used += skipnum + changes[i - 1];
            i++;
            continue;
        }

        bool kata2 = IsUpper(s[i]) || IsUpper(s[i + 1]);
        bool xtu = i > 1 && (s[i - 1] == 'x' || s[i - 1] == 'X') && (s[i] == 't' || s[i] == 'T') && (s[i + 1] == 'u' || s[i + 1] == 'U');
        if (xtu || tolower(s[i + 1]) == tolower(s[i]))
        {
            result[rpos++] = kata2 ? L'ッ' : L'っ';
            used += skipnum + changes[i - 1];
            i++;
            if (xtu)
            {
                i++;
                used++;
            }
            skipnum = 0;
            continue;
        }

        if (i > 1 && i < s.Length() && (s[i - 1] == 'x' || s[i - 1] == 'X'))
        {
            if ((s[i] == 'k' || s[i] == 'K') && (s[i + 1] == 'e' || s[i + 1] == 'E' || s[i + 1] == 'a' || s[i + 1] == 'A'))
            {
                result[rpos++] = s[i + 1] == 'a' || s[i + 1] == 'A' ? L'ヵ' : L'ヶ';
                used += skipnum + changes[i - 1] + 1;
                i += 2;
                skipnum = 0;
                continue;
            }
            if ((s[i] == 'w' || s[i] == 'W') && (s[i + 1] == 'a' || s[i + 1] == 'A'))
            {
                result[rpos++] = kata2 || s[i - 1] == 'X' ? L'ヮ' : L'ゎ';
                used += skipnum + changes[i - 1] + 1;
                i += 2;
                skipnum = 0;
                continue;
            }
        }

        if (!IsKanaVowel(s[i + 1]) || (s[i] == 'y' || s[i] == 'Y') && s[i + 1] != 'a' && s[i + 1] != 'u' && s[i + 1] != 'o' && s[i + 1] != 'A' && s[i + 1] != 'U' && s[i + 1] != 'O')
        {
            i++;
            skipnum += changes[i - 1];
            continue;
        }

        int j;
        //we are at a consonant and next one is a vowel, you know what to do
        for (j = 10; j < 86; j++)
        {
            if (wcslen(kanatable[j]) > 1 && (s[i] == kanatable[j][0] || s[i] == kanatableU[j][0]) && (s[i + 1] == kanatable[j][1] || s[i + 1] == kanatableU[j][1]))
            {
                bool kata = IsUpper(s[i]) || IsUpper(s[i + 1]);
                if (i > 1 && (s[i - 1] == 'x' || s[i - 1] == 'X') && (s[i] == 'y' || s[i] == 'Y'))
                    result[rpos++] = j + (kata ? KatakanaBase : HiraganaBase) - 1;
                else
                    result[rpos++] = j + (kata || s[i] == 'v' || s[i] == 'V' ? KatakanaBase : HiraganaBase);
                break;
            }
        }
        skipnum += changes[i - 1] + changes[i];
        i += 2;
        used += skipnum;
        skipnum = 0;
    }

    str = UnicodeString(result, rpos);
    delete[] result;
    delete[] changes;
    return str;
}

UnicodeString __fastcall ToKatakana(UnicodeString w)
{
    UnicodeString r = L"";
    for (int i = 1; i <= w.Length(); i++)
        if (HIRAGANA(w[i]))
            r += (wchar_t)(w[i] + (0x30a1 - 0x3041));
        else
            r += w[i];
    return r;
}

//turn kana string to simplified romaji
UnicodeString __fastcall Romanize(const wchar_t *c)
{
    int i;
    UnicodeString s;

    unsigned int clen = wcslen(c);
    char *conv = new char[clen * 3 + 1];
    memset(conv, 0, sizeof(char)*(clen*3 + 1));

    int convlen = 0;
    for (unsigned int l = 0; l < clen; l++)
    {
        if (DASH(c[l]) && convlen && KANAVOWEL(conv[convlen - 1]))
        {
            conv[convlen] = conv[convlen - 1];
            convlen++;
            continue;
        }

        if (!KANA(c[l]))
        { //skip unknown chars
            if (c[l] >= L'A' && c[l] <= L'Z' || c[l] >= L'a' && c[l] <= L'z')
            {
                conv[convlen] = (char)c[l]; //leave romaji there, maybe we will need it sometimes
                convlen++;
            }
            continue;
        }
        if (KATAKANA(c[l]))
            i = c[l] - 0x30A1;
        else
            i = c[l] - 0x3041;
        if (kanatable[i][0] != L'*')
        {
            if (convlen && conv[convlen - 1] == L'+')
                if (kanatable[i][0] == L'+' || kanatable[i][0] == L'y' || kanatable[i][0] == L'Y' || kanatable[i][0] == L'w' || kanatable[i][0] == L'v' || kanatable[i][0] == L'a' || kanatable[i][0] == L'i' || kanatable[i][0] == L'u' || kanatable[i][0] == L'e' || kanatable[i][0] == L'o' || kanatable[i][0] == L'x' || kanatable[i][0] == L'W')
                { //error, remove doubling too
                    convlen--;
                    conv[convlen] = 0;
                }
                else
                {
                    conv[convlen - 1] = kanatable[i][0];
                }
            int cl = 0;
            while (kanatable[i][cl])
            {
                conv[convlen] = kanatable[i][cl++];
                convlen++;
            }
        }
    }
    if (convlen && conv[convlen - 1] == L'+')
        conv[--convlen] = 0;

    s = conv;
    delete[] conv;

    return s;
}

//return the hiragana equialent of a katakana character, or the character itself if it's not katakana
//the function wants the whole string and the index of the character to convert because of dashes
wchar_t __fastcall HiraganaCh(const wchar_t *c, int ix)
{
    if (DASH(c[ix]))
    {
        if (ix > 0 && HIRAGANA(HiraganaCh(c, ix - 1)) && vowelcolumn[HiraganaCh(c, ix - 1) - 0x3041] >= 0)
            return kanavowel[vowelcolumn[HiraganaCh(c, ix - 1) - 0x3041]];
        return c[ix];
    }

    if (KATAKANA(c[ix]))
        return c[ix] - 0x60;

    return c[ix];
}

bool __fastcall KanaMatch(const wchar_t *c1, int pos1, const wchar_t *c2, int pos2)
{
    if (HiraganaCh(c1, pos1) == HiraganaCh(c2, pos2))
        return true;

    if ((c1[pos1] == L'ケ' || c1[pos1] == L'ヶ' || c1[pos1] == L'ヵ') && (HiraganaCh(c2, pos2) == L'か' || HiraganaCh(c2, pos2) == L'け' || HiraganaCh(c2, pos2) == L'が') || (c2[pos2] == L'ケ' || c2[pos2] == L'ヶ' || c2[pos2] == L'ヵ') && (HiraganaCh(c1, pos1) == L'か' || HiraganaCh(c1, pos1) == L'け' || HiraganaCh(c1, pos1) == L'が'))
        return true;

    if ((pos1 > 0 && c1[pos1] == L'ー' && (HiraganaCh(c2, pos2) == L'お' || HiraganaCh(c2, pos2) == L'う') && vowelcolumn[HiraganaCh(c1, pos1 - 1) - 0x3041] == 4) || (pos2 > 0 && c2[pos2] == L'ー' && (HiraganaCh(c1, pos1) == L'お' || HiraganaCh(c1, pos1) == L'う') && vowelcolumn[HiraganaCh(c2, pos2 - 1) - 0x3041] == 4) || (pos1 > 0 && c1[pos1] == L'ー' && (HiraganaCh(c2, pos2) == L'い' || HiraganaCh(c2, pos2) == L'え') && vowelcolumn[HiraganaCh(c1, pos1 - 1) - 0x3041] == 3) || (pos2 > 0 && c2[pos2] == L'ー' && (HiraganaCh(c1, pos1) == L'い' || HiraganaCh(c1, pos1) == L'え') && vowelcolumn[HiraganaCh(c2, pos2 - 1) - 0x3041] == 3))
        return true;

    return false;
}

//make a kana text full hiragana (convert katakana)
UnicodeString __fastcall Hiraganize(const wchar_t *c)
{
    UnicodeString s;

    for (int i = 0; i < (int)wcslen(c); i++)
    {
        if (DASH(c[i]))
        {
            if (s.Length() && HIRAGANA(s[s.Length()]))
            {
                int d = s[s.Length()] - 0x3041;
                if (vowelcolumn[d] >= 0)
                    s += kanavowel[vowelcolumn[d]];
            }
            continue;
        }

        if (KATAKANA(c[i]) && c[i] <= 0x30F4)
            s += (wchar_t)((word)c[i] - 0x60);
        else
            s += c[i];
    }

    return s;
}

inline void skip_ext(const wchar_t *c, const int len, int &k, int cnt = 1)
{
    while (cnt && k < len)
    {
        k++;
        cnt--;
        while (k < len && !KANA(c[k]) && !DASH(c[k]))
            k++;
    }
}
inline bool minitsu_ext(const wchar_t &c)
{
    return c == 34;
}
inline void add_double_ext(UnicodeString &s, const wchar_t &c)
{
    if (KANACONSONANT(c) && c != L'n')
        s += c;
}
bool convert_ex(UnicodeString &s, int c1, int c2, bool dbl)
{
    int ln1 = wcslen(kanatable_ext[c1]);
    if (kanatable[c1][0] == L'x' || kanatable[c2][0] != L'x' && kanatable[c2][0] != L'W' || kanatable[c2][0] == L'W' && kanatable[c1][0] != L'k' && kanatable[c1][0] != L'g' || ln1 == 1 && kanatable_ext[c1][0] != L'u' && kanatable_ext[c1][0] != L'i' || kanatable_ext[c1][0] == L'y' || kanatable[c1][0] == L'W')
        return false;

    //all ャュョ
    if (kanatable_ext[c2][0] == L'y')
    {
        if (ln1 > 1)
        {
            if (kanatable_ext[c1][1] == L'h')
            { //シチ
                if (dbl)
                    s += kanatable_ext[c1][0];
                s += UnicodeString(kanatable_ext[c1], 2) + (kanatable_ext[c2] + 1);
                return true;
            }
            if (kanatable_ext[c1][1] == L'i' && kanatable_ext[c1][0] == L'j')
            {
                if (dbl)
                    add_double_ext(s, kanatable_ext[c1][0]);
                s += kanatable_ext[c1][0];
                s += kanatable_ext[c2] + 1;
                return true;
            }
            if (kanatable_ext[c1][1] == L'i' && kanatable_ext[c1][0] != L'w' || kanatable_ext[c1][0] == L'f' || kanatable_ext[c1][1] == L'e' && (kanatable_ext[c1][0] == L't' || kanatable_ext[c1][0] == L'd'))
            { //2 long vowels ending in i or フ
                if (dbl)
                    add_double_ext(s, kanatable_ext[c1][0]);
                s += kanatable_ext[c1][0];
                s += kanatable_ext[c2];
                return true;
            }
            if (ln1 == 3 && kanatable_ext[c1][1] == L's')
            {
                if (dbl)
                    add_double_ext(s, kanatable_ext[c1][0]);
                s += L"ts";
                s += kanatable_ext[c2];
                return true;
            }
            if (ln1 == 2 && kanatable_ext[c1][0] == L'v' && kanatable_ext[c1][1] == L'u')
            {
                if (dbl)
                    add_double_ext(s, kanatable_ext[c1][0]);
                s += L'v';
                s += kanatable_ext[c2];
                return true;
            }
        }
        else if (kanatable_ext[c1][0] == L'u' && kanatable_ext[c2][1] == L'u')
        { //ウ plus ュ
            if (dbl)
                add_double_ext(s, 'w');
            s += L"wyu";
            return true;
        }
        else if (kanatable_ext[c1][0] == L'v')
        { //ヴ
            if (dbl)
                add_double_ext(s, 'v');
            s += L'v';
            s += kanatable_ext[c2];
            return true;
        }

        return false;
    }
    //ending with ェ ィ
    if (kanatable_ext[c1][0] == L'i' && (kanatable_ext[c2][0] == L'i' || kanatable_ext[c2][0] == L'e'))
    {
        if (dbl)
            add_double_ext(s, 'y');
        s += L'y';
        s += kanatable_ext[c2];
        return true;
    }
    if (kanatable_ext[c1][0] == L'u')
    {
        if (dbl)
            add_double_ext(s, 'w');
        s += L'w';
        s += kanatable_ext[c2];
        return true;
    }
    if (ln1 == 1)
        return false;

    if (kanatable[c2][0] == L'W')
    {
        if (ln1 != 2 || kanatable_ext[c1][1] != L'u')
            return false;
        s += kanatable_ext[c1][0];
        s += kanatable_ext[c2];
        return true;
    }

    if (kanatable_ext[c2][0] != L'u' && kanatable_ext[c1][ln1 - 1] == L'u' && (kanatable_ext[c1][0] == L'u' || kanatable_ext[c1][0] == L'v' || kanatable_ext[c1][0] == L'k' || kanatable_ext[c1][0] == L'g' || kanatable_ext[c1][0] == L't' || kanatable_ext[c1][0] == L'f'))
    {
        kanatable_ext[c1][ln1 - 1] = 0;
        if (dbl)
            add_double_ext(s, kanatable_ext[c1][0]);
        s += kanatable_ext[c1];
        if (kanatable_ext[c1][0] == L'k' || kanatable_ext[c1][0] == L'g')
            s += L'w';
        s += kanatable_ext[c2];
        kanatable_ext[c1][ln1 - 1] = L'u';
        return true;
    }
    if (kanatable_ext[c2][0] == L'i')
    {
        if (kanatable_ext[c1][1] == L'u' && (kanatable_ext[c1][0] == L's' || kanatable_ext[c1][0] == L'z') || kanatable_ext[c1][1] == L'e' && (kanatable_ext[c1][0] == L't' || kanatable_ext[c1][0] == L'd'))
        {
            if (dbl)
                add_double_ext(s, kanatable_ext[c1][0]);
            s += kanatable_ext[c1][0];
            s += kanatable_ext[c2];
            return true;
        }
        return false;
    }
    if (kanatable_ext[c2][0] == L'u')
    {
        if (kanatable_ext[c1][1] == L'o' && (kanatable_ext[c1][0] == L't' || kanatable_ext[c1][0] == L'd' || kanatable_ext[c1][0] == L'h'))
        {
            if (dbl)
                add_double_ext(s, kanatable_ext[c1][0]);
            s += kanatable_ext[c1][0];
            s += kanatable_ext[c2];
            return true;
        }
        return false;
    }
    if (kanatable_ext[c2][0] == L'e')
    {
        if (kanatable_ext[c1][1] == L'i' && (kanatable_ext[c1][0] == L'k' || kanatable_ext[c1][0] == L'g' || kanatable_ext[c1][0] == L'n' || kanatable_ext[c1][0] == L'h' || kanatable_ext[c1][0] == L'b' || kanatable_ext[c1][0] == L'p' || kanatable_ext[c1][0] == L'm' || kanatable_ext[c1][0] == L'r'))
        {
            if (dbl)
                add_double_ext(s, kanatable_ext[c1][0]);
            s += kanatable_ext[c1][0];
            s += L"ye";
            return true;
        }
        if (kanatable_ext[c1][1] == L'i' && kanatable_ext[c1][0] == L'j')
        {
            if (dbl)
                add_double_ext(s, 'j');
            s += L"je";
            return true;
        }
        if (kanatable_ext[c1][2] == L'i' && kanatable_ext[c1][1] == L'h')
        {
            kanatable_ext[c1][2] = 0;
            if (dbl)
                add_double_ext(s, kanatable_ext[c1][0]);
            s += kanatable_ext[c1];
            s += kanatable_ext[c2];
            kanatable_ext[c1][2] = L'i';
            return true;
        }
        return false;
    }

    return false;

}
//turn kana string to learner's Hepburn romaji (eg. ou for long wovel)
UnicodeString __fastcall RomanizeEx(const wchar_t *c)
{
    int len = wcslen(c);
    if (!len)
        return L"";

    UnicodeString s = L"";
    int i, j = -1, k = 0;
    bool dbl = false; //double consonant mark found

    while (k < len && !KANA(c[k]))
        k++;

    if (k == len)
        return L"";

    int c1, c2;
    if (KATAKANA(c[k]))
        c2 = c[k] - 0x30A1;
    else
        c2 = c[k] - 0x3041;

    while ((i = k) < len)
    {

        skip_ext(c, len, k);

        if (DASH(c[i]))
        {
            dbl = false;

            if (k < len)
            {
                if (KATAKANA(c[k]))
                    c2 = c[k] - 0x30A1;
                else
                    c2 = c[k] - 0x3041;
            }

            if (j >= 0 && c[j] == L'ウ')
            {
                s = s.SubString(1, s.Length() - 1) + "wu";
                continue;
            }
            if (!i || j < 0 || dbl || DASH(c[j]) || !s.Length() || !KANAVOWEL(s[s.Length()]))
                continue;
            s += s[s.Length()];
            j = i;
            continue;
        }

        if (!KANA(c[i]))
        {
            THROW(L"Character not kana. Tell the programmer!");
        }

        j = i;

        c1 = c2;
        if (k < len)
        {
            if (KATAKANA(c[k]))
                c2 = c[k] - 0x30A1;
            else
                c2 = c[k] - 0x3041;
        }

        if (minitsu_ext(c1))
        {
            dbl = true;
            continue;
        }

        if (c[i] == L'ヴ' && k < len - 1 && c[k] == L'ィ')
        {
            int g = k;
            skip_ext(c, len, g);
            if (g < len && c[g] == L'ェ')
            {
                s += L"vye";
                skip_ext(c, len, g);
                k = g;
                if (k < len)
                {
                    if (KATAKANA(c[k]))
                        c2 = c[k] - 0x30A1;
                    else
                        c2 = c[k] - 0x3041;
                }
                continue;
            }
        }
        if (c[i] == L'フ' && k < len - 1 && c[k] == L'ィ')
        {
            int g = k;
            skip_ext(c, len, g);
            if (g < len && c[g] == L'ェ')
            {
                s += L"fye";
                skip_ext(c, len, g);
                k = g;
                if (k < len)
                {
                    if (KATAKANA(c[k]))
                        c2 = c[k] - 0x30A1;
                    else
                        c2 = c[k] - 0x3041;
                }
                continue;
            }
        }

        if (i + 1 < len && !DASH(c[i + 1]))
        {
            if (convert_ex(s, c1, c2, dbl))
            {
                dbl = false;
                skip_ext(c, len, k);
                if (k < len)
                {
                    if (KATAKANA(c[k]))
                        c2 = c[k] - 0x30A1;
                    else
                        c2 = c[k] - 0x3041;
                }
                continue;
            }
        }

        if (dbl)
            add_double_ext(s, kanatable_ext[c1][0]);
        s += kanatable_ext[c1];
        dbl = false;
    }
    return s;
}

//turn typed romaji text to easily processable one
UnicodeString __fastcall TranslateRomaji(UnicodeString str, byte *spaces)
{
    str = str.Trim();

    if (spaces)
        memset(spaces, 1, str.Length()*3);

    wchar_t *out = new wchar_t[str.Length() * 3];
    int opos = 0;

    for (int i = 1; i <= str.Length(); i++)
    {
        switch (str[i])
        {
        case 'n':
        case 'N':
            out[opos++] = str[i];
            if (i == str.Length() || (str[i + 1] != 'a' && str[i + 1] != 'A' && str[i + 1] != 'i' && str[i + 1] != 'I' && str[i + 1] != 'u' && str[i + 1] != 'U' && str[i + 1] != 'e' && str[i + 1] != 'E' && str[i + 1] != 'o' && str[i + 1] != 'O' && str[i + 1] != 'y' && str[i + 1] != 'Y'))
            {
                out[opos++] = '\'';
            }
            break;
        case 't':
        case 'T':
            if (i < str.Length() && (str[i + 1] == 's' || str[i + 1] == 'S'))
            {
                out[opos++] = str[i] != 'T' && str[i + 1] != 'S' ? 't' : 'T';
                if (spaces != NULL)
                    spaces[opos - 1] = 2;
                i++;
            }
            else
            {
                out[opos++] = str[i];
            }
            break;
        case 's':
        case 'S':
            if (i < str.Length() && (str[i + 1] == 'h' || str[i + 1] == 'H'))
            {
                if (i < str.Length() - 1 && (str[i + 2] == 'i' || str[i + 2] == 'I'))
                {
                    out[opos++] = str[i + 1] == 'h' ? str[i] : 'S';
                    if (spaces != NULL)
                        spaces[opos - 1] = 2;
                    out[opos++] = str[i + 2];
                    i += 2;
                }
                else if (i == str.Length() - 1)
                {
                    out[opos++] = str[i + 1] == 'h' ? str[i] : 'S';
                    if (spaces != NULL)
                        spaces[opos - 1] = 2;
                    i++;
                }
                else
                {
                    memcpy(out + opos, &(str[i] == 's' && str[i + 1] == 'h' ? L"sy" : L"SY"), sizeof(wchar_t)*2);
                    opos += 2;
                    i++;
                }
            }
            else
            {
                out[opos++] = str[i];
            }
            break;
        case 'c':
        case 'C':
            if (i < str.Length())
            {
                if (str[i + 1] == 'c' || str[i + 1] == 'C' || str[i + 1] == 'i' || str[i + 1] == 'I' || str[i + 1] == 'y' || str[i + 1] == 'Y')
                {
                    out[opos++] = str[i] == 'c' && !IsUpper(str[i + 1]) ? 't' : 'T';
                    break;
                }
                else if (str[i + 1] == 'a' || str[i + 1] == 'A' || str[i + 1] == 'u' || str[i + 1] == 'U' || str[i + 1] == 'e' || str[i + 1] == 'E' || str[i + 1] == 'o' || str[i + 1] == 'O')
                {
                    memcpy(out + opos, &(str[i] == 'c' && !IsUpper(str[i + 1]) ? L"ty" : L"TY"), sizeof(wchar_t)*2);
                    opos += 2;
                    if (spaces != NULL)
                        spaces[opos - 1] = 0;
                    i++;
                    out[opos++] = str[i];
                    break;
                }
            }

            if (i < str.Length() && (str[i + 1] == 'h' || str[i + 1] == 'H'))
            {
                if (i < str.Length() - 1 && (str[i + 2] == 'i' || str[i + 2] == 'I'))
                {
                    out[opos++] = str[i] == 'c' && str[i + 1] == 'h' ? 't' : 'T';
                    i++;
                    if (spaces != NULL)
                        spaces[opos - 1] = 2;
                    out[opos++] = str[i + 1];
                    i++;
                }
                else if (i == str.Length() - 1)
                {
                    out[opos++] = str[i] == 'c' && (str[i + 1] == 'h' || str[i + 1] == 'y') ? 't' : 'T';
                }
                else
                {
                    memcpy(out + opos, &(str[i] == 'c' && (str[i + 1] == 'h' || str[i + 1] == 'y') ? L"ty" : L"TY"), sizeof(wchar_t)*2);
                    opos += 2;
                    out[opos++] = str[i + 2];
                    i += 2;
                }
            }
            else if (i == str.Length())
            {
                out[opos++] = str[i] == 'c' ? 't' : 'T';
            }
            else
            {
                out[opos++] = str[i];
            }
            break;
        case 'w':
        case 'W':
            if (i < str.Length() && (str[i + 1] == 'i' || str[i + 1] == 'I' || str[i + 1] == 'u' || str[i + 1] == 'U' || str[i + 1] == 'e' || str[i + 1] == 'E'))
            {
                memcpy(out + opos, &(str[i] == 'w' && !IsUpper(str[i + 1]) ? L"ux" : L"UX"), sizeof(wchar_t)*2);
                opos += 2;
                if (spaces != NULL)
                    spaces[opos - 1] = 0;
                out[opos++] = str[i + 1];
                i++;
            }
            else
            {
                out[opos++] = str[i];
            }
            break;
        case 'v':
        case 'V':
            if (i < str.Length() && (str[i + 1] == 'a' || str[i + 1] == 'A' || str[i + 1] == 'i' || str[i + 1] == 'I' || str[i + 1] == 'e' || str[i + 1] == 'E' || str[i + 1] == 'o' || str[i + 1] == 'O'))
            {
                memcpy(out + opos, &(str[i] == 'v' && !IsUpper(str[i + 1]) ? L"vx" : L"VX"), sizeof(wchar_t)*2);
                opos += 2;
                if (spaces != NULL)
                    spaces[opos - 1] = 0;
                out[opos++] = toupper(str[i + 1]);
                i++;
            }
            else
            {
                out[opos++] = str[i];
            }
            break;
        case 'j':
        case 'J':
            if (i < str.Length() && (str[i + 1] == 'i' || str[i + 1] == 'I'))
            {
                memcpy(out + opos, &(str[i] == 'j' && str[i + 1] == 'i' ? L"zi" : L"ZI"), sizeof(wchar_t)*2);
                opos += 2;
                i++;
            }
            else if (i == str.Length())
            {
                out[opos++] = str[i] == 'j' ? 'z' : 'Z';
            }
            else
            {
                if (str[i + 1] != 'j' && str[i + 1] != 'J')
                {
                    memcpy(out + opos, &(str[i] == 'j' ? L"zy" : L"ZY"), sizeof(wchar_t)*2);
                    opos += 2;
                    if (spaces != NULL)
                        spaces[opos - 1] = 0;
                }
                else
                {
                    out[opos++] = str[i] == 'j' ? 'z' : 'Z';
                }
            }
            break;
        case 'd':
        case 'D':
            if (i < str.Length() && (str[i + 1] == 'z' || str[i + 1] == 'Z'))
            {
                if (i < str.Length() - 1 && (str[i + 2] == 'i' || str[i + 2] == 'I'))
                {
                    out[opos++] = str[i] == 'd' && str[i + 1] == 'z' ? 'd' : 'D';
                    if (spaces != NULL)
                        spaces[opos - 1] = 2;
                    out[opos++] = str[i + 2];
                    i += 2;
                }
                else
                {
                    memcpy(out + opos, &(str[i] == 'd' ? L"dy" : L"DY"), sizeof(wchar_t)*2);
                    opos += 2;
                    if (str[i] == 'D' && i < str.Length() - 1 && (str[i + 2] == 'a' || str[i + 2] == 'A' || str[i + 2] == 'u' || str[i + 2] == 'U' || str[i + 2] == 'e' || str[i + 2] == 'E' || str[i + 2] == 'o' || str[i + 2] == 'O'))
                    {
                        out[opos++] = toupper(str[i + 2]);
                        i++;
                    }
                    i++;
                }
            }
            else
            {
                out[opos++] = str[i];
            }
            break;
        case 'f':
        case 'F':
            if (i < str.Length() && (str[i + 1] == 'u' || str[i + 1] == 'U'))
            {
                memcpy(out + opos, &(str[i] == 'f' && str[i + 1] == 'u' ? L"hu" : L"HU"), sizeof(wchar_t)*2);
                opos += 2;
                i++;
            }
            else if (i <= str.Length())
            {
                out[opos++] = str[i];
            }
            break;
        case 'l':
        case 'L':
            out[opos++] = str[i] == 'l' ? 'x' : 'X';
            break;
        default:
            out[opos++] = str[i];
            break;
        }
    }
    str = UnicodeString(out, opos);
    delete[] out;
    return str;
}

UnicodeString __fastcall TrimRomanized(const UnicodeString &str)
{
    UnicodeString s = Romanize(str.c_str()).LowerCase();
    int n = s.Length();
    while (n && s[n] != L'a' && s[n] != L'i' && s[n] != L'u' && s[n] != L'e' && s[n] != L'o' && s[n] != L'\'')
        s.SetLength(--n);
    return s;
}

UnicodeString __fastcall RemoveUseless(const UnicodeString &str)
{
    UnicodeString s;
    int slen = str.Length();
    for (int ix = 1; ix <= slen; ++ix)
        if (JAPAN(str[ix]))
            s += str[ix];

    return s;
}

#ifdef _DEBUG
static TGlobalTest tester2(137);
#endif
