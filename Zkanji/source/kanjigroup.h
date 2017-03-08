//---------------------------------------------------------------------------

#ifndef kanjigroupH
#define kanjigroupH
//---------------------------------------------------------------------------

#include "kanjicard.h"

class TWordCollection;
class TKanjiGroupList;

class TKanjiGroup : public TKanjiCardList
{
private:
    TKanjiGroupList *fowner;
    UnicodeString fname;
    bool fchecked;
protected:
    __property TKanjiGroupList *owner = { read = fowner };
public:
    TKanjiGroup(TKanjiGroupList *aowner, UnicodeString aname, FILE *f, int version, TGapList<TIntegerList*> *exampleconvert);
    TKanjiGroup(TKanjiGroupList *aowner, UnicodeString aname);

    virtual void LoadFromFile(FILE *f, int version, TGapList<TIntegerList*> *exampleconvert);
    virtual void SaveToFile(FILE *f);

    void Export(FILE *f);

    virtual bool Studied()
    {
        return false;
    }

    __property UnicodeString name = { read = fname, write = fname };
    __property bool checked = { read = fchecked, write = fchecked };
};

class TKanjiGroupList : public TTemplateList<TKanjiGroup, true>
{
private:
    typedef TTemplateList<TKanjiGroup, true> inherited;

    TWordCollection *fcollection;

    UnicodeString get_name(int i);
    void set_name(int i, UnicodeString val);
    int get_count()
    {
        return Count;
    };

    TKanjiGroup* get_item(int i);
    TKanjiList* get_list(int i);
    bool get_checked(int i);
    void set_checked(int i, bool val);
public:
    TKanjiGroupList(TWordCollection *coll);
    virtual ~TKanjiGroupList();

    int Add(TKanjiGroup *g);
    int Add(UnicodeString s);
    int AddKanji(int group, int kindex);
    void RemoveKanji(int group, int ix);

    int IndexOf(UnicodeString &groupname); // Finds the group with the passed name. Updates groupname to contain the name with correct character case.

    void LoadFromFile(FILE *f, int version, TGapList<TIntegerList*> *exampleconvert);
    void SaveToFile(FILE *f);

    void Export(FILE *f);

    int CountChecked();

    __property bool Checked[int i] = { read = get_checked, write = set_checked };
    __property UnicodeString Names[int ix] = { read = get_name, write = set_name };
    __property TWordCollection *collection = { read = fcollection };
};

#endif
