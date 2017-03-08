//---------------------------------------------------------------------------

#ifndef sentencesH
#define sentencesH
//---------------------------------------------------------------------------

#include <stdio.h>
#include "genworddata.h"

#define MAX_BLOCK_COUNT 2000

struct TExampleBlockData
{
    int position;
    int packedsize;
    int unpackedsize;

    bool loaded;
};

// Example data in TStoredWord. It was in a TWord in the past.
struct TExampleData
{
    word block; // Block of example (each example is compressed to 100 line blocks).
    byte num; // Index of example inside the block.
    //char meaning; // The meaning of the word in the example
    byte wordnum; // Index of word in the example's word data.
};

/*
 struct TWordExampleData : public TGenericWordData {
 TExampleData *data; // count long array
 word count;
 virtual void Free() { delete[]  data; }
 };
 */

// Word data in a TExampleSentence.
struct TExampleWordData
{
    TStoredWord *data;

    //int index; // Index of word in words list.
    //char meaning; // Which meaning is connected with the sentence. -1 means not specified.
    word position; // Character position of word in the sentence.
    byte length; // Length in characters of the word in the sentence. might differ from the length of the dictionary form.
};

// Example sentence data.
struct TExampleSentence
{
    byte wordscnt; // Number of recognized words in the japanese text from the dictionary.
    TExampleWordData *words; // Data for the words.
    wchar_t *japanese; // The japanese text (that has the words).
    wchar_t *translation; // The sentence translation.
};

// Sentences in a single block.
struct TSentenceList
{
    TExampleSentence sentence[100];
    TSentenceList *next;
    TSentenceList *prev;
    word block; // Index of the block, 2000 at most.
};

class TLineObj;
class TExampleSentencer
{
private:
    FILE *f;

    static TTemplateList<TLineObj, false> controls; // List of controls displaying example data.

    int fcount;

    UnicodeString fname;
    int fmemsize;
    int fmemused;

    TExampleBlockData *blocks;
    int blockcnt; // Number of allocated blocks. This is NOT the number of blocks in memory.

    TSentenceList *list;
    TSentenceList *oldest;

    void __fastcall delete_block(TSentenceList *alist);
    void __fastcall set_memsize(int val);

    TSentenceList* __fastcall get_block(word ablock); // Return block when loaded or null.

    TExampleSentence* __fastcall get_sentence(word ablock, byte aline);
public:
    __fastcall TExampleSentencer(UnicodeString filename/*, int version*/);
    __fastcall virtual ~TExampleSentencer();

    static void RegisterLineObj(TLineObj *obj);
    static void UnregisterLineObj(TLineObj *obj);

    void clear_cache();

    __property int cache_size = { read = fmemsize, write = set_memsize };
    __property TExampleSentence* sentences[word block][byte line] = { read = get_sentence };
};

extern TExampleSentencer *Examples;

#endif
