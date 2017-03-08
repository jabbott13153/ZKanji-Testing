//---------------------------------------------------------------------------

#ifndef importH
#define importH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
#include "baseform.h"

#include "words.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

class TfImporter : public TBaseForm
{
__published: // IDE-managed Components
    TRichEdit *edit;
    TBevel *bBottom;
    TBevel *bTop;
    TLabel *Label1;
    TButton *btnClose;
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall editKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall editKeyPress(TObject *Sender, wchar_t &Key);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);

private: // User declarations
    bool aborted;
    bool finished;

    bool get_able();
    void set_able(bool value);
    void set_aborted(bool value);
public: // User declarations
    __fastcall TfImporter(TComponent* Owner);

    void Abort();
    void Finish();

    __property bool Aborted = { read = aborted };
    __property bool Finished = { read = finished};
    __property bool Cancellable = { read = get_able, write = set_able };
};
//---------------------------------------------------------------------------
extern PACKAGE TfImporter *fImporter;
//---------------------------------------------------------------------------

class TWordCollection;
class TImportKanjiGroup;
class TImportWordGroup;

// Structures used during import:

// Settings selectable in the import settings dialogs:
enum TGroupImportOptions { gioMerge, gioRename, gioReplace, gioSkip, gioChoose };
enum TImportWordOptions { iwoAuto = 0, iwoChoose = 1, iwoSkip = 2 };
struct TImportOptions
{
    // Options for the group import dialog:
    bool wordgroups; // Import word groups.
    bool modifydict; // Allow modification to the dictionary when importing word groups or for dictionary updates.
    TGroupImportOptions  wduplicate; // What to do on conflict with existing groups.
    bool wordsubst; // Pick substitute words and definitions for missing word entries.

    bool kanjigroups; // Import kanji groups;
    bool kanjiuserdef; // Import user kanji definitions.
    TGroupImportOptions  kduplicate; // What to do on conflict with existing groups.
    bool kanjiex; // Import example words for kanji.
    bool kanjisubst; // Pick substitutes for missing example words.
    bool kanjiexdef; // Import example definitions.

    // Options for the dictionary entry import dialog:
    bool entries; // Import entries.
    bool kanjidictdef; // Import user kanji definitions.
    bool kanjidefselect; // Select which kanji definitions should be updated, and leave the rest unchanged.
    bool skipconflict; // Don't touch existing words whose definitions differ from those in the export file. Does not apply if all definitions match but the export has new definitions.
    bool typediff; // Valid if skipconflict is true. Allows skipping words with imported definition texts matching dictionary definitions, but having different word types.
    bool updatetype; // When the text of a meaning to import matches that in the dictionary, use the imported type to update the existing ones.
    //bool keeporder; // The order of meanings in the current dictionary won't be changed to the order of the imported word. New meanings are added at the end.
    TImportWordOptions newword; // The step to take if a word imported which is not found in the current dictionary.
    TImportWordOptions newmeaning; // The step to take if an imported word found in the dictionary with exact matching definitions, and additional ones.
    TImportWordOptions missingmeaning; // The dictionary has meanings not imported, while the imported meanings match with existing meanings.

    bool fullimport; // Full dictionary import.
};

struct TExportedDefinition;
struct TExportedWordGroup;

// Holds the data of a single word read from the export file. Used for import.
class TExportedWord
{
private:
    int windex; // Index of the word in the target dictionary for import. This can be -1 for words where the kanji/kana was not found during import. The kanji and kana members can be used if this number is positive, which means a word with the index was selected to be replaced with this one.
    // Kana and kanji form of a word in case it does not exist in the current dictionary. Set to NULL in case windex is used for existing words. Words not found in the dictionary have -1 for windex by default, but can still be a positive number if the user selected a word to replace.
    wchar_t *kanji;
    wchar_t *kana;

    short freq; // Frequency of word as used for export. This value defaults to 0 if the frequency was not found in the export file.

    TTemplateList<TExportedDefinition, true> deflist;
    int get_defcnt();
    TExportedDefinition* get_def(int index);

public:
    TExportedWord(int windex, const wchar_t *akanji, const wchar_t *akana); // Pass either kanji/kana or windex. If windex is 0 or positive, kanji/kana value is not used.
    ~TExportedWord();

    void InsertMeaning(int insertindex, int meaningnum, const wchar_t *meaning, int wtype, int wnote, int wfield, int wname); // Inserts a meaning at insertindex with the passed data. The index must be correct for the definitions being sorted by meaningnum.
    bool AddToGroup(int defindex, TImportWordGroup *group, int entryindex); // Only adds definition to group if it weren't added to the group already. Adds groups to definitions sorted by the group pointer.
    int FindDefGroup(int defindex, TImportWordGroup *group); // Returns the index of the passed group in a definition.
    void ChangeDefGroup(int defindex, TImportWordGroup *oldgroup, TImportWordGroup *newgroup, int newentryindex); // Moves a definition from one group to another, updating the entry index value.

    //void DeleteDefinition(int defindex); // Deletes the given definition from the deflist and also from groups it is added to.

    wchar_t* Kanji(TWordCollection *coll); // Returns correct kanji from collection when windex is set, or the value of 'kanji'.
    wchar_t* Kana(TWordCollection *coll); // Returns correct kana from collection when windex is set, or the value of 'kana'.

    int MeaningPos(int meaningnum); // Returns the position of a meaning number in the definitions or its insert position if not in the meanings.

    //void SortDefinitions(short *deforder); // Sort the definitions in deflist by the passed array of indexes. Also sorts deforder to match the order in deflist. The nth element is the wanted position of the nth definition. -1 is for definitions that should be the last ones. Only sort definitions for group import. Dictionary imports allow modifying the dictionary word meaning order.
    void SortDefinitions(); // Sort the definitions in deflist by their exported "meaning number" value for full dictionary import.

    void CleanupGroups(const TImportOptions &options, bool keepnewdefs); // Erase empty and skipped groups. Erase existing word items which were only added to skipped groups or no groups at all. If options does not contain dictionary update, erase definitions not added to groups.

    __property short Frequency = { read = freq, write = freq };
    __property byte DefCount = { read = get_defcnt };
    __property TExportedDefinition* Def[int index] = { read = get_def };

    __property int Index = { read = windex, write = windex };
};

// Structure for the separate definitions found in an export file. Used for import.
struct TExportedDefinition
{
    TMeaningData data; // Same data that is used in words in the dictionary.

    // Meaning number. Holds the original order of the word meanings in the export file. Two TExportedDefinition definitions in TExportedWord cannot have the same number.
    int num;

    TExportedWordGroup *groups; // Array of word groups defined for the definition in the export file. This value is only used in group import.
    word groupcnt; // Number of groups for this meaning.

    TExportedDefinition();
    ~TExportedDefinition();
};

// Group data belonging to each definition.
struct TExportedWordGroup
{
    TImportWordGroup *group; // ID of group. Group names are saved in a separate list of strings so their names don't get duplicated for each meaning they should hold after an import.
    int entry; // Future index of the definition in the group. Two definitions can have the same entry number when multiple definitions as a single item in groups is implemented.
};

struct TExportedKanjiExample;
class TExportedKanji
{
private:
    wchar_t ch; // Character representation of the kanji.
    word kindex; // Index of the kanji in the kanjis list.

    wchar_t *ddef; // Dictionary definition for kanji. Only for dictionary imports.
    wchar_t *udef; // User definition for kanji. Only for group imports.

    TImportKanjiGroup **groups;
    word groupcnt;

    TTemplateList<TExportedKanjiExample, true> exlist;

    int FindExample(TWordCollection *coll, const wchar_t *kanji, const wchar_t *kana);
    void set_ddef(wchar_t *adef);
    void set_udef(wchar_t *adef);

    int get_excnt();
    TExportedKanjiExample* get_ex(int ix);
public:
    TExportedKanji(wchar_t ch);
    ~TExportedKanji();

    bool AddToGroup(TImportKanjiGroup *group); // Adds kanji to group if it is not added already. Entry index duplicates don't prevent adding, only the order will be different.
    TExportedKanjiExample* AddExampleWord(TWordCollection *coll, wchar_t *kanji, wchar_t *kana); // Returns the added example word or NULL if the 65535 limit is reached.

    int FindGroup(TImportKanjiGroup *group); // Returns the index of a group.
    void ChangeGroup(TImportKanjiGroup *oldgroup, TImportKanjiGroup *newgroup, int newentryindex); // Moves the kanji from one group to another, updating the entry index value.

    void CleanupGroups(); // Erase empty and skipped groups. Erase existing word items which were only added to skipped groups or no groups at all. If options does not contain dictionary update, erase definitions not added to groups.

    __property wchar_t Kanji = { read = ch };
    __property word Index = { read = kindex };
    __property word GroupCount = { read = groupcnt };
    __property wchar_t *DictDef = { read = ddef, write = set_ddef };
    __property wchar_t *UserDef = { read = udef, write = set_udef };

    __property word ExampleCount = { read = get_excnt };
    __property TExportedKanjiExample* Example[int] = { read = get_ex };
};

class TExportedKanjiExample
{
private:
    int windex; // Index of example word in dictionary if already added, -1 if not.

    wchar_t *kanji; // Kanji of example word if not added to dictionary.
    wchar_t *kana; // Kana of example word if not added to dictionary.

    wchar_t *meaning; // User defined meaning for example word, or NULL if not present.

    void set_meaning(wchar_t *meaning);
public:
    TExportedKanjiExample(int windex, const wchar_t *akanji, const wchar_t *akana); // Creates a TExportedKanjiExample from windex if it is 0 or higher. Otherwise kanji/kana pair is used.
    ~TExportedKanjiExample();

    wchar_t* Kanji(TWordCollection *coll); // Returns correct kanji from collection when windex is set, or the value of 'kanji'.
    wchar_t* Kana(TWordCollection *coll); // Returns correct kana from collection when windex is set, or the value of 'kana'.

    __property wchar_t* Meaning = { read = meaning, write = set_meaning };

    __property int Index = { read = windex, write = windex };
};

enum TGroupImportAction { giaMerge, giaReplace, giaSkip };
class TImportGroupBase
{
private:
    wchar_t *name;
    int id; // The order of the group as they were added to the corresponding group list.
    TGroupImportAction action;

    void set_name(const wchar_t* newname);
public:
    TImportGroupBase(const wchar_t* name, int id);
    ~TImportGroupBase();

    __property wchar_t* Name = { read = name, write = set_name };
    __property int Id = { read = id }; // The order of the group as they were added to the corresponding group list. Used in sorting.
    __property TGroupImportAction Action = { read = action, write = action };
};

class TImportWordGroup : public TImportGroupBase
{
private:
    typedef TImportGroupBase    inherited;

    struct TImportedWordElem
    {
        TExportedWord *data;
        TExportedDefinition *def;

        int entry;
    };
    TTemplateList<TImportedWordElem, true> items;
    int get_count();
    TExportedWord* get_item(int ix);
public:
    TImportWordGroup(const wchar_t* name, int id);
    ~TImportWordGroup();

    void AddWord(int entryindex, TExportedWord *exportedword, TExportedDefinition *exporteddef); // Adds a word to the group, inserting it in order of entryindex. If entryindex is -1, adds to the end. Call UpdateIndexes() when data reading is finished to assign the real index to each item.
    void RemoveWord(int entryindex, TExportedWord *exportedword, TExportedDefinition *exporteddef); // Removes a word definition from the group.
    void UpdateIndexes(); // Modifies the index for each item to refer to the real index in the created group (unless it is merged to an existing one). Entries with -1 index values get a new index in their order. Other indexes are changed so items with the same index get the same new index number.
    int NextIndex(); // Returns the highest index number + 1. Call after UpdateIndexes() ordered the items indexes.

    __property int Count = { read = get_count };
    __property TExportedWord* Items[int] = { read = get_item };
};

class TImportKanjiGroup : public TImportGroupBase
{
private:
    typedef TImportGroupBase    inherited;

    struct TImportedKanjiElem
    {
        TExportedKanji *data;

        int entry;
    };

    TTemplateList<TImportedKanjiElem, true> items;
    int get_count();
    TExportedKanji* get_item(int ix);
public:
    TImportKanjiGroup(const wchar_t* name, int id);
    ~TImportKanjiGroup();

    void AddKanji(int entryindex, TExportedKanji *item); // Adds a kanji to the group, inserting it in order of entryindex. If entryindex is -1, adds to the end. Call UpdateIndexes() when data reading is finished to assign the real index to each item.
    void UpdateIndexes(); // Modifies the index for each item to refer to the real index in the created group (unless it is merged to an existing one). The new index is the order of kanji in items, not taking matching entry indexes into account.
    int NextIndex(); // Returns the highest index number + 1. Call after UpdateIndexes() ordered the items indexes.

    __property int Count = { read = get_count };
    __property TExportedKanji* Items[int] = { read = get_item };
};

typedef TTemplateList<TExportedWord, true> TExportedWordList;
typedef TTemplateList<TExportedKanji, true> TExportedKanjiList;

template<typename T>
class TImportGroupList : public TTemplateList<T, true>
{
private:
    typedef TTemplateList<T, true>  inherited;
public:
    TImportGroupList();
    ~TImportGroupList();

    void Sort();
    int IndexOf(UnicodeString name);
};

typedef TImportGroupList<TImportWordGroup> TImportWordGroupList;
typedef TImportGroupList<TImportKanjiGroup> TImportKanjiGroupList;

enum TImportErrorType { ietGeneralError, ietDataMissing, ietUnrecognizedToken, ietNotANumber, ietInvalidValue, ietLimit, ietDuplicate, ietBadFormat };
struct TImportError
{
    int linenum;
    TImportErrorType type;
    wchar_t *msg;

    TImportError(int linenum, TImportErrorType type, const wchar_t *message);
    ~TImportError();
};
typedef TTemplateList<TImportError, true> TImportErrorList;

// --- Structures for user selection of words to update. ---
enum TWordImportAction { wiaSkip, wiaCheck, wiaApply };
// Imported entry import types
// * wiitNew - entry not in current dictionary.
// * wiitMatch - entry and dictionary meanings all matched, with additional meanings in either the imported or the dictionary word.
// * wiitDiff - there are differences which have to be resolved.
// +Delete - Same as the others, but also delete any definitions of a word not matched with the imported entry.
enum TWordImportImportType { wiitMatch = 0x0002, wiitDiff = 0x0004,
                             wiitMatchDelete = 0x1002, wiitDiffDelete = 0x1004 };
struct TImportedDefinition;
typedef TTemplateList<TImportedDefinition, true> TImportedDefinitionList;
// TImportedWord stores TExportedWord with information about the update method separately for each definition.
struct TImportedWord
{
    bool newword;
    TExportedWord *data;
    TImportedDefinitionList matchdefs; // Definitions exactly matching dictionary.
    TImportedDefinitionList defs;

    TWordImportAction action; // Selected action by the user. Apply or skip the word. Initially the value is wiaCheck if the word is listed to be selected, otherwise it is not listed.
    TWordImportImportType modtype; // How would the word modify the dictionary entry if applied. Newly inserted, added/removed definitions while not touching others, or full difference update. This value is only applicable if the action was wiaCheck.
};
struct TImportedDefinition
{
    short pos; // Position of the definition in the TExportedWord data. Negative number (-1 - real pos) if it is unchecked to remove from imported.
    byte *dictdefs; // Index of definitions of existing word in the dictionary selected for the given imported definition.
    byte defcnt; // Number of dictdefs.

    ~TImportedDefinition();
};
typedef TTemplateList<TImportedWord, true> TImportedWordList;
// ---------------------------------------------------------
// --- Structures for word group import replacement dlg. ---
struct TReplacedWord
{
    TExportedWord *data;
    bool newword;
    bool skip;

    TImportedDefinitionList matchdefs;
    TImportedDefinitionList defs;
};
typedef TTemplateList<TReplacedWord, true> TReplaceWordList;
// ---------------------------------------------------------

class TImportList
{
private:
    typedef TTemplateList<TExportedWord, true>  inherited;

    TImportWordGroupList wordgroups;
    TImportKanjiGroupList kanjigroups;

    TImportErrorList errors;

    TExportedWordList wordlist;
    TExportedKanjiList kanjilist;

    TWordCollection *coll;
    int cix;

    TImportedWordList importlist; // List to modify the dictionary on import, specifying which action to take for words read from the export file.
    TReplaceWordList replacelist; // List of words in group import with settings how to import them.

    UnicodeString cinfo; // Collection info from the About section.

    int get_wc();
    int get_kc();
    TExportedWord* get_word(int ix);
    TExportedKanji* get_kanji(int ix);

    int get_wgc();
    int get_kgc();
    TImportWordGroup* get_wgroup(int ix);
    TImportKanjiGroup* get_kgroup(int ix);

    int get_ic();
    TImportedWord* get_iword(int ix);
    int get_rc();
    TReplacedWord* get_rword(int ix);
public:
    TImportList(TWordCollection *coll);
    ~TImportList();
    virtual void Clear();

    // Reads the export file with the given name for dictionary or group import. Returns mrClose if a fatal error occurred. mrAbort returned if the abort button is pressed.
    // When the function leaves the list contains a list of words and word groups that can be imported, and a list of kanji with example words (if example words are needed), user definitions (if definitions are needed) and kanji groups.
    // * Word is added if:
    //   - Only the dictionary is updated, no groups added.
    //   - The word is to be added to a group, and the word was found in the original dictionary. Only those definitions added that are found in the base dictionary.
    //   - The word is to be added to a group, and the word was not found in the original dictionary. All definitions are included in case the word must be added to the dictionary.
    int ReadExportedDictionary(UnicodeString fname, const TImportOptions &options);


    int FindWord(const wchar_t *kanji, const wchar_t *kana); // Returns index of word with kanji / kana. Returns -1 - [insert index] if word not present.
    int FindWord(int wordindex); // Uses the word at wordindex in the collection to find the word.
    int FindWord(TExportedWord *w); // Returns index of word in wordlist.

    TExportedWord* AddWord(int windex, const wchar_t *kanji, const wchar_t *kana, short frequency); // Add a new word by word index or kanji and kana to the wordlist. The list is sorted by WordCompare.
    TExportedKanji* AddKanji(wchar_t kanjich); // Add a new kanji to the kanjilist. The list is sorted by character code.

    TImportWordGroup* AddWordGroup(const wchar_t *groupname); // Adds a group to groups if one with the same name does not exist yet. Returns the added group or the existing group with the same name.
    TImportKanjiGroup* AddKanjiGroup(const wchar_t *groupname); // Adds a group to groups if one with the same name does not exist yet. Returns the added group or the existing group with the same name.

    void WordToGroup(const wchar_t *groupname, int entryindex, TExportedWord *w, TExportedDefinition *def); // Adds a word to a wordgroup. The words are ordered by entry index.
    void KanjiToGroup(const wchar_t *groupname, int entryindex, TExportedKanji *k); // Adds a kanji to a kanjigroup. Kanji are ordered by entry index.

    // Functions for use after reading: (items can be sorted differently.)
    void RemoveDefDuplicates(); // Removes duplicate word definitions, even if the meaning numbers are different.
    void SortGroups(); // While reading, the groups for words and kanji are sorted by name. Sort them to "restore" their order in the import file.
    int WordGroupIndex(UnicodeString name); // Return the index of a word group in the wordgroups list, which are sorted by order in the import file.
    int KanjiGroupIndex(UnicodeString name); // Return the index of a word group in the wordgroups list, which are sorted by order in the import file.

    void MergeSameGroups(); // Groups can be renamed after reading in the data. Merge those which have the same name and are not to be skipped.
    void CleanupGroups(const TImportOptions &options); // Erase empty and skipped groups. Erase existing word items which were only added to skipped groups or no groups at all. If options does not contain dictionary update, erase definitions in words that were not in the dictionary which are not added to groups.
    void CleanupNewWords(const TImportOptions &options); // Erase definition of new words not added to any group or the word itself if no definitions remain. Called after the dictionary import, where all definitions of new words are not needed anymore.
    void CollectForImport(const TImportOptions &options); // Creates list to be used in the entry conflict manager dialog.
    void FillImportWord(TImportedWord *rw, const TImportOptions &options); // Called by CollectForImport to fill the matchdef and def arrays of the passed word.

    void CleanupDefs(const TImportOptions &options); // Removes unnecessary definitions and sorts them before updating the dictionary.
    void EntryUpdate(const TImportOptions &options); // Update the dictionary with the selected entries in the selected way.

    void CollectForReplace(const TImportOptions &options); // Creates list to be used in the word group import dialog.
    void FillReplaceWord(TReplacedWord *rw); // Called by CollectForReplace to fill the matchdef and def arrays of the passed word.

    void DoWordReplace(const TImportOptions &options); // Updates word data according to the previous replacement step and prepare them for word group update. Sets skipped words' indexes to -1, and skipped definition num variables to -1. If num is 0 or positive, it represents an index in matchdefs. If -2 or less, it represents an index in defs of the TReplacedWord structure
    void WordGroupUpdate(const TImportOptions &options); // Creates/modifies imported word groups and adds words.

    void DictDefUpdate(); // Update the kanji definitions in the dictionary.

    void ReviewExamples(); // After the dictionary update, some word examples not found previously could have been added to the dictionary. Update kanji example word indexes accordingly.
    void ExamplesUpdate(); // Add the selected kanji examples to the kanji cards.
    void ExampleDefUpdate(); // Import user definition of example words.
    void UserDefUpdate(); // Update the kanji definition set by the user separately from the dictionary.

    void KanjiGroupUpdate(const TImportOptions &options); // Adds kanji to the groups specified in the export file, overwriting groups if necessary.

    int FullDictImport();

    __property TWordCollection* Collection = { read = coll };
    __property int WordCount = { read = get_wc };
    __property int KanjiCount = { read = get_kc };
    __property TExportedWord* Words[int] = { read = get_word };
    __property TExportedKanji* Kanji[int] = { read = get_kanji };
    __property int WordGroupCount = { read = get_wgc };
    __property int KanjiGroupCount = { read = get_kgc };
    __property TImportWordGroup* WordGroups[int] = { read = get_wgroup };
    __property TImportKanjiGroup* KanjiGroups[int] = { read = get_kgroup };

    __property int ImportCount = { read = get_ic };
    __property TImportedWord* ImportWord[int] = { read = get_iword };

    __property int ReplaceCount = { read = get_rc };
    __property TReplacedWord* ReplaceWord[int] = { read = get_rword };

    __property UnicodeString AboutInfo = { read = cinfo };
};

void GroupImport();
void DictImport();
void FullImport();

#endif
