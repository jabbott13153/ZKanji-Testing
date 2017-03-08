//---------------------------------------------------------------------------

#ifndef localeinfoH
#define localeinfoH
//---------------------------------------------------------------------------

class TSystemLocaleSettings
{
private:
    wchar_t fdecimal;
    wchar_t fgroupsep;
    wchar_t fnegsign;
    byte fnumformat;
    int fgrouping[5];
    byte fgroupingcnt;
    bool fgroupingrepeat;

    bool inited;

    void __fastcall init();
    char __fastcall get_decimal();
    char __fastcall get_neg();
    char __fastcall get_groupsep();
    byte __fastcall get_format();
public:
    TSystemLocaleSettings();
    ~TSystemLocaleSettings();
    UnicodeString __fastcall commalize(int i);
    UnicodeString __fastcall negativize(UnicodeString str);

    __property wchar_t decimal = { read = get_decimal };
    __property wchar_t negativesign = { read = get_neg };
    __property wchar_t groupseparator = { read = get_groupsep };
    __property byte numformat = { read = get_format };
};

extern TSystemLocaleSettings locale;

#endif
