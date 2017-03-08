object fClipboard: TfClipboard
  Left = 845
  Top = 612
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'Clipboard view'
  ClientHeight = 243
  ClientWidth = 410
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object Bevel2: TBevel
    Left = 178
    Top = 6
    Width = 2
    Height = 15
    Shape = bsLeftLine
  end
  object btnTranslate: TSpeedButton
    Left = 120
    Top = 2
    Width = 54
    Height = 22
    Caption = 'Translate'
    Flat = True
    OnClick = btnTranslateClick
  end
  object btnClear: TSpeedButton
    Left = 84
    Top = 2
    Width = 36
    Height = 22
    Caption = 'Clear'
    Flat = True
    OnClick = btnClearClick
  end
  object btnGet: TSpeedButton
    Left = 3
    Top = 2
    Width = 81
    Height = 22
    Caption = 'View clipboard'
    Flat = True
    OnClick = btnGetClick
  end
  object bv1: TBevel
    Left = 0
    Top = 26
    Width = 9999
    Height = 1
    Shape = bsTopLine
  end
  object pbLines: TZPaintBox
    AlignWithMargins = True
    Top = 27
    Width = 410
    Height = 197
    Margins.Left = 0
    Margins.Top = 27
    Margins.Right = 0
    Margins.Bottom = 0
    Align = alClient
    BorderStyle = bsNone
    InvalidateOnScroll = True
    TabOrder = 0
    VertScrollBar.Range = 1000
    VertScrollBar.Enabled = False
    OnPaint = pbLinesPaint
    OnKeyDown = pbLinesKeyDown
    OnKeyPress = pbLinesKeyPress
    OnResize = pbLinesResize
    OnMouseDown = pbLinesMouseDown
    OnMouseMove = pbLinesMouseMove
    ExplicitWidth = 352
    ExplicitHeight = 146
  end
  object cbAuto: TCheckBox
    Left = 185
    Top = 5
    Width = 87
    Height = 17
    Caption = 'Auto translate'
    TabOrder = 1
    OnClick = cbAutoClick
  end
  object sbInfo: TStatusBar
    Left = 0
    Top = 224
    Width = 410
    Height = 19
    Panels = <
      item
        Bevel = pbNone
        Text = 'Characters:'
        Width = 65
      end
      item
        Bevel = pbNone
        Text = '0'
        Width = 100
      end>
  end
end
