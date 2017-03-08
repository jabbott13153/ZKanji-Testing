//---------------------------------------------------------------------------

#ifndef zdefinesH
#define zdefinesH
//---------------------------------------------------------------------------

#include <windowsx.h>

#include "myfonts.h"
#include "templatelist.h"
#include "intlist.h"
#include "bits.h"
#include "gaplist.h"
#include "minmax.h"
#include "numedit.h"
#include "angle.h"

//#define HACK_TEST_REMOVE

extern bool AppActive;

extern const DWORD gen_lcid;

extern const int dictionary_name_length_limit;
extern const int group_name_length_limit;

extern double scaleres;

extern THintWindow *hint8;

#define DESKTOPHANDLE  (Win32MajorVersion < 6 ? GetDesktopWindow() : NULL)

#define RAD_COUNT   252

#define BTN_DARK_TRESH 0x60

#define ROUND(x)  ((x) < 0 ? (((float)x)-0.5) : (((float)x)+0.5) )

#define ARROW_CHAR "\x36"

#define MAX(a,b)                 max(a,b)
#define MIN(a,b)                 min(a,b)

#define MAX4(a,b,c,d)            MAX(MAX(MAX(a,b),c),d)
#define MAX8(a,b,c,d,e,f,g,h)    MAX(MAX(MAX(MAX(MAX(MAX(MAX(a,b),c),d),e),f),g),h)
#define MIN4(a,b,c,d)            MIN(MIN(MIN(a,b),c),d)
#define MIN8(a,b,c,d,e,f,g,h)    MIN(MIN(MIN(MIN(MIN(MIN(MIN(a,b),c),d),e),f),g),h)

#define DARKRED(a)     (0xff-REDFROMCOLOR(a))
#define DARKGREEN(a)   (0xff-GREENFROMCOLOR(a))
#define DARKBLUE(a)    (0xff-BLUEFROMCOLOR(a))
#define FROMRED(a)   ((a) & 0xff)
#define FROMGREEN(a) (((a) & 0xff00) >> 8)
#define FROMBLUE(a)  (((a) & 0xff0000) >> 16)
#define TORED(a)   ((a) & 0xff)
#define TOGREEN(a) (((a) & 0xff) << 8)
#define TOBLUE(a)  (((a) & 0xff) << 16)
#define REDFROMCOLOR(a)     FROMRED(ColorToRGB((TColor)a))
#define GREENFROMCOLOR(a)   FROMGREEN(ColorToRGB((TColor)a))
#define BLUEFROMCOLOR(a)    FROMBLUE(ColorToRGB((TColor)a))
#define RGBCOLOR(r,g,b) (TColor)(TORED(r) | TOGREEN(g) | TOBLUE(b))

#define DARKER(a,b)    ((TColor)(TORED(MAX(0,REDFROMCOLOR(a)-DARKRED(b))) | TOGREEN(MAX(0,GREENFROMCOLOR(a)-DARKGREEN(b))) | TOBLUE(MAX(0,BLUEFROMCOLOR(a)-DARKBLUE(b))) ))

#define DARKEN(a,b)    a = DARKER(a,b)

#define COLORINC(c,d)   (TColor)(MIN(0xff0000,(ColorToRGB((TColor)c) & 0xff0000) + (ColorToRGB((TColor)d) & 0xff0000)) | \
                          MIN(0xff00,(ColorToRGB((TColor)c) & 0xff00) + (ColorToRGB((TColor)d) & 0xff00)) | \
                          MIN(0xff,(ColorToRGB((TColor)c) & 0xff) + (ColorToRGB((TColor)d) & 0xff)))
#define COLORDEC(c,d)   (TColor)(MAX(0,(ColorToRGB((TColor)c) & 0xff0000) - (ColorToRGB((TColor)d) & 0xff0000)) | \
                          MAX(0,(ColorToRGB((TColor)c) & 0xff00) - (ColorToRGB((TColor)d) & 0xff00)) | \
                          MAX(0,(ColorToRGB((TColor)c) & 0xff) - (ColorToRGB((TColor)d) & 0xff)))

#define SHIFT_JIS   932
#define KATAKANA(t) (0x30A1 <= (int)(t) && (int)(t) <= 0x30F6)
#define HIRAGANA(t) (0x3041 <= (int)(t) && (int)(t) <= 0x3093)
#define KANA(t)     (KATAKANA(t) || HIRAGANA(t))

extern bool *validkanji;
extern bool *validcode;
extern byte validcode_lowerhalf;
extern const int validcode_count;
extern const word kanji_count;
#define KANJI(t) (0x4e00 <= int(t) && int(t) <= 0x9faf && validkanji[int(t) - 0x4e00])
#define UNICODE_J(t) ( (int(t) >= 0x3000 && int(t) < 0x30ff && validcode[int(t) - 0x3000]) || (int(t) >= 0xff00 && int(t) < 0xffe0 && validcode[int(t) - 0xff00 + validcode_lowerhalf + 1]) )

#define MINITSU     ((wchar_t)(34 + 0x3041))
#define MINIKE      ((wchar_t)(86 + 0x30a0))
#define MINIKA      ((wchar_t)(85 + 0x30a0))
#define NVOWEL      ((wchar_t)0x3093)
#define KDASH       ((wchar_t)0x30fc)
#define KDASHB      ((wchar_t)0xff70)
#define KURIKAESHI  ((wchar_t)0x3005)
#define KURIKAESHI2  ((wchar_t)0x3006)
#define GANAKAESHI1  ((wchar_t)0x309D)
#define GANAKAESHI2  ((wchar_t)0x309E)
#define KANAKAESHI1  ((wchar_t)0x30FD)
#define KANAKAESHI2  ((wchar_t)0x30FE)
#define ISNVOWEL(x) ((int)(x) == 0x3093 || (int)(x) == 0x30f3)
#define DASH(t)    (t == KDASH || t == KDASHB)

#define FULLWIDTHNUMBER(t) (t >= 0xff10 && t <= 0xff19)

#define FULLWIDTHUPPERCASE(t) (t >= 0xff21 && t <= 0xff3a)
#define FULLWIDTHLOWERCASE(t) (t >= 0xff41 && t <= 0xff5a)
#define FULLWIDTHLETTER(t) (FULLWIDTHUPPERCASE(t) || FULLWIDTHLOWERCASE(t))

#define NAMESEPARATOR 0xff1d
#define NAMESEPARATOR2 0x30fb

//#define JAPAN(t)    (KANA(t) || KANJI(t) || t == KURIKAESHI || t == KURIKAESHI2 || DASH(t) ||
//t == GANAKAESHI1 || t == GANAKAESHI2 || t == KANAKAESHI1 || t == KANAKAESHI2)

//#define EXTJAPAN(t) (JAPAN(t) || FULLWIDTHNUMBER(t) || FULLWIDTHLETTER(t) || t == NAMESEPARATOR || t == NAMESEPARATOR2)

#define JAPAN(t)  (KANJI(t) || UNICODE_J(t))
#define VALIDKANA(t)   (KANA(t) || DASH(t))

#define VALIDCODE(t) (UNICODE_J(t) && !VALIDKANA(t))
inline int validindex(int t)
{
    return t >= 0x3000 && t < 0x30ff ? (t - 0x3000) : t >= 0xff00 && t < 0xffe0 ? (t - 0xff00 + validcode_lowerhalf + 1) : -1;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//Explanation (so I understand when I look back to it much later) :
//--------------------------------------------------------------------
//Because of the autosave mechanism, I have to make sure nothing gets
//saved while the dictionary is modified, and nothing gets modified
//while the dictionary gets saved.
//Add MODON at the start of any modification, and MODOFF at the end.
//EVERY MODON has to have a MODOFF!!!
//Calling MODON or MODOFF doesn't set the "modified" state of any
//collection. You must specifically do that every time, specifying exactly
//what is being changed, by calling the corresponding dictionary object's
//Modify function.
#define MODON      TWordCollectionList::Change(true); \
                        try { \
                          TWordCollectionList::ChangeResetObj __tmp_resetobj;
#define MODOFF     } catch(const wchar_t *c) { \
                          MsgBox(c,"ERROR",MB_OK); \
                        } catch(Exception &e) { \
                          MsgBox(e.Message.c_str(),"ERROR",MB_OK); \
                        } catch(...) { \
                        }
#define DISABLEMOD  TWordCollectionList::DisableSaveGuard __tmp_disableobj;
//--------------------------------------------------------------------
//--------------------------------------------------------------------
class TZPopupList : public TPopupList
{
private:
    typedef TPopupList inherited;

    struct TZPopupListCallback
    {
        TNotifyEvent callback;
        int timeout;
        unsigned long timeoutstarted;
        bool executing;
        bool waiting;
    };
    TTemplateList<TZPopupListCallback, true> *callbacks;

    bool fpreventdefault;
protected:
    virtual void __fastcall WndProc(Messages::TMessage &Message);
    void ExecuteAndRemove(int ix);
public:
    __fastcall TZPopupList();
    virtual __fastcall ~TZPopupList();

    void AddCallback(TNotifyEvent acallback, unsigned long atimeout);
    void PreventDefault(); //call at the end of each owner draw function call to prevent sub menu triangle to show up
};
#define EXITLOOP(f,ftimeout)    ((TZPopupList*)PopupList)->AddCallback(f,ftimeout);

extern TZPopupList *ZPopupList;

enum TZPopupCommand : int
{
        pcNone, pcToVocab, pcKanjiInfo, pcTranslate, pcCopy1, pcAppend1, pcCopy2, pcAppend2, pcToGroup,
        pcCollect, pcEditKanjiDef, pcToLongTerm, pcEditEntry, pcDeleteEntry, pcRevertEntry, pcSearchOnline
};

//start a timer for showing a hint on screen. The hint is automatically hidden after some time.
//Set hintowner to any pointer. When hint is called a second time with same owner, nothing happens unless owner was reset
void StartHint(void *hintowner, UnicodeString hint, bool slow = false);
//stop hint showing or stop timer hint. calling StopHint resets owner of hint and StartHint can be called with same value again
void StopHint();

enum TDateFormatSetting { dfDDMMYY, dfMMDDYY, dfYYMMDD, dfFromSettings = -1 };
enum TDictionaryMode { dmKanji, dmRomaji, dmBrowse };

struct TFormSizeInfo
{
    int left, top, width, height;
    bool maxed;
};

struct TWMSizing
{
    Cardinal Msg;
    int fwSide;
    TRect *lprc;
    int Result;

    Word WParamLo;
    Word WParamHi;
    Word LParamLo;
    Word LParamHi;
    Word ResultLo;
    Word ResultHi;
};

struct TWMHotkey
{
    Cardinal Msg;
    int id;
    unsigned int Modifiers;
    unsigned int Key;

    Word WParamLo;
    Word WParamHi;
    Word LParamLo;
    Word LParamHi;
    Word ResultLo;
    Word ResultHi;
};

template<typename T>
void memswap(T &a, T &b)
{
    T c;
    memcpy(&c, &a, sizeof(T));
    memcpy(&a, &b, sizeof(T));
    memcpy(&b, &c, sizeof(T));
}
template<typename T>
void swap(T &a, T &b)
{
    T c = a;
    a = b;
    b = c;
}

extern wchar_t radicals[214];
extern wchar_t parts[252];
extern wchar_t *wtypetext[23];
extern wchar_t *wnotetext[21];
extern wchar_t *wfieldtext[8];
extern wchar_t *ntagtext[10];
extern wchar_t *wlongtypetext[23];
extern wchar_t *wlongnotetext[21];
extern wchar_t *wlongfieldtext[8];
extern wchar_t *nlongtagtext[10];
extern double dictionary_build_version;
extern SYSTEMTIME dictionary_date;
extern SYSTEMTIME example_date;

struct TProgramPaths
{
    UnicodeString inipath; //path where the ini files are saved
    UnicodeString datapath; //path for user data
    UnicodeString installpath; //path where the program resides
    UnicodeString installdatapath; //path for main dictionary data, example sentences etc.
};
extern TProgramPaths paths;

extern bool hasstrokes;
extern bool hasrecdata;

typedef Graphics::TBitmap TBmp;
typedef unsigned short word;

struct TWord;

typedef void(__closure * TFuncPtr)();

TPoint operator +(const TPoint &a, const TPoint &b);
TPoint operator -(const TPoint &a, const TPoint &b);
TPoint& operator += (TPoint &a, const TPoint &b);
TPoint& operator -= (TPoint &a, const TPoint &b);
bool RectOverlap(const TRect &a, const TRect &b);

int rowsizes(int setting = -1);

UnicodeString GenLower(const UnicodeString &str);
UnicodeString GenUpper(const UnicodeString &str);
void GenLower(const wchar_t *src, wchar_t *dest, int len);
void GenUpper(const wchar_t *src, wchar_t *dest, int len);
void GenLower(const wchar_t *src, wchar_t *dest);
void GenUpper(const wchar_t *src, wchar_t *dest);
void GenLower(wchar_t *str);
void GenUpper(wchar_t *str);
int GenCompare(const wchar_t *a, const wchar_t *b, bool ignorecase = false);
int GenCompareN(const wchar_t *a, const wchar_t *b, int len, bool ignorecase = false);
int GenCompareI(const wchar_t *a, const wchar_t *b);
int GenCompareIN(const wchar_t *a, const wchar_t *b, int len);
UnicodeString StringToken(UnicodeString &str, const wchar_t *delim, bool empty); // NOT TESTED! Returns first token from beginning of string, removing it from the string. Any character in delim acts as a delimiter for the token. Set empty to false if empty tokens should be skipped

// Data for FirstToken and NextToken, so separate token searches will be possible.
struct TTokenSearchData
{
    wchar_t *current;
    wchar_t *next;
    wchar_t separator; // Separator found after the last call to FirstToken or NextToken.
};
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t *delim, bool empty = false); // Returns the first token starting from string 'str'. Delim is an array of delimiter characters. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t delim, bool empty = false); // Returns the first token starting from string 'str'. Delim is the token delimiter character. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t *delim, wchar_t *enddelim); // Returns the first token starting from string 'str'. Delim is an array of delimiter characters. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t delim, wchar_t *enddelim); // Returns the first token starting from string 'str'. Delim is an array of delimiter characters. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t *delim, wchar_t enddelim); // Returns the first token starting from string 'str'. Delim is the token delimiter character. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t delim, wchar_t enddelim); // Returns the first token starting from string 'str'. Delim is the token delimiter character. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t *delim, bool empty = false); // Returns the next token after a call to FirstToken or NextToken. Delim is an array of delimiter characters. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t delim, bool empty = false); // Returns the next token after a call to FirstToken or NextToken. Delim is the token delimiter character. If empty is false, tokens are searched until one found which is not 0 long. Searchdata keeps track of the current search position.
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t *delim, const wchar_t *enddelim); // Returns the next token after a call to FirstToken or NextToken. Delim and enddelim are arrays of delimiter characters. Tokens ended with 'delim' are searched until one found which is not 0 long. If a token ends with a character in 'enddelim', it is returned without length checking. Searchdata keeps track of the current search position. If a character in delim found, enddelim is not considered, so avoid using the same delimiter in both arrays.
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t delim, const wchar_t *enddelim); // Returns the next token after a call to FirstToken or NextToken. Delim and enddelim are arrays of delimiter characters. Tokens ended with character from 'delim' are searched until one found which is not 0 long. If a token ends with a character in 'enddelim', it is returned without length checking. Searchdata keeps track of the current search position. If a character in delim found, enddelim is not considered, so avoid using the same delimiter in both arrays.
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t *delim, const wchar_t enddelim); // Returns the next token after a call to FirstToken or NextToken. Delim is an array of delimiter characters, enddelim is a delimiter character. Tokens ended with character from 'delim' are searched until one found which is not 0 long. If a token ends with 'enddelim', it is returned without length checking. Searchdata keeps track of the current search position. If a character in delim found, enddelim is not considered, so avoid using the same delimiter in both arrays.
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t delim, const wchar_t enddelim); // Returns the next token after a call to FirstToken or NextToken. Delim and enddelim are delimiter characters. Tokens ended with 'delim' are searched until one found which is not 0 long. If a token ends with 'enddelim', it is returned without length checking. Searchdata keeps track of the current search position. If a character in delim found, enddelim is not considered, so avoid using the same delimiter in both arrays.
void RestoreToken(TTokenSearchData &searchdata); // Makes another call to NextToken return the same value as before if the same parameters are passed.

//int WCSCMPI(const wchar_t *a, const wchar_t *b);
//int WCSCMPIN(const wchar_t *a, const wchar_t *b,int len);
wchar_t * WCSSTRI(wchar_t *a, const wchar_t *b);

bool GetClipText(UnicodeString &c, int maxlen); //returns true = success
void SetClipText(UnicodeString const &c);
//copies or appends to clipboard and shows clipboard viewer when set
void ClipAppend(UnicodeString const &s);
void ClipAppend(AnsiString const &s);
void ClipReplace(UnicodeString const &s);
void ClipReplace(AnsiString const &s);
void ClipAppend(wchar_t const &c);
void ClipReplace(wchar_t const &c);
void ClipAppend(wchar_t *c);
void ClipReplace(wchar_t *c);
//
void polyrect(TCanvas *c, TRect r, int width = 1); //draw a rectangular line with pen
bool ValidFileName(UnicodeString s);

//Create backup with TEMP extension
void Backup(UnicodeString name);
//Restore backup from TEMP extension
void RestoreBackup(UnicodeString name);

//put this in the wmsizing message handler function to solve all the problems. works better then the borland equivalent.
void ConstraintSizing(TWMSizing &msg, int minwidth, int minheight);
void ConstraintSizing(TWMSizing &msg, int minwidth, int minheight, int maxwidth, int maxheight);
//when we want to constrain the size, but also to set steps in the resizing (eg. kanji box boundaries)
void ConstraintSizing(TWMSizing &msg, int minwidth, int minheight, int maxwidth, int maxheight, int widthdiff, int modx, int heightdiff, int mody);

int StriIndex(UnicodeString &s, TStrings *list);
int RealIndex(TComboBox *box, bool ignorecase = true);

typedef void(__closure * TCPPNotifyEvent)(void *caller);
void AppActivateEvent(void *caller, TCPPNotifyEvent fprc, bool activate, bool add);
enum TSettingsProcState { spsBegin, spsEnd };
typedef void(__closure * TSettingsProc)(void *caller, TSettingsProcState state);
void SettingsChangeEvent(void *caller, TSettingsProc fprc, bool add);

bool has_kanji(const wchar_t *c); // The passed string contains one or more kanji characters.
bool has_kana(const wchar_t *c); // The passed string contains one or more kana characters.
bool only_kana(const wchar_t *c); // The passed string only contains kana characters and the dash. If there is no kana just a dash, false is returned.
bool valid_kanji(const wchar_t *c); // The passed string only contains characters that are valid as the kanji part of a word entry.
bool has_jpchar(const wchar_t *c); // The passed string contains one or more characters which are valid Japanese input.
UnicodeString DateString(TDateTime d, bool fullyear = false, TDateFormatSetting format = dfFromSettings);
UnicodeString DateString(float d, bool fullyear = false, TDateFormatSetting format = dfFromSettings);
UnicodeString DateString(int d, bool fullyear = false, TDateFormatSetting format = dfFromSettings);
UnicodeString DateString(SYSTEMTIME d, bool fullyear = false, TDateFormatSetting format = dfFromSettings);
UnicodeString DateString(int year, int month, int day, bool fullyear = false, TDateFormatSetting format = dfFromSettings);
UnicodeString TimeString(double d);
UnicodeString generate_timestring(int d); // Same as TimeString but based on seconds not days.
int MsgBox(UnicodeString message, UnicodeString caption, int buttons);
bool QueryBox(UnicodeString message, UnicodeString caption, UnicodeString &str);
bool BrowseForFolder(UnicodeString Caption, UnicodeString &path, bool newfolderbtn, bool folderedit, bool showshares, bool showfiles, bool validatedir, TControl *parent);
void DisableRecognize();
void EnableRecognize();
bool ExecuteDialog(TCommonDialog *dlg);

void AddStyledText(TRichEdit *edit, UnicodeString str, TTextAttributes *attrib);
void AddStyledText(TRichEdit *edit, UnicodeString str, TFontName fontname, TFontStyles fontstyle, TColor color, int height, TFontPitch fontpitch, TFontCharset charset);

UnicodeString wtoAS(word w); // Word to UnicodeString. Returns 4 characters, eg 4ab8
word AStow(UnicodeString str); // UnicodeString to Word. Expects string in hex.
//UnicodeString ASstripnegative(const UnicodeString &s); // removes negative sign and useless spaces from string
bool ScreenSaverOn();

// Enum specifying the format of the written UTF8 string.
// The Byte, Word, Int etc. is the size of number type written or read into the file.
// Use sltNoLength if no length is necessary. In that case, read cannot read the string back from the file.
// The ...AddNULL doesn't add a NULL character when writing, but adds a trailing null to the string returned from reading.
enum TStringLengthType { sltByte, sltWord, sltInt, sltByteAddNULL, sltWordAddNULL, sltIntAddNULL, sltNoLength };
void write(const UnicodeString &str, FILE *f, TStringLengthType type, int len = -1);
void write(const wchar_t* str, FILE *f, TStringLengthType type, int len = -1);
void write(const UnicodeString &str, FILE *f, int len = -1); // Same as calling the previous overrides with sltNoLength.
void write(const wchar_t* str, FILE *f, int len = -1); // Same as calling the previous overrides with sltNoLength.
int read(UnicodeString &str, FILE *f, TStringLengthType type);
int read(wchar_t* &str, FILE *f, TStringLengthType type);
int readskip(FILE *f, TStringLengthType type);

bool pathaccess(UnicodeString path, bool checkdelaccess = false);
UnicodeString FullPathFromBackupPath(UnicodeString path);

bool PathsDifferent(UnicodeString path1, UnicodeString path2); // Determines whether two paths to folders point to the same location.

// Can be used in sorting.
int WordCompare(const wchar_t *kanjia, const wchar_t *kanaa, const wchar_t *kanjib, const wchar_t *kanab);

void SplitToStrings(TStringList *strings, UnicodeString str); // Splits the passed str to lines (both \r\n and \n ending) and adds them to the strings list.

int UTF8length(const char *str); // Return number of characters in UTF-8 encoded mbcs string.
char* toUTF8(const wchar_t *str, int &len); // Convert wide character string to UTF-8 mbcs. len specifies original string length in wchar_t units, and returns new string length in char units. WARNING the return value must be freed by the caller.
wchar_t* fromUTF8(const char *str, int &len); // Convert UTF-8 mbcs to wide character string. len specifies original string length in char units, and returns new string length in wchar_t units. WARNING the return value must be freed by the caller.
UTF8String toUTF8String(const wchar_t *str, int len = -1); // Convert wide character array to UTF8String. Specify len for the wchar_t length of the input array.
AnsiString toAnsiString(const wchar_t *str, int len = -1); // Convert wide character array to AnsiString. Specify len for the wchar_t length of the input array. Conversion is done with the local codepage.

bool MatchVersion(int major, int minor, bool exact = false); // Returns true if the operating system is the given version (or above if exact is false). Versions: w2000 5.0, XP 5.1, xp64bit 5.2, vista 6.0, win7 6.1, win8 6.2

namespace Inifiles
{
    class TCustomIniFile;
};

double IniReadDouble(Inifiles::TCustomIniFile *ini, const UnicodeString &section, const UnicodeString &ident, double defaultvalue);
void IniWriteDouble(Inifiles::TCustomIniFile *ini, const UnicodeString &section, const UnicodeString &ident, double value);

tagMONITORINFO MonitorInfo(int x, int y);

bool ExecuteOpenDialog(TOpenDialog *dialog, UnicodeString defext, bool overwritecheck, bool existscheck, UnicodeString &selectedfile); // Executes a dialog appending the passed default extensions to the file name, if the selected file's extension does not match any in the extensions accepted by the dialog. Asks for confirmation if the file exists and overwritecheck is true. Shows dialog again if file does not exist with the appended extension and existscheck is true. This function is necessary because the DefaultExt value of dialogs only accept 3 characters.

const wchar_t* MakeExceptionObj(const wchar_t *str);
#define THROW(c) throw MakeExceptionObj(c)

//---------------------------------------------------------------------------
inline TColor coloravg(TColor base, TColor modifier, double midpoint)
{
    midpoint = min(max(midpoint, 0), 1);
    base = (TColor)ColorToRGB(base);
    modifier = (TColor)ColorToRGB(modifier);

    double modpoint = 1 - midpoint;

    return (TColor)(((int)((base & 0xff) * midpoint + (modifier & 0xff) * modpoint) & 0xff) | (((int)(((base & 0xff00) >> 8) * midpoint + ((modifier & 0xff00) >> 8) * modpoint) & 0xff) << 8) | (((int)(((base & 0xff0000) >> 16) * midpoint + ((modifier & 0xff0000) >> 16) * modpoint) & 0xff) << 16));
}
//---------------------------------------------------------------------------
inline TColor coloravginv(TColor base, TColor modifier, double midpoint)
{
    midpoint = max(midpoint, 0);
    base = (TColor)ColorToRGB(base);
    modifier = (TColor)ColorToRGB(modifier);

    return (TColor)(max(0, min(0xff, (base & 0xff) - (int)(((modifier & 0xff) - (base & 0xff)) * midpoint))) | ((max(0, min(0xff, ((base & 0xff00) >> 8) - (int)((((modifier & 0xff00) >> 8) - ((base & 0xff00) >> 8)) * midpoint))) & 0xff) << 8) | ((max(0, min(0xff, ((base & 0xff0000) >> 16) - (int)((((modifier & 0xff0000) >> 16) - ((base & 0xff0000) >> 16)) * midpoint))) & 0xff) << 16));
}
//---------------------------------------------------------------------------
inline void colordiff(TColor base, TColor modifier, int &r, int &g, int &b)
{
    base = (TColor)ColorToRGB(base);
    modifier = (TColor)ColorToRGB(modifier);

    r = REDFROMCOLOR(base) - REDFROMCOLOR(modifier);
    g = GREENFROMCOLOR(base) - GREENFROMCOLOR(modifier);
    b = BLUEFROMCOLOR(base) - BLUEFROMCOLOR(modifier);
}
//---------------------------------------------------------------------------
inline TColor colormod(TColor col, int r, int g, int b)
{
    col = (TColor)ColorToRGB(col);
    return (TColor)(TORED(min(max(0, (FROMRED(col) + r)), 0xff)) | TOGREEN(min(max(0, (FROMGREEN(col) + g)), 0xff)) | TOBLUE(min(max(0, (FROMBLUE(col) + b)), 0xff)));
}
//---------------------------------------------------------------------------
inline int colorcmp(TColor &a, TColor &b)
{
    return (REDFROMCOLOR(a) - REDFROMCOLOR(b)) + (GREENFROMCOLOR(a) - GREENFROMCOLOR(b)) + (BLUEFROMCOLOR(a) - BLUEFROMCOLOR(b));
}
//---------------------------------------------------------------------------
//change the font's color when it is too similar to the background color
inline void fixtextcolor(TColor &textcolor, TColor backgroundcolor)
{
    int r, g, b;
    colordiff(textcolor, backgroundcolor, r, g, b);
    if (abs(r) + abs(g) + abs(b) < 0xc6)
    {
        colordiff(backgroundcolor, (TColor)0, r, g, b);
        if (abs(r + g + b) < 0x1fe)
        {
            if (colorcmp(backgroundcolor, textcolor) > 0)//too dark
                        textcolor = clWhite;
            else
            {
                int avgdif = (0xc6 - (abs(r) + abs(g) + abs(b))) / 3;
                textcolor = COLORINC(textcolor, RGBCOLOR(avgdif, avgdif, avgdif));

            }
        }
        else
        {
            colordiff(backgroundcolor, (TColor)0xffffff, r, g, b);
            if (abs(r + g + b) < 0x1fe)
            {
                if (colorcmp(backgroundcolor, textcolor) < 0)//too light
                            textcolor = clBlack;
                else
                {
                    int avgdif = (0xc6 - (abs(r) + abs(g) + abs(b))) / 3;
                    textcolor = COLORDEC(textcolor, RGBCOLOR(avgdif, avgdif, avgdif));
                }
            }

        }
    }
}
//---------------------------------------------------------------------------
class TPointList : public TTemplateList<TDoublePoint, true>
{
private:
    typedef TTemplateList<TDoublePoint, true> inherited;
public:
    virtual ~TPointList()
    {
        Clear();
    }

    inline int Add(TDoublePoint const &p)
    {
        return inherited::Add(new TDoublePoint(p));
    }

    inline int Add(double x, double y)
    {
        return Add(DoublePoint(x, y));
    }
};
//---------------------------------------------------------------------------
void ApplicationRealInit();
//---------------------------------------------------------------------------

#ifdef _DEBUG
struct TGlobalTest
{
    static TIntegerList *numbers;

    int num;
    TGlobalTest(int num);
    ~TGlobalTest();
};
#endif

#endif
