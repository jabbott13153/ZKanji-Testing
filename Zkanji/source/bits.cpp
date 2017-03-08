//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include <string.h>

#include "bits.h"

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

static TGlobalTest tester1(8);
#endif

int min(int a, int b);
//---------------------------------------------------------------------------
Bits::Bit::Bit(Bits *aowner, const byte p) : fowner(aowner), pos(p)
{
    ;
}
//---------------------------------------------------------------------------
Bits::Bit::Bit(const Bit &b) : fowner(b.fowner), pos(b.pos)
{
    ;
}
//---------------------------------------------------------------------------
Bits::Bit Bits::Bit::operator= (const bool val)
{
    fowner->setbit(pos, val);
    return *this;
}
//---------------------------------------------------------------------------
Bits::Bit Bits::Bit::operator= (Bit const &b)
{
    fowner->setbit(pos, b.value());
    return *this;
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
Bits::Bits() : fsize(0), fbits(NULL)/*,ownbits(true)*/, fmaxsize(255)
{
    ;
}
//---------------------------------------------------------------------------
/*Bits::Bits(byte *abits, byte amaxsize)
 {
 fsize = amaxsize;
 fbits = abits;
 ownbits = false;
 fmaxsize = amaxsize;
 }*/
//---------------------------------------------------------------------------
Bits::~Bits()
{
    if (fbits/*&& ownbits*/)
        delete[] fbits;
}
//---------------------------------------------------------------------------
Bits::Bits(const Bits &b) : fbits(NULL), fsize(0), fmaxsize(b.fmaxsize)/*,ownbits(b.ownbits)*/
{
    *this = b;
}
//---------------------------------------------------------------------------
Bits::Bits(const void *data, const byte unitsize, const byte count, const bool bits) : /*ownbits(true),*/fbits(NULL), fmaxsize(255)
{
    if (bits && count > fmaxsize || !bits && (fsize + 7) / 8 > fmaxsize)
        throw L"Bits array cannot contain more than 255 bits!";

    fsize = bits ? count : (unitsize * count) * 8;
    fbits = new byte[(fsize + 7) / 8];

    byte d;
    for (int ix = 0; ix < (fsize + 7) / 8; ++ix)
    {
        d = (ix % unitsize) + unitsize * (ix / unitsize);
        memcpy(fbits + ix, (char*)data + d, sizeof(byte));
    }
}
//---------------------------------------------------------------------------
Bits& Bits::operator= (const Bits & b)
{
    if (b.fbits == fbits)
        return *this;

    //if (ownbits)
    delete[] fbits;

    fmaxsize = b.fmaxsize;
    //ownbits = b.ownbits;

    //if (ownbits)
    // {
    fsize = 0;
    fbits = NULL;

    Grow(b.fsize);

    if (b.fsize)
        memcpy(fbits, b.fbits, sizeof(byte)*((b.fsize + 7) / 8));
    /*} else {
     fsize = b.fsize;
     fbits = b.fbits;
     }
     */

    return *this;
}
//---------------------------------------------------------------------------
void Bits::Grow(byte newsize)
{
    /*if (!ownbits)
     throw L"Cannot change size of a borrowed array!";
     */
    if (fsize > newsize)
        return;

    newsize = min(newsize, fmaxsize);

    if ((fsize + 7) / 8 < (newsize + 7) / 8)
    {
        byte *temp = fbits;
        byte nsize = (newsize + 7) / 8;
        fbits = new byte[nsize];
        byte oldsize = (fsize + 7) / 8;
        memset(fbits + oldsize, 0, nsize - oldsize);
        if (temp)
        {
            memcpy(fbits, temp, sizeof(byte)*oldsize);
            delete[] temp;
        }
    }

    fsize = newsize;
}
//---------------------------------------------------------------------------
void Bits::Shrink(byte newsize)
{
    newsize = min(fsize, newsize);

    if ((fsize + 7) / 8 > (newsize + 7) / 8)
    {
        if (!newsize)
        {
            delete[] fbits;
            fbits = NULL;
        }
        else
        {
            byte *temp = fbits;
            byte nsize = (newsize + 7) / 8;
            fbits = new byte[nsize];
            if (temp)
            {
                memcpy(fbits, temp, sizeof(byte)*nsize);
                delete[] temp;
            }
        }
    }

    fsize = newsize;
}
//---------------------------------------------------------------------------
void Bits::setbit(byte bit, bool val)
{
    if (bit >= fmaxsize)
        throw L"Cannot set bit above maximum size!";
    else //if (ownbits)
        Grow(bit + 1);

    byte b = bit / 8;
    byte m = bit % 8;

    if (val)
        fbits[b] |= (1 << m);
    else
        fbits[b] &= 0xff ^ (1 << m);

}
//---------------------------------------------------------------------------
bool Bits::getbit(byte bit) const
{
    if (bit >= fsize)
        return false;

    byte b = bit / 8;
    byte m = bit % 8;

    return (fbits[b] & (1 << m));

}
//---------------------------------------------------------------------------
Bits::Bit Bits::operator[](byte bit)
{
    if (bit >= fmaxsize)
        throw L"Bit over maximum size!";
    else if (bit >= fsize)//if (ownbits)
        Grow(bit + 1);

    Bit b(this, bit);
    return b;
}
//---------------------------------------------------------------------------
const bool Bits::operator[](byte bit) const
{
    if (bit >= fmaxsize)
        throw L"Bit over maximum size!";

    return getbit(bit);
}
//---------------------------------------------------------------------------
void Bits::checkall(byte cnt, bool check)
{
    if (cnt >= fmaxsize)
        throw L"New size over maximum size!";
    else if (cnt > fsize) //if (ownbits)
        Grow(cnt);
    else
        Shrink(cnt);

    while (cnt > 0)
        setbit(--cnt, check);
}
//---------------------------------------------------------------------------
void Bits::SaveToFile(FILE *f)
{
    /*if (!ownbits)
     L"Cannot save borrowed bits to file!";
     */
    fwrite(&fsize, sizeof(byte), 1, f);
    if (fsize)
        fwrite(fbits, sizeof(byte), (fsize + 7) / 8, f);
}
//---------------------------------------------------------------------------
void Bits::LoadFromFile(FILE *f)
{
    byte nsize;
    fread(&nsize, sizeof(byte), 1, f);
    if (nsize != fsize)
    {
        delete[] fbits;
        fbits = new byte[(nsize + 7) / 8];
        fsize = nsize;
    }
    if (fsize)
        fread(fbits, sizeof(byte), (fsize + 7) / 8, f);
}
//---------------------------------------------------------------------------
bool Bits::equals(bool val) const
{
    if (fsize == 0)
        return !val;

    bool eq = true;
    byte size = (fsize + 7) / 8;
    for (int i = 0; i < size - (fsize % 8 ? 1 : 0) && eq; i++)
        eq = (fbits[i] == (val ? 0xff : 0));
    if (eq)
        for (int i = 0; i < fsize % 8 && eq; i++)
            eq = (fbits[size - 1] & (1 << i)) == (val ? (1 << i) : 0);

    return eq;
}
//---------------------------------------------------------------------------
int Bits::difference(const Bits &b) const
{
    int r = 0;
    for (int ix = (fsize > b.fsize ? fsize : b.fsize) - 1; ix >= 0; --ix)
        if (b[ix] != getbit(ix))
            r++;
    return r;
}
//---------------------------------------------------------------------------
/*UnicodeString Bits::toString() const
 {
 AnsiString s = L"";
 for (int ix = 0; ix < fsize; ++ix)
 s = (getbit(ix) ? L"1" : L"0") + s;
 return s;
 }*/
//---------------------------------------------------------------------------
byte Bits::size() const
{
    return fsize;
}
//---------------------------------------------------------------------------
byte Bits::checked() const
{
    byte r = 0;
    for (byte ix = 0; ix < fsize; ++ix)
        if (getbit(ix))
            r++;
    return r;
}
//---------------------------------------------------------------------------
double Bits::percent() const
{
    if (!fsize)
        return 0;
    return 100.0 * (double(checked()) / fsize);
}
//---------------------------------------------------------------------------
byte Bits::arr_size(byte unitsize) const
{
    unitsize *= 8;
    return (fsize + unitsize - 1) / unitsize;
}
//---------------------------------------------------------------------------
void Bits::arr_copy(void *buf, byte unitsize) const
{
    byte d;
    for (int ix = 0; ix < (fsize + 7) / 8; ++ix)
    {
        d = (ix % unitsize) + unitsize * (ix / unitsize);
        memcpy((char*)buf + d, fbits + ix, sizeof(byte));
    }
}
//---------------------------------------------------------------------------
Bits& Bits::operator<< (int sh)
{
    if (!sh || sh < 0 || !fsize)
        return *this;

    if (sh >= fmaxsize)
    {
        Grow(fmaxsize);
        memset(fbits, 0, sizeof(byte)*(fmaxsize + 7) / 8);
        fsize = fmaxsize;
        return *this;
    }

    int g = min(sh, fmaxsize - fsize);
    if (g > 0)
        Grow(fsize + g);

    if (sh < 8)
    {
        for (int ix = (fsize - 1) / 8; ix >= 0; --ix)
            fbits[ix] = (fbits[ix] << sh) | (ix > 0 ? (fbits[ix - 1] >> (8 - sh)) : 0);
    }
    else {
        int blocks = sh / 8;
        for (int ix = (fsize - 1) / 8; ix >= blocks; --ix)
            fbits[ix] = (fbits[ix - blocks] << (sh % 8)) | (ix - blocks > 0 ? (fbits[ix - blocks - 1] >> (8 - (sh % 8))) : 0);
        memset(fbits, 0, sizeof(byte)*(blocks));
    }

    return *this;
}
//---------------------------------------------------------------------------
Bits& Bits::operator>> (int sh)
{
    if (!sh || sh < 0)
        return *this;

    if (sh >= fsize)
    {
        Shrink(0);
        return *this;
    }

    int top = (fsize - 1 - sh) / 8;
    if (sh < 8)
    {
        for (int ix = 0; ix <= top; ++ix)
            fbits[ix] = (fbits[ix] >> sh) | (ix < (fsize - 1) / 8 ? (fbits[ix + 1] << (8 - sh)) : 0);
    }
    else
    {
        int blocks = sh / 8;
        for (int ix = 0; ix <= top; ++ix)
            fbits[ix] = (fbits[ix + blocks] >> (sh % 8)) | (ix + blocks < (fsize - 1) / 8 ? (fbits[ix + blocks + 1] << (8 - (sh % 8))) : 0);
        //for (int ix = (fsize-sh)/8; ix < (fsize+7)/8; ++ix)
        //fbits[ix] = 0;
    }

    Shrink(fsize - sh);
    return *this;
}
//---------------------------------------------------------------------------
void Bits::limit_length(byte newlen)
{
    newlen = min(fmaxsize, newlen);
    if (fsize <= newlen)
        return;

    Shrink(newlen);
}
//---------------------------------------------------------------------------
byte Bits::checked_count() const
{
    int cnt = 0;
    for (int ix = 0; ix < fsize; ++ix)
        if ((*this)[ix])
            cnt++;

    return cnt;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(9);
#endif
