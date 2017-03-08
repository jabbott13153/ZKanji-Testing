object fKanjiDefImport: TfKanjiDefImport
  Left = 1165
  Top = 522
  Caption = 'Kanji definition conflict manager'
  ClientHeight = 373
  ClientWidth = 698
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    698
    373)
  PixelsPerInch = 96
  TextHeight = 13
  object bvBottom: TBevel
    Left = 0
    Top = 343
    Width = 999999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 348
  end
  object b1: TBevel
    Left = 0
    Top = 19
    Width = 99999
    Height = 1
    Shape = bsTopLine
  end
  object Label1: TLabel
    Left = 5
    Top = 3
    Width = 177
    Height = 13
    Caption = 'Select kanji definitions to keep:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 218
    Top = 3
    Width = 136
    Height = 13
    Caption = 'Only differences are shown.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object pbKanji: TZPaintBox
    Top = 20
    Width = 60
    Height = 323
    Anchors = [akLeft, akTop, akBottom]
    BorderStyle = bsNone
    HorzScrollBar.Visible = False
    TabOrder = 3
    TabStop = False
    VertScrollBar.Visible = False
    OnPaint = pbKanjiPaint
    OnEnter = pbKanjiEnter
  end
  object btnNext: TBitBtn
    Left = 296
    Top = 346
    Width = 105
    Height = 25
    Anchors = [akBottom]
    Caption = 'Import'
    Default = True
    DoubleBuffered = True
    NumGlyphs = 4
    ParentDoubleBuffered = False
    TabOrder = 1
    OnClick = btnNextClick
  end
  object btnAbort: TButton
    Left = 618
    Top = 346
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Abort'
    TabOrder = 2
    OnClick = btnAbortClick
  end
  object pbDef: TZPaintBox
    Left = 60
    Top = 20
    Width = 638
    Height = 323
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    HorzScrollBar.Visible = False
    TabOrder = 0
    OnVScroll = pbDefVScroll
    OnKeyPress = pbDefKeyPress
    OnEnter = pbDefEnter
    OnExit = pbDefExit
    OnResize = pbDefResize
  end
end
