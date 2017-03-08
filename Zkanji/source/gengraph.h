//---------------------------------------------------------------------------

#ifndef gengraphH
#define gengraphH
//---------------------------------------------------------------------------

class TBaseForm;

// Generic drawing functions

// Draws a plastic looking toolbar background. Set wether you want the top different colored bar strecth to the half width if the height is too large.
void __fastcall DrawButtonBar(TCanvas *c, const TRect &r, bool stretch = false);
void __fastcall DrawButtonBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel, bool stretch = false);
// Draws a shaded background with it's top and bottom parts lightened and darkened. if sunken the colors are swapped.
void __fastcall DrawPillowPanel(TCanvas *c, const TRect &r, bool dim = false, bool sunken = false);
void __fastcall DrawPillowPanel(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel, bool dim = false, bool sunken = false);
void __fastcall DrawHorzPillowPanel(TCanvas *c, const TRect &r, bool dim = false, bool sunken = false);
void __fastcall DrawHorzPillowPanel(TBaseForm *form, TBevel *leftbevel, TBevel *rightbevel, bool dim = false, bool sunken = false);
// Draws a shaded background with it's top and bottom parts lightened and darkened for topmost caption like text.
void __fastcall DrawCaptionBar(TCanvas *c, const TRect &r, bool active);
void __fastcall DrawCaptionBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel, bool active);
// Draws the background of a fake status bar made of labels.
void __fastcall DrawStatusBar(TCanvas *c, const TRect &r);
void __fastcall DrawStatusBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel);
// Draws a background for the bottom bar in windows with close or ok buttons.
void __fastcall DrawCloseBar(TCanvas *c, const TRect &r);
void __fastcall DrawCloseBar(TBaseForm *form, TBevel *topbevel, TBevel *bottombevel);
// Draws the blueish background of tab controls or page controls.
void __fastcall DrawTabBar(TCanvas *c, const TRect &r);
// Calls gradientfillcanvas with default values while checks for decoration in settings.
void __fastcall DrawGrad(TBaseForm *form, TColor c1, TColor c2, TBevel *topbevel, TBevel *bottombevel, bool vertical = true);
void __fastcall DrawGrad(TCanvas *c, TColor c1, TColor c2, const TRect &r, bool vertical = true);
void __fastcall DrawGrad(TBaseForm *form, TColor c1, TColor c2, TColor c3, double mid, TBevel *topbevel, TBevel *bottombevel, bool vertical = true);
void __fastcall DrawGrad(TCanvas *c, TColor c1, TColor c2, TColor c3, double mid, const TRect &r, bool vertical = true);

// Clipping functions:
void __fastcall SetClipRect(TCanvas *c, const TRect &r);
void __fastcall ExcludeClipRect(TCanvas *c, const TRect &r);
void __fastcall ClearClipRect(TCanvas *c);

#endif
