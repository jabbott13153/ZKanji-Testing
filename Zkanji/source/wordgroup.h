//---------------------------------------------------------------------------

#ifndef wordgroupH
#define wordgroupH
//---------------------------------------------------------------------------

#include <stdio.h>

class TWordGroupStudy;
class TWordCollection;
class TWordIndexList;
class TWordList;
struct TConvertItem;

class TWordGroup;
class TWordStatGlobal;
class TWordGroupList;

// Base class for word groups.
class TWordGroup
{
private:
    UnicodeString fname;
    bool fchecked;
protected:
    TWordIndexList *flist;
    //TWordList *words;
    TWordCollection* get_collection();
    TWordList* get_words();

    void FreeWordLink(int ix);
    void set_name(UnicodeString aname);

    TWordStatGlobal* get_wordstat(int ix);

    virtual void RemoveLine(int windex);
    virtual void RemoveMeaning(int windex, char meaning);
    virtual void MoveMeaning(int windex, char meaning, char newpos);

    virtual void CleanUp(); // Remove items with a negative index from the group. Only used on dictionary update!

    friend class TWordGroupList;
    friend class TWordStateList;
public:
    TWordGroup(TWordCollection *acollection, UnicodeString aname);
    virtual ~TWordGroup();

    //virtual void Export(FILE *f);

    virtual void DeleteWord(int ix);
    int AddWord(int aindex, char ameaning);

    __property TWordIndexList *list = { read = flist };
    __property UnicodeString name = { read = fname, write = set_name };
    __property bool checked = { read = fchecked, write = fchecked };
    __property TWordCollection *collection = { read = get_collection };
    __property TWordList *words = { read = get_words };

    __property TWordStatGlobal *WordStats[int ix] = { read = get_wordstat };
};

class TWordGroupList : public TTemplateList<TWordGroup, true>
{
private:
    typedef TTemplateList<TWordGroup, true> inherited;

    TWordGroupStudy* get_test(int ix);
    TWordIndexList* get_list(int i);
    bool get_checked(int i);
    void set_checked(int i, bool val);
    UnicodeString get_name(int i);
    void set_name(int i, UnicodeString val);

    bool fstudy;
    TWordCollection *fcollection;

    int get_checkedcount();
    int AddWord(int group, int aindex, char ameaning);
public:
    TWordGroupList(TWordCollection *acollection, bool astudy);
    virtual ~TWordGroupList();

    bool WordInGroup(int group, int aindex, int ameaning);

    void LoadFromFile(TWordCollection *collection, TConvertItem *wt, int cnt, FILE *f, byte ver);
    void SaveToFile(FILE *f);

    //void Export(FILE *f);

    int Add(const UnicodeString s);
    using inherited::Add;
    void DeleteWord(int group, int ix);
    void RemoveLine(int windex);
    void RemoveMeaning(int windex, char meaning);
    void MoveMeaning(int windex, char meaning, char newpos);
    using inherited::IndexOf;
    int IndexOf(UnicodeString s);

    __property TWordIndexList* Lists[int i] = { read = get_list };
    __property UnicodeString Names[int i] = { read = get_name, write = set_name };
    __property bool Checked[int i] = { read = get_checked, write = set_checked };
    __property int CheckedCount = { read = get_checkedcount };
    __property TWordGroupStudy* Tests[int i] = { read = get_test };

    __property bool study = { read = fstudy };
};

#endif
