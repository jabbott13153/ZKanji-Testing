//---------------------------------------------------------------------------

#ifndef wordlistboxH
#define wordlistboxH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
//---------------------------------------------------------------------------

#include "baseform.h"

class TWordCollection;
class TWordIndexList;
class TLineObj;

enum TBoxPosition { bpLeft, bpTop, bpRight, bpBottom };
typedef Set<TBoxPosition, bpLeft, bpBottom> TBoxPositions;

class TfWordBox : public TBaseForm
{
__published: //IDE-managed Components
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormMouseLeave(TObject *Sender);
    void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall FormMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
private: //User declarations

    TBoxPositions pos;
    TWordIndexList *list;
    TLineObj *lobj;

    int item_height;
    int item_row;
    TRect area;

    void __fastcall WMMouseActivate(TWMMouseActivate &msg);
    int get_count();
public: //User declarations
    BEGIN_MESSAGE_MAP
        MESSAGE_HANDLER(WM_MOUSEACTIVATE, TWMMouseActivate, WMMouseActivate);
    END_MESSAGE_MAP(TBaseForm)

    __fastcall TfWordBox(TComponent* Owner);
    virtual __fastcall ~TfWordBox();

    void ShowEx(TLineObj *LineObj, TWordIndexList *alist, TRect arect);

    __property TBoxPositions Position = { read = pos };
    __property int Count = { read = get_count };
};
//---------------------------------------------------------------------------
extern PACKAGE TfWordBox *fWordBox;
//---------------------------------------------------------------------------
#endif
