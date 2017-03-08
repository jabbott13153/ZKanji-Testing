//---------------------------------------------------------------------------

#ifndef settingsH
#define settingsH
//---------------------------------------------------------------------------

#define FREE_HEADING_COUNT     7
#define LOCK_HEADING_COUNT     13
#define TEST_HEADING_COUNT     17

#define KANJI_REFERENCE_COUNT  18

enum TKWarnTime { kwtNever,kwtStartup,kwt1,kwt2,kwt6,kwt24 };
enum TKInterval { ki1Day, ki2Days, ki3Days };
enum TKRInterval { kriShort, kriMedium, kriLong };
enum TKSuspendTime { stNoSuspend, st1, st1_5, st2, st3, st5 };
enum TKTestReadings { trNone, trON, trKun, trBoth };
enum TKReadingWords { rwNew, rwMistakes, rwNewAndMistakes, rwAllTested };
enum TQuestionType;

struct TColorSettings
{
    int maintext;
    int background;
    int highlight;
    int gridline;

    int kanausage;

    int wordinf;
    int wordtype;
    int wordusage;
    int wordfield;

    int N[5];

    int kanjinoword;
    int kanjiunsorted;

    int wordpop;
    int wordavg;
    int wordunpop;

    int activeborder;
};

struct TProfileSettings
{
    // Common options.
    int daystart; // Hour in the morning till noon.

    TKWarnTime kwarntime; // When to warn after too much items are piled up for repetition.
    int kwarnthres; // Number of piled up items for warning.
    double klastwarning; // Date and time of last warning.

    TKInterval includeinterval; // Interval when to include new items.
    int kiteminclude; // Number of items to include daily or when new items are included. 0 = ask.

    // Test settings.
    TKRInterval krepetition; // Length of interval for items that were incorrectly answered to re-appear in the same test.
    bool klabelinterval; // Show label indicating interval when word gets repeated after pressing a given answer button.

    bool kwarnitems; // Warn after the given number of items have been tested.
    int kwarnitemscount; // Number of items to study before warning is shown.

    bool kwarnafter; // Warn if the test is running longer than the given time.
    int kwarnaftertime; // Time after a worning message is shown.

    bool ktypekanji; // Offer an edit box to enter the written form of a word during a test.
    bool ktypekana; // Offer an edit box to enter the kana form of a word during a test.
    bool ktypemeaning; // Offer an edit box to enter the meaning of a word during a test.

    bool knewfirst; // Show newly added words first.

    TQuestionType kkanjihint; // Hint line to show when the tested word part is kanji. valid values: qtKana, qtMeaning.
    TQuestionType kkanahint; // Hint line to show when the tested word part is kana. valid values: qtKanji, qtMeaning.
    TQuestionType kmeaninghint; // Hint line to show when the tested word part is meaning. valid values: qtKanji, qtKana.

    TKSuspendTime ksuspendafter; // Time after test is suspended when left alone by the user..

    byte kdecklimitretry; // Item deck limit after retrying a word.
    byte kdecklimithard; // Item deck limit after a word wasn't answered correctly.

    // Readings test settings.
    TKTestReadings ktestreadings; // Which readings should be tested after a test.
    TKReadingWords kreadingwords; // Words where the readings appear.
};

enum TVPrintItem { viMeaning, viKana, viKanji, viNothing };

struct TSettings
{
    bool debug;
    //bool changed; // not saved nor loaded. used to notify components that the settings have changed
    UnicodeString settingspage;

    byte dictionary_index;

    int filterwindowwidth;
    int filterwindowheight;

    bool firstrun;

    //TMainWindowLayout baselayout;
    //bool docked;

    TDateFormatSetting datefmt; //0: dd.mm.yy, 1: mm.dd.yy, 2: yy.mm.dd

    bool kanjiantialias;
    UnicodeString kanjifont;
    int kanjicharset;
    UnicodeString kanafont;
    int kanacharset;
    UnicodeString romajifont;
    UnicodeString smallromajifont;

    UnicodeString vocabcollection;
    UnicodeString vocabgroup;

    UnicodeString kanjicollection;
    UnicodeString kanjigroup;

    bool searchsave;
    bool positionrestore;
    bool kanjirestore;

    int vocabwidth;
    int vocabheight;

    bool autosave;
    int autosavedelay;

    bool minimizetotray;

    bool windowdecoration;
    bool oldstylepopup;
    bool toolshadow;

    bool showclipcopy;
    bool showclipappend;

    bool shownonkanjielements;

    bool remembertray;
    bool startintray;
    bool intray;

    bool autocheck;
    int checkinterval;
    int lastcheck;

    int reference_index;
    Bits referenceshow;
    byte reference_order[KANJI_REFERENCE_COUNT];

    bool columnreset;
    bool popupcolumnreset;
    bool popuppositionrestore;
    bool popupdisappear;
    bool popupuseclipboard;
    bool kanjireset;

    byte popuptransparency;

    bool showinftext;
    bool showinfalways;
    int alphaorder;
    int freqsymbols;

    bool showingroup;
    bool showinJLPT;

    int histcnt;
    bool timedhistsave;
    int histdelay;

    int colkanji;
    int colkana;
    int popupcolkanji;
    int popupcolkana;
    int studycolkanji;
    int studycolkana;

    // Popup window minimized size.
    int popupwidth;
    int popupheight;

    // Popup window floating size and position.
    TRect popupfloatrect;

    int studyeditwidth;
    int studyeditheight;
    int studyeditmainsplit;
    int studyeditwordsplit;

    int studyreadingwidth;
    int studyreadingheight;

    //int docksplitwidth;
    //int docksplitheight;

    int dictfontsize;
    int popfontsize;

    bool syskeys;
    int JKeys;
    bool JShift;
    Word JChar;
    int EKeys;
    bool EShift;
    Word EChar;
    int KKeys;
    bool KShift;
    Word KChar;

    // Word study settings.
    byte M1Num;
    byte M1Order;
    bool M1Rand;
    byte M1Add;
    byte M1Selection;
    bool M1Global;
    int M2Num;
    byte M2Inc;

    byte M0Practice; // Only used for loading old word study settings. 2013 07 04
    byte M0Choices; // Only used for loading old word study settings. 2013 07 04
    bool M0Type; // Only used for loading old word study settings. 2013 07 04

    byte M0Question; // Used in the new version of word study settings. 2013 07 04
    byte M0Answer; // Used in the new version of word study settings. 2013 07 04

    byte M0Timer;
    bool M0HideKana;
    int statdays; // Number of days after a word statistic doesn't count.
    bool compactreview;

    UnicodeString exportdir;

    bool save_gen; // Save word generation settings.
    byte gen_src;
    byte gen_dest;
    bool gen_linked;
    UnicodeString gen_group;
    byte gen_len;
    word gen_pop;
    byte gen_pos;
    bool gen_only;
    byte gen_grad;
    byte gen_num;

    // Kanji classical 2 lookup.
    bool classic2group;
    bool classic2exact;
    bool classic2hidenames;

    // Vocabulary printing.
    bool vdictfonts;
    UnicodeString vkanafont;
    int vkanacharset;
    UnicodeString vromajifont;
    int vheight;
    TVPrintItem vleft;
    TVPrintItem vright;
    bool vseparate;
    bool vlines;
    bool vshade;
    bool vdouble;
    bool vnumber;
    bool vfurigana;

    int printerindex;
    bool vportrait;
    bool vtofile;

    // Word example settings.
    int example_cache;

    // Kanji hint window settings.
    bool show_kanji_hint;
    bool hide_kanji_hint;
    int kanji_hint_time;

    bool kanatests;
    // Bits selected in kana practice.
    Bits bitskata;
    Bits bitshira;

    // Long-term window options.
    bool ltopentab;
    bool ltrestoresizes;
    bool ltshowlast;

    int lttab;
    int ltlist;
    int ltitemstat;
    int ltitemstatsize;
    bool ltshowitemstat;
    int ltglobalstat;
    int ltfreevisible;
    int ltlockedvisible;
    int lttestedvisible;
    int ltfreesizes[FREE_HEADING_COUNT];
    int ltlockedsizes[LOCK_HEADING_COUNT];
    int lttestedsizes[TEST_HEADING_COUNT];
    int ltfreeordercolumn;
    int ltlockedordercolumn;
    int lttestedordercolumn;

    // Long-term list word import.
    int ltimportwidth;
    int ltimportheight;

    // Long-term study stroke order dialog settings.
    bool ltsodnumber;

    // Data backup settings.
    bool backupdata;
    int bckcount;
    int bckspacing;
    UnicodeString bckpath;
};

extern TSettings settings;
extern TProfileSettings profilesettings;
extern TColorSettings colorsettings;

bool ShowSettings(UnicodeString page = UnicodeString());

#endif
