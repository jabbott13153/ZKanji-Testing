// ---------------------------------------------------------------------------

#ifndef collectionH
#define collectionH
// ---------------------------------------------------------------------------

class TTextSearchTree;
class TWordList;
struct TWord;
struct TKanji;
class TWordGroupList;
class TKanjiGroupList;
class TKanjiCard;
class TStudyList;
class TWordStudyList;
class TWordCollectionList;
class TWordStateList;

class TGapStringList;
class TRepetitionCategory;

class TWordFilterCopyList;

struct TMeaningData;

// Data types used to save the original word data in the base English dictionary:

// enum TOriginalWordChange:
// * owcNone: no change. This is only used as a temporary value when updating the list. Any item with this change type must be deleted at the end of the operation.
// * owcAdded: the user's own word, which was not part of the original dictionary. In this case meaningcnt is 0 and no meaning data is used.
// * owcModified: the user modified a word in the dictionary. The meaningcnt must be equal to the original count of meanings and data must contain the original meaning data.
// * owcDeleted: used as the action to take when updating the originals list. Only words added by the user can be deleted, that is, only the owcAdded change type is valid for an original, before taking this action.
enum TOriginalWordChange { owcNone, owcAdded, owcModified, owcDeleted }; // Deleted is invalid when change is not owcAdded originally.

// Data for words that were changed by the user in the base English dictionary. The words' original data is stored
// so it can be reverted when needed, and it is used when updating the dictionary. On the other hand the dictionary
// will contain the changed word instead. The original data is stored but only for changed words. The saved data
// must reflect the current base dictionary, so it might be updated when updating the dictionary.
struct TOriginalWord
{
    TOriginalWordChange change; // Type of change.
    wchar_t *kanji;
    wchar_t *kana;

    byte meaningcnt; // The number of meanings originally in the dictionary when the change type is owcModified. 0 otherwise.
    TMeaningData *data; // The meaning data originally in the dictionary when the change type is owcModified. NULL otherwise.
};
// List of words that were changed by the user in the main English dictionary.
class TOriginalWordsList : public TTemplateList<TOriginalWord, true>
{
private:
    typedef TTemplateList<TOriginalWord, true> inherited;

    virtual void FreeItem(TOriginalWord *elem, int ix);

    int SortCompare(wchar_t *kanji, wchar_t *kana, int comppos);
public:
    virtual ~TOriginalWordsList();

    void LoadFromFile(FILE *f);
    void SaveToFile(FILE *f);
    bool SkipInFile(FILE *f); // Skips the originals data in a file. Only works if the list in the file would have 0 elements. Returns false on error.

    int Add(TWord *w, TOriginalWordChange change);
    TOriginalWord* Find(TWord *w);
    TOriginalWord* Find(wchar_t *kanji, wchar_t *kana);

    void Revert(int wordindex); // Updates the word to match its original state and removes its original word data from the list.
    int Revertable(int wordindex, TGapList<int> &pos, TGapList<int> &posorig); // Returns -1 if the word is missing or has a meaning in some group which would be missing after revert. Returns 0 if the word has no missing meanings, but those in groups do not match the original in some way. Returns 1 otherwise.

    bool IsAdded(TWord *w);
    bool IsModified(TWord *w);

    // Returns the index of the word or the kanji/kana pair in the originals list. If the word is not present the result is -1 - [insert position].
    int IndexOf(TWord *w);
    int IndexOf(wchar_t *kanji, wchar_t *kana);
};

// List of words that contain a kanji or valid character, set for each kanji and valid character:
struct TKanjiData
{
    int *words; // Index of words containing this kanji.
    int wordscnt;
    wchar_t *meaning; // Null for user dictionaries when no new meaning is added.
    TKanjiCard *card;
};

struct TValidData
{
    int *words; // Index of words containing this character.
    int wordscnt;
};

/*
//  wctBase: unchangable base English dictionary
//  wctWords: Normal user dictionary
//  wctNames: Names dictionary
enum TWordCollectionType { wctBase, wctWords, wctNames };
*/

// Which alphabetic order to pick from in a dictionary.
// woSelect picks from woAlpha and woAiueo depending on the settings
enum TWordOrder { woAlpha, woAiueo, woSelect, woFiltered };

struct TWordFilter;

// Ennumeration in TWordCollection to store modified state.
// wccNone: collection not modified, no need to save it.
// wccMain: collection was modified, save both the dictionary and the user data.
// wccUser: only the user data was modified. There is no need to save the main data as well.
// wccMainStartup, wccUserStartup: the collection was modified during startup and saving should happen. Otherwise the modified state is set to wccNone after the dictionary is loaded.
enum TWordCollectionChanges { wccNone, wccMain, wccUser, wccStartupMain, wccStartupUser };

// WARNING: make sure the update checker in dictupdater also mirrors any new data!
struct TWordCollection
{
    // TWordCollectionType type;

    TWordCollectionChanges modification; // The collection was modified or not.

    int imgindex;
    UnicodeString info; // Custom information entered. ie. dictionary author, copyright, license etc.

    SYSTEMTIME lastwritetime;

    TTextSearchTree *rtree; // Word romaji tree
    TTextSearchTree *ktree; // Word kana tree
    TTextSearchTree *btree; // Word kana tree - word endings

    TIntegerList *abcde; // Indexes for the "words" list in english alphabetical order.
    TIntegerList *aiueo; // Indexes for the "words" list in japanese order.

    TIntegerList *abcfilter; // Filtered list made from abcde or aiueo when filter was selected
    TWordFilterCopyList *setfilters;

    TWordList *words;
    TGapStringList *deflist; // Word definitions
    TGapStringList *kdeflist; // Kanji definitions
    TWordGroupList *groups;
    TWordGroupList *studygroups;

    TKanjiGroupList *kgroups;

    TWordStudyList *wordstudylist;

    TValidData *validdat; // List of words that contain a given character from the validcode array.
    TKanjiData *kanjidat; // An array for extra kanji data, like custom meaning and words in dictionary

    TRepetitionCategory *kcategory;

    /*Constructor*/ TWordCollection();
    /*Destructor*/ ~TWordCollection();

    int WordPosition(const wchar_t *kanji, const wchar_t *kana, TWordOrder order);

    // Call when data is changed, after the change was correctly done.
    void Modify(bool main); // Set main to true when the dictionary was modified, false when only the user data.
    void StartupModify(bool main); // Sets modification state during startup, which is always taken into account. Set main to true when the dictionary was modified, false when only the user data.
private:
    friend class TWordCollectionList;
    friend class TWordStateList;

    void ExpandRomajiTree(int wordindex, wchar_t *meaning);
    int AddWord(TWord *w); // Adds new word to the dictionary and updates the indexes and trees.
    int AddWord(TWord *w, char meaning, UnicodeString str); // Add a word from an existing one, adding a single meaning and if specified a string for the meaning. If meaning is -1, add a copy of the word with all its meanings.
    int AddWord(const wchar_t *kanji, const wchar_t *kana, const wchar_t *meaning, const int type, const int note, const byte field, const word nametag, const word frequency);
    byte CopyWordMeaning(int wordix, TWord *w, byte meaning);
    byte AddWordMeaning(int wordix, const wchar_t *meaning, int types, int notes, byte fields, word nametag);
    char WordMeaningIndex(int wordix, const wchar_t *meaning); // Returns -1 if the word doesn't have a meaning which exactly matches the passed string, or the index of the first meaning that matches.
    void DeleteWordMeaning(int wordindex, byte meaning); // Update romaji tree and delete definition from groups.
    void MoveWordMeaning(int wordindex, byte meaning, byte newpos); // Change position of word meaning and update groups.
    void DeleteWord(int wordindex); // Update romaji tree and delete definition from everywhere.
    void ChangeWordMeaning(int wordindex, byte meaning, wchar_t *str, int wtype, int wnote, byte wfield, word ntag, word wfrequency);

    void ReplaceWord(int wordix, TWord *replacement); // USED IN: dictionary update. Replaces a word's meanings with an existing one. WARNING: Doesn't check anything in groups! Only used when updating dictionary on startup.
    void ReplaceWord(int windex, TWord *w, byte *replacecnt, byte **replacers); // USED IN: dictionary import. Replaces a word at windex in the dictionary by w, updating the word groups. replacecnt is an array holding the number of meanings in w that replace a meaning at a given index. replacers is the list of meaning indexes in w that replace a meaning at a given index.
};

class TWordGroupStudy;

enum TWordChangeType { wctAdd, wctDelete, wctAddMeaning, wctDeleteMeaning, wctMoveMeaning, wctChange };
struct TChangedWord
{
    wchar_t *kanji;
    wchar_t *kana;
    TWordChangeType type;
};
class TChangedWordList : public TTemplateList<TChangedWord, true>
{
private:
    typedef TTemplateList<TChangedWord, true> inherited;
protected:
    virtual void FreeItem(TChangedWord *elem, int ix);
    using inherited::Add;

    byte fcollection; // Collection whose words are being changed.
public:
    TChangedWordList(byte acollection);
    virtual ~TChangedWordList();

    int Add(const wchar_t *ckanji, const wchar_t *ckana, const TWordChangeType ctype);

    __property byte CollectionIndex = { read = fcollection };
};

class TWordCollectionList : public TStringList
{
private:
    static TWordCollectionList *instance;
    TWordCollectionList();
    __fastcall TWordCollectionList(TWordCollectionList const &copy)
    {
    }
    virtual __fastcall ~TWordCollectionList();

    static int schanging; // All save operations are on hold while this is non null.
    static int schangesave; // Saved state of schanging in case we need to reset and then restore it with ResetChange and RestoreChange.
    static int sdisabled; // All save operations are disabled when this is non null. ResetChange and RestoreChange has no effect on this value.

    static bool ssaving;
    static bool sprofchanged;
    static bool sbackingup;

    static bool sloading;

    //TDateTime lastsavetime;

    /* List of words that were changed by the user in the main English dictionary.
     * This list is NULL until it is needed, when the past changes are loaded from
     * file to memory, so it won't take memory for those who don't change the main
     * dictionary. The items in the list are not the changed words but the
     * original words that were in the main dictionary before they were changed.
     * When the main dictionary didn't have this word only the kanji/kana is
     * added, but not the meanings.
     * Changes in other dictionaries are not stored. */
    TOriginalWordsList *originals;
    // void LoadOriginals(); //  Creates the originals list if it's null and loads it from file.
    // TOriginalWordsList* get_orig();

    TChangedWordList *changelist; // List of words being changed. This is NULL when no change is taking place.

    int savedsortorder; // settings.alphaorder saved before settings change to see if we need to reorder filtered word list in dictionary browse mode
    void settingsChanged(void *caller, TSettingsProcState state);

    TWordList* get_words(byte ix);
    TWordCollection* get_items(byte ix);

    TStringList *backups; // List of folders containing backup files in the backup root folder. The format of the folders is yyyymmdd.
    TDateTime lastbackup;

    // void ExpandRomajiTree(byte ix, int wordindex, wchar_t *meaning);

    bool get_changing();
    bool get_disabled();
    int get_index(TWordCollection *wc);

    __property Names;
    __property Values;

    // void WordStatisticExport(FILE *f, byte coll);
    // void KanjiExampleExport(FILE *f, byte coll);

    // int Add(const UnicodeString S) { THROW(L"Please replace programmer! Don't use the base class' Add function!"); }
    TIntegerList* alphabet(byte ix, TWordOrder order);

    bool get_modified();

    struct LoadingDictionaryGuard
    {
        LoadingDictionaryGuard()
        {
            sloading = true;
        }
        ~LoadingDictionaryGuard()
        {
            sloading = false;
        }
    };

    friend struct LoadingDictionaryGuard;
public:
    static TWordCollectionList* Create();
    static void FreeUp();

    struct ChangeResetObj
    {
    public:
        ChangeResetObj()
        {
        }
        ~ChangeResetObj()
        {
            ::TWordCollectionList::Change(false);
        }
    };

    struct DisableSaveGuard
    {
        DisableSaveGuard()
        {
            ::TWordCollectionList::DisableSave();
        }
        ~DisableSaveGuard()
        {
            ::TWordCollectionList::EnableSave();
        }
    };

    static void Change(bool add);
    static void ProfileModify(); // Save the profile only (which goes to a separate file.

    // Disable or enable saving of dictionaries. Create a DisableSaveGuard object instead to automatically resume
    // saving when exiting a block. DisableSaveGuard automatically disables saving, and re-enables it when it is destroyed.
    static void DisableSave(); // Disables saving the dictionaries, even if they were modified. Call EnableSave the same number of times to restore saving.
    static void EnableSave(); // Call the same number of times as DisableSave() was called.

    void ResetChange(); // Allow saving the dictionary even during a modification. Call RestoreChange to restore this behavior to its state when ResetChange was called.
    void RestoreChange(); // Resets the value of modification, preventing saving if it was set before a ResetChange call.

    void __fastcall Clear();
    void __fastcall Delete(int coll);
    int __fastcall Add(const UnicodeString aname); // Add new dictionary with name.
    bool Rename(byte coll, const UnicodeString aname);
    int AddWordGroup(byte coll, const UnicodeString aname, bool astudy);
    int AddKanjiGroup(byte coll, UnicodeString aname);

    virtual int __fastcall IndexOf(const System::UnicodeString S);

    // return position in the alphabetic order. Or "-1 - would_be_position" when not present.
    // int WordPosition(byte coll, const wchar_t *kanji, const wchar_t *kana, TWordOrder order);
    int WordIndex(byte coll, int index, TWordOrder order);
    int ListCount(byte coll, TWordOrder order);
    // Fill abcfiltered list with words of the current filter.
    void SetABCFilter(byte coll);
    void ClearABCFilters();
    void SortFilters();

    void BeginWordChange(byte ix);
    void EndWordChange();
    //  Add new word to specified dictionary. Returns the index of the word added in this dictionary.
    //  The meaning is the meaning index to add, if any, and returns the selected meaning. If meaning is -1, add a copy of the word with all its meanings.
    int AddWord(byte coll, TWord *w, char meaning, UnicodeString str);
    int AddWord(byte coll, const wchar_t *kanji, const wchar_t *kana, const wchar_t *meaning, const int type, const int note, const byte field, const word nametag, const word frequency);
    int AddWord(byte coll, TWord *w); // Adds new word to the dictionary and updates the indexes and trees.
    void ReplaceWord(byte coll, int windex, TWord *w, byte *replacecnt, byte **replacers); // Replaces a word at windex in the dictionary by w, updating the word groups. replacecnt is an array holding the number of meanings in w that replace a meaning at a given index. replacers is the list of meaning indexes in w that replace a meaning at a given index.
    byte CopyWordMeaning(byte coll, int wordix, TWord *w, byte meaning);
    byte AddWordMeaning(byte coll, int wordix, const wchar_t *c, int types, int notes, byte fields, word nametag);
    char WordMeaningIndex(byte coll, int wordix, const wchar_t *meaning); // Returns -1 if the word doesn't have a meaning which exactly matches the passed string, or the index of the first meaning that matches.
    int WordIndex(byte coll, const wchar_t *kanji, const wchar_t *kana);
    void DeleteWordMeaning(byte coll, int wordindex, byte meaning); //  Update romaji tree and delete definition from groups.
    void MoveWordMeaning(byte coll, int wordindex, byte meaning, byte newpos); // Change position of word meaning and update groups.
    void DeleteWord(byte coll, int wordindex); //  Update romaji tree and delete definition from everywhere.
    void ChangeWordMeaning(byte coll, int wordindex, byte meaning, wchar_t *str, int wtype, int wnote, byte wfield, word ntag, word wfrequency = 10001);
    void RevertWord(int wordindex); // Restore word to its original state, undoing modifications. Only works for words in the base dictionary, and only if the word is not already added to a word group.
    int Revertable(int wordindex); // Returns -1 if the word is not in originals or has a meaning in some group which would be missing after revert. Returns 0 if the word has no missing meanings, but those in groups do not match the original in some way. Returns 1 otherwise.

    int BaseIndex(TWordCollection *col, int wordindex); // returns index of word in English base dictionary from the word's collection and index
    int BaseIndex(byte ix, int wordindex);

    int ConvertIndex(TWordCollection *src, int srcwordindex, TWordCollection *dest);

    UnicodeString CustomKanjiMeaning(byte coll, word kindex); //  Returns custom meaning of kanji or an empty string when no custom meaning is present.
    UnicodeString KanjiMeaning(byte coll, word kindex); //  Returns custom meaning of kanji or the original meaning when no custom meaning is present.
    bool ModifyKanjiMeaning(byte coll, word kindex, UnicodeString meaning); // Returns whether any modification was necessary.

    void Save();
    void Load();
    bool Reload(byte coll);

    bool Loading();

    void ReadBackup(); // Create a list of existing backup folders in the backup root, even if backup is disabled. (In case it gets enabled later.)
    void CreateBackup(TDateTime now, bool onload); // Copies files to the backup folder if necessary. Set onload to false when not calling directly from dictionary load.
    void DeleteBackup(int keep); // Delete a number of past backups form disk, until count number of backups remain. The oldest backups are deleted.
    int BackupCount(); // Number of backup folders on disk. The disk is accessed only on startup.
    void MoveBackup(UnicodeString newpath); // Move all existing backup folders to a new location.

    // void SQLExport(UnicodeString fname, byte coll);
    // void Export(UnicodeString fname, byte coll);
    // void ExportGroups(UnicodeString fname, byte coll, bool words, bool wstudy, bool kanji, bool kexamples);

    __property bool save_in_progress = { read = ssaving };

    __property TWordList *Words[byte] = { read = get_words };
    __property TWordCollection *Items[byte] = { read = get_items };

    __property bool Modified = { read = get_modified };
    __property bool Changing = { read = get_changing };
    __property bool SaveDisabled = { read = get_disabled };
    __property int Index[TWordCollection * wc] = { read = get_index };
    __property TOriginalWordsList *Originals = { read = originals }; // List of words that were changed by the user.
};

extern TWordCollectionList *dictionary;
extern int error_code;

void find_kanji_words(TWordCollection *coll, int *kanjiindex); // Fill in the kanjidat structure.
void add_word_line(TKanjiData &k, int i); // Adds word to the words list of a kanji.
void add_valid_word_line(TValidData &d, int i); // Adds word to the words list of a valid code character.
void delete_word_line(TKanjiData &k, int i); // Deletes word from words list of a kanji.
void delete_valid_word_line(TValidData &d, int i); // Deletes word from words list of a valid code character.

// Returns true when the given kanji is in the given word with the given reading (or altered reading!)
bool IsReading(TKanji *k, TWord *w, byte readingpos);
int FindReading(byte kanjipos, TWord *w);

void Translate(UnicodeString txt);
void Translate(byte coll, UnicodeString kanji, UnicodeString kana);

#endif
