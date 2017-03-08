//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include <IniFiles.hpp>
#include <Character.hpp>
#include <CheckLst.hpp>
#include <RegularExpressions.hpp>
#include "main.h"
#include "modallist.h"
#include "dropdown.h"
#include "module.h"
#include "controlimages.h"
#include "settings.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#ifdef _DEBUG
static TGlobalTest tester1(204);

TIntegerList *TGlobalTest::numbers = NULL;
TGlobalTest::TGlobalTest(int num) : num(num)
{
    if (numbers == NULL)
        numbers = new TIntegerList;
    TGlobalTest::numbers->Add(num);
}
TGlobalTest::~TGlobalTest()
{
    TGlobalTest::numbers->Remove(num);
    if (TGlobalTest::numbers->Count == 0)
    {
        delete TGlobalTest::numbers;
        TGlobalTest::numbers = NULL;
    }
}
#endif

TZPopupList *ZPopupList = NULL;

//---------------------------------------------------------------------------
__fastcall TZPopupList::TZPopupList() : fpreventdefault(false), inherited()
{
    callbacks = new TTemplateList<TZPopupListCallback, true>;
}
//---------------------------------------------------------------------------
__fastcall TZPopupList::~TZPopupList()
{
    //for (int ix = 0; ix < callbacks->Count; ix++)
    //    delete callbacks->Items[ix];
    delete callbacks;
    callbacks = NULL;
}
//---------------------------------------------------------------------------
void TZPopupList::AddCallback(TNotifyEvent acallback, unsigned long atimeout)
{
    if (!callbacks)
        return;
    TZPopupListCallback *cb = new TZPopupListCallback;
    cb->callback = acallback;
    cb->timeout = atimeout;
    cb->timeoutstarted = 0;
    cb->executing = false;
    cb->waiting = false;
    callbacks->Add(cb);
}
//---------------------------------------------------------------------------
void TZPopupList::ExecuteAndRemove(int ix)
{
    callbacks->Items[ix]->callback(NULL);
    //delete cb;
    callbacks->Delete(ix);
}
//---------------------------------------------------------------------------
#define WM_Z_MENUTIMEOUT     (WM_APP + 211)
void __fastcall TZPopupList::WndProc(Messages::TMessage &Message)
{
    TZPopupListCallback *cb;
    if (Message.Msg == WM_ENTERMENULOOP && callbacks && callbacks->Count)
    {
        while (callbacks->Count)
        {
            cb = callbacks->Items[0];
            if (callbacks->Count == 1 && !cb->waiting)
                break;
            ExecuteAndRemove(0);
        }
    }

    if (Message.Msg == WM_EXITMENULOOP && callbacks && callbacks->Count)
    {
        Message.Result = 0;
        while (callbacks->Count)
        {
            cb = callbacks->Items[0];
            if (callbacks->Count == 1 && !cb->waiting)
                break;
            ExecuteAndRemove(0);
            cb = NULL;
        }
        if (!cb)
            return;
        cb->timeoutstarted = GetTickCount();
        cb->waiting = true;
        PostMessage(Window, WM_Z_MENUTIMEOUT, 0, 0);
        return;
    }

    if (Message.Msg == WM_Z_MENUTIMEOUT && callbacks && callbacks->Count)
    {
        Message.Result = 0;
        while (callbacks->Count > 1)
            ExecuteAndRemove(0);

        cb = callbacks->Items[0];
        if (!cb->waiting || cb->executing)
            return;
        if (abs(long(GetTickCount() - cb->timeoutstarted)) >= cb->timeout)
            ExecuteAndRemove(0);
        else
            PostMessage(Window, WM_Z_MENUTIMEOUT, 0, 0);
        return;
    }

    if (Message.Msg == WM_COMMAND && callbacks && callbacks->Count)
    {
        while (callbacks->Count)
        {
            cb = callbacks->Items[0];
            if (callbacks->Count == 1 && cb->waiting && !cb->executing)
                break;
            ExecuteAndRemove(0);
            cb = NULL;
        }
        if (cb)
            cb->executing = true;
    }

    fpreventdefault = false;
    TPopupList::WndProc(Message);

    if (Message.Msg == WM_DRAWITEM && fpreventdefault)
    {
        PDrawItemStruct pd = (PDrawItemStruct)Message.LParam;
        ExcludeClipRect(pd->hDC, pd->rcItem.left, pd->rcItem.top, pd->rcItem.right, pd->rcItem.bottom);
    }
    fpreventdefault = false;

    if (Message.Msg == WM_COMMAND && callbacks && callbacks->Count)
    {
        while (callbacks->Count)
        {
            cb = callbacks->Items[0];
            if (callbacks->Count == 1 && !cb->waiting)
                break;
            ExecuteAndRemove(0);
        }
    }
}
//---------------------------------------------------------------------------
void TZPopupList::PreventDefault()
{
    fpreventdefault = true;
}
//---------------------------------------------------------------------------

struct THintData
{
    void *owner;
    UnicodeString hint;
    int id;
    bool visible;

    THintData()
    {
        owner = NULL, hint = "", id = 0, visible = false;
    }
};

THintData defhintdata;
void CALLBACK defhintproc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    static bool timing = false;
    if (timing)
        return;
    timing = true;
    KillTimer(NULL, defhintdata.id);
    Application->HideHint();
    if (!defhintdata.visible)
    {
        TRect r = hint8->CalcHintRect(Screen->Width / 3, defhintdata.hint, NULL);
        TPoint pos;
        GetCursorPos(&pos);
        OffsetRect(&r, pos.x, pos.y + GetSystemMetrics(SM_CYCURSOR)*2 / 3);
        hint8->ActivateHint(r, defhintdata.hint);
        defhintdata.visible = true;
        int htime;
        SystemParametersInfo(SPI_GETMESSAGEDURATION, 0, &htime, NULL);
        defhintdata.id = SetTimer(NULL, 1, htime * 1000, defhintproc);
    }
    else
    {
        hint8->ReleaseHandle();
        defhintdata.visible = false;
        defhintdata.id = 0;
    }
    timing = false;
}
void StartHint(void *hintowner, UnicodeString hint, bool slow)
{
    if (defhintdata.owner == hintowner)
        return;
    if (defhintdata.id)
        StopHint();
    defhintdata.owner = hintowner;
    defhintdata.hint = hint;
    int htime;
    SystemParametersInfo(SPI_GETMOUSEHOVERTIME, 0, &htime, NULL);
    if (slow)
        htime *= 3;
    defhintdata.id = SetTimer(NULL, 1, htime, defhintproc);
}
void StopHint()
{
    if (!defhintdata.id)
        return;
    KillTimer(NULL, defhintdata.id);
    defhintdata.id = 0;
    defhintdata.owner = NULL;
    defhintdata.visible = false;
    hint8->ReleaseHandle();
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

wchar_t *compare_a_w = NULL;
int compare_a_len_w = 0;
wchar_t *compare_b_w = NULL;
int compare_b_len_w = 0;

const DWORD LOCALE_ENGLISH_DEFAULT = MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
const DWORD gen_lcid = Win32MajorVersion < 5 ? LOCALE_ENGLISH_DEFAULT : LOCALE_INVARIANT;

// General character functions that do not depend on the current locale
UnicodeString GenLower(const UnicodeString &str)
{
    return TCharacter::ToLower(str);
}
UnicodeString GenUpper(const UnicodeString &str)
{
    return TCharacter::ToUpper(str);
}
void GenLower(const wchar_t *src, wchar_t *dest, int len)
{
    LCMapString(gen_lcid, LCMAP_LOWERCASE, src, len, dest, len);
}
void GenUpper(const wchar_t *src, wchar_t *dest, int len)
{
    LCMapString(gen_lcid, LCMAP_UPPERCASE, src, len, dest, len);
}
void GenLower(const wchar_t *src, wchar_t *dest)
{
    GenLower(src, dest, wcslen(src) + 1);
    //int len = wcslen(src)+1;
    //LCMapString(gen_lcid,LCMAP_LOWERCASE,src,len,dest,len);
}
void GenUpper(const wchar_t *src, wchar_t *dest)
{
    GenUpper(src, dest, wcslen(src) + 1);
    //int len = wcslen(src)+1;
    //LCMapString(gen_lcid,LCMAP_UPPERCASE,src,len,dest,len);
}
void GenLower(wchar_t *str)
{
    GenLower(str, str, wcslen(str) + 1);
    //int len = wcslen(str)+1;
    //LCMapString(gen_lcid,LCMAP_LOWERCASE,str,len,str,len);
}
void GenUpper(wchar_t *str)
{
    GenUpper(str, str, wcslen(str) + 1);
    //int len = wcslen(str)+1;
    //LCMapString(gen_lcid,LCMAP_UPPERCASE,str,len,str,len);
}

/*int GenCompare(const wchar_t *a, const wchar_t *b, bool ignorecase);
 {
 return CompareStringW(gen_lcid, SORT_STRINGSORT | (ignorecase ? NORM_IGNORECASE : 0), a,-1,b,-1)-2;
 }
 */
inline int minlen(const wchar_t *c, int len)
{
    for (int ix = 0; ix < len; ++ix)
        if (c[ix] == 0)
            return ix + 1;
    return len;
}

/*int GenCompareN(const wchar_t *a, const wchar_t *b, int len, bool ignorecase);
 {
 return CompareStringW(gen_lcid, SORT_STRINGSORT | (ignorecase ? NORM_IGNORECASE : 0), a,minlen(a,len),b,minlen(b,len))-2;
 }*/

//---------------------------------------------------------------------------
//int WCSCMPI(const wchar_t *a, const wchar_t *b)
int GenCompare(const wchar_t *a, const wchar_t *b, bool ignorecase)
{
    int t = wcslen(a);
    if (t > compare_a_len_w)
    {
        delete[] compare_a_w;
        compare_a_len_w = t;
        compare_a_w = new wchar_t[t + 1];
    }
    if (ignorecase)
        GenLower(a, compare_a_w);
    else
        wcscpy(compare_a_w, a);

    //AnsiStrLower(compare_a_w);

    t = wcslen(b);
    if (t > compare_b_len_w)
    {
        delete[] compare_b_w;
        compare_b_len_w = t;
        compare_b_w = new wchar_t[t + 1];
    }
    if (ignorecase)
        GenLower(b, compare_b_w);
    else
        wcscpy(compare_b_w, b);
    //AnsiStrLower(compare_b_w);

    return wcscmp(compare_a_w, compare_b_w);

}
int GenCompareI(const wchar_t *a, const wchar_t *b)
{
    return GenCompare(a, b, true);
}
//---------------------------------------------------------------------------
//int WCSCMPIN(const wchar_t *a, const wchar_t *b,int len)
int GenCompareN(const wchar_t *a, const wchar_t *b, int len, bool ignorecase)
{
    int t = minlen(a, len);
    if (t > compare_a_len_w)
    {
        delete[] compare_a_w;
        compare_a_len_w = t;
        compare_a_w = new wchar_t[t + 1];
    }
    if (ignorecase)
        GenLower(a, compare_a_w, t);
    else
        wcsncpy(compare_a_w, a, t);
    compare_a_w[t] = 0;
    //AnsiStrLower(compare_a_w);

    t = minlen(b, len);
    if (t > compare_b_len_w)
    {
        delete[] compare_b_w;
        compare_b_len_w = t;
        compare_b_w = new wchar_t[t + 1];
    }
    if (ignorecase)
        GenLower(b, compare_b_w, t);
    else
        wcsncpy(compare_b_w, b, t);
    compare_b_w[t] = 0;
    //AnsiStrLower(compare_b_w);

    return wcscmp(compare_a_w, compare_b_w);

}
int GenCompareIN(const wchar_t *a, const wchar_t *b, int len)
{
    return GenCompareN(a, b, len, true);
}
//---------------------------------------------------------------------------
//FindNLSString - for GenSearch that replaces wcsstri for non localized strstri searches

wchar_t * WCSSTRI(wchar_t *a, const wchar_t *b)
{
    int t = wcslen(a);
    if (t > compare_a_len_w)
    {
        if (compare_a_w)
            delete[] compare_a_w;
        compare_a_len_w = t;
        compare_a_w = new wchar_t[t + 1];
    }
    //LCMapString(gen_lcid,LCMAP_LOWERCASE,a,t+1,compare_a_w,t+1);
    wcscpy(compare_a_w, a);
    AnsiStrLower(compare_a_w);

    t = wcslen(b);
    if (t > compare_b_len_w)
    {
        if (compare_b_w)
            delete[] compare_b_w;
        compare_b_len_w = t;
        compare_b_w = new wchar_t[t + 1];
    }
    //LCMapString(gen_lcid,LCMAP_LOWERCASE,b,t+1,compare_b_w,t+1);
    wcscpy(compare_b_w, b);
    AnsiStrLower(compare_b_w);

    wchar_t *r = wcsstr(compare_a_w, compare_b_w);
    if (!r)
        return NULL;
    return (a + (r - compare_a_w));
}
//---------------------------------------------------------------------------
// NOT TESTED!
UnicodeString StringToken(UnicodeString &str, const wchar_t *delim, bool empty)
{
    int pos = 0;

    int len = str.Length();
    do
    {
        ++pos;
        while (pos <= len && wcschr(delim, str[pos]) == NULL)
            ++pos;
    }
    while(!empty && pos == 1 && pos <= len);

    UnicodeString result;
    if (pos > 1)
        result = str.SubString(1, pos - 1);
    str = str.SubString(pos + 1, str.Length() - pos);
    return result;
}
//---------------------------------------------------------------------------
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t *delim, bool empty)
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextToken(searchdata, delim, empty);
}
//---------------------------------------------------------------------------
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t delim, bool empty)
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextToken(searchdata, delim, empty);
}
//---------------------------------------------------------------------------
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t *delim, const wchar_t *enddelim)
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextToken(searchdata, delim, enddelim);
}
//---------------------------------------------------------------------------
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t *delim, const wchar_t enddelim)
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextToken(searchdata, delim, enddelim);
}
//---------------------------------------------------------------------------
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t delim, const wchar_t *enddelim)
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextToken(searchdata, delim, enddelim);
}
//---------------------------------------------------------------------------
wchar_t *FirstToken(TTokenSearchData &searchdata, wchar_t *str, const wchar_t delim, const wchar_t enddelim)
{
    searchdata.separator = 0;
    searchdata.next = str;

    return NextToken(searchdata, delim, enddelim);
}
//---------------------------------------------------------------------------
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t *delim, bool empty)
{
    searchdata.current = searchdata.next;
    if (searchdata.current)
    {
        while (true)
        {
            while (*searchdata.next != 0 && wcschr(delim, *searchdata.next) == NULL)
                ++searchdata.next;

            searchdata.separator = *searchdata.next;
            if (!empty && searchdata.next == searchdata.current)
            {
                if (searchdata.separator == 0)
                {
                    searchdata.next = 0;
                    searchdata.current = 0;
                }
                else
                {
                    ++searchdata.next;
                    ++searchdata.current;
                    continue;
                }
            }

            if (searchdata.separator == 0)
                searchdata.next = NULL;
            else
            {
                *searchdata.next = NULL;
                ++searchdata.next;
            }
            break;
        }
    }
    return searchdata.current;
}
//---------------------------------------------------------------------------
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t delim, bool empty)
{
    searchdata.current = searchdata.next;
    if (searchdata.current)
    {
        while (true)
        {
            while (*searchdata.next != 0 && delim != *searchdata.next)
                ++searchdata.next;

            searchdata.separator = *searchdata.next;
            if (!empty && searchdata.next == searchdata.current)
            {
                if (searchdata.separator == 0)
                {
                    searchdata.next = 0;
                    searchdata.current = 0;
                }
                else
                {
                    ++searchdata.next;
                    ++searchdata.current;
                    continue;
                }
            }

            if (searchdata.separator == 0)
                searchdata.next = NULL;
            else
            {
                *searchdata.next = NULL;
                ++searchdata.next;
            }
            break;
        }
    }
    return searchdata.current;
}
//---------------------------------------------------------------------------
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t *delim, const wchar_t *enddelim)
{
    searchdata.current = searchdata.next;
    if (searchdata.current)
    {
        while (true)
        {
            bool endfound = false;
            while (*searchdata.next != 0 && wcschr(delim, *searchdata.next) == NULL && (endfound = (wcschr(enddelim, *searchdata.next) != NULL)) == false)
                ++searchdata.next;

            searchdata.separator = *searchdata.next;
            if (!endfound && searchdata.next == searchdata.current)
            {
                if (searchdata.separator == 0)
                {
                    searchdata.next = 0;
                    searchdata.current = 0;
                }
                else
                {
                    ++searchdata.next;
                    ++searchdata.current;
                    continue;
                }
            }

            if (searchdata.separator == 0)
                searchdata.next = NULL;
            else
            {
                *searchdata.next = NULL;
                ++searchdata.next;
            }
            break;
        }
    }
    return searchdata.current;
}
//---------------------------------------------------------------------------
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t *delim, const wchar_t enddelim)
{
    searchdata.current = searchdata.next;
    if (searchdata.current)
    {
        while (true)
        {
            bool endfound = false;
            while (*searchdata.next != 0 && wcschr(delim, *searchdata.next) == NULL && (endfound = (enddelim == *searchdata.next)) == false)
                ++searchdata.next;

            searchdata.separator = *searchdata.next;
            if (!endfound && searchdata.next == searchdata.current)
            {
                if (searchdata.separator == 0)
                {
                    searchdata.next = 0;
                    searchdata.current = 0;
                }
                else
                {
                    ++searchdata.next;
                    ++searchdata.current;
                    continue;
                }
            }

            if (searchdata.separator == 0)
                searchdata.next = NULL;
            else
            {
                *searchdata.next = NULL;
                ++searchdata.next;
            }
            break;
        }
    }
    return searchdata.current;
}
//---------------------------------------------------------------------------
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t delim, const wchar_t *enddelim)
{
    searchdata.current = searchdata.next;
    if (searchdata.current)
    {
        while (true)
        {
            bool endfound = false;
            while (*searchdata.next != 0 && delim != *searchdata.next && (endfound = (wcschr(enddelim, *searchdata.next) != NULL)) == false)
                ++searchdata.next;

            searchdata.separator = *searchdata.next;
            if (!endfound && searchdata.next == searchdata.current)
            {
                if (searchdata.separator == 0)
                {
                    searchdata.next = 0;
                    searchdata.current = 0;
                }
                else
                {
                    ++searchdata.next;
                    ++searchdata.current;
                    continue;
                }
            }

            if (searchdata.separator == 0)
                searchdata.next = NULL;
            else
            {
                *searchdata.next = NULL;
                ++searchdata.next;
            }
            break;
        }
    }
    return searchdata.current;
}
//---------------------------------------------------------------------------
wchar_t *NextToken(TTokenSearchData &searchdata, const wchar_t delim, const wchar_t enddelim)
{
    searchdata.current = searchdata.next;
    if (searchdata.current)
    {
        while (true)
        {
            bool endfound = false;
            while (*searchdata.next != 0 && delim != *searchdata.next && (endfound = (enddelim == *searchdata.next)) == false)
                ++searchdata.next;

            searchdata.separator = *searchdata.next;
            if (!endfound && searchdata.next == searchdata.current)
            {
                if (searchdata.separator == 0)
                {
                    searchdata.next = 0;
                    searchdata.current = 0;
                }
                else
                {
                    ++searchdata.next;
                    ++searchdata.current;
                    continue;
                }
            }

            if (searchdata.separator == 0)
                searchdata.next = NULL;
            else
            {
                *searchdata.next = NULL;
                ++searchdata.next;
            }
            break;
        }
    }
    return searchdata.current;
}
//---------------------------------------------------------------------------
void RestoreToken(TTokenSearchData &searchdata)
{
    if (searchdata.next)
        *(searchdata.next - 1) = searchdata.separator; // Restore zeroed separator in line.
    searchdata.next = searchdata.current;
}
//---------------------------------------------------------------------------
TPoint operator +(const TPoint &a, const TPoint &b)
{
    return Point(a.x + b.x, a.y + b.y);
}
//---------------------------------------------------------------------------
TPoint operator -(const TPoint &a, const TPoint &b)
{
    return Point(a.x - b.x, a.y - b.y);
}
//---------------------------------------------------------------------------
TPoint& operator += (TPoint &a, const TPoint &b)
{
    return a = a + b;
}
//---------------------------------------------------------------------------
TPoint& operator -= (TPoint &a, const TPoint &b)
{
    return a = a - b;
}
//---------------------------------------------------------------------------
bool RectOverlap(const TRect &a, const TRect &b)
{
    TRect tmp;
    return IntersectRect(tmp, a, b);
}
//---------------------------------------------------------------------------
int RealIndex(TComboBox *box, bool ignorecase)
{
    UnicodeString btext = box->Text.Trim();


    if (box->Items->Count && box->ItemIndex >= 0 && ((ignorecase && btext.LowerCase() == box->Items->Strings[box->ItemIndex].LowerCase()) || (!ignorecase && btext == box->Items->Strings[box->ItemIndex])))
        return box->ItemIndex;

    for (int i = 0; i < box->Items->Count; i++)
        if ((ignorecase && btext.LowerCase() == box->Items->Strings[i].LowerCase()) || (!ignorecase && btext == box->Items->Strings[i]))
            return i;
    return -1;
}
//---------------------------------------------------------------------------
int StriIndex(UnicodeString &s, TStrings *list)
{
    int i = -1;
    s = s.Trim();
    for (int ix = 0; ix < list->Count && i < 0; ++ix)
    {
        if (s.LowerCase() == list->Strings[ix].LowerCase())
        {
            i = ix;
            s = list->Strings[ix]; // Case insensitive match so make sure to preserve case.
        }
    }
    return i;
}
//---------------------------------------------------------------------------
bool HasChecked(Checklst::TCheckListBox *box, int first, int last)
{
    if (last == -1)
        last = box->Items->Count - 1;
    for (int ix = max(first, 0); ix < min(last + 1, box->Items->Count); ++ix)
        if (box->Checked[ix])
            return true;
    return false;
}
//---------------------------------------------------------------------------
//word to UnicodeString. Returns 4 characters, eg 4ab8
UnicodeString wtoAS(word w)
{
    const wchar_t c[] = L"0123456789abcdef";
    return (UnicodeString)c[(w & 0xf000) >> 12] + c[(w & 0xf00) >> 8] + c[(w & 0xf0) >> 4] + c[w & 0xf];
}
//---------------------------------------------------------------------------
//UnicodeString to word. reverse of wtoAS
word AStow(UnicodeString str)
{
    str = str.Trim().LowerCase();
    if (str.Length() < 4)
        throw EConvertError(L"4 bytes needed in UnicodeString for word conversion.");

    int a = str[1] >= L'0' && str[1] <= L'9' ? str[1] - L'0' : str[1] >= L'a' && str[1] <= L'f' ? str[1] - L'a' + 10 : -1;
    int b = str[2] >= L'0' && str[2] <= L'9' ? str[2] - L'0' : str[2] >= L'a' && str[2] <= L'f' ? str[2] - L'a' + 10 : -1;
    int c = str[3] >= L'0' && str[3] <= L'9' ? str[3] - L'0' : str[3] >= L'a' && str[3] <= L'f' ? str[3] - L'a' + 10 : -1;
    int d = str[4] >= L'0' && str[4] <= L'9' ? str[4] - L'0' : str[4] >= L'a' && str[4] <= L'f' ? str[4] - L'a' + 10 : -1;
    if (a < 0 || b < 0 || c < 0 || d < 0)
        throw EConvertError(L"Invalid UnicodeString for word conversion.");

    return (a << 12) | (b << 8) | (c << 4) | d;
}
//---------------------------------------------------------------------------
/*UnicodeString ASstripnegative(const UnicodeString &s)
 {

 }*/
//---------------------------------------------------------------------------
bool has_kanji(const wchar_t *c)
{
    bool haskanji = false;
    int len = wcslen(c);
    for (int i = 0; i < len && !haskanji; ++i)
        if (KANJI(c[i]))
            haskanji = true;
    return haskanji;
}
//---------------------------------------------------------------------------
bool has_kana(const wchar_t *c)
{
    bool haskana = false;
    int len = wcslen(c);
    for (int i = 0; i < len && !haskana; ++i)
        if (KANA(c[i]))
            haskana = true;
    return haskana;
}
//---------------------------------------------------------------------------
bool only_kana(const wchar_t *c)
{
    bool onlykana = true;
    bool kana = false;
    int len = wcslen(c);
    for (int i = 0; i < len && onlykana; ++i)
        if (KANA(c[i]))
            kana = true;
        else if (!DASH(c[i]))
            onlykana = false;
    return onlykana && kana; // Return false if word only has dash and no real kana.
}
//---------------------------------------------------------------------------
bool valid_kanji(const wchar_t *c)
{
    int len = wcslen(c);
    for (int i = 0; i < len; ++i)
        if (!JAPAN(c[i]))
            return false;
    return len > 0;
}
//---------------------------------------------------------------------------
bool has_jpchar(const wchar_t *c)
{
    int len = wcslen(c);
    for (int i = 0; i < len; ++i)
        if (JAPAN(c[i]))
            return true;
    return false;
}
//---------------------------------------------------------------------------
bool ScreenSaverOn()
{
    BOOL sactive = false;
    SystemParametersInfo(SPI_GETSCREENSAVERRUNNING, 0, &sactive, 0);
    return sactive;
}
//---------------------------------------------------------------------------
tagMONITORINFO MonitorInfo(int x, int y)
{
    TPoint p = Point(x, y);
    HMONITOR hmon = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
    tagMONITORINFO inf;
    inf.cbSize = sizeof(tagMONITORINFO);
    GetMonitorInfo(hmon, &inf);
    return inf;
}
//---------------------------------------------------------------------------
/*This is a hack to replace original VCL functions with my own by         *
 * incjecting a jmp call in memory to my own function. */
void InjectFunction(void *forig, void *ftocall)
{
    void *p = (void*)(int(ftocall) -int(forig) - 5);
    union
    {
        void *func;
        char ch[4];
    } u;
    u.func = p;
    char *c = (char*)(forig);
    ULONG prev;
    if (!VirtualProtect(c, 5, PAGE_EXECUTE_READWRITE, &prev))
        prev = 0;
    *c++ = 0xe9;
    for (int ix = 0; ix < 4; ++ix)
        * c++ = u.ch[ix];
    if (prev)
        VirtualProtect(c, 5, prev, &prev);
}

//void* OwnDisableTaskWindows(HWND ActiveWindow);
//void OwnEnableTaskWindows(void* WindowList);
void ApplicationRealInit()
{
    fMain = new TfMain(Application);
    fDropDown = new TfDropDown(Application);
    data = new Tdata(Application);
    cdata = new Tcdata(Application);

    fMain->Visible = true;
    ShowWindow(Application->Handle, SW_HIDE);

    //InjectFunction(&DisableTaskWindows,&OwnDisableTaskWindows);
    //InjectFunction(&EnableTaskWindows,&OwnEnableTaskWindows);
}
//---------------------------------------------------------------------------
/*
struct TOwnTaskWindowList;
typedef TOwnTaskWindowList* TOwnTaskWindowListPtr;
struct TOwnTaskWindowList
{
    TBaseForm *form;
    TOwnTaskWindowListPtr next;
};
*/
//TOwnTaskWindowListPtr taskwinlist = NULL;
//TBaseForm *taskwinactivewindow = NULL;
//---------------------------------------------------------------------------
/*
void ClearTaskWindowsList(bool enable = false)
{
    while (taskwinlist)
    {
        TOwnTaskWindowListPtr t = taskwinlist;
        taskwinlist = taskwinlist->next;
        t->form->Enabled = true;
        delete t;
    }
}
*/
//---------------------------------------------------------------------------
/*
TBaseForm *FindForm(HWND handle)
{
    for (int ix = 0; handle && ix < Screen->FormCount; ix++)
    {
        if (!Screen->Forms[ix]->HandleAllocated())
            continue;
        TBaseForm *frm = dynamic_cast<TBaseForm *>(Screen->Forms[ix]);
        if (frm && frm->Handle == handle)
            return frm;
    }
    return NULL;
}
*/
//---------------------------------------------------------------------------
/*
int __stdcall OwnDoDisableWindow(HWND h, long data)
{
    TBaseForm *frm = FindForm(h);
    if (!frm || frm == taskwinactivewindow || !frm->Enabled)
        return true;

    TOwnTaskWindowListPtr ptr = new TOwnTaskWindowList;
    ptr->next = taskwinlist;
    ptr->form = frm;
    taskwinlist = ptr;
    frm->Enabled = false;
    return true;
}
*/
//---------------------------------------------------------------------------
/*
void* OwnDisableTaskWindows(HWND ActiveWindow)
{
    TBaseForm *frm = FindForm(ActiveWindow);
    taskwinactivewindow = frm;
    taskwinlist = NULL;
    try
    {
        EnumThreadWindows(GetCurrentThreadId(), &OwnDoDisableWindow, 0);
    }
    catch (...)
    {
        ClearTaskWindowsList();
        taskwinlist = NULL;
    }
    return taskwinlist;
}
//---------------------------------------------------------------------------
void OwnEnableTaskWindows(void* WindowList)
{
    if (!WindowList)
        return;
    taskwinlist = (TOwnTaskWindowListPtr)WindowList;
    ClearTaskWindowsList(true);
}
*/
//---------------------------------------------------------------------------
bool MatchVersion(int major, int minor, bool exact)
{
    return !exact ? (Win32MajorVersion > major || Win32MajorVersion == major && Win32MinorVersion >= minor) :
            /*exact*/(Win32MajorVersion == major && Win32MinorVersion == minor);
}
//---------------------------------------------------------------------------
bool ValidFileName(UnicodeString s)
{
    for (int i = 1; i <= s.Length(); i++)
    {
        if (strchr("'\\/><*\"?:|", s[i]))
            return false;
    }
    return s.Trim().Length();
}
//---------------------------------------------------------------------------
void Backup(UnicodeString name)
{
    CopyFileW(name.c_str(), (name + L".TEMP").c_str(), false);
}
//---------------------------------------------------------------------------
void RestoreBackup(UnicodeString name)
{
    if (FileExists(name))
        DeleteFile(name);
    if (FileExists(name + L".TEMP"))
        RenameFile(name + L".TEMP", name);
}
//---------------------------------------------------------------------------
const wchar_t* MakeExceptionObj(const wchar_t *str)
{
    //MsgBox(str, L"Exception thrown", MB_OK);
    return str;
}
//---------------------------------------------------------------------------
bool ExecuteOpenDialog(TOpenDialog *dialog, UnicodeString defext, bool overwritecheck, bool existscheck, UnicodeString &selectedfile)
{
    UnicodeString filter = L"(?:";

    if (defext != L"")
    {
        if (defext[1] != L'.')
            defext = L"." + defext;
        TRegEx r;
        TStringDynArray arr = r.Split(dialog->Filter, L"\\|");
        UnicodeString tmp;
        for (int ix = 1; ix < arr.Length; ix += 2)
        {
            tmp = arr[ix];
            if (tmp.Length() == 0)
                continue;
            for (int j = 1; j <= tmp.Length(); ++j)
            {
                switch (tmp[j])
                {
                case L'*':
                    filter += L".*";
                    break;
                case L'?':
                    filter += L".";
                    break;
                case L'^':
                    if (j == 0)
                        filter += L"\\";
                    filter += L"^";
                    break;
                case L'\\':
                case L'(':
                case L')':
                case L'[':
                case L']':
                case L'+':
                case L'.':
                case L'$':
                case L'{':
                case L'}':
                    filter += L"\\";
                default:
                    filter += tmp[j];
                    break;
                }
            }
            if (ix < arr.Length - 1)
                filter += L"|";
        }
        filter += L")$";
    }

    TRegEx r(filter);
    while (1)
    {
        if (ExecuteDialog(dialog))
        {
            selectedfile = dialog->FileName;
            if (defext != L"" && !r.IsMatch(dialog->FileName))
                selectedfile += defext;

            bool exists;
            if (overwritecheck || existscheck)
                exists = FileExists(selectedfile);
            if (overwritecheck && exists)
            {
                int mb = MsgBox(L"A file with the same name already exists. Do you want to overwrite it?", L"Query", MB_YESNOCANCEL);
                if (mb == IDCANCEL)
                    return false;
                if (mb == IDNO)
                    continue;
            }
            else if (existscheck && !exists)
                continue;
            return true;
        }
        else
            return false;
    }
}
//---------------------------------------------------------------------------
UnicodeString DateString(TDateTime d, bool fullyear, TDateFormatSetting format)
{
    if (format == dfFromSettings)
        format = settings.datefmt;

    UnicodeString str;

    switch (format)
    {
    case dfDDMMYY:
        if (fullyear)
            DateTimeToString(str, "dd.mm.yyyy", d);
        else
            DateTimeToString(str, "dd.mm.yy", d);
        break;
    case dfMMDDYY:
        if (fullyear)
            DateTimeToString(str, "mm.dd.yyyy", d);
        else
            DateTimeToString(str, "mm.dd.yy", d);
        break;
    case dfYYMMDD:
        if (fullyear)
            DateTimeToString(str, "yyyy.mm.dd", d);
        else
            DateTimeToString(str, "yy.mm.dd", d);
        break;
    }

    return str;
}
//---------------------------------------------------------------------------
UnicodeString DateString(SYSTEMTIME d, bool fullyear, TDateFormatSetting format)
{
    return DateString(SystemTimeToDateTime(d), fullyear, format);
}
//---------------------------------------------------------------------------
UnicodeString DateString(int year, int month, int day, bool fullyear, TDateFormatSetting format)
{
    SYSTEMTIME st;
    memset(&st, 0, sizeof(SYSTEMTIME));
    st.wYear = year;
    st.wMonth = month;
    st.wDay = day;
    return DateString(SystemTimeToDateTime(st), fullyear, format);
}
//---------------------------------------------------------------------------
UnicodeString DateString(float d, bool fullyear, TDateFormatSetting format)
{
    return DateString((TDateTime)d, fullyear, format);
}
//---------------------------------------------------------------------------
UnicodeString DateString(int d, bool fullyear, TDateFormatSetting format)
{
    return DateString((TDateTime)d, fullyear, format);
}
//---------------------------------------------------------------------------
void AddStyledText(TRichEdit *edit, UnicodeString str, TTextAttributes *attrib)
{
    AddStyledText(edit, str, attrib->Name, attrib->Style, attrib->Color, attrib->Height, attrib->Pitch, attrib->Charset);
    /*TFontCharset ch = edit->SelAttributes->Charset;
    TColor col = edit->SelAttributes->Color;
    int h = edit->SelAttributes->Height;
    TFontName nm = edit->SelAttributes->Name;
    TFontPitch pt = edit->SelAttributes->Pitch;
    TFontStyles st = edit->SelAttributes->Style;

    int sel = edit->SelStart;
    edit->Lines->Add(str);
    int sel2 = edit->SelStart;
    edit->SelStart = sel;
    edit->SelLength = sel2 - sel;
    edit->SelAttributes->Assign(attrib);
    edit->SelStart = sel2;
    edit->SelLength = 0;

    edit->SelAttributes->Charset = ch;
    edit->SelAttributes->Color = col;
    edit->SelAttributes->Height = h;
    edit->SelAttributes->Name = nm;
    edit->SelAttributes->Pitch = pt;
    edit->SelAttributes->Style = st;*/
}
//---------------------------------------------------------------------------
void AddStyledText(TRichEdit *edit, UnicodeString str, TFontName fontname, TFontStyles fontstyle, TColor color, int height, TFontPitch fontpitch, TFontCharset charset)
{
    TFontCharset ch = edit->SelAttributes->Charset;
    TColor col = edit->SelAttributes->Color;
    int h = edit->SelAttributes->Height;
    TFontName nm = edit->SelAttributes->Name;
    TFontPitch pt = edit->SelAttributes->Pitch;
    TFontStyles st = edit->SelAttributes->Style;

    int sel = edit->SelStart;
    edit->Lines->Add(str);
    int sel2 = edit->SelStart;
    edit->SelStart = sel;
    edit->SelLength = sel2 - sel;

    edit->SelAttributes->Charset = charset;
    edit->SelAttributes->Color = color;
    edit->SelAttributes->Height = height;
    edit->SelAttributes->Name = fontname;
    edit->SelAttributes->Pitch = fontpitch;
    edit->SelAttributes->Style = fontstyle;

    edit->SelStart = sel2;
    edit->SelLength = 0;

    edit->SelAttributes->Charset = ch;
    edit->SelAttributes->Color = col;
    edit->SelAttributes->Height = h;
    edit->SelAttributes->Name = nm;
    edit->SelAttributes->Pitch = pt;
    edit->SelAttributes->Style = st;
}
//---------------------------------------------------------------------------
void SplitToStrings(TStringList *strings, UnicodeString str)
{
    int pos = 1;
    int pos2 = 1;

    int len = str.Length() + 1;

    while (pos != len)
    {
        while (pos2 != len && str[pos2] != L'\n' && str[pos2] != L'\r')
            ++pos2;

        strings->Add(str.SubString(pos, pos2 - pos));

        if (pos2 != len && str[pos2] == L'\r')
            ++pos2;
        if (pos2 != len && str[pos2] == L'\n')
            ++pos2;
        pos = pos2;
    }
}
//---------------------------------------------------------------------------
bool pathaccess(UnicodeString path, bool checkdelaccess)
{
    if (!path.IsEmpty() && path[path.Length()] != L'\\')
        path += L"\\";
    path += "z_delete.me";
    FILE *tmpf = _wfopen(path.c_str(), L"w+");
    if (tmpf)
    {
        fclose(tmpf);
        if (!DeleteFile(path) && checkdelaccess)
            return false;
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
UnicodeString FullPathFromBackupPath(UnicodeString path)
{
    if (path.Pos(L"<>") == 1)
    {
        if (path.Length() != 2 && path[3] == L'\\')
            path = paths.inipath + path.SubString(4, path.Length() - 3);
        else
            path = paths.inipath + path.SubString(3, path.Length() - 2);
    }

    int endper = 0;
    while (path.Length() - endper != 0 && path[path.Length() - endper] == L'\\')
        ++endper;
    if (endper != 0)
        path = path.SubString(1, path.Length() - endper);

    return path;
}
//---------------------------------------------------------------------------
double IniReadDouble(TCustomIniFile *ini, const UnicodeString &section, const UnicodeString &ident, double defaultvalue)
{
    __int64 hr = *((__int64*)&defaultvalue);
    int h1 = hr & 0xFFFFFFFF;
    int h2 = (hr >> 32) & 0xFFFFFFFF;

    int hr1 = ini->ReadInteger(section, ident.Trim() + L" part1", h1);
    int hr2 = ini->ReadInteger(section, ident.Trim() + L" part2", h2);
    memcpy(&hr, &hr1, sizeof(int));
    memcpy(((char*)(&hr)) + sizeof(int), &hr2, sizeof(int));
    return *((double*) &hr);
}
//---------------------------------------------------------------------------
void IniWriteDouble(TCustomIniFile *ini, const UnicodeString &section, const UnicodeString &ident, double value)
{
    __int64 hr = *((__int64*)&value);
    int h1 = hr & 0xFFFFFFFF;
    int h2 = (hr >> 32) & 0xFFFFFFFF;
    ini->WriteInteger(section, ident.Trim() + L" part1", h1);
    ini->WriteInteger(section, ident.Trim() + L" part2", h2);
}
//---------------------------------------------------------------------------
bool PathsDifferent(UnicodeString path1, UnicodeString path2)
{
    if (path1.LowerCase() == path2.LowerCase())
        return false;

    bool fail;
    HANDLE h1;
    HANDLE h2 = INVALID_HANDLE_VALUE;
    BY_HANDLE_FILE_INFORMATION info1;
    BY_HANDLE_FILE_INFORMATION info2;

    h1 = CreateFile(path1.c_str(), 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    fail = (h1 == INVALID_HANDLE_VALUE);
    if (!fail)
        fail = GetFileInformationByHandle(h1, &info1) == 0;
    if (!fail)
        h2 = CreateFile(path2.c_str(), 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    fail = fail | (h2 == INVALID_HANDLE_VALUE);
    if (!fail)
        fail = GetFileInformationByHandle(h2, &info2) == 0;
    if (h1 != INVALID_HANDLE_VALUE)
        CloseHandle(h1);
    if (h2 != INVALID_HANDLE_VALUE)
        CloseHandle(h2);

    return fail || info1.dwVolumeSerialNumber != info2.dwVolumeSerialNumber || info1.nFileIndexLow != info2.nFileIndexLow || info1.nFileIndexHigh != info2.nFileIndexHigh;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(205);
#endif

