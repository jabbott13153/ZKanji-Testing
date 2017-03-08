//---------------------------------------------------------------------------

#ifndef zkformatsH
#define zkformatsH
//---------------------------------------------------------------------------

class TTextSearchTree;
class TWordCollection;
struct TFuriganaData;

extern const unsigned int DICTIONARY_BASE_VERSION;
extern const unsigned int DICTIONARY_VERSION;
extern const unsigned int GLOBALGROUP_VERSION;
extern const unsigned int STUDYPROFILE_VERSION;
extern const unsigned int EXAMPLESENTENCE_VERSION;


// Word type defines !! for 30 elements at most !! don't use the 31st.
#define WT_NOUN           (1 << 0) // p&n;
#define WT_PSURU          (1 << 1) // p&p-suru;
#define WT_VERB_U         (1 << 2) // p&v-u;
#define WT_EXP            (1 << 3) // p&exp;
#define WT_ADJ_NA         (1 << 4) // p&a-na;
#define WT_VERB_TRANS     (1 << 5) // p&v-t;
#define WT_VERB_RU        (1 << 6) // p&v-ru;
#define WT_VERB_INTRANS   (1 << 7) // p&v-i;
#define WT_ADVERB         (1 << 8) // p&adv;
#define WT_ADJ_BASE       9
#define WT_ADJ            (1 << WT_ADJ_BASE) // p&a;
#define WT_ADJ_NO         (1 << 10) // p&a-no;
#define WT_SUFFIX         (1 << 11) // p&suf;
#define WT_ADV_TO         (1 << 12) // p&adv-to;
#define WT_PREFIX         (1 << 13) // p&pref;
#define WT_VERB_SURU      (1 << 14) // p&v-suru;
#define WT_ADJ_PRENOUN    (1 << 15) // p&a-npre;
#define WT_CONJ           (1 << 16) // p&conj;
#define WT_INT            (1 << 17) // p&int;
#define WT_VERB_IKU       (1 << 18) // p&v-iku;
#define WT_VERB_AUX       (1 << 19) // p&v-aux;
#define WT_VERB_KURU      (1 << 20) // p&v-kuru;
#define WT_ADJ_AUX        (1 << 21) // p&a-aux;
#define WT_VERB_ARU       (1 << 22) // p&v-aru;
#define WT_COUNT          23
//word note defines
#define WN_SENSE          (1 << 0) // m&sens;
#define WN_ABBREV         (1 << 1) // m&abbr;
#define WN_HONORIFIC      (1 << 2) // m&hon;
#define WN_HUMBLE         (1 << 3) // m&hum;
#define WN_COLLOQUIAL     (1 << 4) // m&col;
#define WN_POLITE         (1 << 5) // m&pol;
#define WN_SLANG          (1 << 6) // m&sl;
#define WN_FAMILIAR       (1 << 7) // m&fam;
#define WN_MALE           (1 << 8) // m&male;
#define WN_FEMALE         (1 << 9) // m&fem;
#define WN_KANAONLY       (1 << 10) // m&kana;
#define WN_KANJIONLY      (1 << 11) // m&kanji;
#define WN_ARCHAIC        (1 << 12) // m&arch;
#define WN_RARE           (1 << 13) // m&rare;
#define WN_OBSOLETE       (1 << 14) // m&obs;
#define WN_VULGAR         (1 << 15) // m&vulg;
#define WN_XRATED         (1 << 16) // m&X;
#define WN_DEROGATORY     (1 << 17) // m&derog;
#define WN_OBSCURE        (1 << 18) // m&obsc;
#define WN_IDIOMATIC      (1 << 19) // m&id;
#define WN_MALESLENG      (1 << 20) // m&m-sl;
#define WN_COUNT          21
// Word field defines.
#define WF_MATH           (1 << 0) // f&math;
#define WF_LING           (1 << 1) // f&ling;
#define WF_COMP           (1 << 2) // f&comp;
#define WF_FOOD           (1 << 3) // f&food;
#define WF_MART           (1 << 4) // f&MA;
#define WF_GEOMETRY       (1 << 5) // f&geom;
#define WF_PHYSICS        (1 << 6) // f&physics;
#define WF_BUDDHISM       (1 << 7) // f&physics;
#define WF_COUNT          8
// Name tag defines.
#define NM_MALE           (1 << 0)
#define NM_FEMALE         (1 << 1)
#define NM_SURNAME        (1 << 2)
#define NM_GIVENNAME      (1 << 3)
#define NM_PERSON         (1 << 4)
#define NM_PLACE          (1 << 5)
#define NM_STATION        (1 << 6)
#define NM_ORGANIZATION   (1 << 7)
#define NM_COMPANY        (1 << 8)
#define NM_PRODUCT        (1 << 9)
#define NM_COUNT          10
// Word inflection type defines.
#define WI_MASU           1
#define WI_MASHITA        2
#define WI_MASEN          3
#define WI_MASHOU         4
#define WI_NAI            5
#define WI_RERU           6
#define WI_SERU           7
#define WI_SERARERU       8
#define WI_I              9
#define WI_TAI            10
#define WI_TAGARU         11
#define WI_SOU            12
#define WI_TE             13
#define WI_TA             14
#define WI_TARA           15
#define WI_TARI           16
#define WI_EBA            17
#define WI_ERU            18
#define WI_E              19
#define WI_OU             20
#define WI_SURU           21
#define WI_RARERU         22
#define WI_KU             23
#define WI_SA             24
#define WI_NA             25
#define WI_TERU           26
#define WI_KYA            27
#define WI_CHAU           28
#define WI_NAIDE          29
#define WI_ZU             30
#define WI_NU             31
#define WI_TENASAI        32
#define WI_TEOKU          33

struct TWordStatGlobal;

void import_kanji(const wchar_t *name);
void import_radicals(const wchar_t *name);
void import_zradicals(const wchar_t *name);
void import_dictionary(const wchar_t *name, TWordCollection *coll);
void import_tree(const wchar_t *name, const wchar_t *skipname, TTextSearchTree *tree);
void import_index(const wchar_t *name, TTextSearchTree *tree, bool kana);
void import_jlpt(TWordCollection *coll, const wchar_t *name);
void filter_kanji(TWordCollection *coll);
void sort_kanji_readings();
void set_word_frequencies(TWordCollection *coll);
void export_all_furigana(const wchar_t *name1, const wchar_t *name2);
/*void import_taka_dir(AnsiString dir, AnsiString dest);*/
bool import_examples(const wchar_t *inputfile, const wchar_t *outputfile); // Returns false when aborted or on error.

void __fastcall generate_validkanji();
void __fastcall generate_validunicode();

void free_kanji();
void import_confused(const wchar_t *name);
void import_irregulars(const wchar_t *name);
bool import_one_furigana(const wchar_t *name, const wchar_t *kanji, const wchar_t *kana, TFuriganaData *dat, int &datpos);


int getlinew(wchar_t *line, int buffsize, FILE *f, bool &bigendian); // Returns a line from a UTF-16 encoded file. Returns the length of line or -1 when there is a line but starts with line break. Returns 0 for EOF.
int getlinemb(wchar_t *line, int buffsize, FILE *f); // Returns a line from a UTF-8 encoded file. Returns the length of line or -1 when there is a line but starts with line break. Returns 0 for EOF.
void feol(FILE *f);
void fdelim(FILE *f);
void funicode(FILE *f);

// Save dictionary and base kanji/radical etc. data. Only saves original words list for the main dictionary.
bool save_dictionary(const UnicodeString &name, TWordCollection *dictionary, bool main);
bool load_dictionary(const UnicodeString &name, TWordCollection *dictionary, bool main, bool skiporiginals);
bool save_dictionary_base(const UnicodeString &name);
bool load_dictionary_base(const UnicodeString &name);
// Loads the dictionary date used for validating. main is true when loading date from main dictionary, false for user dictionary.
bool get_dictionary_date(const UnicodeString &name, SYSTEMTIME &date, double *buildver = NULL); // Value only returned in buildver for the base dictionary.

//bool check_file_integrity(const UnicodeString &name); // Returns whether the file has an integer with the correct file size at the end of data files.

bool save_groups(const UnicodeString &name, TWordCollection *dictionary);
void load_groups(const UnicodeString &name, TWordCollection *dictionary);

bool save_profile(const UnicodeString &name);
bool load_profile(const UnicodeString &name);

bool load_examples(const UnicodeString &name);

extern int *kanjiindex;

#endif
