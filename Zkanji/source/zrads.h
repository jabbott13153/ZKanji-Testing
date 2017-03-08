//---------------------------------------------------------------------------

#ifndef zradsH
#define zradsH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
//---------------------------------------------------------------------------
#include <stdio.h>
#include "searchtree.h"

class TSimpleSearchTree;

struct TKanjiRadical
{
    wchar_t ch; //unicode character representing radical
    wchar_t *list; //list of kanji containing this radical
    byte strokes; //number of strokes in radical
    byte radical; //number of radical
    wchar_t *names; //list for names of radical (many radicals can have the same name.)
};

class TRadicalList : public TTemplateList<TKanjiRadical, true>
{
public:
    typedef TTemplateList<TIntegerList, true> TRadGroup;
private:
    typedef TTemplateList<TKanjiRadical, true> inherited;

    /*void setlength(int ix, word len);
     word getlength(int ix);*/
    //TKanjiRadical* get_item(int ix);
    int get_group_count();
    UnicodeString get_group_string(int ix);

    //void Delete(int ix);

    TRadGroup *fgroups;

    class TRadicalSearchTree : public TTextSearchTreeBase
    {
    private:
        typedef TTextSearchTreeBase inherited;
        virtual UnicodeString DoGetWord(int index);
        TRadicalList *fowner;

        using inherited::Expand;
    public:
        TRadicalSearchTree(TRadicalList *aowner); //creates empty tree
        void FindElement(const wchar_t *c, TIntegerList *list); //returns the element number in list under the nod, not the elements themselves. Doesn't look for inflections or exact words! This is currently used for finding radicals by name so the entered string must not contain spaces and has to be correct romanized kana

        void ExpandTree(UnicodeString str, int index, byte name);
    };
    TRadicalSearchTree *ntree;

    friend TRadicalSearchTree;
protected:
    virtual void FreeItem(TKanjiRadical *item, int ix);
public:
    TRadicalList();
    virtual ~TRadicalList();

    void ImportAdd(wchar_t *w);
    //void Clear();

    void Reset();
    void BeginAdd();
    void EndAdd();
    void AddRad(word ix, bool group); //group adds all radicals from same group
    void SetRadicals(TRadGroup *l);
    void GetRadicals(TRadGroup* &l);
    bool MatchRadicals(wchar_t kanji, TRadGroup *l);

    void SaveToFile(FILE *f);
    void LoadFromFile(FILE *f, int version);

    void Filter(UnicodeString name, byte strokes, TIntegerList *l, bool exact); //l recieves data! Create it before calling this function!

    //__property TKanjiRadical* Items[int] = { read = get_item };
    __property int GroupCount = { read = get_group_count };
    __property UnicodeString GroupString[int] = { read = get_group_string };
    __property TRadGroup* Groups = { read = fgroups };
};

extern TRadicalList *radlist;

#endif
