//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <ZLib.hpp>

#include "zkformats.h"
#include "zrads.h"
#include "kanji.h"
#include "words.h"
#include "collection.h"
#include "searchtree.h"
#include "romajizer.h"
#include "wordgroupstudy.h"
#include "romajizer.h"
#include "sentences.h"
#include "kanjielement.h";
#include "kanjicard.h"
#include "repetition.h"
#include "studylist.h"
#include "kanjigroup.h"
#include "msgform.h"
#include "genworddata.h"
#include "sentences.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester1(208);
#endif

int exampleheadersize = 8;

// Important for correct updates: change DICTIONARY_VERSION if DICTIONARY_BASE_VERSION changes!

const unsigned int DICTIONARY_BASE_VERSION = 1;
//1 - Added 2012 04 17 - First introduction of separate base dictionary.

const unsigned int DICTIONARY_VERSION = 10;
//10 changed 2012 04 15 - First introduction of separate base dictionary.
//9 - Added JLPT data

const unsigned int GLOBALGROUP_VERSION = 20;
//20 chagned 2013 07 04 - Updated the word group study, which requires new settings for the study groups.
//19 chagned 2013 07 01 - Removed kanjiwords gaplist from the TItemTree which complicated word deletion and dictionary updates. It was used for fast filtering by kanji which will be solved differently.
//18 changed 2013 05 24 - Removed fusage from TWordStudyDefinition because the number of studied items for a word can be guessed from ftakentype.
//17 changed 2012 04 15 - The groups are connected to the dictionary. Only indexes are saved, not words. Replacing user data files separately is not possible. Use the import instead.
//16 changed 2011 12 27 - changing how the long-term study remembers newly added items in the test
//15 changed 2011 08 06 - fixing v0.591 -> v0.593

const unsigned int STUDYPROFILE_VERSION = 2;

const unsigned int EXAMPLESENTENCE_VERSION = 4;
//4 changed 2012 05 01 - Attempt to make examples file independent of main dictionary
//3 - saving tree for examples instead of the unstructured binary data

//#define SEARCHTREE_VERSION   - defined in searchtree.cpp
//#define ELEMENTS_VERSION  -  SOD element version definition in kanjielement.cpp

/*to be removed, Nov. 2010 for v0.57*/
#define WORDGROUP_VERSION    3
#define KANJIGROUP_VERSION   7
/*----------------------------------*/

//#define JLPTLIST_VERSION   - defined in jlpteditor.cpp

//#define DROPDOWNLIST_VERSION - defined in dropdown.cpp

wchar_t radicals[214] = {
    0x4e00, 0x4e28, 0x4e36, 0x4e3f, 0x4e59, 0x4e85, 0x4e8c, 0x4ea0,
    0x4eba, 0x513f, 0x5165, 0x516b, 0x5182, 0x5196, 0x51ab, 0x51e0,
    0x51f5, 0x5200, 0x529B, 0x52F9, 0x5315, 0x531A, 0x5338, 0x5341,
    0x535C, 0x5369, 0x5382, 0x53B6, 0x53C8, 0x53E3, 0x56D7, 0x571F,
    0x58EB, 0x5902, 0x590A, 0x5915, 0x5927, 0x5973, 0x5B50, 0x5B80,
    0x5BF8, 0x5C0F, 0x5C22, 0x5C38, 0x5C6E, 0x5C71, 0x5DDB, 0x5DE5,
    0x5DF1, 0x5DFE, 0x5E72, 0x5E7A, 0x5E7F, 0x5EF4, 0x5EFE, 0x5F0B,
    0x5F13, 0x5F50, 0x5F61, 0x5F73, 0x5FC3, 0x6208, 0x6236, 0x624B,
    0x652F, 0x6534, 0x6587, 0x6597, 0x65A4, 0x65B9, 0x65E0, 0x65E5,
    0x66F0, 0x6708, 0x6728, 0x6B20, 0x6B62, 0x6B79, 0x6BB3, 0x6BCB,
    0x6BD4, 0x6BDB, 0x6C0F, 0x6C14, 0x6C34, 0x706B, 0x722A, 0x7236,
    0x723B, 0x723F, 0x7247, 0x7259, 0x725B, 0x72AC, 0x7384, 0x7389,
    0x74DC, 0x74E6, 0x7518, 0x751F, 0x7528, 0x7530, 0x758B, 0x7592,
    0x7676, 0x767D, 0x76AE, 0x76BF, 0x76EE, 0x77DB, 0x77E2, 0x77F3,
    0x793A, 0x79B8, 0x79BE, 0x7A74, 0x7ACB, 0x7AF9, 0x7C73, 0x7CF8,
    0x7F36, 0x7F51, 0x7F8A, 0x7FBD, 0x8001, 0x800C, 0x8012, 0x8033,
    0x807F, 0x8089, 0x81E3, 0x81EA, 0x81F3, 0x81FC, 0x820C, 0x821B,
    0x821F, 0x826E, 0x8272, 0x8278, 0x864D, 0x866B, 0x8840, 0x884C,
    0x8863, 0x897F, 0x898B, 0x89D2, 0x8A00, 0x8C37, 0x8C46, 0x8C55,
    0x8C78, 0x8C9D, 0x8D64, 0x8D70, 0x8DB3, 0x8EAB, 0x8ECA, 0x8F9B,
    0x8FB0, 0x8FB5, 0x9091, 0x9149, 0x91C6, 0x91CC, 0x91D1, 0x9577,
    0x9580, 0x961C, 0x96B6, 0x96B9, 0x96E8, 0x9751, 0x975E, 0x9762,
    0x9769, 0x97CB, 0x97ED, 0x97F3, 0x9801, 0x98A8, 0x98DB, 0x98DF,
    0x9996, 0x9999, 0x99AC, 0x9AA8, 0x9AD8, 0x9ADF, 0x9B25, 0x9B2F,
    0x9B32, 0x9B3C, 0x9B5A, 0x9CE5, 0x9E75, 0x9E7F, 0x9EA5, 0x9EBB,
    0x9EC4, 0x9ECD, 0x9ED1, 0x9EF9, 0x9EFD, 0x9F0E, 0x9F13, 0x9F20,
    0x9F3B, 0x9F4A, 0x9F52, 0x9F8D, 0x9F9C, 0x9FA0
};
//---------------------------------------------------------------------------
wchar_t parts[252] = {
    0x4e00, 0xff5c, 0x4e36, 0x30ce, 0x4e59, 0x4e85, 0x4e8c, 0x4ea0,
    0x4eba, 0x4ebb, 0x4e2a, 0x513f, 0x5165, 0x30cf, 0x4e37, 0x5182,
    0x5196, 0x51ab, 0x51e0, 0x51f5, 0x5200, 0x5202, 0x529b, 0x52f9,
    0x5315, 0x531a, 0x5341, 0x535c, 0x5369, 0x5382, 0x53b6, 0x53c8,
    0x30de, 0x4e5d, 0x30e6, 0x4e43, 0x8fb6, 0x53e3, 0x56d7, 0x571f,
    0x58eb, 0x5902, 0x5915, 0x5927, 0x5973, 0x5b50, 0x5b80, 0x5bf8,
    0x5c0f, 0x5c1a, 0x5c22, 0x5c38, 0x5c6e, 0x5c71, 0x5ddd, 0x5ddb,
    0x5de5, 0x5df2, 0x5dfe, 0x5e72, 0x5e7a, 0x5e7f, 0x5ef4, 0x5efe,
    0x5f0b, 0x5f13, 0x30e8, 0x5f51, 0x5f61, 0x5f73, 0x5fc4, 0x624c,
    0x6c35, 0x72ad, 0x8279, 0x9092, 0x961d, 0x4e5f, 0x4ea1, 0x53ca, //80
    0x4e45, 0x8002, 0x5fc3, 0x6208, 0x6238, 0x624b, 0x652f, 0x6535,
    0x6587, 0x6597, 0x65a4, 0x65b9, 0x65e0, 0x65e5, 0x66f0, 0x6708,
    0x6728, 0x6b20, 0x6b62, 0x6b79, 0x6bb3, 0x6bd4, 0x6bdb, 0x6c0f,
    0x6c14, 0x6c34, 0x706b, 0x706c, 0x722a, 0x7236, 0x723b, 0x723f,
    0x7247, 0x725b, 0x72ac, 0x793b, 0x738b, 0x5143, 0x4e95, 0x52ff,
    0x5c24, 0x4e94, 0x5c6f, 0x5df4, 0x6bcb, 0x7384, 0x74e6, 0x7518,
    0x751f, 0x7528, 0x7530, 0x758b, 0x7592, 0x7676, 0x767d, 0x76ae, //136
    0x76bf, 0x76ee, 0x77db, 0x77e2, 0x77f3, 0x793a, 0x79b9, 0x79be, //144
    0x7a74, 0x7acb, 0x8864, 0x4e16, 0x5de8, 0x518a, 0x6bcd, 0x7f52, //152
    0x7259, 0x74dc, 0x7af9, 0x7c73, 0x7cf8, 0x7f36, 0x7f8a, 0x7fbd,
    0x800c, 0x8012, 0x8033, 0x807f, 0x8089, 0x81ea, 0x81f3, 0x81fc,
    0x820c, 0x821f, 0x826e, 0x8272, 0x864d, 0x866b, 0x8840, 0x884c,
    0x8863, 0x897f, 0x81e3, 0x898b, 0x89d2, 0x8a00, 0x8c37, 0x8c46,
    0x8c55, 0x8c78, 0x8c9d, 0x8d64, 0x8d70, 0x8db3, 0x8eab, 0x8eca,
    0x8f9b, 0x8fb0, 0x9149, 0x91c6, 0x91cc, 0x821b, 0x9ea6, 0x91d1,
    0x9577, 0x9580, 0x96b6, 0x96b9, 0x96e8, 0x9752, 0x975e, 0x5944,
    0x5ca1, 0x514d, 0x6589, 0x9762, 0x9769, 0x97ed, 0x97f3, 0x9801,
    0x98a8, 0x98db, 0x98df, 0x9996, 0x9999, 0x54c1, 0x99ac, 0x9aa8,
    0x9ad8, 0x9adf, 0x9b25, 0x9b2f, 0x9b32, 0x9b3c, 0x7adc, 0x97cb,
    0x9b5a, 0x9ce5, 0x9e75, 0x9e7f, 0x9ebb, 0x4e80, 0x6ef4, 0x9ec4,
    0x9ed2, 0x9ecd, 0x9ef9, 0x7121, 0x6b6f, 0x9efd, 0x9f0e, 0x9f13,
    0x9f20, 0x9f3b, 0x9f4a, 0x9fa0
};

// For database import:
//change redefine.cpp and newword.cpp when editing this (alter checkboxes)  and the filter dialog as well
wchar_t *wordtypes[WT_COUNT] = {
    L"p&n", L"p&p-suru", L"p&v-u", L"p&exp", L"p&a-na",
    L"p&v-t", L"p&v-ru", L"p&v-i", L"p&adv", L"p&a", L"p&a-no",
    L"p&suf", L"p&adv-to", L"p&pref", L"p&v-suru", L"p&a-npre",
    L"p&conj", L"p&int", L"p&v-iku", L"p&v-aux", L"p&v-kuru",
    L"p&a-aux", L"p&v-aru"
};

wchar_t *wordnotes[WN_COUNT] = {
    L"m&sens", L"m&abbr", L"m&hon", L"m&hum", L"m&col",
    L"m&pol", L"m&sl", L"m&fam", L"m&male", L"m&fem", L"m&kana",
    L"m&kanji", L"m&arch", L"m&rare", L"m&obs", L"m&vulg", L"m&X",
    L"m&derog", L"m&obsc", L"m&id", L"m&m-sl"
};

wchar_t *wordfields[WF_COUNT] = {
    L"f&math", L"f&ling", L"f&comp", L"f&food", L"f&MA", L"f&geom",
    L"f&physics", L"f&Buddh"
};

// Short text of word tags. WARNING: These cannot contain space nor the tab character nor comma.
wchar_t *wtypetext[WT_COUNT] = {
    L"n.", L"-suru", L"-u", L"exp.", L"-na", L"tr.", L"-ru", L"intr.", L"adv.", L"a.", L"-no",
    L"suf.", L"-to", L"pref.", L"suru", L"pn-a.", L"conj.", L"int.", L"-iku",
    L"aux.v.", L"-kuru", L"aux.a.", L"-aru"
};

wchar_t *wnotetext[WN_COUNT] = {
    L"sens.", L"abr.", L"hon.", L"hum.", L"col.", L"pol.", L"sl.", L"fam.", L"m.", L"f.", L"kana",
    L"kanji", L"arch.", L"rare", L"obs.", L"vulg.", L"X", L"derog.", L"obsc.", L"idiom.",
    L"m.sl."
};

wchar_t *wfieldtext[WF_COUNT] = {
    L"math", L"ling.", L"comp.", L"food", L"m.a.", L"geom.", L"phys.", L"Buddh."
};

wchar_t *ntagtext[NM_COUNT] = {
    L"m.", L"fem.", L"sur.", L"g", L"ps.", L"p.",
    L"st.", L"org.", L"com.", L"pro."
};

// On screen in dictionary.
wchar_t *wlongtypetext[WT_COUNT] = {
    L"noun", L"takes -suru", L"Godan verb", L"expression", L"-na adjective", L"transitive verb", L"Ichidan verb", L"intransitive verb",
    L"adverb", L"adjective", L"may take -no", L"suffix", L"Taru adjective", L"prefix", L"suru verb", L"pre-noun adjective",
    L"conjunction", L"interjection", L"-iku verb", L"auxiliary verb", L"-kuru verb", L"auxiliary adjective", L"-aru verb"
};

wchar_t *wlongnotetext[WN_COUNT] = {
    L"sensitivity in usage", L"abbreviation", L"honorific", L"humble language", L"colloquialism", L"polite", L"slang", L"familiar language", L"male", L"female",
    L"kana alone", L"kanji alone", L"archaism", L"rare", L"obsolete term", L"vulgar", L"X rated", L"derogatory", L"obscure", L"idiomatic", L"manga slang"
};

wchar_t *wlongfieldtext[WF_COUNT] = {
    L"math", L"linguistics", L"computers", L"food", L"martial arts", L"geometry", L"physics", L"Buddhist term"
};

wchar_t *nlongtagtext[NM_COUNT] = {
    L"male name", L"female name", L"surname", L"given name", L"person name", L"place name",
    L"station name", L"organization", L"company name", L"product name"
};

wchar_t *winflectiontext[] = {
    L"polite", L"polite past", L"polite neg.", L"polite volitional / tentative",
    L"neg.", L"passive", L"causative", L"passive causative", L"noun base",
    L"intent", L"3rd intent", L"appearance", L"conjunctive", L"past",
    L"-tara conditional", L"partial conjunctive", L"-ba conditional", L"potential",
    L"command", L"volitional / tentative", L"suru base", L"potential / passive",
    L"adverb", L"noun adj.", L"-na adj.", L"continuous", L"must / neg. -ba cond.",
    L"finished / bothersome", L"without / neg. request", L"without / neg. conjunctive",
    L"archaic neg.", L"imperative", L"do in advance / keep ~ing"
};

//WARNING: only used when generating kanji dictionary from imported txt files! This array is not allocated normally!
//holds index of a kanji given by its unicode, that is: kanjiindex[unicode-0x4e00] = index of kanji in kanji list
int *kanjiindex;

void feol(FILE *f)
{
    wchar_t w[] = L"\r\n";
    fwrite(w, sizeof(wchar_t), 2, f);
}

void funicode(FILE *f)
{
    wchar_t w = L'\xFEFF';
    fwrite(&w, sizeof(wchar_t), 1, f);
}

void fdelim(FILE *f)
{
    wchar_t w = L'\t';
    fwrite(&w, sizeof(wchar_t), 1, f);
}

void swapbytes(wchar_t &b)
{
    b = ((((char*) &b)[0]) << 8) | (((char*) &b)[1] & 0xff);
}


int getlinew(wchar_t *line, int buffsize, FILE *f, bool &bigendian)
{
    int pos = 0;
    int n;

    n = fread(line, 2, 1, f);
    if (ftell(f) == 2 && n && (line[0] == 0xFEFF || line[0] == 0xFFFE))
    {
        if (line[0] == 0xFFFE)
            bigendian = true;
        else
            bigendian = false;
        n = fread(line, 2, 1, f);
        if (bigendian)
            swapbytes(line[0]);
    }
    pos += n;

    while (n && line[pos - 1] != 0x000a)
    {
        if (pos + 2 >= buffsize)
            THROW(L"Error!! Trying to read in line that is over the buffer size!");
        n = fread(line + pos, 2, 1, f);
        if (bigendian)
            swapbytes(line[pos]);
        pos += n;
    }

    n = pos - (pos >= 2 && line[pos - 1] == 0x000a && line[pos - 2] == 0x000d ? 2 : pos >= 1 && line[pos - 1] == 0x000a ? 1 : 0);
    line[n] = 0x0000;
    return (n ? n : pos ? -1 : 0);
}

static bool getmbchar(FILE *f, wchar_t *dest)
{
    char c[7];
    int n = fread(c, 1, 1, f);
    if (n == 0)
        return false;
    if ((c[0] & (1 << 7)) == 0)
    {
        *dest = c[0];
        return true;
    }

    wchar_t &wc = *dest;
    wc = 0;

    byte len;
    if ((c[0] & 0xFC) == 0xFC)
    {
        if ((c[0] & 0x2) == 0x2)
            return false;
        len = 6;
    }
    else if ((c[0] & 0xF8) == 0xF8)
        len = 5;
    else if ((c[0] & 0xF0) == 0xF0)
        len = 4;
    else if ((c[0] & 0xE0) == 0xE0)
        len = 3;
    else if ((c[0] & 0xC0) == 0xC0)
        len = 2;
    else
        return false;

    fread(c + 1, 1, len - 1, f);

    switch (len)
    {
    case 3:
        if ((c[1] & 0x40) != 0 || (c[2] & 0x40) != 0)
            return false;
        wc = ((c[0] & 0xF) << 12) | ((c[1] & 0x3F) << 6) | (c[2] & 0x3F);
        return true;
    case 2:
        if ((c[1] & 0x40) != 0)
            return false;
        wc = ((c[0] & 0xF) << 6) | (c[1] & 0x3F);
        return true;
    default:
        return false;
    }
}

int getlinemb(wchar_t *line, int buffsize, FILE *f)
{
    int pos = ftell(f);
    if (!getmbchar(f, line))
        return 0;

    if (pos == 0 && (line[0] == 0xFEFF || line[0] == 0xFFFE) && !getmbchar(f, line)) // Skip BOM
        return 0;
    pos = 1;

    while (line[pos - 1] != 0x000a)
    {
        if (pos + 2 >= buffsize)
            THROW(L"Error!! Trying to read in line that is over the buffer size!");
        if (getmbchar(f, line + pos))
            ++pos;
        else
            return pos;
    }

    if (pos > 1 && line[pos - 2] == 0x000d)
    {
        line[pos - 2] = 0;
        return pos != 2 ? pos - 2 : -1;
    }
    line[pos - 1] = 0;
    return pos != 1 ? pos - 1 : -1;
}

void __fastcall generate_validkanji()
{
    if (!validkanji)
        validkanji = new bool[0x9faf - 0x4e00 + 1];
    memset(validkanji, 0, sizeof(bool) * (0x9faf - 0x4e00 + 1));
    for (int ix = 0; ix < kanjis->Count; ++ix)
        validkanji[((int)kanjis->Kanjis[ix]->ch) - 0x4e00] = true;
}

//generating table for hiragana, katakana, brackets, double width roomaji, half width katakana
void __fastcall generate_validunicode()
{
    wchar_t validchar[] = {
        L'　', L'、', L'。', L'〃', L'〄', L'々', L'〆', L'〇', L'〈', L'〉', L'《', L'》', L'「', L'」', L'『', L'』',
        L'【', L'】', L'〒', L'〓', L'〔', L'〕', L'〖', L'〗', L'〘', L'〙', L'〚', L'〛', L'〜', L'〝', L'〞', L'〟',
        L'〠', L'〡', L'〢', L'〣', L'〤', L'〥', L'〦', L'〧', L'〨', L'〩', L'〰', L'〱', L'〲', L'〳', L'〴',
        L'〵', L'〶', L'〷', L'〻', L'〼', L'〽', L'〾', L'ぁ', L'あ', L'ぃ', L'い', L'ぅ', L'う', L'ぇ', L'え',
        L'ぉ', L'お', L'か', L'が', L'き', L'ぎ', L'く', L'ぐ', L'け', L'げ', L'こ', L'ご', L'さ', L'ざ', L'し',
        L'じ', L'す', L'ず', L'せ', L'ぜ', L'そ', L'ぞ', L'た', L'だ', L'ち', L'ぢ', L'っ', L'つ', L'づ', L'て',
        L'で', L'と', L'ど', L'な', L'に', L'ぬ', L'ね', L'の', L'は', L'ば', L'ぱ', L'ひ', L'び', L'ぴ', L'ふ',
        L'ぶ', L'ぷ', L'へ', L'べ', L'ぺ', L'ほ', L'ぼ', L'ぽ', L'ま', L'み', L'む', L'め', L'も', L'ゃ', L'や',
        L'ゅ', L'ゆ', L'ょ', L'よ', L'ら', L'り', L'る', L'れ', L'ろ', L'ゎ', L'わ', L'ゐ', L'ゑ', L'を', L'ん',
        L'ゔ', L'ゕ', L'ゖ', 'ゟ', L'゠', L'ァ', L'ア', L'ィ', L'イ', L'ゥ', L'ウ', L'ェ', L'エ', L'ォ', L'オ',
        L'カ', L'ガ', L'キ', L'ギ', L'ク', L'グ', L'ケ', L'ゲ', L'コ', L'ゴ', L'サ', L'ザ', L'シ', L'ジ', L'ス',
        L'ズ', L'セ', L'ゼ', L'ソ', L'ゾ', L'タ', L'ダ', L'チ', L'ヂ', L'ッ', L'ツ', L'ヅ', L'テ', L'デ', L'ト',
        L'ド', L'ナ', L'ニ', L'ヌ', L'ネ', L'ノ', L'ハ', L'バ', L'パ', L'ヒ', L'ビ', L'ピ', L'フ', L'ブ', L'プ',
        L'ヘ', L'ベ', L'ペ', L'ホ', L'ボ', L'ポ', L'マ', L'ミ', L'ム', L'メ', L'モ', L'ャ', L'ヤ', L'ュ', L'ユ',
        L'ョ', L'ヨ', L'ラ', L'リ', L'ル', L'レ', L'ロ', L'ヮ', L'ワ', L'ヰ', L'ヱ', L'ヲ', L'ン', L'ヴ', L'ヵ',
        L'ヶ', L'ヷ', L'ヸ', L'ヹ', L'ヺ', L'・', L'ー', L'ヽ', L'ヾ', L'ヿ', L'！', L'＂', L'＃', L'＄', L'％',
        L'＆', L'＇', L'（', L'）', L'＊', L'＋', L'，', L'－', L'．', L'／', L'０', L'１', L'２', L'３', L'４',
        L'５', L'６', L'７', L'８', L'９', L'：', L'；', L'＜', L'＝', L'＞', L'？', L'＠', L'Ａ', L'Ｂ', L'Ｃ',
        L'Ｄ', L'Ｅ', L'Ｆ', L'Ｇ', L'Ｈ', L'Ｉ', L'Ｊ', L'Ｋ', L'Ｌ', L'Ｍ', L'Ｎ', L'Ｏ', L'Ｐ', L'Ｑ', L'Ｒ',
        L'Ｓ', L'Ｔ', L'Ｕ', L'Ｖ', L'Ｗ', L'Ｘ', L'Ｙ', L'Ｚ', L'［', L'＼', L'］', L'＾', L'＿', L'｀', L'ａ', L'ｂ',
        L'ｃ', L'ｄ', L'ｅ', L'ｆ', L'ｇ', L'ｈ', L'ｉ', L'ｊ', L'ｋ', L'ｌ', L'ｍ', L'ｎ', L'ｏ', L'ｐ', L'ｑ', L'ｒ',
        L'ｓ', L'ｔ', L'ｕ', L'ｖ', L'ｗ', L'ｘ', L'ｙ', L'ｚ', L'｛', L'｜', L'｝', L'～', L'｟', L'｠', L'｡', L'｢', L'｣',
        L'､', L'･', L'ｦ', L'ｧ', L'ｨ', L'ｩ', L'ｪ', L'ｫ', L'ｬ', L'ｭ', L'ｮ', L'ｯ', L'ｰ', L'ｱ', L'ｲ', L'ｳ', L'ｴ',
        L'ｵ', L'ｶ', L'ｷ', L'ｸ', L'ｹ', L'ｺ', L'ｻ', L'ｼ', L'ｽ', L'ｾ', L'ｿ', L'ﾀ', L'ﾁ', L'ﾂ', L'ﾃ', L'ﾄ',
        L'ﾅ', L'ﾆ', L'ﾇ', L'ﾈ', L'ﾉ', L'ﾊ', L'ﾋ', L'ﾌ', L'ﾍ', L'ﾎ', L'ﾏ', L'ﾐ', L'ﾑ', L'ﾒ', L'ﾓ', L'ﾔ',
        L'ﾕ', L'ﾖ', L'ﾗ', L'ﾘ', L'ﾙ', L'ﾚ', L'ﾛ', L'ﾜ', L'ﾝ', L'ﾞ', L'ﾟ', L'￠', L'￡', L'￢', L'￣', L'￤',
        L'￥', L'￦', L'￨', L'￩', L'￪', L'￫', L'￬', L'￭', L'￮'
    };

    if (!validcode)
        validcode = new bool[validcode_count];
    memset(validcode, 0, sizeof(bool)*validcode_count);

    validcode_lowerhalf = 0;
    int size = sizeof(validchar) / sizeof(wchar_t);
    for (int ix = 0; ix < size; ++ix)
    {
        if (validchar[ix] >= 0x3000 && validchar[ix] < 0x30ff)
        {
            validcode[int(validchar[ix]) - 0x3000] = true;
            validcode_lowerhalf = int(validchar[ix]) - 0x3000;
        }
        else if (validchar[ix] >= 0xff00 && validchar[ix] < 0xffe0)
        {
            validcode[int(validchar[ix]) - 0xff00 + validcode_lowerhalf + 1] = true;
        }
    }
}

void import_kanji(const wchar_t *name)
{
    int n;
    wchar_t *tmp;
    wchar_t line[1024];

    kanjiindex = new int[0x9faf - 0x4e00 + 1];

    TKanji *k;

    FILE *f = _wfopen((ExtractFileDir(ParamStr(0)) + L"\\" + name).c_str(), L"rb");

    if (!f) //error
        THROW(L"Error opening kanji file!");

    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {

            k = new TKanji;
            memset(k, 0, sizeof(TKanji));
            k->element = -1;

            k->SnH[0] = 'x'; //for checking not set value
            k->c4[0] = 'x'; //for checking not set value

            wchar_t *token = wcstok(line, L" ");
            tmp = token;

            k->ch = *tmp;

            kanjiindex[k->ch - 0x4e00] = kanjis->Count;

            token = wcstok(NULL, L" ");
            k->_jis = AStow(token);
            token = wcstok(NULL, L" ");

            while (!KANA(token[0]) && token[0] != L'-' && token[0] != L'.' && token[0] != L'T')
            {
                switch (token[0])
                {
                case (L'U'):
                    k->_uni = AStow(token + 1);
                    break;
                case (L'B'):
                    k->radical = StrToInt(token + 1);
                    break;
                case (L'J'):
                    k->JLPT = StrToInt(token + 1);
                    break;
                case (L'G'):
                    k->jouyou = StrToInt(token + 1);
                    break;
                case (L'S'):
                    k->strokes = MAX(k->strokes, StrToInt(token + 1));
                    break;
                case (L'F'):
                    k->frequency = StrToInt(token + 1);
                    break;
                case (L'N'):
                    k->nelson = StrToInt(token + 1);
                    break;
                case (L'H'):
                    k->halpern = StrToInt(token + 1);
                    break;
                case (L'L'):
                    k->heisig = StrToInt(token + 1);
                    break;
                case (L'K'):
                    k->gakken = StrToInt(token + 1);
                    break;
                case (L'Q'):
                    token++;
                    strncpy(k->c4, toAnsiString(token).c_str(), 4);
                    k->c4[4] = 0;
                    token += 5;
                    k->c4b = *token;
                    break;
                case (L'E'):
                    k->henshall = StrToInt(token + 1);
                    break;
                case (L'I'):
                    if (token[1] == L'N')
                        k->knk = StrToInt(token + 2);
                    else
                    {
                        token++;
                        strcpy(k->SnH, toAnsiString(token).c_str());
                    }
                    break;
                case (L'P'):
                    tmp = token;
                    *wcschr(tmp, L'-') = 0;
                    k->SKIP1 = StrToInt(tmp + 1);
                    tmp += wcslen(tmp) + 1;

                    *wcschr(tmp, L'-') = 0;
                    k->SKIP2 = StrToInt(tmp);
                    tmp += wcslen(tmp) + 1;

                    k->SKIP3 = StrToInt(tmp);
                    break;
                default:;
                }
                token = wcstok(NULL, L" ");
            }

            bool hira = false;
            //first kana encountered
            while (KANA(token[0]) || (token[0] == L'-' && KANA(token[1])) || (token[0] == L'.' && KANA(token[1])))
            {
                //katakana or hiragana?
                for (unsigned int i = 0; i < wcslen(token) && !hira; i++)
                {
                    if (KATAKANA(token[i]))
                    {
                        if (hira) //error in format
                            THROW(L"Katakana after hiragana!");
                        break; //this is katakana
                    }
                    if (HIRAGANA(token[i]))
                        hira = true;
                }

                if (!hira)
                {
                    //add reading
                    if (!k->on)
                    {
                        k->on = new wchar_t*[1];
                    }
                    else
                    {
                        wchar_t **wtemp = k->on;
                        k->on = new wchar_t*[k->oncnt + 1];
                        memcpy(k->on, wtemp, sizeof(void*)*k->oncnt);
                        delete[] wtemp;
                    }
                    k->on[k->oncnt] = new wchar_t[wcslen(token) + 1];
                    wcscpy(k->on[k->oncnt], token);
                    k->oncnt++;
                }
                else
                {
                    //filter readings, drop duplicates with dash
                    bool good = true;
                    n = wcslen(token) + (token[wcslen(token) - 1] == L'-' || token[0] == L'-' ? -1 : 0);
                    for (int i = 0; i < k->kuncnt && good; i++)
                        good = n != (int)wcslen(k->kun[i]) || wcsncmp(k->kun[i], token + (token[0] == L'-' ? 1 : 0), n);
                    //add reading, remove dash
                    if (good)
                    {
                        if (!k->kun)
                        {
                            k->kun = new wchar_t*[1];
                        }
                        else
                        {
                            wchar_t **wtemp = k->kun;
                            k->kun = new wchar_t*[k->kuncnt + 1];
                            memcpy(k->kun, wtemp, sizeof(void*)*k->kuncnt);
                            delete[] wtemp;
                        }

                        k->kun[k->kuncnt] = new wchar_t[n + 1];
                        wcsncpy(k->kun[k->kuncnt], token + (token[0] == L'-' ? 1 : 0), n);
                        k->kun[k->kuncnt][n] = 0;
                        k->kuncnt++;
                    }
                }
                token = wcstok(NULL, L" {}");
            }

            //skip name and radical readings of kanji. Those start with T(N) or kana.
            if (token[0] == L'T' && token[1] >= L'0' && token[1] <= L'9')
            {
                while (token[0] == L'T' && token[1] >= L'0' && token[1] <= L'9' || KANA(token[0]))
                {
                    token = wcstok(NULL, L"{}");
                }
                token[-1] = L'{';
            }

            //see what kind of word comes next (find the first bracket or end of line
            for (int i = wcslen(token) + 1; token[i]; i++)
            {
                if (token[i] == L'{')//good job, don't do anything
                            break;
                if (token[i] == L'}')
                {
                    token[wcslen(token)] = L' ';
                    token = wcstok(token, L"}");
                    break;
                }
            }
            token--;

            while (token && token[0] == L'{')
            {
                token++;
                if (!k->meaning)
                {
                    int tlen = wcslen(token) + 1;
                    k->meaning = new wchar_t[tlen];
                    wcsncpy(k->meaning, token, tlen);
                }
                else
                {
                    wchar_t *tmp = k->meaning;
                    int len = wcslen(tmp);
                    int tlen = wcslen(token) + 1;
                    k->meaning = new wchar_t[len + 2 + tlen];
                    memcpy(k->meaning, tmp, (len + 1)*sizeof(wchar_t));
                    wcscat(k->meaning, L", ");
                    wcscat(k->meaning, token);
                    delete[] tmp;
                }

                token = wcstok(NULL, L"}");
                if (token)
                    token++; //skip spaces;
            }
            k->index = kanjis->AddNew(k);
        }
    }
    catch (...)
    {
        ;
    }
    fclose(f);
}

void delete_kanji(int i)
{
    TKanji *k = kanjis->Kanjis[i];

    if (k->similar)
        delete[] k->similar;
    for (int j = 0; j < k->oncnt; j++)
        delete[] k->on[j];
    for (int j = 0; j < k->kuncnt; j++)
        delete[] k->kun[j];
    for (int j = 0; j < k->irregcnt; j++)
        delete[] k->irreg[j];
    if (k->irreg)
        delete[] k->irreg;

    delete[] k->on;
    delete[] k->kun;

    if (k->meaning)
        delete[] k->meaning;
    delete k;
}

void free_kanji()
{
    for (int i = kanjis->Count - 1; i >= 0; i--)
        delete_kanji(i);

    kanjis->Clear();
}

void import_irregulars(const wchar_t *name)
{
    FILE *f = _wfopen(name, L"rb");
    wchar_t line[1024];

    TKanji *k;
    int ix;
    wchar_t *c;
    int pos;

    if (!f)//error
                THROW(L"Error reading irregular readings file!");
    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            if (wcslen(line) < 2)
                continue;

            if ((ix = kanjis->IndexOf(line[0])) < 0)
                continue;
            k = kanjis->Kanjis[ix];
            if (k->irregcnt)
                continue;

            k->irregcnt = 1;
            pos = 1;
            while ((c = wcschr(line + pos, L'、')) != NULL)
            {
                k->irregcnt++;
                pos = c - line + 1;
            }
            k->irreg = new wchar_t*[k->irregcnt];

            c = wcstok(line + 1, L"、\x0000");
            k->irregcnt = 0;
            while (c)
            {
                k->irreg[k->irregcnt] = new wchar_t[wcslen(c) + 1];
                wcscpy(k->irreg[k->irregcnt], c);
                c = wcstok(NULL, L"、\x0000");
                k->irregcnt++;
            }

        }
    }
    catch (...)
    {
        ;
    }

    fclose(f);
}

void export_all_furigana(const wchar_t *name1, const wchar_t *name2)
{
    FILE *f = _wfopen(name1, L"rb");
    FILE *f2 = _wfopen(name2, L"wb");
    wchar_t line[1024];
    wchar_t *c;
    int len;
    wchar_t tmp = L',';
    wchar_t num[8];

    try
    {
        if (!f || !f2)//error
                    THROW(L"Error reading additional furigana file!");
        bool bigendian = false;
        while ((len = getlinew(line, 1024, f, bigendian)) != 0)
        {
            c = wcstok(line, L"|");
            fwrite(c, 2, wcslen(c), f2);
            fwrite(&tmp, 2, 1, f2);
            c = wcstok(NULL, L"|");
            fwrite(c, 2, wcslen(c), f2);
            fwrite(&tmp, 2, 1, f2);
            c += wcslen(c) + 1;
            len -= c - line;
            if ((len) % 4)
                THROW(L"Bad furigana file format!");
            while (len)
            {
                swprintf(num, L"%i,", c[0]);
                fwrite(num, 2, wcslen(num), f2);
                swprintf(num, L"%i,", c[1]);
                fwrite(num, 2, wcslen(num), f2);
                swprintf(num, L"%i,", c[2]);
                fwrite(num, 2, wcslen(num), f2);
                if (len > 4)
                    swprintf(num, L"%i,", c[3]);
                else
                    swprintf(num, L"%i", c[3]);
                fwrite(num, 2, wcslen(num), f2);
                c += 4;
                len -= 4;
            }
            num[0] = 0x000d;
            num[1] = 0x000a;
            fwrite(num, 2, 2, f2);
        }
    }
    catch (...)
    {
    }
    fclose(f);
    fclose(f2);
}

bool import_one_furigana(const wchar_t *name, const wchar_t *kanji, const wchar_t *kana, TFuriganaData *dat, int &datpos)
{
    FILE *f = _wfopen(name, L"rb");
    wchar_t line[1024];
    bool res = false;
    wchar_t *c;

    if (!f)//error
                THROW(L"Error reading additional furigana file!");
    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            c = wcstok(line, L",");
            if (wcscmp(c, kanji))
                continue;
            c = wcstok(NULL, L",\x0000");
            if (wcscmp(c, kana))
                continue;
            c = wcstok(NULL, L",\x0000");
            while (c)
            {
                dat[datpos].kanji = StrToInt(c);
                if ((c = wcstok(NULL, L",\x0000")) == NULL)
                    THROW(L"Bad furigana file format!");
                dat[datpos].kanjilen = StrToInt(c);
                if ((c = wcstok(NULL, L",\x0000")) == NULL)
                    THROW(L"Bad furigana file format!");
                dat[datpos].kana = StrToInt(c);
                if ((c = wcstok(NULL, L",\x0000")) == NULL)
                    THROW(L"Bad furigana file format!");
                dat[datpos].kanalen = StrToInt(c);
                datpos++;
                c = wcstok(NULL, L",\x0000");
                res = true;
            }
            break;
        }
    }
    catch (...)
    {
        res = false;
    }
    fclose(f);
    return res;
}

void import_confused(const wchar_t *name)
{
    FILE *f = _wfopen(name, L"rb");
    wchar_t line[1024];

    if (!f)//error
                THROW(L"Error reading similar kanji file!");

    bool found;
    int i, j, s1, s2;
    wchar_t *tmp;

    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            if (!wcslen(line))
                continue;

            TKanji *k;
            i = kanjis->IndexOf(line[0]);

            if (i < 0)
                continue;
            k = kanjis->Kanjis[i];
            found = k->similar;
            //count sim 1 and sim 2
            i = 1;
            s1 = 0;
            while (KANJI(line[i]))
            {
                for (j = 0; j < k->sim1; j++)
                    if (k->similar[j] == line[i])
                        break;
                if (j == k->sim1)
                {
                    line[s1] = line[i];
                    s1++;
                }
                i++;
            }
            s2 = 0;
            if (line[i] == L';')
            {
                i++;
                while (KANJI(line[i]))
                {
                    for (j = 0; j < k->sim2; j++)
                        if (k->similar[j + k->sim1] == line[i])
                            break;
                    if (j == k->sim2)
                    {
                        line[s1 + s2] = line[i];
                        s2++;
                    }
                    i++;
                }
            }
            if (!s1 && !s2)
                continue;
            tmp = k->similar;
            k->similar = new wchar_t[k->sim1 + s1 + k->sim2 + s2];
            if (found)
            {
                memcpy(k->similar, tmp, k->sim1*sizeof(wchar_t));
                memcpy(k->similar + k->sim1 + s1, tmp + k->sim1, k->sim2*sizeof(wchar_t));
                delete[] tmp;
            }

            for (i = 0; i < s1; i++)
                k->similar[k->sim1 + i] = line[i];
            for (i = s1; i < s1 + s2; i++)
                k->similar[k->sim1 + k->sim2 + i] = line[i];
            k->sim1 += s1;
            k->sim2 += s2;
        }

    }
    catch (...)
    {
        ;
    }

    fclose(f);
}

//-------------------

TWordCollection *kanasort_coll;
int kanastatic = 0;
int kana_alphasortfunc(int a, int b)
{
    TWord *wa = kanasort_coll->words->Items[a];
    TWord *wb = kanasort_coll->words->Items[b];
    UnicodeString sa = RomanizeEx(wa->kana);
    UnicodeString sb = RomanizeEx(wb->kana);

    int r = wcscmp(sa.c_str(), sb.c_str());
    if (!r)
    {
        int len = wcslen(wa->kana);
        for (int ix = 0; ix < len && !r; ++ix)
        {
            bool ka = KATAKANA(wa->kana[ix]) || DASH(wa->kana[ix]);
            bool kb = KATAKANA(wb->kana[ix]) || DASH(wb->kana[ix]);
            if (!ka && kb)
                r = -1;
            else if (ka && !kb)
                r = 1;
        }
    }

    if (!r)
        r = wcslen(wa->kanji) - wcslen(wb->kanji); //shorter first?
    if (!r)
        r = wcscmp(wa->kanji, wb->kanji);

    kanastatic++;
    if (!(kanastatic % 200))
        Application->ProcessMessages();

    return r;
}
int kana_japansortfunc(int a, int b)
{
    TWord *wa = kanasort_coll->words->Items[a];
    TWord *wb = kanasort_coll->words->Items[b];

    UnicodeString sa = Hiraganize(wa->kana);
    UnicodeString sb = Hiraganize(wb->kana);

    int r = wcscmp(sa.c_str(), sb.c_str());
    if (!r)
    {
        int len = wcslen(wa->kana);
        for (int ix = 0; ix < len && !r; ++ix)
        {
            bool ka = KATAKANA(wa->kana[ix]) || DASH(wa->kana[ix]);
            bool kb = KATAKANA(wb->kana[ix]) || DASH(wb->kana[ix]);
            if (!ka && kb)
                r = -1;
            else if (ka && !kb)
                r = 1;
        }
    }

    if (!r)
        r = wcslen(wa->kanji) - wcslen(wb->kanji); //shorter first?
    if (!r)
        r = wcscmp(wa->kanji, wb->kanji);

    kanastatic++;
    if (!(kanastatic % 200))
        Application->ProcessMessages();

    return r;
}

//Currently only called on import or load. If other needs surface, must use MODON and MODOFF with Modify.
void generate_alphabet(TWordCollection *c)
{
    TIntegerList *l1 = c->abcde, *l2 = c->aiueo;

    l1->Capacity = c->words->Count;
    l2->Capacity = c->words->Count;
    for (int i = 0; i < l1->Capacity; i++)
    {
        l1->Add(i);
        l2->Add(i);
    }

    kanasort_coll = c;
    l1->Sort(kana_alphasortfunc);
    l2->Sort(kana_japansortfunc);
}
void import_dictionary(const wchar_t *name, TWordCollection *coll)
{
    int n;
    wchar_t line[1024];
    TWordList *words = coll->words;
    TWord *w;

    FILE *f = _wfopen((ExtractFileDir(ParamStr(0)) + "\\" + name).c_str(), L"rb");

    if (!f)
        THROW(L"Error opening kanji file!");

    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            w = new TWord;
            memset(w, 0, sizeof(TWord));

            words->Add(w);
            w->kanji = new wchar_t[wcslen(line) + 1];
            wcscpy(w->kanji, line);

            getlinew(line, 1024, f, bigendian);
            n = wcscspn(line, L" ");
            w->kana = new wchar_t[n + 1];
            wcsncpy(w->kana, line, n);
            w->kana[n] = 0;

            w->romaji = new wchar_t[wcslen(line) - n];
            wcscpy(w->romaji, line + n + 1);
            getlinew(line, 1024, f, bigendian);
            w->frequency = min(10000, StrToInt(line));
            getlinew(line, 1024, f, bigendian);
            w->meaningcnt = StrToInt(line);
            w->data = new TMeaningData[w->meaningcnt];
            memset(w->data, 0, sizeof(TMeaningData)*w->meaningcnt);
            for (int i = 0; i < w->meaningcnt; i++)
            {
                getlinew(line, 1024, f, bigendian);
                w->data[i].meaning = new wchar_t[wcslen(line) + 1];
                wcscpy(w->data[i].meaning, line);
                getlinew(line, 1024, f, bigendian);
                wchar_t *token = wcstok(line, L";");
                while (token && wcslen(token) > 0)
                {
                    if (token[0] == L'p')
                    {
                        for (int j = 0; j < WT_COUNT; j++)
                        {
                            if (!wcscmp(token, wordtypes[j]))
                            {
                                w->data[i].types |= (1 << j);
                                break;
                            }
                        }
                    }
                    else if (token[0] == L'm')
                    {
                        for (int j = 0; j < WN_COUNT; j++)
                        {
                            if (!wcscmp(token, wordnotes[j]))
                            {
                                w->data[i].notes |= (1 << j);
                                break;
                            }
                        }
                    }
                    else if (token[0] == L'f')
                    {
                        for (int j = 0; j < WF_COUNT; j++)
                        {
                            if (!wcscmp(token, wordfields[j]))
                            {
                                w->data[i].fields |= (1 << j);
                                break;
                            }
                        }
                    };
                    token = wcstok(NULL, L";");
                }
            }
        }
        //store->FromWords(words);
    }
    catch (...)
    {
        ;
    }
    fclose(f);

    generate_alphabet(coll);
}

void import_tree(const wchar_t *name, const wchar_t *skipname, TTextSearchTree *tree)
{
    wchar_t line[1024];
    wchar_t *c;

    FILE *f = _wfopen((ExtractFileDir(ParamStr(0)) + "\\" + name).c_str(), L"rt");

    if (!f)//error
                THROW(L"Error opening the tree index file!");
    try
    {
        while (fgetws(line, 1024, f))
        {
            if (wcslen(line) < 2)
                continue;

            c = wcspbrk(line, L"\r\n");
            if (c)
                * c = 0;

            if (line[0] < L'0' || line[0] > L'9')
            { //it's a new node
                tree->AddNode(line);
                continue;
            }

            tree->AddLine(StrToInt(line));
        }
    }
    catch (...)
    {
    }

    tree->CountWords(); //walk through the whole tree and count every word

    fclose(f);

    f = _wfopen((ExtractFileDir(ParamStr(0)) + "\\" + skipname).c_str(), L"rt");
    while (fgetws(line, 1023, f))
    {
        c = wcspbrk(line, L"\r\n");
        if (c)
            * c = 0;
        tree->AddSkip(line);
    }
    fclose(f);
}

void filter_kanji(TWordCollection *coll)
{
    bool dropout[60]; //there is not this many in any database, so no worries
    int dropcnt;
    UnicodeString s, s2;
    TIntegerList *l = new TIntegerList;
    wchar_t **tmp;
    TWordList *words = coll->words;

    for (int k = 0; k < kanjis->Count; ++k)
    {
        memset(dropout, true, sizeof(bool)*60);
        dropcnt = kanjis->Kanjis[k]->kuncnt;
        for (int c = 0; c < kanjis->Kanjis[k]->kuncnt; c++)
        {
            if (!wcschr(kanjis->Kanjis[k]->kun[c], L'.'))
            {
                dropout[c] = false;
                dropcnt--;
                continue;
            }
            s = Romanize(kanjis->Kanjis[k]->kun[c]);
            l->Clear();
            //now look for the words that exactly match this reading
            coll->ktree->FindExactWord(s.c_str(), l);
            //now look for word with exactly the same reading and kanji
            for (int i = 0; i < l->Count && dropout[c]; i++)
            {
                if (words->Items[l->Items[i]]->kanji[0] != kanjis->Kanjis[k]->ch || wcscmp(words->Items[l->Items[i]]->kanji + 1, wcschr(kanjis->Kanjis[k]->kun[c], L'.') + 1))
                {
                    continue;
                }
                dropout[c] = false;
                dropcnt--;
            }
        }
        //now filter
        if (dropcnt)
        {
            tmp = kanjis->Kanjis[k]->kun;
            kanjis->Kanjis[k]->kun = new wchar_t*[kanjis->Kanjis[k]->kuncnt - dropcnt];
            dropcnt = kanjis->Kanjis[k]->kuncnt;
            kanjis->Kanjis[k]->kuncnt = 0; //count again
            for (int i = 0; i < dropcnt; i++)
            {
                if (dropout[i])
                    delete[] tmp[i];
                else
                {
                    kanjis->Kanjis[k]->kun[kanjis->Kanjis[k]->kuncnt++] = tmp[i]; //recycle
                }
            }
            delete[] tmp;
        }
    }
    delete l;
}

int kuncmpproc(const void *va, const void *vb)
{
    wchar_t *a = ((wchar_t**)va)[0];
    wchar_t *b = ((wchar_t**)vb)[0];
    int al = wcslen(a);
    int bl = wcslen(b);

    wchar_t *ax = wcschr(a, L'.');
    wchar_t *bx = wcschr(b, L'.');
    int r;

    if (ax && bx)
    {
        *ax = 0;
        *bx = 0;
        r = wcscmp(a, b);
        *ax = L'.';
        *bx = L'.';
        if (r)
            return r;
    }
    else if (ax || bx)
    {
        r = wcsncmp(a, b, MIN((ax ? ax - a : al), (bx ? bx - b : bl)));

        if (!r)
        {
            return (ax && (bl > ax - a) || bx && (al <= bx - b) ? -1 : 1);
        }
    }

    wchar_t *aa = a;
    wchar_t *bb = b;
    if (ax)
    {
        aa = new wchar_t[al];
        aa[al - 1] = 0;
        memcpy(aa, a, sizeof(wchar_t)*(ax - a));
        memcpy(aa + (ax - a), ax + 1, sizeof(wchar_t)*(al - 1 - (ax - a)));
    }
    if (bx)
    {
        bb = new wchar_t[bl];
        bb[bl - 1] = 0;
        memcpy(bb, b, sizeof(wchar_t)*(bx - b));
        memcpy(bb + (bx - b), bx + 1, sizeof(wchar_t)*(bl - 1 - (bx - b)));
    }

    r = wcscmp(aa, bb);

    if (aa != a)
        delete[] aa;
    if (bb != b)
        delete[] bb;

    return r;
}

void sort_kanji_readings()
{
    for (int k = 0; k < kanjis->Count; ++k)
        qsort((void*)kanjis->Kanjis[k]->kun, kanjis->Kanjis[k]->kuncnt, sizeof(wchar_t*), &kuncmpproc);
}

bool save_dictionary_base(const UnicodeString &name)
{
    bool error = false;
    error_code = 0;
    FILE *f = _wfopen(name.c_str(), L"wb");
    if (!f)//error
        return false;

    TKanji *k;
    byte b;

    try
    {
        // Write some nonsense so we recognize ourselves later.
        char verstr[9];
        sprintf(verstr, "zdict%03i", DICTIONARY_BASE_VERSION);
        fwrite(verstr, sizeof(char), 8, f);
        sprintf(verstr, "zwrds%03i", DICTIONARY_VERSION);
        fwrite(verstr, sizeof(char), 8, f);

        GetSystemTime(&dictionary_date);
        fwrite(&dictionary_date, sizeof(SYSTEMTIME), 1, f);
        fwrite(&dictionary_build_version, sizeof(double), 1, f);

        store->SaveToFile(f);

        // Write kanji.
        for (int i = 0; i < kanjis->Count; i++)
        {
            k = kanjis->Kanjis[i];
            fwrite(&k->ch, sizeof(wchar_t), 1, f);
            fwrite(&k->_jis, sizeof(word), 1, f);
            fwrite(&k->_uni, sizeof(word), 1, f);
            fwrite(&k->radical, sizeof(byte), 1, f);
            fwrite(&k->jouyou, sizeof(byte), 1, f);
            fwrite(&k->JLPT, sizeof(byte), 1, f);
            fwrite(&k->NJLPT, sizeof(byte), 1, f); // Added 2011.10.30 for ver9.
            fwrite(&k->strokes, sizeof(byte), 1, f);
            fwrite(&k->frequency, sizeof(word), 1, f);
            fwrite(&k->knk, sizeof(word), 1, f);
            fwrite(&k->SKIP1, sizeof(byte), 1, f);
            fwrite(&k->SKIP2, sizeof(byte), 1, f);
            fwrite(&k->SKIP3, sizeof(byte), 1, f);

            fwrite(&k->nelson, sizeof(word), 1, f);
            fwrite(&k->halpern, sizeof(word), 1, f);
            fwrite(&k->heisig, sizeof(word), 1, f);
            fwrite(&k->gakken, sizeof(word), 1, f);
            fwrite(&k->henshall, sizeof(word), 1, f);
            fwrite(k->SnH, sizeof(char), 8, f);
            fwrite(k->c4, sizeof(char), 5, f);
            fwrite(&k->c4b, sizeof(char), 1, f);
            fwrite(&k->word_freq, sizeof(unsigned int), 1, f);

            fwrite(&k->oncnt, sizeof(byte), 1, f);
            for (int j = 0; j < k->oncnt; j++)
            {
                b = wcslen(k->on[j]);
                fwrite(&b, sizeof(byte), 1, f);
                fwrite(k->on[j], sizeof(wchar_t), b, f);
            }
            fwrite(&k->kuncnt, sizeof(byte), 1, f);
            for (int j = 0; j < k->kuncnt; j++)
            {
                b = wcslen(k->kun[j]);
                fwrite(&b, sizeof(byte), 1, f);
                fwrite(k->kun[j], sizeof(wchar_t), b, f);
            }

            write(k->meaning, f, sltWord);
        }

        // Write radical parts.
        for (int i = 0; i < 252; i++)
        {
            fwrite(partlist[i], sizeof(word), 1, f);
            fwrite(partlist[i] + 1, sizeof(word), partlist[i][0], f);
        }
        radlist->SaveToFile(f);

        /*- Unnecessary since 2012.05.01
         // since 2011.10.30 for v0.7
        if (!JLPT)
            JLPT = new TWordNJLPTDataTree;
        JLPT->SaveToFile(f);
        */

        // New format after 2012.04.15 for v0.73
        // File size for error checking.
        int s = ftell(f) +sizeof(int);
        fwrite(&s, sizeof(int), 1, f);

    }
    catch (...)
    {
        error = true;
    }
    fclose(f);

    if (!error)
        error_code = 0;

    return !error;
}

bool load_dictionary_base(const UnicodeString &name)
{
    FILE *f = _wfopen(name.c_str(), L"rb");
    if (!f)
        return false;

    TKanji *k;
    word cc;
    byte b;

    int version;
    int dversion;

    try
    {
        char tmp[9];
        tmp[8] = 0;
        fread(tmp, sizeof(char), 8, f);
        if (strncmp("zdict", tmp, 5))
            THROW(L"Bad base dictionary format! Quitting...");
        version = strtol(tmp + 5, 0, 10);

        fread(tmp, sizeof(char), 8, f);
        if (strncmp("zwrds", tmp, 5))
            THROW(L"Bad base dictionary format! Quitting...");
        dversion = strtol(tmp + 5, 0, 10);

        /* Kept for later when minimal base version might be specified.
        if (version != DICTIONARY_BASE_VERSION)
        {
            if (version < XXXX)
            {
                MsgBox(L"This version of zkanji is not compatible with dictionaries that were compiled before zkanji v0.73!\n\nPlease install the full version.", L"Error",MB_OK);
                THROW(L"Bad version!";
            }
        }
        */

        fread(&dictionary_date, sizeof(SYSTEMTIME), 1, f);
        fread(&dictionary_build_version, sizeof(double), 1, f);

        store->LoadFromFile(f);

        bool kreload = kanjis->Count != 0;

        // Read kanji.
        for (int i = 0; i < kanji_count; i++)
        {
            if (!kreload)
            {
                k = new TKanji;
                memset(k, 0, sizeof(TKanji));
                k->element = -1;
                fread(&k->ch, sizeof(wchar_t), 1, f);
            }
            else
            {
                wchar_t ch;
                fread(&ch, sizeof(wchar_t), 1, f);
                k = kanjis->Kanjis[kanjis->IndexOf(ch)];
            }
            fread(&k->_jis, sizeof(word), 1, f);
            fread(&k->_uni, sizeof(word), 1, f);
            fread(&k->radical, sizeof(byte), 1, f);
            fread(&k->jouyou, sizeof(byte), 1, f);
            fread(&k->JLPT, sizeof(byte), 1, f);
            fread(&k->NJLPT, sizeof(byte), 1, f);
            fread(&k->strokes, sizeof(byte), 1, f);
            fread(&k->frequency, sizeof(word), 1, f);
            fread(&k->knk, sizeof(word), 1, f);
            fread(&k->SKIP1, sizeof(byte), 1, f);
            fread(&k->SKIP2, sizeof(byte), 1, f);
            fread(&k->SKIP3, sizeof(byte), 1, f);

            fread(&k->nelson, sizeof(word), 1, f);
            fread(&k->halpern, sizeof(word), 1, f);
            fread(&k->heisig, sizeof(word), 1, f);
            fread(&k->gakken, sizeof(word), 1, f);
            fread(&k->henshall, sizeof(word), 1, f);
            fread(k->SnH, sizeof(char), 8, f);
            fread(k->c4, sizeof(char), 5, f);
            fread(&k->c4b, sizeof(char), 1, f);
            fread(&k->word_freq, sizeof(unsigned int), 1, f);

            if (!kreload)
            {
                fread(&k->oncnt, sizeof(byte), 1, f);
                k->on = new wchar_t*[k->oncnt];
                for (int j = 0; j < k->oncnt; j++)
                {
                    fread(&b, sizeof(byte), 1, f);
                    k->on[j] = new wchar_t[b + 1];
                    fread(k->on[j], sizeof(wchar_t), b, f);
                    k->on[j][b] = 0;
                }
                fread(&k->kuncnt, sizeof(byte), 1, f);
                k->kun = new wchar_t*[k->kuncnt];
                for (int j = 0; j < k->kuncnt; j++)
                {
                    fread(&b, sizeof(byte), 1, f);
                    k->kun[j] = new wchar_t[b + 1];
                    fread(k->kun[j], sizeof(wchar_t), b, f);
                    k->kun[j][b] = 0;
                }
            }
            else
            {
                byte ncnt;
                fread(&ncnt, sizeof(byte), 1, f);
                for (int j = 0; j < ncnt; j++)
                {
                    fread(&b, sizeof(byte), 1, f);
                    fseek(f, sizeof(wchar_t) * b, SEEK_CUR);
                }
                fread(&ncnt, sizeof(byte), 1, f);
                for (int j = 0; j < ncnt; j++)
                {
                    fread(&b, sizeof(byte), 1, f);
                    fseek(f, sizeof(wchar_t) * b, SEEK_CUR);
                }

                delete[] k->meaning;
            }
            read(k->meaning, f, sltWordAddNULL);

            if (!kreload)
                k->index = kanjis->AddNew(k);
        }

        if (!kreload)
        {
            generate_validkanji();
            generate_validunicode();

            // Read radical parts.
            for (int i = 0; i < 252; i++)
            {
                fread(&cc, sizeof(word), 1, f);
                partlist[i] = new word[cc + 1];
                partlist[i][0] = cc;
                fread(partlist[i] + 1, sizeof(word), cc, f);
            }
            radlist->LoadFromFile(f, dversion);
        }
        else
            fseek(f, -(int)sizeof(int), SEEK_END);

        int fs = 0;
        fread(&fs, sizeof(int), 1, f);
        if (fs != ftell(f))
            THROW(L"Corrupt file. No size indicator at end.");
    }
    catch (...)
    {
        version = -1;
    }
    fclose(f);

    return version != -1;
}

bool save_dictionary(const UnicodeString &name, TWordCollection *coll, bool main)
{
    bool error = false;
    error_code = 0;
    FILE *f = _wfopen(name.c_str(), L"wb");
    if (!f)
    { //error
        error_code = 1;
        return false;
    }

    TWord *w;
    word cc, wcnt;
    byte b;
    int cnt;

    error_code = 2;
    try
    {
        // Write some nonsense so we recognize ourselves later.
        char verstr[9];
        sprintf(verstr, "zwrds%03i", DICTIONARY_VERSION);
        fwrite(verstr, sizeof(char), 8, f);

        // Save date of base dictionary to be able to check whether we have an updated file installed.
        fwrite(&dictionary_date, sizeof(SYSTEMTIME), 1, f); //added 2012.04.15 for v0.73
        GetSystemTime(&coll->lastwritetime);
        fwrite(&coll->lastwritetime, sizeof(SYSTEMTIME), 1, f);

        // Writing general information:
        write(coll->info, f, sltInt); // Added 2013.05.12 for v0.73

        // Writing trees.
        error_code = 3;
        coll->rtree->SaveToFile(f);
        error_code = 4;
        coll->ktree->SaveToFile(f);
        error_code = 5;
        coll->btree->SaveToFile(f);

        error_code = 6;

        error_code = 7;

        // RLE save kanji meanings.
        TKanjiData *kd = coll->kanjidat;
        cc = 0;
        cnt = 0;
        while (cnt < kanjis->Count)
        {
            if (!kd[cnt].meaning)
            {
                cc++;
                cnt++;
            }
            else
            {
                fwrite(&cc, sizeof(word), 1, f);
                cc = cnt;
                while (cnt < kanjis->Count && kd[cnt].meaning)
                    cnt++;
                wcnt = cnt - cc;
                fwrite(&wcnt, sizeof(word), 1, f);

                for (; cc < cnt; cc++)
                {
                    write(kd[cc].meaning, f, sltWord);
                }
                cc = 0;
            }
        }
        if (cc)
            fwrite(&cc, sizeof(word), 1, f);
        error_code = 9;

        cc = 0;
        cnt = 0;
        while (cnt < kanjis->Count)
        {
            if (kd[cnt].wordscnt)
            {
                fwrite(&cc, sizeof(word), 1, f);
                wcnt = cnt; // Save position.
                while (cnt < kanjis->Count && kd[cnt].wordscnt)
                    cnt++;
                wcnt = cnt - wcnt;
                fwrite(&wcnt, sizeof(word), 1, f);

                for (cc = cnt - wcnt; cc < cnt; cc++)
                {
                    fwrite(&kd[cc].wordscnt, sizeof(int), 1, f);
                    fwrite(kd[cc].words, sizeof(int), kd[cc].wordscnt, f);
                }
                cc = 0;
            }
            else
            {
                cc++;
                cnt++;
            }
        }
        if (cc)
            fwrite(&cc, sizeof(word), 1, f);
        error_code = 10;

        // Write words.
        cnt = coll->words->Count;
        fwrite(&cnt, sizeof(int), 1, f);
        error_code = 13;
        for (int i = 0; i < coll->words->Count; i++)
        {
            w = coll->words->Items[i];
            fwrite(&w->frequency, sizeof(word), 1, f); // Changed nov 2010, v0.57.
            b = wcslen(w->kanji);
            fwrite(&b, sizeof(byte), 1, f);
            fwrite(w->kanji, sizeof(wchar_t), b, f);
            b = wcslen(w->kana);
            fwrite(&b, sizeof(byte), 1, f);
            fwrite(w->kana, sizeof(wchar_t), b, f);
            write(w->romaji, f, sltByte);

            fwrite(&w->meaningcnt, sizeof(byte), 1, f);
            for (int j = 0; j < w->meaningcnt; j++)
            {
                write(w->data[j].meaning, f, sltWord);

                fwrite(&w->data[j].types, sizeof(int), 1, f);
                fwrite(&w->data[j].notes, sizeof(int), 1, f);
                fwrite(&w->data[j].fields, sizeof(byte), 1, f); // Changed nov 2010, v0.57
                fwrite(&w->data[j].nametag, sizeof(word), 1, f);
            }
        }
        error_code = 14;

        // Added Jan 2011, v0.57
        for (int i = 0; i < validcode_count; ++i)
        {
            fwrite(&coll->validdat[i].wordscnt, sizeof(int), 1, f);
            if (coll->validdat[i].wordscnt)
            {
                fwrite(coll->validdat[i].words, sizeof(int), coll->validdat[i].wordscnt, f);
            }
        }

        error_code = 141;

        // Write word alphabet.
        for (int i = 0; i < coll->words->Count; i++)
        {
            cnt = coll->abcde->Items[i];
            fwrite(&cnt, sizeof(int), 1, f);
            cnt = coll->aiueo->Items[i];
            fwrite(&cnt, sizeof(int), 1, f);
        }

        error_code = 15;

        // Save originals that were changed, but only for the base English dictionary.
        if (main)
            dictionary->Originals->SaveToFile(f);

        // Added 2012.04.15 for v0.73.
        // Write full file size for error checking.
        int s = ftell(f) + sizeof(int);
        fwrite(&s, sizeof(int), 1, f);

    }
    catch (...)
    {
        error = true;
    }
    fclose(f);

    if (!error)
        error_code = 0;

    return !error;
}

bool load_dictionary(const UnicodeString &name, TWordCollection *coll, bool main, bool skiporiginals)
{
    FILE *f = _wfopen(name.c_str(), L"rb");
    if (!f) // Error
        return false;

    TWord *w;
    word cc, wcnt;
    byte b;

    int cnt;

    int version;

    try
    {
        char tmp[9];
        fread(tmp, sizeof(char), 8, f);
        if (strncmp("zwrds", tmp, 5))
            THROW(L"Bad dictionary format! Quitting...");
        tmp[8] = 0;

        version = strtol(tmp + 5, 0, 10);

        if (version >= 10)
        {
            // Added 2012.04.15 for v0.73.
            fseek(f, sizeof(SYSTEMTIME), SEEK_CUR); // Main dictionary write date. It is returned in get_dictionary_date but not used here.
            fread(&coll->lastwritetime, sizeof(SYSTEMTIME), 1, f);

            read(coll->info, f, sltInt);
        }

        // Reading trees.
        if (version <= 4)
        {
            // Added Nov 2010, for v0.57.
            bool dummy;
            fread(&dummy, sizeof(bool), 1, f);
        }
        coll->rtree->LoadFromFile(f);

        if (version <= 4)
        {
            // Added Nov 2010, for v0.57.
            bool dummy;
            fread(&dummy, sizeof(bool), 1, f);
        }
        coll->ktree->LoadFromFile(f);

        if (version <= 4)
        {
            // Added Nov 2010, for v0.57.
            bool dummy;
            fread(&dummy, sizeof(bool), 1, f);
        }
        coll->btree->LoadFromFile(f);

        // Read kanji
        if (version <= 5)
        {
            // Format change 2011 Jan 29, v0.57
            fread(&cnt, sizeof(int), 1, f);
            kanjis->Capacity = cnt;
        }

        // Open "RLE-compressed" data.
        TKanjiData *kd = coll->kanjidat;
        cc = 0;
        cnt = 0;

        int kcnt = version <= 5 ? 6353 : 6355;

        while (cnt < kcnt)
        {
            fread(&cc, sizeof(word), 1, f);
            cnt += cc;
            if (cnt == kcnt)
                break;
            fread(&cc, sizeof(word), 1, f);
            cc = cnt + cc;
            while (cnt < cc)
            {
                if (version >= 8)
                {
                    read(kd[cnt].meaning, f, sltWordAddNULL);
                }
                else
                {
                    fread(&wcnt, sizeof(word), 1, f);
                    char *tmp = new char[wcnt + 1];
                    if (wcnt)
                        fread(tmp, sizeof(char), wcnt, f);
                    tmp[wcnt] = 0;
                    kd[cnt].meaning = new wchar_t[wcnt + 1];
                    MultiByteToWideChar(CP_ACP, 0, tmp, wcnt + 1, kd[cnt].meaning, wcnt + 1);
                    delete[] tmp;
                }
                cnt++;
            }
        }

        cc = 0;
        cnt = 0;
        while (cnt < kcnt)
        {
            fread(&cc, sizeof(word), 1, f);
            cnt += cc;
            // Skip zeroes.
            if (cnt == kcnt)
                break;
            fread(&cc, sizeof(word), 1, f);
            cc += cnt;
            while (cnt < cc)
            {

                if (version >= 4) // Added 10.01.27 v0.533.
                    fread(&kd[cnt].wordscnt, sizeof(int), 1, f);
                else
                {
                    word tmp;
                    fread(&tmp, sizeof(word), 1, f);
                    kd[cnt].wordscnt = tmp;
                }

                kd[cnt].words = new int[kd[cnt].wordscnt];
                fread(kd[cnt].words, sizeof(int), kd[cnt].wordscnt, f);
                cnt++;
            }
        }

        // 碵 4739 鬥 6168 - included after 2011 Jan 29, v0.57
        if (version <= 5)
        {
            memmove(kd + 4740, kd + 4739, sizeof(TKanjiData)*(6353 - 4740));
            memmove(kd + 6169, kd + 6168, sizeof(TKanjiData)*(6354 - 6169));
            memset(kd + 4739, 0, sizeof(TKanjiData));
            memset(kd + 6168, 0, sizeof(TKanjiData));
        }

        // Read words.
        fread(&cnt, sizeof(int), 1, f);
        coll->words->Capacity = cnt;

        for (int i = 0; i < cnt; i++)
        {
            w = new TWord;
            coll->words->Add(w);
            memset(w, 0, sizeof(TWord));

            if (version >= 5)
            {
                // Changed Nov 2010, for v0.57
                fread(&w->frequency, sizeof(word), 1, f);
            }
            else
            {
                int freqtmp;
                fread(&freqtmp, sizeof(int), 1, f);
                w->frequency = min(freqtmp, 10000);
            }

            fread(&b, sizeof(byte), 1, f);
            w->kanji = new wchar_t[b + 1];
            fread(w->kanji, sizeof(wchar_t), b, f);
            w->kanji[b] = 0;

            fread(&b, sizeof(byte), 1, f);
            w->kana = new wchar_t[b + 1];
            fread(w->kana, sizeof(wchar_t), b, f);
            w->kana[b] = 0;

            if (version >= 8)
            {
                // Added 2011.04.04
                read(w->romaji, f, sltByteAddNULL);
            }
            else
            {
                char *tmp = NULL;
                fread(&b, sizeof(byte), 1, f);
                tmp = new char[b + 1];
                if (b)
                    fread(tmp, sizeof(char), b, f);
                tmp[b] = 0;
                w->romaji = new wchar_t[b + 1];
                MultiByteToWideChar(CP_ACP, 0, tmp, b + 1, w->romaji, b + 1);
                delete[] tmp;
            }

            fread(&w->meaningcnt, sizeof(byte), 1, f);
            w->data = new TMeaningData[w->meaningcnt];
            for (int j = 0; j < w->meaningcnt; j++)
            {
                if (version >= 8)
                    read(w->data[j].meaning, f, sltWordAddNULL);
                else
                {
                    fread(&cc, sizeof(word), 1, f);
                    char *tmp = new char[cc + 1];
                    if (cc)
                        fread(tmp, sizeof(byte), cc, f);
                    tmp[cc] = 0;
                    w->data[j].meaning = new wchar_t[cc + 1];
                    MultiByteToWideChar(CP_ACP, 0, tmp, cc + 1, w->data[j].meaning, cc + 1);
                    delete[] tmp;
                }

                fread(&w->data[j].types, sizeof(int), 1, f);
                fread(&w->data[j].notes, sizeof(int), 1, f);

                if (version >= 5)
                {
                    // Changed nov 2010, for v0.57.
                    fread(&w->data[j].fields, sizeof(byte), 1, f);
                }
                else
                {
                    int ftmp;
                    fread(&ftmp, sizeof(int), 1, f);
                    w->data[j].fields = ftmp & 0xFF;
                }
                if (version >= 4)
                    fread(&w->data[j].nametag, sizeof(word), 1, f);
                else
                    w->data[j].nametag = 0;
            }
            w->stats = NULL;
        }

        // Added Jan 2011, for v0.57.
        if (version >= 6)
        {
            for (int i = 0; i < validcode_count; ++i)
            {
                fread(&coll->validdat[i].wordscnt, sizeof(int), 1, f);
                if (coll->validdat[i].wordscnt)
                {
                    coll->validdat[i].words = new int[coll->validdat[i].wordscnt];
                    fread(coll->validdat[i].words, sizeof(int), coll->validdat[i].wordscnt, f);
                }
            }
        }
        else
        {
            // Check for non Japanese wide characters just in case.
            for (int i = 0; i < coll->words->Count; ++i)
            {
                w = coll->words->Items[i];
                for (int j = wcslen(w->kanji) - 1; j >= 0; --j)
                {
                    if (VALIDCODE(w->kanji[j]))
                        add_valid_word_line(coll->validdat[validindex(w->kanji[j])], i);
                }
            }
        }

        coll->abcde->Capacity = coll->words->Count;
        coll->aiueo->Capacity = coll->words->Count;
        // Read word alphabet.
        for (int i = 0; i < coll->words->Count; i++)
        {
            fread(&cnt, sizeof(int), 1, f);
            coll->abcde->Add(cnt);
            fread(&cnt, sizeof(int), 1, f);
            coll->aiueo->Add(cnt);
        }

        // Added Jan 2011, for v0.57.
        if (version <= 5)
        {
            // Regenerate alphabet lists.
            coll->abcde->Clear();
            coll->aiueo->Clear();
            generate_alphabet(coll);
        }

        if (version == 3)
        {
            // Added 10.02.01 v0.533
            profile->LoadFromFile(f, version);
            profile->Clear();
            save_profile(paths.datapath + "zkanji.zsp");
            coll->StartupModify(false);
        }

        if (version == 6)
        {
            // Added 2011 Feb 12 for fix release v0.571.
            ShowMessageForm("Please wait while fixing user dictionary...");
            // Check whether there are words in the dictionary with kanji above index of 4739.
            TIntegerList *wlist = new TIntegerList;
            try
            {
                for (int ix = 4739; ix < 6355; ++ix)
                {
                    coll->kanjidat[ix].wordscnt = 0;
                    delete[] coll->kanjidat[ix].words;
                    coll->kanjidat[ix].words = NULL;
                }
                TGapList<bool> kadded(false);

                for (int ix = 0; ix < coll->words->Count; ++ix)
                {
                    wchar_t *k = coll->words->Items[ix]->kanji;
                    int len = wcslen(k);
                    for (int j = 0; j < len; ++j)
                    {
                        if (KANJI(k[j]))
                        {
                            int kix = kanjis->IndexOf(k[j]);
                            if (kix >= 4739 && !kadded.Items[kix])
                            {
                                if (!wlist->Count || wlist->Items[wlist->Count - 1] != ix)
                                    wlist->Add(ix);
                                kadded.Items[kix] = true;
                                coll->kanjidat[kix].wordscnt++;
                            }
                        }
                    }
                    kadded.Clear();
                }

                for (int ix = 0; ix < wlist->Count; ++ix)
                {
                    wchar_t *k = coll->words->Items[wlist->Items[ix]]->kanji;
                    int len = wcslen(k);
                    for (int j = 0; j < len; ++j)
                    {
                        if (KANJI(k[j]))
                        {
                            int kix = kanjis->IndexOf(k[j]);
                            if (kix >= 4739 && !kadded.Items[kix])
                            {
                                kadded.Items[kix] = true;
                                if (!coll->kanjidat[kix].words)
                                {
                                    if (!coll->kanjidat[kix].wordscnt)
                                        THROW(L"This kanji should have no words! Error!");
                                    coll->kanjidat[kix].words = new int[coll->kanjidat[kix].wordscnt];
                                    coll->kanjidat[kix].wordscnt = 0;
                                }
                                coll->kanjidat[kix].words[coll->kanjidat[kix].wordscnt] = wlist->Items[ix];
                                coll->kanjidat[kix].wordscnt++;
                            }
                        }
                    }
                    kadded.Clear();
                }
            }
            catch (...)
            {
                HideMessageForm();

                MsgBox("Error occurred while loading/updating dictionary. Contact the author of the program.", "Error", MB_OK);
                Application->Terminate();
            }
            delete wlist;

            HideMessageForm();
        }

        if (version >= 10)
        {
            // Added 2012.04.15 for v0.73, originals added 2012.05.03
            if (main)
            {
                if (!skiporiginals)
                    dictionary->Originals->LoadFromFile(f);
                else if (!dictionary->Originals->SkipInFile(f))
                {
                    MsgBox(L"Error occurred while loading dictionary. Please make sure the base dictionary is valid.", L"Error", MB_OK);
                    THROW(L"Corrupt file. Originals list must be empty.");
                }
            }

            int fs = 0;
            fread(&fs, sizeof(int), 1, f);
            if (fs != ftell(f))
                THROW(L"Corrupt file. No size indicator at end.");
        }

    }
    catch (...)
    {
        version = -1;
    }
    fclose(f);

    return version != -1;
}

/* NOT TESTED. ONLY UNCOMMENT WHEN NEEDED AND CHECK WHETHER IT WORKS CORRECTLY.
bool check_file_integrity(const UnicodeString &name)
{
    FILE *f = _wfopen(name.c_str(), L"rb");
    if (!f)
        return false;

    bool result = false;
    try
    {
        int pos = 0;
        if (fseek(f, SEEK_END, -(int)sizeof(int)) == 0 && fread(&pos, sizeof(int), 1, f) == 1)
            result = pos == ftell(f);
    }
    catch (...)
    {
        ;
    }

    fclose(f);

    return result;
}
*/

bool get_dictionary_date(const UnicodeString &name, SYSTEMTIME &date, double *buildver)
{
    FILE *f = _wfopen(name.c_str(), L"rb");
    if (!f)
        return false;

    bool error = false;
    int version;
    int filesize;
    memset(&date, 0, sizeof(SYSTEMTIME));

    try
    {
        char tmp[9];
        fread(tmp, sizeof(char), 8, f);
        if (strncmp("zdict", tmp, 5) && strncmp("zwrds", tmp, 5))
            THROW(L"Bad dictionary format! Quitting...");

        bool basedict = !strncmp("zdict", tmp, 5);

        if (basedict)
        {
            fread(tmp, sizeof(char), 8, f);
            if (strncmp("zwrds", tmp, 5))
                THROW(L"Bad dictionary format! Quitting...");
        }
        else
        {
            tmp[8] = 0;
            version = strtol(tmp + 5, 0, 10);

            if (version <= 9)
                THROW(L"Data doesn't have validation information. Must generate new dictionary.");
        }

        fread(&date, sizeof(SYSTEMTIME), 1, f);

        if (basedict && buildver != NULL)
        {
            fread(buildver, sizeof(double), 1, f);
        }

        fseek(f, -(int)sizeof(int), SEEK_END);
        fread(&filesize, sizeof(int), 1, f);
        if (filesize != ftell(f))
            THROW(L"Corrupt data!");
/*
        char verstr[9];
        sprintf(verstr, "zdict%03i", DICTIONARY_BASE_VERSION);
        fwrite(verstr, sizeof(char), 8, f);
        sprintf(verstr, "zwrds%03i", DICTIONARY_VERSION);
        fwrite(verstr, sizeof(char), 8, f);

        GetSystemTime(&dictionary_date);
        fwrite(&dictionary_date, sizeof(SYSTEMTIME), 1, f);
        fwrite(&dictionary_build_version, sizeof(double), 1, f);
*/
    }
    catch (...)
    {
        error = true;
    }
    fclose(f);
    return !error;
}

bool save_profile(const UnicodeString &name)
{
    FILE *f = _wfopen(name.c_str(), L"wb");
    if (!f)
        return false;

    bool error = false;

    try
    {
        char tmp[9];
        sprintf(tmp, "zltsp%03i", STUDYPROFILE_VERSION);
        fwrite(tmp, sizeof(char), 8, f);
        profile->SaveToFile(f);
    }
    catch (const wchar_t *ch)
    {
        MsgBox(ch, L"Error", MB_OK);
        error = true;
    }
    catch (...)
    {
        error = true;
    }
    fclose(f);

    return !error;
}

bool load_profile(const UnicodeString &name)
{
    profile->Clear();
    FILE *f = _wfopen(name.c_str(), L"rb");
    if (!f)
        return false;

    bool error = false;
    unsigned int version;

    try
    {
        char tmp[9];
        tmp[8] = 0;
        fread(tmp, sizeof(char), 8, f);
        if (strncmp("zltsp", tmp, 5))
            THROW(L"Bad dictionary format! Quitting...");
        version = strtol(tmp + 5, 0, 10);
        if (version > STUDYPROFILE_VERSION)
            THROW(L"The long-term study profile data is corrupt!");
        profile->LoadFromFile(f, version);
    }
    catch (const wchar_t *ch)
    {
        MsgBox(ch, "Error", MB_OK);
        error = true;
    }
    catch (...)
    {
        error = true;
    }
    fclose(f);

    return !error;
}

void import_radicals(const wchar_t *name)
{
    FILE *f = _wfopen((ExtractFileDir(ParamStr(0)) + L"\\" + name).c_str(), L"rb");
    wchar_t line[1024];
    if (!f)
        THROW(L"Error reading radicals file!");

    int pos = -1;
    int num = 0;
    // First allocate the array.
    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            if (line[0] == L'#' || !wcslen(line)) // Skip comment and empty lines.
                continue;
            if (line[0] == L'$')
            {
                if (pos >= 0)
                    partlist[pos] = new word[num + 1];
                pos++;
                num = 0;
                continue;
            }
            num += wcslen(line);
            for (int i = 0; i < (int)wcslen(line); i++)
                if (kanjis->IndexOf(line[i]) < 0)
                    num--; // Count only common kanji.
        }
        partlist[pos] = new word[num + 1]; // For the last one too.
    }
    catch (...)
    {
        ;
    }
    fclose(f);

    pos = -1;
    int n;
    FILE *f2 = _wfopen((ExtractFileDir(ParamStr(0)) + "\\" + name).c_str(), L"rb");
    if (!f2)
        THROW(L"Error reading radicals file!");
    byte *wdb = new byte[kanjis->Count];
    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f2, bigendian))
        {
            if (line[0] == L'#' || !wcslen(line)) // Skip comment and empty lines.
                        continue;
            if (line[0] == L'$')
            {
                memset(wdb, 0, sizeof(byte)*kanjis->Count);
                pos++;
                partlist[pos][0] = 0;
                continue;
            }
            for (int i = 0; i < (int)wcslen(line); i++)
            {
                n = kanjis->IndexOf(line[i]);
                if (n >= 0 && !wdb[n])
                {
                    wdb[n]++;
                    partlist[pos][0]++;
                    partlist[pos][partlist[pos][0]] = n;
                }
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete[] wdb;
    fclose(f2);
}

void import_zradicals(const wchar_t *name)
{
    FILE *f = _wfopen((ExtractFileDir(ParamStr(0)) + "\\" + name).c_str(), L"rb");
    wchar_t line[1024];
    word length = 0;
    word cnt = 0;

    if (!f)
        THROW(L"Error reading radicals file!");

    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            if (line[0] == L'#' || !wcslen(line)) // Skip comment and empty lines.
                continue;
            if (line[0] == L'$')
            {
                if (cnt)
                {
                    radlist->Items[cnt - 1]->list = new wchar_t[length + 1];
                    memset(radlist->Items[cnt - 1]->list, 0, (length + 1)*sizeof(wchar_t));
                    length = 0;
                }
                radlist->ImportAdd(line + 1);
                cnt++;
                continue;
            }
            length += wcslen(line);
            for (int i = 0; i < (int)wcslen(line); i++)
                if (kanjis->IndexOf(line[i]) < 0)
                    length--; // Count only common kanji.
        }
    }
    catch (...)
    {
        ;
    }
    fclose(f);
    if (cnt)
    {
        radlist->Items[cnt - 1]->list = new wchar_t[length + 1];
        memset(radlist->Items[cnt - 1]->list, 0, (length + 1)*sizeof(wchar_t));
    }
    length = 0;

    int pos = -1;
    FILE *f2 = _wfopen((ExtractFileDir(ParamStr(0)) + "\\" + name).c_str(), L"rb");
    if (!f2)
        THROW(L"Error reading radicals file!");
    byte *wdb = new byte[kanjis->Count];
    try
    {
        bool bigendian = false;
        while (getlinew(line, 1024, f2, bigendian))
        {
            if (line[0] == L'#' || !wcslen(line)) // Skip comment and empty lines.
                        continue;
            if (line[0] == L'$')
            {
                memset(wdb, 0, sizeof(byte)*kanjis->Count);
                pos++;
                continue;
            }

            for (int i = 0; i < (int)wcslen(line); i++)
            {
                short int n = kanjis->IndexOf(line[i]);
                if (n >= 0 && !wdb[n])
                {
                    wdb[n]++;

                    // This is dangerous, I hope it works.
                    *(wcschr(radlist->Items[pos]->list, 0)) = line[i];
                }
            }
        }
    }
    catch (...)
    {
        ;
    }
    delete[] wdb;
    fclose(f2);
}

int wordextrasavesort(int a, int b)
{
    return a - b;
}

bool save_groups(const UnicodeString &name, TWordCollection *collection)
{
    bool error = false;
    FILE *f;
    word k;
    int n;
    TIntegerList *l = 0;
    TWord *w;

    error_code = 0;
    //start with wordgroups
    f = _wfopen(name.c_str(), L"wb");
    if (!f)
    { //error
        error_code = 20;
        return false;
    }

    try
    {
        error_code = 21;

        char verstr[7];
        sprintf(verstr, "zgf%03i", GLOBALGROUP_VERSION);

        fwrite(verstr, sizeof(char), 6, f); //global file version

        //new format after 2012.04.15 for v0.73
        fwrite(&collection->lastwritetime, sizeof(SYSTEMTIME), 1, f); //Write time that can be checked with the dictionary. If these times differ, the group cannot be loaded anymore.

        l = new TIntegerList;

        //make list of all words with statistics
        for (int i = 0; i < collection->words->Count; i++)
        {
            bool hasstats = false;
            if (collection->words->Items[i]->stats)
            {
                for (int j = 0; j < collection->words->Items[i]->meaningcnt; j++)
                    if ((double)(collection->words->Items[i]->stats[j].testtime) != 0)
                    {
                        hasstats = true;
                        break;
                    }
            }
            if (hasstats)
                l->Add(i);
        }
        error_code = 23;

        n = l->Count;
        fwrite(&n, sizeof(int), 1, f);

        for (int i = 0; i < l->Count; i++)
        {
            n = l->Items[i];
            fwrite(&n, sizeof(int), 1, f);
            w = collection->words->Items[n];

            for (int j = 0; j < w->meaningcnt; j++)
            {
                TWordStatGlobal *wg = w->stats + j;
                fwrite(&wg->included, sizeof(word), 1, f);
                fwrite(&wg->score, sizeof(char), 1, f);
                fwrite(&wg->testtime, sizeof(TDateTime), 1, f);
            }
        }
        error_code = 24;
        l->Clear();

        error_code = 25;
        collection->groups->SaveToFile(f);

        error_code = 26;
        collection->studygroups->SaveToFile(f);

        error_code = 27;
        collection->kcategory->SaveToFile(f);
        error_code = 28;
        collection->wordstudylist->SaveToFile(f);
        error_code = 29;

        /*
         // save every word that has an example or kanji card definition
         for (int i = 0; i < kanjis->Count; i++) {
         if (collection->kanjidat[i].card) {
         for (int j = 0; j < collection->kanjidat[i].card->ExampleCount; j++) {
         l->Add(collection->kanjidat[i].card->Examples[j]->ix);
         }
         }
         }

         collection->deflist->IterateIndexes((TGapStringList::TGapIndexFunc)&l->Add);

         l->Sort((TListSortCompare)wordextrasavesort); // important that l is sorted when loading data
         error_code = 32;

         for (int i = l->Count-1; i > 0; i--)
         if (l->Items[i] == l->Items[i-1])
         l->Delete(i);

         n = l->Count;
         fwrite(&n,sizeof(int),1,f);
         for(int i = 0; i < l->Count; i++) {
         n = l->Items[i];
         fwrite(&n,sizeof(int),1,f);
         w = collection->words->Items[n];
         k = wcslen(w->kanji);
         fwrite(&k,sizeof(word),1,f);
         fwrite(w->kanji,sizeof(wchar_t),k,f);
         k = wcslen(w->kana);
         fwrite(&k,sizeof(word),1,f);
         fwrite(w->kana,sizeof(wchar_t),k,f);
         }
         */

        error_code = 33;
        delete l;
        l = NULL;

        error_code = 34;
        //save examples
        int cnt = 0;
        k = 0; //skip count
        while (cnt < kanjis->Count)
        {
            if (!collection->kanjidat[cnt].card || !collection->kanjidat[cnt].card->HasData())
            {
                cnt++;
                ++k;
            }
            else
            {
                fwrite(&k, sizeof(word), 1, f);
                k = cnt;
                while (cnt < kanjis->Count && collection->kanjidat[cnt].card && collection->kanjidat[cnt].card->HasData())
                    cnt++;
                k = cnt - k;
                fwrite(&k, sizeof(word), 1, f);
                cnt -= k;
                for (k += cnt; cnt < k; cnt++)
                    collection->kanjidat[cnt].card->SaveToFile(f);
                k = 0;
            }
        }
        fwrite(&k, sizeof(word), 1, f);
        error_code = 35;

        collection->deflist->SaveToFile(f);
        collection->kdeflist->SaveToFile(f);
        error_code = 36;

        collection->kgroups->SaveToFile(f);
        error_code = 37;

    }
    catch (...)
    {
        error = true;
    }

    fclose(f);
    delete l;

    if (!error)
        error_code = 0;

    return !error;
}

void load_groups(const UnicodeString &name, TWordCollection *collection)
{
    FILE *f = NULL;
    int cindex = dictionary->IndexOfObject((TObject*)collection);

    char *ctmp = 0;

    byte version;
    byte dversion; // Dictionary version when this group file was saved.

    int n;
    int cnt, c;
    word k;
    TWord *w;
    TConvertItem *ci = NULL;
    TGapList<TIntegerList *> *exampleconvert = NULL;

    error_code = 1;

    if (!FileExists(name.c_str()))
        THROW(L"File does not exist!");

    f = _wfopen(name.c_str(), L"rb");
    if (!f)
        THROW(L"Couldn't open group file!");

    error_code = 2;

    try
    {
        ctmp = new char[7];
        ctmp[6] = 0;
        fread(ctmp, sizeof(char), 6, f);
        if (strncmp("zgf", ctmp, 3) || ctmp[5] > '9' && ctmp[5] < '0' || ctmp[4] > '9' && ctmp[4] < '0' || ctmp[3] > '9' && ctmp[3] < '0')
        {
            delete[] ctmp;
            THROW(L"Bad signature in group file!");
        }
        version = strtol(ctmp + 3, 0, 10);

        if (version >= 17)
        {
            SYSTEMTIME st;
            fread(&st, sizeof(SYSTEMTIME), 1, f);
            if (memcmp(&st, &collection->lastwritetime, sizeof(SYSTEMTIME)))
            {
                MsgBox(L"Dictionary date stored in user file doesn't match dictionary. The file is corrupt or the dictionary was updated incorrectly.", L"Error", MB_OK);
                dictionary->DisableSave();
                fclose(f);
                Application->Terminate();
                return;
            }
        }
        else if (version >= 8)
        {
            // Added Jan 2011, v0.57
            fread(&c, sizeof(int), 1, f);
            fread(&dversion, sizeof(byte), 1, f);
        }
        else
        {
            fread(&c, sizeof(int), 1, f);
            dversion = 5;
        }

        error_code = 250;

        if (version < 17 && version != GLOBALGROUP_VERSION) // New version, set modify.
            collection->StartupModify(true);
        else if (version != GLOBALGROUP_VERSION)
            collection->StartupModify(false);

        error_code = 3;

        if (version < 8)
        {
            profile->Clear();

            fread(ctmp, sizeof(char), 6, f);
            fread(&c, sizeof(int), 1, f);

            if (strncmp("zwf", ctmp, 3) || ctmp[5] > '9' && ctmp[5] < '0' || ctmp[4] > '9' && ctmp[4] < '0' || ctmp[3] > '9' && ctmp[3] < '0')
            {
                delete[] ctmp;
                THROW(L"Bad signature!");
            }
            version = ctmp[5] - '0';

            error_code = 4;

            if (version != WORDGROUP_VERSION) // New version, set modify
                collection->StartupModify(true);

            error_code = 5;
        }

        delete[] ctmp;
        ctmp = NULL;

        error_code = 6;

        fread(&cnt, sizeof(int), 1, f);

        TWordIndexList *l = NULL; //Only used before version 17
        TWordStatGlobal *st;
        byte stcnt;

        error_code = 7;

        ci = NULL;

        if (version < 17 && cnt)
        {
            l = new TWordIndexList(collection);
            ci = new TConvertItem[cnt];
        }

        error_code = 8;

        for (int i = 0; i < cnt; i++)
        {
            error_code = 81;
            if (version >= 17)
            {
                fread(&c, sizeof(int), 1, f);

                error_code = 87;

                collection->words->Items[c]->stats = st = new TWordStatGlobal[collection->words->Items[c]->meaningcnt];
                memset(st, 0, sizeof(TWordStatGlobal) * collection->words->Items[c]->meaningcnt);

                error_code = 88;

                for (int j = 0; j < collection->words->Items[c]->meaningcnt; j++)
                {
                    fread(&st[j].included, sizeof(word), 1, f);
                    fread(&st[j].score, sizeof(char), 1, f);
                    fread(&st[j].testtime, sizeof(TDateTime), 1, f);
                }
            }
            else // Versions before 17
            {
                fread(&ci[i].original, sizeof(int), 1, f);
                if (!cindex || version >= 2)
                {
                    fread(&k, sizeof(word), 1, f);
                    fread(ci[i].kanji, sizeof(wchar_t), k, f);
                    ci[i].kanji[k] = NULL;
                    fread(&k, sizeof(word), 1, f);
                    fread(ci[i].kana, sizeof(wchar_t), k, f);
                    ci[i].kana[k] = NULL;

                    error_code = 82;

                    //get the word we need
                    FindWord(Romanize(ci[i].kana).c_str(), l, true, true, false, false); //look for the kana/kanji and hopefuly only one result will be there

                    error_code = 83;

                    for (int j = l->Count - 1; j >= 0; j--)
                    {
                        w = collection->words->Items[l->Indexes[j]];
                        if (wcscmp(w->kanji, ci[i].kanji) || wcscmp(w->kana, ci[i].kana))
                            l->Delete(j);
                    }

                    error_code = 84;

                    if (l->Count == 1)
                        ci[i].changed = l->Indexes[0];
                    else
                        ci[i].changed = -1;

                    error_code = 85;

                    l->Clear();
                }
                else
                    ci[i].changed = ci[i].original;

                error_code = 86;

                fread(&stcnt, sizeof(stcnt), 1, f);

                if (stcnt)
                {
                    error_code = 87;

                    st = new TWordStatGlobal[stcnt];
                    memset(st, 0, sizeof(TWordStatGlobal)*stcnt);

                    error_code = 88;

                    for (int j = 0; j < stcnt; j++)
                    {
                        if (version >= 15)
                        {
                            if (ci[i].changed >= 0 && j < (collection->words->Items[ci[i].changed])->meaningcnt)
                            {
                                TWordStatGlobal *wg = st + j;
                                fread(&wg->included, sizeof(word), 1, f);
                                fread(&wg->score, sizeof(char), 1, f);
                                fread(&wg->testtime, sizeof(TDateTime), 1, f);
                            }
                            else
                                fseek(f, sizeof(word) +sizeof(char) +sizeof(TDateTime), SEEK_CUR);
                        }
                        else
                        {
                            if (ci[i].changed >= 0 && j < (collection->words->Items[ci[i].changed])->meaningcnt)
                            {
                                TWordStatGlobal *wg = st + j;
                                fread(&wg->included, sizeof(word), 1, f);
                                fread(&wg->score, sizeof(char), 1, f);
                                fread(&wg->testtime, sizeof(TDateTime), 1, f);
                                int dummy;
                                fread(&dummy, sizeof(int), 1, f);
                            }
                            else
                                fseek(f, sizeof(word) +sizeof(char) +sizeof(TDateTime) +sizeof(int), SEEK_CUR);
                        }
                    }

                    error_code = 89;

                    if (ci[i].changed >= 0)
                    {
                        if ((collection->words->Items[ci[i].changed])->stats)
                            delete[]  (collection->words->Items[ci[i].changed])->stats;

                        error_code = 891;

                        (collection->words->Items[ci[i].changed])->stats = new TWordStatGlobal[(collection->words->Items[ci[i].changed])->meaningcnt];

                        error_code = 892;

                        memset((collection->words->Items[ci[i].changed])->stats, 0, sizeof(TWordStatGlobal)*(collection->words->Items[ci[i].changed])->meaningcnt);

                        error_code = 893;

                        memcpy((collection->words->Items[ci[i].changed])->stats, st, sizeof(TWordStatGlobal)*MIN(stcnt, (collection->words->Items[ci[i].changed])->meaningcnt));

                    }
                    delete[] st;
                }
            }
        }

        delete l; // Not needed after version 17.

        error_code = 9;

        collection->groups->LoadFromFile(collection, ci, cnt, f, version);

        error_code = 10;

        collection->studygroups->LoadFromFile(collection, ci, cnt, f, version);

        error_code = 11;

        error_code = 12;

        if (version < 8)
        {
            delete[] ci;
            ci = NULL;

            ctmp = new char[6];
            fread(ctmp, sizeof(char), 6, f);
            fread(&c, sizeof(int), 1, f);

            error_code = 13;

            if (strncmp("zkf", ctmp, 3) || ctmp[5] < '0' || ctmp[5] > '9' || ctmp[4] < '0' || ctmp[4] > '9' || ctmp[3] < '0' || ctmp[3] > '9')
            {
                delete[] ctmp;
                THROW(L"Bad signature in group file!");
            }

            error_code = 14;

            version = ctmp[5] - '0';

            delete[] ctmp;
            ctmp = NULL;

            error_code = 15;

            if (version != KANJIGROUP_VERSION) // New version, set modify.
                collection->StartupModify(false);

            error_code = 18;
        }

        error_code = 19;

        collection->kgroups->Clear();

        error_code = 20;

        collection->kcategory->LoadFromFile(f, version);

        error_code = 21;

        if (version >= 8)
        { //added Nov. 2010, for v0.57
            collection->wordstudylist->LoadFromFile(ci, cnt, f, version);
            delete[] ci;
            ci = NULL;
        }
        else
        {
            TStudyList *studylist = new TStudyList(collection);
            studylist->LoadFromFile(f, version);
            studylist->Convert(collection->wordstudylist);
            delete studylist;
            studylist = NULL;
        }

        error_code = 22;

        bool itemchanges = false; //remove after version 17
        n = 0;
        if (version < 17 && (!cindex || version >= 7))
        {
            fread(&n, sizeof(int), 1, f);

            error_code = 221;

            if (n)
            {
                TWordIndexList *l = new TWordIndexList(collection); //this is only a temporary word list for loading!

                error_code = 222;

                ci = new TConvertItem[n];

                error_code = 223;

                for (int i = 0; i < n; i++)
                {

                    fread(&ci[i].original, sizeof(int), 1, f);
                    fread(&k, sizeof(word), 1, f);
                    fread(ci[i].kanji, sizeof(wchar_t), k, f);
                    ci[i].kanji[k] = NULL;
                    fread(&k, sizeof(word), 1, f);
                    fread(ci[i].kana, sizeof(wchar_t), k, f);
                    ci[i].kana[k] = NULL;

                    error_code = 224;

                    //get the word we need
                    FindWord(Romanize(ci[i].kana).c_str(), l, true, true, false, false); //look for the kanji and hopefuly only one result will be there

                    error_code = 225;

                    for (int j = l->Count - 1; j >= 0; j--)
                    {
                        w = collection->words->Items[l->Indexes[j]];

                        error_code = 2251;

                        if (wcscmp(w->kanji, ci[i].kanji) || wcscmp(w->kana, ci[i].kana))
                            l->Delete(j);
                    }

                    error_code = 226;

                    if (l->Count == 1)
                        ci[i].changed = l->Indexes[0];
                    else
                        ci[i].changed = -1;

                    error_code = 227;

                    if (ci[i].changed != ci[i].original)
                        itemchanges = true;

                    error_code = 228;

                    l->Clear();
                }
                delete l;
            }
        }

        error_code = 23;

        //read examples
        int cnt = 0;
        k = 0; //skip count

        int kcnt = dversion <= 5 ? 6353 : 6355; //added Jan 2011, v0.57

        exampleconvert = NULL;
        if (version < 17)
            exampleconvert = new TGapList<TIntegerList *>(false);
        while (cnt < kcnt)
        {

            error_code = 231;

            fread(&k, sizeof(word), 1, f);
            cnt += k;

            error_code = 232;

            if (cnt >= kcnt)
                break;

            error_code = 233;

            int d = 0;

            fread(&k, sizeof(word), 1, f);
            for (k += cnt; cnt < k; cnt++)
            {

                error_code = 234;

                //碵 4739 鬥 6168 - included after 2011 Jan 29, v0.57
                if (dversion <= 5 && cnt == 4739 || cnt + d == 6168)
                    d++;

                if (!collection->kanjidat[cnt + d].card)
                    collection->kanjidat[cnt + d].card = new TKanjiCard(kanjis->Kanjis[cnt + d]);
                if (version >= 17)
                    collection->kanjidat[cnt + d].card->LoadFromFile(f, version, kanjis->Kanjis[cnt + d], ci, n, /* Remove when version > 17 */ collection);
                else
                    exampleconvert->Items[cnt + d] = collection->kanjidat[cnt + d].card->LoadFromFile(f, version, kanjis->Kanjis[cnt + d], ci, n, collection);
            }
        }

        error_code = 24;

        collection->deflist->LoadFromFile(f, version >= 11, version);

        error_code = 25;

        if (itemchanges)
        { //remove after version 17
            TGapStringList *tmpgap = new TGapStringList;
            try
            {
                TIntegerList *l = NULL;
                try
                {
                    l = new TIntegerList;
                    error_code = 251;

                    collection->deflist->IterateIndexes((TGapStringList::TGapIndexFunc)&l->Add);

                    error_code = 252;

                    for (int i = 0; i < l->Count; i++)
                    {
                        error_code = 2521;

                        int ix = l->Items[i];

                        error_code = 2522;

                        int min = 0, max = n - 1;
                        while (min < max)
                        {
                            int mid = (min + max) / 2;
                            if (ci[mid].original < ix)
                                min = mid + 1;
                            else if (ci[mid].original > ix)
                                max = mid - 1;
                            else
                                min = max = mid;
                        }

                        error_code = 2523;

                        if (ci[min].original != ix)//may not be in the convert list
                                    THROW(L"No conversion for gap item!");

                        error_code = 2524;

                        if (ci[min].changed >= 0)
                            tmpgap->Items[ci[min].changed] = collection->deflist->Items[ix];
                    }
                }
                catch (...)
                {
                    delete l;
                    throw;
                }

                delete l;

                error_code = 253;

                collection->deflist->Assign(tmpgap);
            }
            catch (...)
            {
                delete tmpgap;
                throw;
            }
            delete tmpgap;
        }

        error_code = 26;

        collection->kdeflist->LoadFromFile(f, version >= 11, version);

        error_code = 27;

        collection->kgroups->LoadFromFile(f, version, exampleconvert);

        delete[] ci;
        ci = NULL;

        if (version < 17)
        {
            for (unsigned int i = 0; i < exampleconvert->TrueCount; ++i)
                delete exampleconvert->TrueItems[i];
            delete exampleconvert;
            exampleconvert = NULL;
        }

    }
    catch (...)
    {
        fclose(f);
        delete[] ctmp;
        delete[] ci;
        for (unsigned int i = 0; i < exampleconvert->TrueCount; ++i)
            delete exampleconvert->TrueItems[i];
        delete exampleconvert;
        throw;
    }

    delete[] ctmp;
    delete[] ci;

    fclose(f);

    error_code = 0;
}

void set_word_frequencies(TWordCollection *coll)
{
    TKanjiData *dat = coll->kanjidat;
    for (int i = 0; i < kanjis->Count; i++)
    {
        kanjis->Kanjis[i]->word_freq = 0;
        for (int j = 0; j < dat[i].wordscnt; j++)
        {
            kanjis->Kanjis[i]->word_freq += coll->words->Items[dat[i].words[j]]->frequency;
            if (kanjis->Kanjis[i]->word_freq < 0)
                THROW(L"Negative word frequency!");
        }
    }
}

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------
//import_examples and sub functions
//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------

//example_to_buff - adds one sentence line to the buffer data
//buff - the buffer to add the example, linenum - the index of the line IN THE BUFFER
//buffsize - allocated size of buffer, reference in case I have to increase buffer size
//returns bytes used in buffer
int example_to_buff(char* &buff, int &buffsize, int linenum, TExampleWordData *data, byte datalen, wchar_t *jp, wchar_t *en)
{
    static word buffpos;
    if (!(linenum % 100))
    {
        memset(buff, 0, 200);
        buffpos = 200;
    }

    *((word*)buff + (linenum % 100)) = buffpos - 200;

    word realenlen = wcslen(en);
    word enlen = WideCharToMultiByte(CP_UTF8, 0, en, realenlen, NULL, 0, NULL, NULL);
    word jplen = wcslen(jp);

    //word * 2 is the lengths of kanji and kana of the current word. Byte would be enough but I set that word to 0 when reading, as an ending wchar_t character.
    //TWord *w;
    TStoredWord *sw;
    int klen = 0;
    for (int ix = 0; ix < datalen; ++ix)
    {
        //w = dictionary->Items[0]->words->Items[data[ix].data->wordindex];
        sw = data[ix].data;
        klen += wcslen(sw->kanji) + wcslen(sw->kana);
    }

    if (buffsize - buffpos < (int)(enlen + jplen * 2 +sizeof(word) +sizeof(word) +sizeof(word) +sizeof(byte) + datalen * (sizeof(word) +sizeof(byte) +sizeof(word) * 2) + klen *sizeof(wchar_t)/*sizeof(TExampleWordData)*/))
    {
        char *tmp = buff;
        buff = new char[buffsize * 2];
        memcpy(buff, tmp, buffpos);
        buffsize *= 2;
        delete[] tmp;
    }

    //number of words in the example
    *((byte*)(buff + buffpos)) = datalen;
    buffpos += sizeof(byte);

    //example words data

    //memcpy(buff+buffpos,data,sizeof(TExampleWordData)*datalen);
    //buffpos += sizeof(TExampleWordData)*datalen;
    for (int ix = 0; ix < datalen; ++ix)
    {
        //w = dictionary->Items[0]->words->Items[data[ix].data->wordindex];
        sw = data[ix].data;

        memcpy(buff + buffpos, &data[ix].position, sizeof(word));
        buffpos += sizeof(word);
        memcpy(buff + buffpos, &data[ix].length, sizeof(byte));
        buffpos += sizeof(byte);

        word slen = wcslen(sw->kanji);
        memcpy(buff + buffpos, &slen, sizeof(word));
        buffpos += sizeof(word);
        memcpy(buff + buffpos, sw->kanji, sizeof(wchar_t)*slen);
        buffpos += sizeof(wchar_t) * slen;

        slen = wcslen(sw->kana);
        memcpy(buff + buffpos, &slen, sizeof(word));
        buffpos += sizeof(word);
        memcpy(buff + buffpos, sw->kana, sizeof(wchar_t)*slen);
        buffpos += sizeof(wchar_t) * slen;
    }

    //english text length
    *((word*)(buff + buffpos)) = enlen; //UTF8 length
    buffpos += sizeof(word);
    *((word*)(buff + buffpos)) = realenlen; //wide length
    buffpos += sizeof(word);

    //english text
    WideCharToMultiByte(CP_UTF8, 0, en, realenlen, buff + buffpos, enlen, NULL, NULL);
    buffpos += sizeof(char) * enlen;

    //japanese text length
    *((word*)(buff + buffpos)) = jplen;
    buffpos += sizeof(word);

    //japanese text
    memcpy(buff + buffpos, jp, sizeof(wchar_t)*jplen);
    buffpos += sizeof(wchar_t) * jplen;

    return buffpos;
}

//Contains words found in the example sentences data.
struct TImportExampleWord
{
    wchar_t *kanji;
    wchar_t *kana;
    TImportExampleWord(wchar_t *akanji, wchar_t *akana)
    {
        kanji = new wchar_t[wcslen(akanji) + 1];
        wcscpy(kanji, akanji);
        kana = akana ? new wchar_t[wcslen(akana) + 1] : NULL;
        if (kana)
            wcscpy(kana, akana);
    }
    ~TImportExampleWord()
    {
        delete[] kanji;
        delete[] kana;
    }
};

static bool importaborted;
static void _eximportabortproc()
{
    importaborted = true;
}

bool import_examples(const wchar_t *fname, const wchar_t *fname2)
{
    importaborted = false;
    wchar_t *tmp, *tmp2, *tok;

    wchar_t *jp = NULL, *jppos = NULL; // Japanese sentence, position of processing in japanese sentence.
    wchar_t *en = NULL; // English sentence.

    char *buff; // This will hold sentences to compress.
    void *outbuff;
    int outsize;
    int buffsize;
    int buffpos;
    FILE *f, *f2;
    int f2pos = 0;

    wchar_t *kanji; // Word written.
    wchar_t *reading; // Word reading.
    wchar_t *form; // Word as in sentence.
    char meaning; // Meaning index of word in the current jmdict specified in examples. because the dictionary changes a lot, I don't consider this safe to use so I'll ignore its value.

    int linenum = 0;
    int reallinenum = 0;
    byte wordnum;
    //int wordwithdata = 0;
    //int worddatacnt = 0;
    TExampleWordData data[100]; // There won't be this many in a single sentence.
    //TWord *w;

    wchar_t line[1024];

    f = _wfopen((/*ExtractFileDir(ParamStr(0)) + L"\\" +*/ fname)/*.c_str()*/, L"rb");

    if (!f)
    {
        MsgBox(L"Couldn't open example sentences file for reading.", L"Error", MB_OK);
        return false;
    }

    f2 = _wfopen((/*ExtractFileDir(ParamStr(0)) + L"\\" +*/ fname2)/*.c_str()*/, L"wb");
    if (!f2)
    {
        fclose(f);
        MsgBox(L"Error creating new example sentences file.", L"Error", MB_OK);
        return false;
    }

    store->ClearExampleData();

    bool Aline = true;

    TWordIndexList *l = new TWordIndexList(dictionary->Items[0]);
    if (!l)
    {
        fclose(f);
        fclose(f2);
        MsgBox(L"Not enough memory.", L"Error", MB_OK);
        return false;
    }

    buffsize = 30000;
    buff = new char[buffsize];

    if (!buff)
    {
        fclose(f);
        fclose(f2);
        delete l;
        MsgBox(L"Not enough memory.", L"Error", MB_OK);
        return false;
    }

    TTemplateList<TImportExampleWord, true> *exwords = new TTemplateList<TImportExampleWord, true>; //List of word kanji and kana in the current sentence line.

    //TGapList<TStoredWord*> storedwords;
    //TWordExampleDataTree extree;

    ShowMessageForm(L"Importing example sentences. This can take several minutes...", &_eximportabortproc);
    try
    {
        /*!! Change headersize when adding something here!*/
        exampleheadersize = 8 + sizeof(int) + sizeof(SYSTEMTIME) * 2;

        char c[9];
        sprintf(c, "zexmp%03i", EXAMPLESENTENCE_VERSION);
        fwrite(c, sizeof(char), 8, f2);

        fwrite(&dictionary_date, sizeof(SYSTEMTIME), 1, f2);
        GetSystemTime(&example_date);
        fwrite(&example_date, sizeof(SYSTEMTIME), 1, f2);
        fwrite(&exampleheadersize, sizeof(int), 1, f2);

        while (getlinemb(line, 1024, f))
        {
            if (importaborted)
                break;

            reallinenum++;
            if ((reallinenum % 50) == 0)
                Application->ProcessMessages();

            if (Aline)
            {
                delete[] jp;
                delete[] en;
                jp = NULL;
                en = NULL;
            }
            if (line[0] == L'#')
            {
                continue;
            }

            if (Aline && wcsncmp(line, L"A: ", 3))
            {
                continue;
            }
            else if (!Aline && wcsncmp(line, L"B: ", 3))
                THROW(L"Error! Bad example txt format! Not a B line!");

            if (Aline) // Prepare the sentences.
            {
                tmp = wcschr(line + 3, L'\t');
                if (!tmp)
                {
                    continue;
                }
                *tmp = 0;

                int len = wcslen(line + 3);
                jp = new wchar_t[len + 1];
                wcscpy(jp, line + 3);
                jppos = jp;

                tmp++;
                if ((en = wcsrchr(tmp, L'#')) != NULL)
                {
                    *en = NULL;
                    en = NULL;
                }

                len = wcslen(tmp);

                en = new wchar_t[len + 1];
                wcscpy(en, tmp);
                Aline = false;
                continue;
            }
            else
            {
                wordnum = 0;

                // Add another NULL after the trailing NULL of the line.
                line[wcslen(line) + 1] = NULL;
                tmp = line + 3; //Skip "B: "
                tok = wcschr(tmp, L' ');
                if (tok)
                    * tok = NULL;

                /* Example sentence line legend:
                 * kanji form
                 * () - reading
                 * [] - meaning number
                 * {} - how it is found in the sentence
                 * ~ this character means that the sentence is a checked and good example for the word
                 * (None of these have to be present, but the order is always the same.)
                 * space after the single word data
                 */
                while (tmp)
                {
                    exwords->Clear();
                    l->Clear();

                    meaning = -1;
                    kanji = tmp;

                    if ((reading = wcschr(tmp, L'(')) != NULL)
                    {
                        if ((tmp2 = wcschr(reading, L')')) == NULL)
                            reading = NULL;
                        else
                        {
                            *reading = NULL;
                            reading++;
                            *tmp2 = NULL;
                            tmp = tmp2 + 1;
                        }
                    }
                    if ((tmp2 = wcschr(tmp, L'[')) != NULL && (tmp2[2] == L']' || tmp2[3] == L']'))
                    {
                        *tmp2 = NULL;
                        tmp2++;
                        tmp = wcschr(tmp2, L']');
                        *tmp = NULL;
                        tmp++;

                        meaning = StrToInt(tmp2) - 1;
                    }
                    if ((form = wcschr(tmp, L'{')) != NULL/*|| (form = wcschr(tmp,L'｛')) != NULL*/)
                    {
                        if ((tmp2 = wcschr(form, L'}')) == NULL/*&& (tmp2 = wcschr(form,L'｝')) == NULL*/)
                            form = NULL;
                        else
                        {
                            *form = NULL;
                            form++;
                            *tmp2 = NULL;
                        }
                    }

                    if (kanji[wcslen(kanji) - 1] == L'~')
                        kanji[wcslen(kanji) - 1] = 0;

                    bool kanaword = only_kana(kanji);

                    // Get a list of all possible combinations for the word in the sentence in the current main dictionary.
                    if (reading || kanaword)
                    {
                        FindWord(Romanize((reading ? reading : kanji)).c_str(), l, true, true, false, false, NULL);

                        int plainfound = -1;
                        for (int i = l->Count - 1; i >= 0; i--)
                        {
                            bool wordkanaword = only_kana(l->Words[i]->kanji);
                            if (reading && wcscmp(reading, l->Words[i]->kana) || !kanaword && wcscmp(l->Words[i]->kanji, kanji) || kanaword && !(l->Words[i]->data[(meaning >= 0 && meaning < l->Words[i]->meaningcnt) ? meaning : 0].notes & WN_KANAONLY) && !wordkanaword)
                                l->Delete(i);
                            else if (kanaword && wordkanaword)
                                plainfound = l->Indexes[i];
                        }

                        if (l->Count > 1 && kanaword && plainfound >= 0)
                        {
                            l->Clear();
                            l->Add(plainfound);
                        }
                    }
                    else
                    {
                        FindKanjiWord(l, kanji, false, NULL);
                        if (form && l->Count > 1) // Look for a form match and if found delete all other choices.
                        {
                            // Form might be inflected, try to deinflect and look for those variants in the dictionary as well..
                            int i;
                            for (i = 0; i < l->Count; i++)
                            {
                                wchar_t *k = l->Words[i]->kana;
                                if (!wcscmp(k, form))
                                    break;
                            }
                            if (i < l->Count)
                            {
                                i = l->Indexes[i];
                                l->Clear();
                                if (i != -1)
                                    l->Add(i);
                            }
                        }
                    }

                    // Add anything for processing if found in the current dictionary.
                    for (int i = l->Count - 1; i >= 0; --i)
                        exwords->Add(new TImportExampleWord(l->Words[i]->kanji, l->Words[i]->kana));

                    // Nothing matches in the current main dictionary. Add the word with the specified form, to make it available in case the dictionary changes.
                    if (!l->Count)
                    {
                        // Check for invalid characters in the kanji and reading form of the found word.
                        bool valid = true;
                        for (int k = wcslen(kanji) - 1; k >= 0 && valid; --k)
                            if (!JAPAN(kanji[k]))
                                valid = false;
                        if (reading)
                        {
                            for (int k = wcslen(reading) - 1; k >= 0 && valid; --k)
                                if (!VALIDKANA(reading[k]))
                                    valid = false;
                        }
                        else if (!kanaword)
                            valid = false;
                        if (valid)
                            exwords->Add(new TImportExampleWord(kanji, reading ? reading : kanji));
                    }

                    if (exwords->Count && (tmp2 = wcsstr(jppos, (form ? form : kanji))) != NULL)
                    {
                        jppos = tmp2 + wcslen(form ? form : kanji);
                        for (int k = 0; k < exwords->Count; ++k)
                        {
                            //if (l->Indexes[k] >= dictionary->Items[0]->words->Count)
                            //THROW(L"Error! Word index overflow!";
                            TImportExampleWord *w = exwords->Items[k];
                            //w = l->Words[k];

                            /*
                             if (!wcscmp(w->kanji,L"は") && !wcscmp(w->kana,L"は") ||
                             !wcscmp(w->kanji,L"が") && !wcscmp(w->kana,L"が") ||
                             !wcscmp(w->kanji,L"乃") && !wcscmp(w->kana,L"の") ||
                             !wcscmp(w->kanji,L"為る") && !wcscmp(w->kana,L"する") ||
                             !wcscmp(w->kanji,L"を") && !wcscmp(w->kana,L"を") ||
                             !wcscmp(w->kanji,L"と") && !wcscmp(w->kana,L"と") ||
                             !wcscmp(w->kanji,L"で") && !wcscmp(w->kana,L"で") ||
                             !wcscmp(w->kanji,L"に") && !wcscmp(w->kana,L"に") ||
                             !wcscmp(w->kanji,L"や") && !wcscmp(w->kana,L"や") ||
                             !wcscmp(w->kanji,L"か") && !wcscmp(w->kana,L"か") ||
                             !wcscmp(w->kanji,L"よ") && !wcscmp(w->kana,L"よ") ||
                             !wcscmp(w->kanji,L"も") && !wcscmp(w->kana,L"も") ||
                             !wcscmp(w->kanji,L"な") && !wcscmp(w->kana,L"な") ||
                             !wcscmp(w->kanji,L"わ") && !wcscmp(w->kana,L"わ") ||
                             !wcscmp(w->kanji,L"です") && !wcscmp(w->kana,L"です") ||
                             !wcscmp(w->kanji,L"である") && !wcscmp(w->kana,L"である") ||
                             !wcscmp(w->kanji,L"だ") && !wcscmp(w->kana,L"だ") ||
                             reading && wcscmp(reading,w->kana) ||
                             !(w->data[(meaning >= 0 && meaning < w->meaningcnt) ? meaning : 0].notes & WN_KANAONLY) &&
                             wcscmp(kanji, w->kanji))
                             continue;
                             */
                            if (!wcscmp(w->kanji, L"は") && !wcscmp(w->kana, L"は") ||
                                !wcscmp(w->kanji, L"が") && !wcscmp(w->kana, L"が") ||
                                !wcscmp(w->kanji, L"乃") && !wcscmp(w->kana, L"の") ||
                                !wcscmp(w->kanji, L"為る") && !wcscmp(w->kana, L"する") ||
                                !wcscmp(w->kanji, L"を") && !wcscmp(w->kana, L"を") ||
                                !wcscmp(w->kanji, L"と") && !wcscmp(w->kana, L"と") ||
                                !wcscmp(w->kanji, L"で") && !wcscmp(w->kana, L"で") ||
                                !wcscmp(w->kanji, L"に") && !wcscmp(w->kana, L"に") ||
                                !wcscmp(w->kanji, L"や") && !wcscmp(w->kana, L"や") ||
                                !wcscmp(w->kanji, L"か") && !wcscmp(w->kana, L"か") ||
                                !wcscmp(w->kanji, L"よ") && !wcscmp(w->kana, L"よ") ||
                                !wcscmp(w->kanji, L"も") && !wcscmp(w->kana, L"も") ||
                                !wcscmp(w->kanji, L"な") && !wcscmp(w->kana, L"な") ||
                                !wcscmp(w->kanji, L"わ") && !wcscmp(w->kana, L"わ") ||
                                !wcscmp(w->kanji, L"です") && !wcscmp(w->kana, L"です") ||
                                !wcscmp(w->kanji, L"である") && !wcscmp(w->kana, L"である") ||
                                !wcscmp(w->kanji, L"だ") && !wcscmp(w->kana, L"だ"))
                                continue;

                            //worddatacnt++;

                            TStoredWord *sw;
                            TExampleData *old = NULL;
                            /*
                             if (storedwords.ItemSet(l->Indexes[k])) {
                             sw = storedwords.Items[l->Indexes[k]];
                             old = sw->examples;
                             } else {
                             sw = store->FindWord(l->Words[k]);
                             if (!sw)
                             continue;
                             }
                             */
                            sw = store->FindWord(w->kanji, w->kana);
                            if (!sw)
                                sw = store->Items[store->Add(w->kanji, w->kana, true)];
                            else
                                old = sw->examples;

                            sw->examples = new TExampleData[sw->examplecnt + 1];

                            if (old && sw->examplecnt)
                            {
                                //if (!sw->examplecnt)
                                //THROW(L"Error! Example data count not initialized!";
                                memcpy(sw->examples, old, sizeof(TExampleData)*sw->examplecnt);
                                delete[] old;
                            }
                            else if (sw->examplecnt)
                                THROW(L"Error! Example data not initialized!");

                            sw->examples[sw->examplecnt].block = linenum / 100;
                            sw->examples[sw->examplecnt].num = linenum % 100;
                            sw->examples[sw->examplecnt].wordnum = wordnum;
                            sw->examplecnt++;

                            //storedwords.Items[l->Indexes[k]] = sw;

                            data[wordnum].data = sw;

                            data[wordnum].position = tmp2 - jp;
                            data[wordnum].length = jppos - tmp2;
                            ++wordnum;

                            if (wordnum == 100)
                                THROW(L"Unexpected number of words in line!");
                        }
                    }
                    else
                    {
                        /*
                         bool found = false;
                         for (int i = wcslen(kanji)-1; i >= 0 && !found; --i)
                         found = KANJI(kanji[i]) || KANA(kanji[i]) || VALIDCODE(kanji[i]);

                         // skip line where one word doesn't match
                         if (found) {
                         for (int i = wordnum-1; i >= 0; i--) {
                         worddatacnt--;

                         TWordExampleData &ex = examples.Items[data[i].index];
                         ex.count--;
                         if (!ex.count) {
                         delete[]   examples.Items[data[i].index].data;
                         examples.ClearIndex(data[i].index);
                         }

                         }
                         wordnum = 0;
                         break;
                         }
                         */
                    }

                    if (tok)
                    {
                        tmp = tok + 1;
                        if ((tok = wcschr(tmp, L' ')) != NULL)
                            * tok = NULL;
                    }
                    else
                        tmp = NULL;
                }

                if (wordnum)
                {
                    buffpos = example_to_buff(buff, buffsize, linenum, data, wordnum, jp, en);
                    if (linenum % 100 == 99)
                    {
                        outsize = 0;
                        outbuff = NULL;
                        ZCompress(buff, buffpos, outbuff, outsize);
                        fseek(f2, exampleheadersize + 4 + (linenum / 100) * 8, SEEK_SET); // Header + 4 = number of example lines (written for last).
                        fwrite(&f2pos, sizeof(int), 1, f2); // Position in data block.
                        fwrite(&buffpos, sizeof(int), 1, f2); // Uncompressed size.

                        fseek(f2, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8 + 4 + f2pos, SEEK_SET);
                        fwrite(outbuff, outsize, 1, f2);
                        f2pos += outsize;
                        zlibFreeMem(NULL, outbuff);
                    }
                    linenum++;
                }
                Aline = true;
            }
        }

        if (!importaborted)
        {
            if ((linenum % 100) != 0)
            {
                outsize = 0;
                ZCompress(buff, buffpos, outbuff, outsize);
                fseek(f2, exampleheadersize + 4 + (linenum / 100) * 8, SEEK_SET); // Header + 4 = number of example lines (written for last).
                fwrite(&f2pos, sizeof(int), 1, f2); // Position in data block.
                fwrite(&buffpos, sizeof(int), 1, f2); // Uncompressed size.

                fseek(f2, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8 + 4 + f2pos, SEEK_SET);
                fwrite(outbuff, outsize, 1, f2);
                f2pos += outsize;
                zlibFreeMem(NULL, outbuff);
            }

            fseek(f2, exampleheadersize, SEEK_SET);
            fwrite(&linenum, sizeof(int), 1, f2); // Write number of lines processed.

            // Position of word data.
            fseek(f2, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8, SEEK_SET);
            fwrite(&f2pos, sizeof(int), 1, f2);

            // Time to write compressed word data.
            fseek(f2, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8 + 4 + f2pos, SEEK_SET);
        }

        delete[] buff;

        int maxexcount = 0;
        int stored = 0;

        if (!importaborted)
        {
            fwrite(&stored, sizeof(int), 1, f2);

            for (int ix = 0; ix < store->Count; ++ix)
            {
                TStoredWord *sw = store->Items[ix];
                if (sw->examplecnt == 0)
                    continue;
                //TWord *w = store->Words[ix];

                stored++;

                maxexcount = max(maxexcount, sw->examplecnt);
                //extree.ExpandData(ex);

                write(sw->kanji, f2, sltByteAddNULL);
                write(sw->kana, f2, sltByteAddNULL);

                fwrite(&sw->examplecnt, sizeof(word), 1, f2);
                for (int b = 0; b < sw->examplecnt; ++b)
                {
                    fwrite(&sw->examples[b].block, sizeof(word), 1, f2);
                    fwrite(&sw->examples[b].num, sizeof(byte), 1, f2);
                    fwrite(&sw->examples[b].wordnum, sizeof(byte), 1, f2);
                }
            }
            fseek(f2, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8 + 4 + f2pos, SEEK_SET);
            fwrite(&stored, sizeof(int), 1, f2);
        }

        //extree.Sort();
        //extree.SaveToFile(f2);

        /*for (unsigned int ix = 0; ix < storedwords.TrueCount; ++ix) {
         TStoredWord *sw = storedwords.TrueItems[ix];
         maxexcount = max(maxexcount, sw->examplecnt);
         //extree.ExpandData(ex);
         }
         //extree.Sort();
         //extree.SaveToFile(f2);
         */
    }
    catch (const wchar_t *c)
    {
        MsgBox(c + UnicodeString(L"\r\nAborting..."), L"Error", MB_OK);
        importaborted = true;
    }
    catch (...)
    {
        MsgBox(L"Error occurred while reading example file.", L"Error", MB_OK);
        importaborted = true;
    }

    //extree.Clear();

    fclose(f2);
    fclose(f);

    exwords->Clear();
    delete exwords;
    delete l;
    delete[] buff;
    if (jp)
        delete[] jp;
    if (en)
        delete[] en;

    if (importaborted)
        DeleteFile(fname2);

    HideMessageForm();

    return !importaborted;
}

void import_jlpt(TWordCollection *coll, const wchar_t *name)
{
    FILE *f = _wfopen(name, L"rb");

    if (!f)//error
                THROW(L"Error reading JLPT file!");

    if (!store/*|| !store->Count*/)
        THROW(L"No store!");
    /*
     if (!JLPT)
     JLPT = new TWordNJLPTDataTree;

     JLPT->Clear();
     */

    store->ClearJLPTData();

    //TWordIndexList *l = NULL;
    try
    {
        //l = new TWordIndexList(coll);
        wchar_t line[1024];
        bool bigendian = false;
        while (getlinew(line, 1024, f, bigendian))
        {
            if (!wcslen(line))
                break;
            wchar_t *kanji = wcstok(line, L"\t");
            wchar_t *kana = wcstok(NULL, L"\t");
            int n = _wtoi(wcstok(NULL, L"\t"));
            //bool usekanji = _wtoi( wcstok(NULL,L"\0") ) == 1;

            /*
             l->Clear();
             FindWord(Romanize(kana).c_str(),l,true,true,false,false,NULL);
             for (int ix = l->Count-1; ix >= 0; --ix) {
             if (wcscmp(l->Words[ix]->kanji,kanji) || wcscmp(l->Words[ix]->kana,kana))
             l->Delete(ix);
             }

             if (l->Count == 1) {
             TWordJLPTNData jdat;
             jdat.windex = l->Indexes[0];
             jdat.Nlevel = n;
             //jdat.usekanji = usekanji;

             JLPT->ExpandData(jdat);
             }
             */
            TStoredWord *sw = store->FindWord(kanji, kana);
            if (!sw)
                sw = store->Items[store->Add(kanji, kana, true)];
            if (sw)
                sw->jlptnlevel = n;
        }

        //JLPT->Sort();

        while (getlinew(line, 1024, f, bigendian))
        {
            if (!KANJI(line[0]))
                continue;
            kanjis->Kanjis[kanjis->IndexOf(line[0])]->NJLPT = _wtoi(line + 1);
        }
    }
    catch (...)
    {
        ;
    }
    fclose(f);
    //delete l;
}

bool load_examples(const UnicodeString &fname)
{

    char *c;
    int d;

    //int cnt;

    FILE *f = NULL;

    f = _wfopen(fname.c_str(), L"rb");

    if (!f)
    {
        MsgBox(L"Can't open example data file.", L"Error", MB_OK);
        return false;
    }
    try
    {
        c = new char[9];
        c[8] = 0;
        fread(c, sizeof(char), 8, f);
        if (strncmp(c, "zexmp", 5))
            THROW(L"Error! Bad example file signature!");

        //int version = strtol(c+5,0,10);

        fread(&example_date, sizeof(SYSTEMTIME), 1, f);
        fread(&example_date, sizeof(SYSTEMTIME), 1, f);
        fread(&exampleheadersize, sizeof(int), 1, f);

        delete[] c;

        fseek(f, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8, SEEK_SET);
        fread(&d, sizeof(int), 1, f);

        fseek(f, exampleheadersize + 4 + MAX_BLOCK_COUNT * 8 + 4 + d, SEEK_SET);

        int storecnt = store->Count;

        int stored = 0;
        fread(&stored, sizeof(int), 1, f);
        //int storepos = 0;
        for (int ix = 0; ix < stored; ++ix)
        {
            wchar_t *kanji;
            wchar_t *kana;
            read(kanji, f, sltByteAddNULL);
            read(kana, f, sltByteAddNULL);

            /*
             while (storepos < store->Count) {
             cmp = wcscmp(kanji, store->Kanji[storepos]);
             if (!cmp)
             cmp = wcscmp(kana, store->Kana[storepos]); //WordStoreSortCompare(kanji, kana, storepos);
             if (cmp <= 0)
             break;
             storepos++;
             }
             */

            TStoredWord *sw = store->FindWord(kanji, kana, storecnt - 1);
            if (!sw)
                sw = store->Items[store->Add(kanji, kana, false)];
            delete[] kanji;
            delete[] kana;

            //if (sw) {
            //TStoredWord *sw = store->Items[storepos];
            fread(&sw->examplecnt, sizeof(word), 1, f);
            sw->examples = new TExampleData[sw->examplecnt];
            for (int b = 0; b < sw->examplecnt; ++b)
            {
                fread(&sw->examples[b].block, sizeof(word), 1, f);
                fread(&sw->examples[b].num, sizeof(byte), 1, f);
                fread(&sw->examples[b].wordnum, sizeof(byte), 1, f);
            }
            /*
             } else {
             word cnt;
             fread(&cnt, sizeof(word), 1, f);
             fseek(f, ( sizeof(word) + sizeof(byte) + sizeof(byte) ) * cnt, SEEK_CUR);
             }
             */
        }

        fclose(f);
        f = NULL;

        Examples = new TExampleSentencer(fname);
        store->Sort();
    }
    catch (...)
    {
        if (f)
            fclose(f);
        try
        {
            delete Examples;
        }
        catch (...)
        {
            ;
        }
        Examples = NULL;
    }

    return Examples != NULL;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//returns the contents for the first occurance of tag in s
UnicodeString TagString(UnicodeString &s, UnicodeString tag, bool del = false)
{
    int l = tag.Length() + 2;
    int p1 = s.Pos(L"<" + tag + L">");
    int p2;
    if (!p1 || (p2 = s.Pos(L"</" + tag + L">")) < p1)
        return "";

    if (!del)
        return s.SubString(p1 + l, p2 - p1 - l);

    UnicodeString r = s.SubString(p1 + l, p2 - p1 - l);
    s = s.SubString(1, p1 - 1) + s.SubString(p1 + l + r.Length() + l + 1, s.Length() - 2 * l - 1 - r.Length());

    return r;
}

TElemPattern FindPattern(UnicodeString pat)
{
    wchar_t *patterns[] = { L"", L"ebl", L"eblt", L"elt", L"eltr", L"etr", L"erbl", L"ew", L"elr", L"w", L"h2", L"h3", L"v2", L"v3", L"v4", L"t", L"c", L"b", L"x" };
    for (int i = sizeof(patterns) / sizeof(patterns[0]) - 1; i >= 0; i--)
        if (pat.LowerCase() == patterns[i])
            return (TElemPattern)i;
    return (TElemPattern) - 1;
}

template <class T>
void TagNumArray(UnicodeString &s, UnicodeString tag, T arr[], int arrlen)
{
    memset(arr, 0, arrlen*sizeof(T));
    UnicodeString a = TagString(s, tag);
    if (!a.Length())
        return;

    int arrpos = 0;
    UnicodeString num = TagString(a, L"int", true);
    while (arrpos < arrlen && num.Length())
    {
        arr[arrpos] = num.ToInt();
        num = TagString(a, L"int", true);
        arrpos++;
    }
}

int TagCount(UnicodeString &s, UnicodeString tag)
{
    wchar_t *c = s.c_str();
    int cnt = 0;

    UnicodeString ctag = L"<" + tag + L">";
    while ((c = wcsstr(c, ctag.c_str())) != NULL)
        c++ , cnt++;

    return cnt;
}

int added = 0;
int deleted = 0;

//---------------------------------------------------------------------------
char* toUTF8(const wchar_t *str, int &len)
{
    int newlen = WideCharToMultiByte(CP_UTF8, 0, str, len, NULL, 0, NULL, NULL);
    if (!newlen)
    {
        len = newlen;
        return NULL;
    }

    char *c2 = new char[newlen];
    len = WideCharToMultiByte(CP_UTF8, 0, str, len, c2, newlen, NULL, NULL);
    return c2;
}
//---------------------------------------------------------------------------
wchar_t* fromUTF8(const char *str, int &len)
{
    int newlen = MultiByteToWideChar(CP_UTF8, 0, str, len, NULL, 0);
    if (!newlen)
    {
        len = 0;
        return NULL;
    }
    wchar_t *c2 = new wchar_t[newlen];
    len = MultiByteToWideChar(CP_UTF8, 0, str, len, c2, newlen);
    return c2;
}
//---------------------------------------------------------------------------
int UTF8length(const char *str)
{
    int len = 0;
    while (*str)
    {
        if ((*str & 0xC0) == 0xC0)
        {
            if ((*str & 0x20) == 0x20)
            {
                if ((*str & 0x10) == 0x10)
                    str++;
                str++;
            }
            str += 2;
        }
        else
            str++;
        len++;
    }
    return len;
}
//---------------------------------------------------------------------------
UTF8String toUTF8String(const wchar_t *str, int len)
{
    UTF8String r(str, len >= 0 ? len : wcslen(str));
    return r;
}
//---------------------------------------------------------------------------
AnsiString toAnsiString(const wchar_t *str, int len)
{
    AnsiString r(str, len >= 0 ? len : wcslen(str));
    return r;
}
//---------------------------------------------------------------------------
void write(const UnicodeString &str, FILE *f, TStringLengthType type, int len)
{
    if (len < 0)
        len = str.Length();
    else
        len = min(len, str.Length());

    write(str.c_str(), f, type, len);
}
//---------------------------------------------------------------------------
void write(const UnicodeString &str, FILE *f, int len)
{
    write(str, f, sltNoLength, len);
}
//---------------------------------------------------------------------------
void write(const wchar_t* str, FILE *f, int len)
{
    write(str, f, sltNoLength, len);
}
//---------------------------------------------------------------------------
void write(const wchar_t* str, FILE *f, TStringLengthType type, int len)
{
    if (len < 0)
        len = wcslen(str);

    char *c;

    switch (type)
    {
    case sltNoLength:
        c = toUTF8(str, len);
        break;
    case sltByte:
    case sltByteAddNULL:
        len = (byte)len;
        c = toUTF8(str, len);
        fwrite(&len, sizeof(byte), 1, f);
        break;
    case sltWord:
    case sltWordAddNULL:
        len = (word)len;
        c = toUTF8(str, len);
        fwrite(&len, sizeof(word), 1, f);
        break;
    case sltInt:
    case sltIntAddNULL:
        c = toUTF8(str, len);
        fwrite(&len, sizeof(int), 1, f);
        break;
    }

    if (len)
        fwrite(c, sizeof(char), len, f);

    delete[] c;
}
//---------------------------------------------------------------------------
int read(UnicodeString &str, FILE *f, TStringLengthType type)
{
    wchar_t *c = NULL;
    int len = read(c, f, type);
    str = UnicodeString(c, len);
    delete[] c;

    return len;
}
//---------------------------------------------------------------------------
int read(wchar_t* &str, FILE *f, TStringLengthType type)
{
    int len = 0;

    switch (type)
    {
    case sltByte:
    case sltByteAddNULL:
        fread(&len, sizeof(byte), 1, f);
        break;
    case sltWord:
    case sltWordAddNULL:
        fread(&len, sizeof(word), 1, f);
        break;
    case sltInt:
    case sltIntAddNULL:
        fread(&len, sizeof(int), 1, f);
        break;
    }

    if (len)
    {
        char *c = new char[len + (type == sltByteAddNULL || type == sltWordAddNULL || type == sltIntAddNULL ? 1 : 0)];
        fread(c, sizeof(char), len, f);
        if (type == sltByteAddNULL || type == sltWordAddNULL || type == sltIntAddNULL)
        {
            c[len] = 0;
            len++;
            str = fromUTF8(c, len);
            len--;
        }
        else
            str = fromUTF8(c, len);
        delete[] c;
    }
    else
    {
        if (type == sltByteAddNULL || type == sltWordAddNULL || type == sltIntAddNULL)
        {
            str = new wchar_t[1];
            str[0] = 0;
        }
        else
            str = NULL;
    }

    return len;
}
//---------------------------------------------------------------------------
int readskip(FILE *f, TStringLengthType type)
{
    int len = 0;

    switch (type)
    {
    case sltByte:
    case sltByteAddNULL:
        fread(&len, sizeof(byte), 1, f);
        break;
    case sltWord:
    case sltWordAddNULL:
        fread(&len, sizeof(word), 1, f);
        break;
    case sltInt:
    case sltIntAddNULL:
        fread(&len, sizeof(int), 1, f);
        break;
    }

    if (len)
        fseek(f, len * sizeof(char), SEEK_CUR);

    return len;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(209);
#endif

