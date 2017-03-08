//---------------------------------------------------------------------------

#ifndef kanjihintH
#define kanjihintH
//---------------------------------------------------------------------------

class TWordCollection;

class TKanjiHint
{
private:
    int timeid;
    bool fvisible;
    wchar_t kanjich;
    int fx;
    int fy;
    TWordCollection *coll;
    TControl *control;
    TBmp *bmp;

    HFONT Kfont;

    void TimePassed();

    void fpaintproc(TObject *Sender, TCanvas *Canvas, TRect area, void *data);
    friend void CALLBACK timeproc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
    void StopShowHint(bool inside = true, bool immediate = false);
    void Show(TControl *Sender, int ax, int ay, wchar_t akanji, TWordCollection *acoll);
    void Hide(bool reset, bool immediate = false);

    friend void HideKanjiHint(bool reset);
    friend void HideKanjiHintNow();
    friend void ShowKanjiHint(TControl *Sender, int ax, int ay, wchar_t akanji, TWordCollection *acoll);
    void settingsChanged(void *caller, TSettingsProcState state);
public:
    TKanjiHint();
    virtual ~TKanjiHint();

    __property bool Visible = { read = fvisible };
    __property TControl *Control = { read = control };
};

void HideKanjiHint(bool reset = true); // Hides kanji hint with nice transition effect. Set reset to false to avoid popping up hint twice for same kanji.
void HideOwnedKanjiHint(TControl *form);  // Hides the kanji hint window if it was shown for a control on the passed base form.
void HideKanjiHintNow(); // Hides kanji hint without transition effect.
void ShowKanjiHint(TControl *Sender, int ax, int ay, wchar_t akanji, TWordCollection *acoll);
bool KanjiHintVisible();

#endif
