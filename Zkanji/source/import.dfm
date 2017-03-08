object fImporter: TfImporter
  Left = 356
  Top = 264
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'zkanji import manager'
  ClientHeight = 289
  ClientWidth = 382
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    382
    289)
  PixelsPerInch = 96
  TextHeight = 13
  object bBottom: TBevel
    Left = 0
    Top = 257
    Width = 999999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object bTop: TBevel
    Left = 0
    Top = 38
    Width = 999999
    Height = 2
    Shape = bsTopLine
  end
  object Label1: TLabel
    Left = 5
    Top = 5
    Width = 372
    Height = 28
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 
      'Import in progress. Dialog windows will be shown when user input' +
      ' is needed.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
    WordWrap = True
    ExplicitWidth = 484
  end
  object edit: TRichEdit
    Left = 8
    Top = 40
    Width = 374
    Height = 217
    TabStop = False
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    OnKeyDown = editKeyDown
    OnKeyPress = editKeyPress
  end
  object btnClose: TButton
    Tag = 2
    Left = 154
    Top = 261
    Width = 75
    Height = 25
    Anchors = [akBottom]
    Cancel = True
    Caption = 'Close'
    Enabled = False
    TabOrder = 1
    OnClick = btnCloseClick
  end
end
