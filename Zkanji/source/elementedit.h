//---------------------------------------------------------------------------

#ifndef elementeditH
#define elementeditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ZPaintBox.h"
#include <Buttons.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include "edit2.h"
//---------------------------------------------------------------------------

#include "baseform.h"
#include "kanjielement.h"
#include "customgrid.h"
#include <ImgList.hpp>

/*Normally element coordinates go from 0-10000. When drawing to a canvas we have to
 * convert that to "canvas coordinates". Canvas coordinates are the coordinates on
 * a bitmap or screen.
 */

enum TElementAction { eaNone, eaMoveStart, eaMoving, eaSelectStart, eaSelecting, eaAddStart, eaAdding };
enum TVertexAlignment { vaVert, vaHorz, vaVertFrame, vaHorzFrame };
enum TPartAlignment { paVert, paHorz, paVertFrame, paHorzFrame };
enum TPartResize { prWidth, prHeight };
enum TMouseHitItem { mhNone, /* single */ mhLine, mhPoint, /* compound */ mhInside, mhSide, mhCornerDiag, mhCornerDiagB };
enum TElementView { evEmpty, evSingle, evCompound };

struct TKanji;

/*GRIDS USED ONLY ON THIS WINDOW*/
class TElementGrid : public TCustomGridObj
{
private:
    typedef TCustomGridObj inherited;
protected:
    virtual int get_count();
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell);
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);

    word *fitems;
    int fcount;
    bool fshowval;

    void set_showval(bool val);
    void(__closure * fcallback)(int elem);
public:
    TElementGrid(TZPaintBox *pb, int aboxsize, void(__closure *acallback)(int elem));
    virtual ~TElementGrid();

    void set_items(const word *aitems, int acount);

    __property bool showvalue = { read = fshowval, write = set_showval };
};

struct TElementVariant;
class TVariantGrid : public TCustomMultiselectGridObj
{
private:
    typedef TCustomMultiselectGridObj inherited;

    int fcount;
    bool fshowpos;
    bool fshowval;
    TElementVariant callelem;

    TElementVariant getitem(int apos);
    void callback();

    typedef void(__closure * TEVCallBack)(int element, byte variant);
    typedef void(__closure * TEVCallBackR)(int pos, int & element, byte & variant);
    TEVCallBack fcallback;
    TEVCallBackR fgetitem;
protected:
    void set_showval(bool val);
    virtual int get_count()
    {
        return fcount;
    }
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell);
    virtual void DoMouseDown(TMouseButton Button, TShiftState Shift, int x, int y);
public:
    TVariantGrid(TZPaintBox *apb, int aboxsize, bool showpos, bool showval, TEVCallBack acallback, TEVCallBackR agetitem);
    virtual ~TVariantGrid();

    void empty();
    void set_items(int acount);

    __property bool showvalue = { read = fshowval, write = set_showval };
};

class TPointListEx;
class TStrokeModels;
class TPointListGrid : public TCustomMultiselectGridObj
{
private:
    typedef TCustomMultiselectGridObj inherited;
protected:
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell);
    virtual TPointListEx* GetPoints(int index) = 0;
public:
    TPointListGrid(TZPaintBox *apb, int aboxsize, TFuncPtr acallback);
    virtual ~TPointListGrid();
};

typedef UnicodeString(__closure *TSrokeNumbersProc)(int ix);
class TStrokeModelGrid : public TPointListGrid
{
private:
    typedef TPointListGrid inherited;

    TStrokeModels *list;
    TSrokeNumbersProc fnumproc;
protected:
    virtual TPointListEx* GetPoints(int index);
    virtual int get_count();
    virtual void DoDrawCell(TCanvas *c, const TRect &r, int acell);
public:
    TStrokeModelGrid(TZPaintBox *apb, int aboxsize, TFuncPtr acallback);
    virtual ~TStrokeModelGrid();

    void set_models(TStrokeModels *amodels);
    using inherited::changed;

    __property TSrokeNumbersProc numproc = { read = fnumproc, write = fnumproc };
};

/*----------------------------------------------------------------------------------------*/

class TfEditElem : public TBaseForm
{
__published: //IDE-managed Components
    TZPaintBox *pbEditor;
    TEdit2 *edElem;
    TLabel *Label1;
    TBitBtn *btnClose;
    TSpeedButton *btnUp;
    TSpeedButton *btnDown;
    TPopupMenu *pmVertex;
    TPopupMenu *pmSegment;
    TMenuItem *miDelVertex;
    TMenuItem *miInsert;
    TMenuItem *miCurve;
    TMenuItem *N1;
    TMenuItem *miDelSegment;
    TPopupMenu *pmMulti;
    TMenuItem *miDelVertexMul;
    TMenuItem *miDelSegmentMul;
    TMenuItem *miCurveMul;
    TMenuItem *N2;
    TMenuItem *miWeld;
    TMenuItem *miWeldMul;
    TMenuItem *N3;
    TMenuItem *N4;
    TMenuItem *miInsertMul;
    TMenuItem *miAlign;
    TMenuItem *miHorz;
    TMenuItem *miVert;
    TMenuItem *N5;
    TMenuItem *miHorzFrame;
    TMenuItem *miVertFrame;
    TMenuItem *miAlignMul;
    TMenuItem *N7;
    TMenuItem *miVertFrameMul;
    TMenuItem *miHorzFrameMul;
    TMenuItem *N8;
    TMenuItem *miVertMul;
    TMenuItem *miHorzMul;
    TMenuItem *miVertAlign;
    TMenuItem *miHorzAlign;
    TMenuItem *miVertAlignMul;
    TMenuItem *miHorzAlignMul;
    TMenuItem *miConnectMul;
    TMenuItem *Align1;
    TMenuItem *miVertFrameVertex;
    TMenuItem *miHorzFrameVertex;
    TMenuItem *N9;
    TMenuItem *N10;
    TMenuItem *miReverseVertex;
    TMenuItem *miReverse;
    TMenuItem *miReverseMul;
    TOpenDialog *dlgOpen;
    TSaveDialog *dlgSave;
    TMainMenu *MainMenu1;
    TMenuItem *File1;
    TMenuItem *miSaveAs;
    TMenuItem *miOpenFile;
    TMenuItem *miSaveFile;
    TMenuItem *N11;
    TMenuItem *Close1;
    TPanel *pParts;
    TLabel *Label3;
    TZPaintBox *pbPart1;
    TZPaintBox *pbPart2;
    TZPaintBox *pbPart3;
    TZPaintBox *pbPart4;
    TEdit2 *edPart1;
    TEdit2 *edPart2;
    TEdit2 *edPart3;
    TEdit2 *edPart4;
    TBevel *b1;
    TPanel *pParents;
    TLabel *Label2;
    TZPaintBox *pbVariants;
    TZPaintBox *pbParents;
    TLabel *Label4;
    TCheckBox *cbStandalone;
    TMenuItem *Deletevertices1;
    TMenuItem *miCenterFrameVertex;
    TMenuItem *N13;
    TMenuItem *miCenter;
    TMenuItem *miCenterFrame;
    TMenuItem *miCenterMul;
    TMenuItem *miCenterFrameMul;
    TImageList *ilPatterns;
    TPanel *pVar1;
    TSpeedButton *sbVar1;
    TPanel *pVar2;
    TSpeedButton *sbVar2;
    TPanel *pVar3;
    TSpeedButton *sbVar3;
    TPanel *pVar4;
    TSpeedButton *sbVar4;
    TPopupMenu *pmVariants;
    TPopupMenu *pmPart;
    TMenuItem *miVertCenterPart;
    TMenuItem *miHorzCenterPart;
    TMenuItem *N15;
    TMenuItem *miVertFramePart;
    TMenuItem *miHorzFramePart;
    TMenuItem *miCenterPart;
    TMenuItem *miCenterFramePart;
    TMenuItem *N17;
    TMenuItem *miVertToPart;
    TMenuItem *miHorzToPart;
    TMenuItem *Align2;
    TMenuItem *Resize1;
    TMenuItem *miHeightToPart;
    TMenuItem *miWidthToPart;
    TMenuItem *N12;
    TMenuItem *Setheightto90001;
    TMenuItem *Widthto90001;
    TMenuItem *miW9000;
    TMenuItem *miW8000;
    TMenuItem *miW7000;
    TMenuItem *miW6000;
    TMenuItem *miW5000;
    TMenuItem *miW4000;
    TMenuItem *miW3000;
    TMenuItem *miW2000;
    TMenuItem *miW1000;
    TMenuItem *miH9000;
    TMenuItem *miH8000;
    TMenuItem *miH7000;
    TMenuItem *miH6000;
    TMenuItem *miH5000;
    TMenuItem *miH4000;
    TMenuItem *miH3000;
    TMenuItem *miH2000;
    TMenuItem *miH1000;
    TPopupMenu *pmNewVertex;
    TMenuItem *miNewStroke;
    TMenuItem *miAddVertex;
    TMenuItem *miAddVert;
    TMenuItem *miAddVertMul;
    TSpeedButton *sbBg;
    TMenuItem *ools1;
    TMenuItem *Settings1;
    TSpeedButton *sbSettings;
    TBevel *Bevel4;
    TBevel *Bevel5;
    TSpeedButton *btnUndo;
    TSpeedButton *btnRedo;
    TMenuItem *Edit1;
    TMenuItem *miUndo;
    TMenuItem *miRedo;
    TBevel *Bevel6;
    TBevel *Bevel7;
    TSpeedButton *sbDelVariant;
    TSpeedButton *sbAddVariant;
    TSpeedButton *sbNumbers;
    TMenuItem *miPosVertex;
    TMenuItem *miPos;
    TMenuItem *miPosMul;
    TSpeedButton *sbShownBg;
    TPanel *pPart1;
    TSpeedButton *sbPart1;
    TPanel *pPart2;
    TSpeedButton *sbPart2;
    TPanel *pPart3;
    TSpeedButton *sbPart3;
    TPanel *pPart4;
    TSpeedButton *sbPart4;
    TSpeedButton *sbElem;
    TSpeedButton *sbAdd;
    TBitBtn *btnReady;
    TBevel *Bevel3;
    TBevel *Bevel8;
    TImageList *ilMenu;
    TMenuItem *miGen;
    TPanel *pRepository;
    TLabel *Label8;
    TComboBox *cbRepository;
    TZPaintBox *pbRepository;
    TSpeedButton *sbAddRep;
    TSpeedButton *sbDelRep;
    TSpeedButton *sbToRep;
    TSpeedButton *sbDelRepVar;
    TLabel *Label9;
    TPopupMenu *pmVariantEdit;
    TMenuItem *miDelVar;
    TMenuItem *N6;
    TMenuItem *miDupVar;
    TPanel *pElement;
    TLabel *Label10;
    TComboBox *cbUNICODE;
    TLabel *Label7;
    TLabel *Label5;
    TComboBox *cbPattern;
    TLabel *Label6;
    TEdit2 *edCentre;
    TMenuItem *N14;
    TMenuItem *miMerge;
    TBevel *Bevel10;
    TSpeedButton *btnMerge;
    TSpeedButton *sbDelete;
    TBevel *Bevel11;
    TSpeedButton *sbRegen;
    TBitBtn *btnGenerate;
    TSpeedButton *btnDuplicate;
    TPanel *pModels;
    TZPaintBox *pbModels;
    TSpeedButton *sbAddChar;
    TSpeedButton *sbDelChar;
    TLabel *Label11;
    TBevel *bb;
    TSpeedButton *sbKModels;
    TSpeedButton *sbCModels;
    TMenuItem *miTip;
    TMenuItem *miTipS1;
    TMenuItem *miTipS2;
    TMenuItem *miTipS3;
    TMenuItem *N16;
    TMenuItem *miTipE1;
    TMenuItem *miTipE2;
    TMenuItem *miTipE3;
    TSpeedButton *btnNext;
    TButton *btnFix;
    TMenuItem *N18;
    TMenuItem *miTipD;
    TSpeedButton *sbFixAll;
    TEdit *edVariant;
    TLabel *Label12;
    TBevel *b2;
    TButton *btnSaveVarName;
    void __fastcall pbEditorPaint(TObject *Sender);
    void __fastcall btnCloseClick(TObject *Sender);
    void __fastcall pbEditorMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnUpMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnDownMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall pbEditorMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
    void __fastcall pbEditorMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miDelVertexClick(TObject *Sender);
    void __fastcall miDelSegmentClick(TObject *Sender);
    void __fastcall miCurveClick(TObject *Sender);
    void __fastcall miInsertClick(TObject *Sender);
    void __fastcall miWeldClick(TObject *Sender);
    void __fastcall miVertClick(TObject *Sender);
    void __fastcall miHorzClick(TObject *Sender);
    void __fastcall miVertFrameClick(TObject *Sender);
    void __fastcall miHorzFrameClick(TObject *Sender);
    void __fastcall miConnectMulClick(TObject *Sender);
    void __fastcall pbEditorKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbEditorKeyUp(TObject *Sender, WORD &Key, TShiftState Shift);
    void __fastcall pbEditorMouseLeave(TObject *Sender);
    void __fastcall pmVertexPopup(TObject *Sender);
    void __fastcall miReverseClick(TObject *Sender);
    void __fastcall dlgSaveClose(TObject *Sender);
    void __fastcall dlgSaveShow(TObject *Sender);
    void __fastcall miOpenFileClick(TObject *Sender);
    void __fastcall miSaveAsClick(TObject *Sender);
    void __fastcall miSaveFileClick(TObject *Sender);
    void __fastcall Close1Click(TObject *Sender);
    void __fastcall btnReadyClick(TObject *Sender);
    void __fastcall pbEditorResize(TObject *Sender);
    void __fastcall miCenterFrameVertexClick(TObject *Sender);
    void __fastcall miCenterClick(TObject *Sender);
    void __fastcall cbStandaloneClick(TObject *Sender);
    void __fastcall cbPatternDrawItem(TWinControl *Control, int Index, TRect &Rect, TOwnerDrawState State);
    void __fastcall cbPatternMeasureItem(TWinControl *Control, int Index, int &Height);
    void __fastcall cbPatternChange(TObject *Sender);
    void __fastcall sbVar1Click(TObject *Sender);
    void __fastcall miVarMeasureItem(TObject *Sender, TCanvas *ACanvas, int &Width, int &Height);
    void __fastcall miVarDrawItem(TObject* Sender, TCanvas* ACanvas, const TRect &ARect, bool Selected);
    void __fastcall miVarClick(TObject *Sender);
    void __fastcall miVertCenterPartClick(TObject *Sender);
    void __fastcall miHorzCenterPartClick(TObject *Sender);
    void __fastcall miVertFramePartClick(TObject *Sender);
    void __fastcall miHorzFramePartClick(TObject *Sender);
    void __fastcall miCenterPartClick(TObject *Sender);
    void __fastcall miCenterFramePartClick(TObject *Sender);
    void __fastcall miW1000Click(TObject *Sender);
    void __fastcall miH1000Click(TObject *Sender);
    void __fastcall miNewStrokeClick(TObject *Sender);
    void __fastcall pbEditorExit(TObject *Sender);
    void __fastcall miAddVertexClick(TObject *Sender);
    void __fastcall sbBgClick(TObject *Sender);
    void __fastcall sbSettingsClick(TObject *Sender);
    void __fastcall btnUndoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall btnRedoMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall sbAddVariantClick(TObject *Sender);
    void __fastcall sbDelVariantClick(TObject *Sender);
    void __fastcall sbNumbersClick(TObject *Sender);
    void __fastcall sbShownBgClick(TObject *Sender);
    void __fastcall pbParentsMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miUndoClick(TObject *Sender);
    void __fastcall miRedoClick(TObject *Sender);
    void __fastcall sbPart1Click(TObject *Sender);
    void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
    void __fastcall sbDeleteClick(TObject *Sender);
    void __fastcall sbAddClick(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);
    void __fastcall miGenClick(TObject *Sender);
    void __fastcall sbAddRepClick(TObject *Sender);
    void __fastcall sbToRepClick(TObject *Sender);
    void __fastcall cbRepositoryChange(TObject *Sender);
    void __fastcall sbDelRepVarClick(TObject *Sender);
    void __fastcall sbDelRepClick(TObject *Sender);
    void __fastcall pbVariantsMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
    void __fastcall miMergeClick(TObject *Sender);
    void __fastcall sbRegenClick(TObject *Sender);
    void __fastcall sbAddCharClick(TObject *Sender);
    void __fastcall sbDelCharClick(TObject *Sender);
    void __fastcall btnGenerateClick(TObject *Sender);
    void __fastcall btnDuplicateClick(TObject *Sender);
    void __fastcall sbKModelsClick(TObject *Sender);
    void __fastcall miTipS1Click(TObject *Sender);
    void __fastcall btnNextClick(TObject *Sender);
    void __fastcall btnFixClick(TObject *Sender);
    void __fastcall sbFixAllClick(TObject *Sender);
    void __fastcall btnSaveVarNameClick(TObject *Sender);
private: //User declarations

    class TPartList
    {
    private:
        bool selected[4];

        int fthres;

        TKanjiElement *felem;
        short felemindex;
        byte fvarindex;

        TPoint selectpt;
        TPoint fmovestart;
        TPoint fmovepos;
        TPoint fmovelast;
        bool fmoving;
        short fmovepart;
        byte fmoveside;

        void drawpart(TCanvas *c, const TRect &r, int apart);
        void onpart(const TRect &r, int x, int y, bool alternate, int &apart);

        bool get_partselected(int ix);
        void set_partselected(int ix, bool val);
        int get_selcount();
        int get_count();

        TPartPosition* get_pos(byte ix);

        double partdistance(int x, int y, byte apart);

        __property TPartPosition *position[byte ix] = { read = get_pos };
    public:
        TPartList();
        virtual ~TPartList();
        void Clear();

        void Add(TKanjiElement *aelem, short aindex, byte avar);

        void Deselect();

        void draw(TCanvas *c, const TRect &r);
        bool nearside(const TRect &r, int x, int y, TRect &ahitrect, byte &aside);
        bool select(const TRect &r, int x, int y, bool alternate, bool add);
        bool select(const TRect &r, const TRect &selr, bool add);

        void startmove(const TRect &r, int x, int y);
        void endmove();
        void move(const TRect &r, int x, int y, bool striding);

        void alignpart(TPartAlignment align);
        void alignpart(TPartAlignment align, int topart);
        void resizepart(TPartResize resize, int size);
        void fixcenter();

        __property int threshold = { read = fthres, write = fthres };
        __property bool part[int ix] = { read = get_partselected, write = set_partselected };
        __property int selcount = { read = get_selcount };
        __property int Count = { read = get_count };
    };

    class TStrokeManager
    {
    private:
        void freeup();

        TKanjiVariant *var;
        bool **sel;
        int fcount;
        int fselcount;

        int get_count();
        TElementStroke* get_elem();
        int get_selcount(int astroke);
        int get_len(int astroke);
        bool get_selected(int astroke, int avert);
        void set_selected(int astroke, int avert, bool val);
        TElementPoint* get_point(int astroke, int avert);
        int prev_vert(int astroke, int avert, bool &connected); //returns index of previous non-selected vertex either connected to this one or not
        int next_vert(int astroke, int avert, bool &connected); //returns index of next non-selected vertex either connected to this one or not
        bool keep_vertex_del(int astroke, int avert); //in case we might delete the selected ones
        bool keep_segment_del(int astroke, int avert); //in case we might delete the selected ones
    public:
        TStrokeManager(TKanjiVariant *avariant);
        virtual ~TStrokeManager()
        {
            freeup();
        }

        void delete_vertices(int astroke); //removes selected vertices from stroke. also ensures curve is kept intact
        void delete_stroke(int astroke);
        void delete_segments(int astroke);
        void deselect();
        void deselect(int ix);
        void insert_vertex(int astroke, int avert);
        void curve_segments(int astroke, bool tocurve);
        void split_segment(int astroke, int avert, double where);
        void weld_selected(int astroke);
        void reverse_stroke(int astroke);
        void add_stroke(int x, int y);
        void add_vertex(int x, int y, int astroke);
        void move_stroke(int astroke, int topos);

        __property int selcount[int stroke] = { read = get_selcount };
        __property bool selected[int stroke][int vertex] = { read = get_selected, write = set_selected };
        __property int length[int stroke] = { read = get_len };
        __property TElementStroke *elem = { read = get_elem };
        __property TElementPoint *points[int stroke][int vertex] = { read = get_point };

        __property int count = { read = get_count };
    };

    class TStrokeList
    {
    private:
        TKanjiElement *felem;
        short felemindex;
        byte fvarindex;

        TStrokeManager *items;

        int fthres;
        int fvsize; //size of a drawn vertex (canvas coordinates)

        bool fnumbers;
        bool fmoving;
        bool fadding;
        int fmovestroke;
        int fmovevertex;
        byte fmovecp;
        double fmovecpdist;

        TPoint fmovepos;
        TPoint fmovestart;

        TPoint selectpt;

        int ptdistance(int fstroke, int x, int y, int &segment, bool endpoints);

        int get_length(int astroke)
        {
            return items ? items->length[astroke] : 0;
        }
        bool get_strokesel(int ix);
        bool get_vertsel(int astroke, int ix);
        void set_vertsel(int astroke, int ix, bool val);
        bool get_curved(int astroke, int avert);
        TZStrokeType get_tips(int astroke);
        int get_selcount();
        int get_strokeselcount();
        int get_segselcount();
        int get_curveselcount();
        int get_count()
        {
            return items ? items->count : 0;
        }

        void onvertex(const TRect &r, int x, int y, bool alternate, bool controlpoints, int &restroke, int &revertex, byte &recontrol);
        double onstroke(const TRect &r, int x, int y, bool endpoints, int &restroke, int &revertex); //returns point's distance from stroke

        void moveselected(int dx, int dy);
    public:
        TStrokeList();
        virtual ~TStrokeList()
        {
            delete items;
        }
        void Clear()
        {
            endmove();
            delete items;
            items = 0;
        }

        void Add(TKanjiElement *aelem, short aindex, byte avar);
        void Deselect();

        void draw(TCanvas *c, const TRect &r);
        void drawstroke(TCanvas *c, const TRect &r, int astroke);
        void drawvertex(TCanvas *c, const TRect &r, int astroke, int avert);
        void drawcontrolpoint(TCanvas *c, const TRect &r, int astroke, int avert, int acontrol);
    //alternate = select something else when clicked at the same spot twice
        bool select(const TRect &r, int x, int y, bool alternate, bool controlpoints, bool add);
        bool select(const TRect &r, const TRect &selr, bool add);
        TRect vertrect(const TRect &r, int astroke, int avert, int vsize = -1);

        void nearestpt(const TRect &r, int x, int y, int &ptx, int &pty);

        void startmove(const TRect &r, int x, int y);
        void endmove();
        void move(const TRect &r, int x, int y, bool striding);
        void startadd(const TRect &r, int x, int y, int astroke = -1);
        void add(const TRect &r, int x, int y);
        void endadd(bool keeplast);

        bool onpoint(const TRect &r, int x, int y, bool controlpoints, int *astroke = NULL, int *avertex = NULL); //true when coordinates are near a point
        bool pointpt(const TRect &r, int x, int y, bool controlpoints, TPoint &pt); //receive exact point coordinates we are near

        void deletevertices();
        void deletesegments();
        void curvesegments(bool acurve);
        void insertvertex(const TRect &r, int x, int y);
        void weldselected();
        bool canweld();
        void connectvertices();
        void reversestrokes();
        void selectwholestroke(int ix, bool select);
        void movestroke(int pos);
        void changetip(TZStrokeType type, bool remove);
        void fixcenter();

        void alignvertex(TVertexAlignment alignment, int coord); //align to given coordinate
        void alignvertex(TVertexAlignment alignment, int astroke, int avert); //align to coordinate of given vertex
        void alignvertex(TVertexAlignment alignment); //align to centerpoint of vertices
        void alignvertex(TVertexAlignment alignment, const TRect &r); //align to centerpoint of frame

        __property int selcount = { read = get_selcount };
        __property int strokeselcount = { read = get_strokeselcount };
        __property int segselcount = { read = get_segselcount };
        __property int curveselcount = { read = get_curveselcount };

        __property bool numbers = { read = fnumbers, write = fnumbers };

        __property bool stroke[int ix] = { read = get_strokesel };
        __property bool vertex[int stroke][int ix] = { read = get_vertsel, write = set_vertsel };
        __property bool curved[int stroke][int vert] = { read = get_curved };
        __property bool tips[int stroke] = { read = get_tips };

        __property int threshold = { read = fthres, write = fthres };
        __property int vertsize = { read = fvsize, write = fvsize };

        __property int count = { read = get_count };
        __property int length[int stroke] = { read = get_length };
    };

    enum TUndoItemType
    {
            uiNone, uiPartRemove, uiPartAdd, uiPart, uiVariant, uiPosition, uiStroke, uiPattern,
            uiCentre, uiOwner, uiStandalone, uiTip, uiFix, uiVariantName
    };

    struct TUndoItem
    {
        TUndoItemType type;

        short owner; //kanji index when directly part of kanji
        TElemPattern pattern;
        TKanjiVariant variant;
        short parts[4]; //index of sub-elements
        char partpos[4];

        UnicodeString varname;
    };

    typedef UnicodeString(__closure *getvnameproc)();
    typedef void(__closure * setvnameproc)(UnicodeString name);
    class TUndoItems : public TTemplateList<TUndoItem, true>
    {
    private:
        typedef TTemplateList<TUndoItem, true> inherited;

        virtual void Clear();
        void FreeItem(TUndoItem *item, int ix);

        TUndoItem *copy; //copy of elem when first added
        TKanjiElement *elem; //current element with changes
        byte variant; //variant we are working with
        char partpos[4]; //part positions to know when part was added or removed

        getvnameproc getvname;
        setvnameproc setvname;

        int fpos; //current step we are on in the undo list

        void Add(TUndoItemType);
        void FreeCopy(TUndoItem *copy);
        TUndoItem* CreateCopy(TUndoItemType type);
        void AssignCopy();
        TUndoItem* get_item(int ix)
        {
            return ix < 0 ? copy : inherited::Items[ix];
        }
    public:
        TUndoItems(getvnameproc agetvname, setvnameproc asetvname);
        virtual ~TUndoItems()
        {
            Clear();
        }

        void SetItem(TKanjiElement *aelem, byte avariant);

        bool HasType(TUndoItemType type); //returns whether there is a type of undo item between fbase and fpos. fbase can be higher!

        void PartRemoved(byte part);
        void PartAdded();
        void PartChanged();
        void VariantChanged();
        void PositionChanged();
        void StrokeChanged();
        void PatternChanged();
        void StandaloneChanged();
        void CentreChanged();
        void OwnerChanged();
        void TipChanged();
        void FixChanged();
        void VarnameChanged();

        void Undo();
        void Redo();

        __property int Pos = { read = fpos };
        __property TUndoItem *Items[int] = { read = get_item };
    }
    *fundo;
    friend class TfEditElem::TUndoItems;

    int felemindex;
    int fvarindex;
    TKanjiElement *elem;
    wchar_t bgchar;

    bool destroying; //prevents painting of non existing things

    enum TVariantChangeState
    {
        vsUntouched, vsModified, vsFinal
    }
    *vstate;

    TElementView elemview;

    TElementGrid *egrid;
    TVariantGrid *pgrid1;
    TVariantGrid *pgrid2;
    TVariantGrid *pgrid3;
    TVariantGrid *pgrid4;

    TVariantGrid *vgrid;

    TVariantGrid *rgrid;

    TStrokeModelGrid *mgrid;
    TGapList<TIntegerList *> knumbers;
    TGapList<TIntegerList *> cnumbers;

    TNumEdit *numElem;
    TNumEdit *numPart1;
    TNumEdit *numPart2;
    TNumEdit *numPart3;
    TNumEdit *numPart4;

    TNumEdit *numCentre;
    TNumEdit *numJIS;

    THexEdit *numUNICODE;

    TStringList *varnames;

    TBmp *back;
    TBmp *bmp;
    TRect bmprect;

    TStrokeList *strokelist;
    TPartList *partlist;

    TElementAction faction;
    TPoint mousestart; //first mouse coordinates when mouse was pressed
    TPoint mousepos; //current mouse coordinates
    TPoint mouselast; //previous coordinates updated by mousepos every time before the mouse moves

    TPoint linepos; //exact point of curve or control point or vertex the mouse is near
    TRect partpos; //rectangle of part the mouse is near or on
    byte parthit; //side of rectangle the mouse is on. defines for the sides are at the top of this header

    TMouseHitItem mousehit; //what does linepos point to when no mouse button is pressed

    bool noupdate;
    bool popupshown; //a popup menu appears

    void addfirst();

    void ecallback(int aelem); //parent elements grid callback
    void vcallback(int element, byte variant); //element variant grid callback
    void pcallback(int element, byte variant); //part grids callback
    void rcallback(int element, byte variant); //element variant grid callback
    void ncallback(); //element selection edit box callback
    void n1callback(); //part num edit selector callback
    void n2callback(); //part num edit selector callback
    void n3callback(); //part num edit selector callback
    void n4callback(); //part num edit selector callback
    void ccallback(); //centre point num edit selector callback
    void jcallback(); //kanji JIS setter function

    UnicodeString strokenumproc(int ix);

    void mcallback(); //models list callback function

    void pgiveelem1(int pos, int & element, byte & variant);
    void pgiveelem2(int pos, int & element, byte & variant);
    void pgiveelem3(int pos, int & element, byte & variant);
    void pgiveelem4(int pos, int & element, byte & variant);
    void vgiveelem(int pos, int & element, byte & variant);
    void rgiveelem(int pos, int & element, byte & variant);

    void deactivated(void *caller);
    void activated(void *caller);

    void __fastcall vertToVertex(TObject *Sender);
    void __fastcall horzToVertex(TObject *Sender);
    void __fastcall vertToPart(TObject *Sender);
    void __fastcall horzToPart(TObject *Sender);
    void __fastcall widthToPart(TObject *Sender);
    void __fastcall heightToPart(TObject *Sender);
    void __fastcall strokeToPos(TObject *Sender);

    void setpart(int part, int newindex);

    void redrawbg();
    void redraw();
    void update_points();

    void setuptipsmenu(TZStrokeType stype);
    void fillalign(TMenuItem *mvert, TMenuItem *mhorz);
    void fillposition(TMenuItem *mpos);
    void fillresize(TMenuItem *mvert, TMenuItem *mhorz);

    bool partquery();
    void updatevariants();

    void setunchanged();
    bool changed();
    bool cansave();
    void setundo();
    void setcentre();
    void updatevarbuttons();
    void elemchanged();
    void set_varindex(int aindex);
    void set_elemindex(int aindex);

    bool get_modified();
    void set_modified(bool val);

    int elementbrowse(int start);
    void controlinit();

    void update_repositories(int newpos);
    void update_repbuttons();

    void clear_recparts();
    void fill_recparts();

    bool fixelement(TKanjiElement *e, byte variant, bool onlydimensions = false);

    UnicodeString savevarname();
    void undovarname(UnicodeString vname);

    bool Finalize();

    __property int varindex = { read = fvarindex, write = set_varindex };
    __property int elemindex = { read = felemindex, write = set_elemindex };

    __property bool modified = { read = get_modified, write = set_modified };

public: //User declarations
    friend class TfElemSettings;

    __fastcall TfEditElem(TComponent* Owner);
    virtual __fastcall ~TfEditElem();

    int ShowModalEx(int aindex = 0);
    void AddUNICODE(TKanji *k);
};

//---------------------------------------------------------------------------
extern PACKAGE TfEditElem *fEditElem;
//---------------------------------------------------------------------------
#endif
