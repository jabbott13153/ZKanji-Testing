//---------------------------------------------------------------------------

#include <vcl.h>
#include "zdefines.h"
#pragma hdrstop

#include "newword.h"
#include "kanjiedit.h"
#include "settings.h"
#include "zkformats.h"
#include "gengraph.h"
#include "recognizer.h"
#include "collection.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ZPaintBox"
#pragma link "edit2"
#pragma resource "*.dfm"

#ifdef _DEBUG
static TGlobalTest tester1(116);
#endif

TfNewWord *fNewWord;
//---------------------------------------------------------------------------
extern bool ThemesAreTurnedOn;

__fastcall TfNewWord::TfNewWord(TComponent* Owner) : TBaseForm(Owner)
{
    EraseBackground = false;
    edreading = new TKanjiEdit(edReading, false, false, NULL, &edcallback);
    edkanji = new TKanjiEdit(edKanji, false, false, NULL, &edcallback);
    edkanji->AcceptKanji = true;
    edkanji->EnableRecognizer(sbRecognize, true, true, true, 1, false, false, rpDefault);
    edreading->MaxLength = 255;
    edkanji->MaxLength = 255;

    cbDict->Items->Clear();
    for (int ix = 0; ix < dictionary->Count; ++ix)
        cbDict->Items->Add(dictionary->Strings[ix]);

    UnicodeString s;
    GetClipText(s, 255);
    UnicodeString fkanji;
    for (int i = 1; i <= s.Length(); i++)
        if (JAPAN(s[i]))
            fkanji += s[i];
    edkanji->Text = fkanji;

    if (!edkanji->Text.Length())
        ActiveControl = edKanji;

    typestart = 0;
    while (pBoxes->Controls[typestart] != cb1)
        typestart++;

    notestart = typestart + WT_COUNT;
    while (pBoxes->Controls[notestart] != cb24)
        notestart++;

    fieldstart = notestart + WN_COUNT;
    while (pBoxes->Controls[fieldstart] != cb45)
        fieldstart++;

}
//---------------------------------------------------------------------------
__fastcall TfNewWord::~TfNewWord()
{
    delete edreading;
    delete edkanji;
}
//---------------------------------------------------------------------------
int TfNewWord::ShowModalEx(byte coll)
{
    cbDict->ItemIndex = coll;
    return ShowModal();
}
//---------------------------------------------------------------------------
byte TfNewWord::get_coll()
{
    return cbDict->ItemIndex;
}
//---------------------------------------------------------------------------
void TfNewWord::edcallback()
{
    btnNew->Enabled = edreading->Text.Length() && edkanji->Text.Length() && edMeaning->Text.Length() && edreading->Text.Length() < 255 && edkanji->Text.Length() < 255;
}
//---------------------------------------------------------------------------
UnicodeString TfNewWord::get_kanji()
{
    return edkanji->Text;
}
//---------------------------------------------------------------------------
UnicodeString TfNewWord::get_kana()
{
    return edreading->Text;
}
//---------------------------------------------------------------------------
UnicodeString TfNewWord::get_meaning()
{
    return edMeaning->Text;
}
//---------------------------------------------------------------------------
int TfNewWord::get_type()
{
    int result = 0;
    for (int i = typestart; i < typestart + WT_COUNT; i++)
        if (((TCheckBox*)pBoxes->Controls[i])->Checked)
            result = result | (1 << (i - typestart));
    return result;
}
//---------------------------------------------------------------------------
int TfNewWord::get_note()
{
    int result = 0;
    for (int i = notestart; i < notestart + WN_COUNT; i++)
        if (((TCheckBox*)pBoxes->Controls[i])->Checked)
            result = result | (1 << (i - notestart));
    return result;
}
//---------------------------------------------------------------------------
int TfNewWord::get_field()
{
    int result = 0;
    for (int i = fieldstart; i < fieldstart + WF_COUNT; i++)
        if (((TCheckBox*)pBoxes->Controls[i])->Checked)
            result = result | (1 << (i - fieldstart));
    return result;
}
//---------------------------------------------------------------------------
word TfNewWord::get_frequency()
{
    word freqs[] = { 6000, 4000, 3501, 3000, 2000, 1501, 1000, 600, 0 };
    return freqs[cbFrequency->ItemIndex];
}
//---------------------------------------------------------------------------
word TfNewWord::get_nametag()
{
    return 0; //not implemented yet
}
//---------------------------------------------------------------------------
void __fastcall TfNewWord::FormPaint(TObject *Sender)
{
    DrawPillowPanel(this, NULL, b0, false);
    DrawPillowPanel(this, b0, b1, false, true);
    DrawPillowPanel(this, b1, b2, false);

    TRect r = ChildRect(pBoxes);
    ExcludeClipRect(Canvas, r);
    DrawPillowPanel(this, b2, b3, false, true);
    ClearClipRect(Canvas);

    if (ThemesAreTurnedOn)
    {
        Canvas->Brush->Color = clBtnFace;
        Canvas->FillRect(r);
    }
//    DrawPillowPanel(Canvas, Rect(r.Left, r.Top, ChildRect(bvV).Left, ChildRect(bvH).Top), false);
//    DrawPillowPanel(Canvas, Rect(r.Left, ChildRect(bvH).Top + 2, ChildRect(bvV).Left, r.Bottom), false);
//    DrawPillowPanel(Canvas, Rect(ChildRect(bvV).Left + 2, r.Top, r.Right, r.Bottom), false);

    DrawCloseBar(this, b3, NULL);
}
//---------------------------------------------------------------------------
void __fastcall TfNewWord::edMeaningClick(TObject *Sender)
{
    edcallback();
}
//---------------------------------------------------------------------------
void AddNewWord(byte collection)
{
    fNewWord = new TfNewWord(Application);

    if (fNewWord->ShowModalEx(collection) != mrCancel)
    {
        if (dictionary->WordIndex(fNewWord->CollectionIndex, fNewWord->kanji.c_str(), fNewWord->kana.c_str()) >= 0)
        {
            MsgBox("A word with the same kanji and kana already exists.", "Error", MB_OK);
            return;
        }

        MODON
        dictionary->BeginWordChange(fNewWord->CollectionIndex);
        try
        {
            dictionary->AddWord(fNewWord->CollectionIndex, fNewWord->kanji.c_str(), fNewWord->kana.c_str(), fNewWord->meaning.c_str(), fNewWord->type, fNewWord->note, fNewWord->field, fNewWord->nametag, fNewWord->frequency);
        }
        catch (...)
        {
            ;
        }
        dictionary->EndWordChange();
        dictionary->Items[fNewWord->CollectionIndex]->Modify(true);
        MODOFF
    }

    delete fNewWord;
}
//---------------------------------------------------------------------------

#ifdef _DEBUG
static TGlobalTest tester2(117);
#endif

