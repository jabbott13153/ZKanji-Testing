//---------------------------------------------------------------------------

#ifndef bitsH
#define bitsH
//---------------------------------------------------------------------------
#include <stdio.h>

typedef unsigned short word;
typedef unsigned char byte;

class Bits
{
private:
    byte *fbits;
    byte fsize;

    byte fmaxsize;
    //bool ownbits;

    void setbit(byte bit, bool val);
    bool getbit(byte bit) const;
    void Grow(byte bit);
    void Shrink(byte bit);

    class Bit
    {
    private:
        Bits *fowner;
        const byte pos;
    public:
        Bit(Bits *aowner, const byte p);
        Bit(const Bit &b);

        Bit operator = (const bool val);
        Bit operator = (Bit const &b);
        bool value() const
        {
            return fowner->getbit(pos);
        }
        operator bool() const
        {
            return value();
        }
    };

public:
    friend Bit;

    Bits();
    Bits(const Bits &b);
    //initialize bits with buffer data of unitsize sized elements. count is either the bitcount (when bits = true) or the number of items in the buffer
    Bits(const void *data, const byte unitsize, const byte count, const bool bits = false);
    //Bits(byte *abits, byte amaxsize);
    ~Bits();

    Bits& operator = (const Bits & b);
    Bits& operator << (int sh);
    Bits& operator >> (int sh);

    int difference(const Bits &b) const;

    Bit operator[](byte bit);
    const bool operator[](byte bit) const;
    void LoadFromFile(FILE *f);
    void SaveToFile(FILE *f);
    void checkall(byte cnt, bool check = true);
    bool equals(bool val) const; //returns true when all bits are set to val

    //UnicodeString toString() const;

    byte size() const;
    byte checked() const;
    double percent() const; //returns percent of 1 bits in fsize bits

    byte arr_size(byte unitsize) const;
    void arr_copy(void *buf, byte unitsize) const;
    void limit_length(byte newlen); //deletes elements above given bit size
    byte checked_count() const; //return number of bits checked
};

#endif
