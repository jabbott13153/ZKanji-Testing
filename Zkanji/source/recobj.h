//---------------------------------------------------------------------------

#ifndef recobjH
#define recobjH
//---------------------------------------------------------------------------

class Bits;
class TPointListEx;
class TKanjiElement;

struct TCompareData
{
    word index;
    int distance;
};

struct TRecognizeData
{
    TCompareData data;
    Bits *pos; //relative position of other strokes to this one
};

struct TRecPointData
{
    TCompareData *data;
    TCompareData *cdata;
    TPointListEx *original;
    TPointListEx *points;
    Bits *pos;
};

class TRecPointList : public TTemplateList<TRecPointData, true>
{
private:
    typedef TTemplateList<TRecPointData, true> inherited;
    TPointListEx* get_point(int ix);
    TPointListEx* get_orig(int ix);

    TRect dimensions;

    int pos_difference(const Bits &p1, const Bits &p2);
    word get_width();
    word get_height();

    using inherited::Add;
    using inherited::Delete;
protected:
    void FreeItem(TRecPointData *item, int ix);
public:
    virtual ~TRecPointList()
    {
        Clear();
    }
    int add(TPointListEx *pt);
    void compare(int *results, int &cnt, int order_errorlimit, int cntlimit, bool kanji, bool general);

    virtual void Clear();
    void remove(int ix);

    __property TPointListEx *points[int ix] = { read = get_point };
    __property TPointListEx *original[int ix] = { read = get_orig };

    __property word width = { read = get_width };
    __property word height = { read = get_height };
};

class TStrokeModels : public TTemplateList<TPointListEx, true>
{
private:
    typedef TTemplateList<TPointListEx, true> inherited;

    virtual void FreeItem(TPointListEx *item, int ix);
public:
    virtual ~TStrokeModels()
    {
        Clear();
    }
    void ReadModels(UnicodeString fname);
    void LoadFromFile(FILE *f, int version, bool main);
    void SaveToFile(FILE *f);
};

extern TStrokeModels *models; //built-in model strokes used in kanji recognition
extern TStrokeModels *cmodels; //strokes and positions for characters used in character recognition

#endif
