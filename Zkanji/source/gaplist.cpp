//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "gaplist.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)


#ifdef _DEBUG
class TIntegerList;

struct TGlobalTest
{
    static TIntegerList *numbers;

    int num;
    TGlobalTest(int num);
    ~TGlobalTest();
};
static TGlobalTest tester1(44);
#endif

const unsigned int UINTMAX = (unsigned int) - 1;

//---------------------------------------------------------------------------
TGapStringList::TGapStringList(bool ExceptionForMissing, int allocsize) : inherited(ExceptionForMissing, allocsize)
{
    ;
}
//---------------------------------------------------------------------------
TGapStringList::~TGapStringList()
{
    Clear();
}
//---------------------------------------------------------------------------
void TGapStringList::set_item(unsigned int ix, wchar_t* const elem)
{
    if (!elem || !wcslen(elem))
    {
        ClearIndex(ix);
        return;
    }

    if (ItemSet(ix))
        delete[] inherited::Items[ix];
    wchar_t *c = new wchar_t[wcslen(elem) + 1];
    wcscpy(c, elem);
    inherited::Items[ix] = c;
}
//---------------------------------------------------------------------------
void TGapStringList::DeleteProc(wchar_t* &c)
{
    delete[] c;
}
//---------------------------------------------------------------------------
enum TStringLengthType { sltByte, sltWord, sltInt, sltByteAddNULL, sltWordAddNULL, sltIntAddNULL };
void write(const wchar_t* str, FILE *f, TStringLengthType type, int len = -1 );
int read(wchar_t* &str, FILE *f, TStringLengthType type);
void TGapStringList::WriteData(FILE *f, wchar_t* &dat)
{
    write(dat, f, sltWord);
}
//---------------------------------------------------------------------------
void TGapStringList::ReadData(FILE *f, wchar_t* &dat)
{
    if (utf8)
    {
        if (version >= 12)//added 2011.04.04
                    read(dat, f, sltWordAddNULL);
        else
        {
            read(dat, f, sltWord);
            if (!dat)
            {
                dat = new wchar_t[1];
                dat[0] = 0;
            }
        }
    }
    else
    { //backwards compatible. remove 2011 july    since group ver 11
        word len;
        fread(&len, sizeof(word), 1, f);
        char *tmp = new char[len + 1];
        if (len)
            fread(tmp, sizeof(char), len, f);
        dat = new wchar_t[len + 1];
        wcscpy(dat, UnicodeString(tmp, len).c_str());
        delete[] tmp;
    }
}
//---------------------------------------------------------------------------
void TGapStringList::CopyData(wchar_t **data, wchar_t **orig, unsigned int cnt)
{
    for (unsigned int i = 0; i < cnt; ++i)
    {
        data[i] = new wchar_t[wcslen(orig[i]) + 1];
        wcscpy(data[i], orig[i]);
    }
}
//---------------------------------------------------------------------------
void TGapStringList::LoadFromFile(FILE *f, bool fileutf8, byte fileversion)
{
    utf8 = fileutf8;
    version = fileversion;
    inherited::LoadFromFile(f);
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(45);
#endif
