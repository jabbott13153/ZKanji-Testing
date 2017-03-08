//---------------------------------------------------------------------------

#ifndef kanjielementH
#define kanjielementH
//---------------------------------------------------------------------------

enum TPointType {ptMoveTo, ptLineTo, ptCurve};
enum TElemPattern { epUndef, epEbl, epEblt, epElt, epEltr, epEtr, epErbl, epEw, epElr, epW, epH2, epH3, epV2, epV3, epV4, epT, epC, epB, epX, epHira, epKata };

extern const word NO_OWNER;

struct TRecognizeData;

struct TElementVariant
{
    int element;
    byte variant;
};
TElementVariant ElementVariant(int aelem, byte avar);
bool operator == (TElementVariant const & a, TElementVariant const & b);
bool operator != (TElementVariant const & a, TElementVariant const & b);

class TRepository : public TTemplateList<TElementVariant, true>
{
private:
    typedef TTemplateList<TElementVariant, true> inherited;
    UnicodeString fname;
public:
    TRepository(UnicodeString aname);
    virtual ~TRepository()
    {
        Clear();
    }

    int IndexOf(word aelem, byte avar);

    __property UnicodeString Name = { read = fname, write = fname };
};

class TRepositoryList : public TTemplateList<TRepository, true>
{
private:
    typedef TTemplateList<TRepository, true> inherited;
    UnicodeString get_name(int ix);
    void set_name(int ix, UnicodeString val);

    using inherited::IndexOf;
    int IndexOf(UnicodeString aname);
    int get_cnt(int ix);
    TElementVariant get_repitem(int rep, int ix);
public:
    virtual ~TRepositoryList()
    {
        Clear();
    }

    int Add(UnicodeString aname);
    int Add(int arepos, word aelem, byte avar);
    void RemoveItem(int rep, int ix);

    void VariantRemoved(short aelement, byte avar);
    void ElementRemoved(short aelement);
    void VariantChanged(short srcelem, byte srcvar, short destelem, byte destvar);

    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f, int version);

    __property UnicodeString Names[int ix] = { read = get_name, write = set_name };

    __property int Counts[int ix] = { read = get_cnt };
    __property TElementVariant RepItem[int rep][int ix] = { read = get_repitem };
};

struct TElementPoint
{
    short x;
    short y;
    short c1x;
    short c1y;
    short c2x;
    short c2y;
    TPointType type;
};

struct TElementStroke
{
    TElementPoint *points;
    byte pointcount;

  //the first/last segment in the stroke is drawn with a thinning pen, forming a thin pointy tip.
  //TElementStrokeType enumerated type is saved in this variable, but because the compiler uses a whole integer for it in memory, we substitute it here with a byte
    byte tips; //added 2011 feb 15. for v0.573
};

struct TPartPosition
{
    byte variant;
    word height;
    word width;
    short x;
    short y;
};

struct TKanjiVariant
{
    TElementStroke *strokes;
    byte strokecount;

    word width;
    word height;
    short x;
    short y;

    bool standalone; //kanji parts may be present even when element is standalone, but they are not drawn
    byte centrepoint;

    TPartPosition *positions;
};

struct TKanjiElement
{
    word owner; //kanji index when directly part of kanji. Numbers above 10000 represent unicode. The lowest acceptable unicode value is 12288 (0x3000), so this is possible. 0x2000 means no owner;
    TElemPattern pattern;

    TKanjiVariant *variants;
    byte variantcount;

    short parts[4]; //index of sub-elements
    word *parents;
    word parentcnt;

    TRecognizeData *recdata;
};

enum TDrawBitDirection { dbdUnset, dbdLeft, dbdUp, dbdRight, dbdDown, dbdUpLeft, dbdUpRight, dbdDownLeft, dbdDownRight };

class TElementList : public TTemplateList<TKanjiElement, true>
{
private:
    typedef TTemplateList<TKanjiElement, true> inherited;

    byte version;

    void FreeItem(TKanjiElement *item, int ix);
    byte get_stroke_count(int ix, byte var);
    TKanjiVariant* get_variant(int ix, byte var);

    int StepsRequired(short parentelement, byte parentvariant, short toelement, short tovariant);

    TRepositoryList *repositories;

  //names for element variants
    struct TElementNames
    {
        byte count; //number of variants that get a name
        wchar_t *names; //the names are separated by \0 characters. if only the first variant's name is set, it is used for all variants, but if any other one is set as well, they are only used for the specific variant
    };
    TGapList<TElementNames> elementnames;

    int ParentPos(short aelement, short aparent);
    void RemoveParent(short aelement, short aparent);
    void AddParent(short aelement, short aparent);

    void __Reposition(short aelement, byte avariant, TGapList<int> &gl, bool reparts);
    void __RepositionAllHelper(int aelement, TGapList<int> &gl);
    void Reposition(short element, bool reparts); //Determines element variant size and coordinates. Also repositions in other elements when set.

    void DeleteRecognizeData(int element);
    void GenerateRecognizeData(int element);

    int DrawElement(TRect r, HDC handle, double width, int color, short element, char variant, char from, char to, bool bits = false, bool draw = false, int speed = -1, int bit = -1, TDoublePoint *startpoint = NULL, TDrawBitDirection *dir = NULL, TRect *bitrect = NULL);

    int VariantNamesLength(int element);
    wchar_t* VariantNameStart(int element, byte avar);

    int get_repcount();
    int get_repscount(int ix);
    UnicodeString get_repname(int ix);
    void set_repname(int ix, UnicodeString val);
    TElementVariant get_repitem(int repix, int ix);
public:
    TElementList(word count);
    TElementList(UnicodeString fname);
    virtual ~TElementList();

    void SaveToFile(UnicodeString fname);
    void LoadFromFile(UnicodeString fname);

    void Draw(TRect r, HDC handle, double width, int color, short element, char variant = 0, char from = -1, char to = -1, TDoublePoint *startpoint = NULL, TDrawBitDirection *dir = NULL);
  //measures and draws a single part of a stroke from a given element. MeasureBit returns the number of bits
  //for the given pixel speed. Speed of -1 guesses speed from rectangle size
    int MeasureBit(TRect r, double width, short element, char variant, char stroke, int speed = -1, TDoublePoint *startpoint = NULL, TDrawBitDirection *dir = NULL);
    int DrawBit(TRect r, HDC handle, double width, int color, short element, char variant, char stroke, int bit, int speed = -1, TDoublePoint *startpoint = NULL, TDrawBitDirection *dir = NULL, TRect *bitrect = NULL);

    void RepositionAll();

    void Merge(short srcelem, short destelem); //merge src into dest and delete src
    void Update(short element, TKanjiElement *elem); //copy elem to item and update its parents data
    void Duplicate(short element); //creates a copy of element and adds it to the end of the list
    void PositionUpdate(short element, byte avar); //update position (and recognize data) of variant
    void ChangeVariant(short element, byte avar, byte newvar);
    bool HasPart(short element, short part);
    void RegenerateRecognizeData(int element);
    void DeleteElement(short element);
  //adds and removes character model to the cmodel list.
    void AddCharacterModel(TKanjiElement *elem, byte variant, byte stroke);
    void AddKanjiModel(TKanjiElement *elem, byte variant, byte stroke);
    void RemoveKanjiModel(int ix);
    void RemoveCharacterModel(int ix);

    void RemoveVariant(short aelement, byte avar);
    void DuplicateVariant(short aelement, byte avar);
    void ElementRemoved(short aelement);
    void DecrementElement(short aelement);

    UnicodeString VariantName(short aelement, byte avar, bool global_accepted = false);
    void RemoveVariantName(short aelement, byte avar);
    void SetVariantName(short aelement, byte avar, UnicodeString name);
    void DeleteVariantNames(short aelement);

    int AddRepository(UnicodeString aname);
    void DeleteRepository(int arepos);
    void RemoveFromRepository(int arepos, int pos);
    int ToRepository(int arepos, word aelem, byte avar);
    int Repository(int arepos, word & aelem, byte & avar);

    int IndexOfChar(wchar_t ch);

    virtual void Clear();

    __property byte Strokes[int ix][byte var] = { read = get_stroke_count };
    __property TKanjiVariant *Variants[int ix][byte var] = { read = get_variant };

    __property int RepositoryCount = { read = get_repcount };
    __property UnicodeString RepositoryNames[int ix] = { read = get_repname, write = set_repname };

    __property int RepositoriesCount[int ix] = { read = get_repscount };
    __property TElementVariant RepositoryItem[int rep][int ix] = { read = get_repitem };
    __property byte Version = { read = version };
};

void elem_free(TKanjiElement*&);
TKanjiElement* elem_copy(const TKanjiElement *, bool copy_rec);

void position_elem(TPartPosition &part, TElemPattern pattern, int ix, int count);

extern TElementList *elements;
extern word elementcount;

#endif
