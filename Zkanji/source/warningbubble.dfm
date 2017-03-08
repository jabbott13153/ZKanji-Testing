object fBubble: TfBubble
  Left = 385
  Top = 678
  AlphaBlend = True
  AlphaBlendValue = 0
  BorderIcons = []
  BorderStyle = bsNone
  Caption = 'fBubble'
  ClientHeight = 61
  ClientWidth = 177
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnClose = FormClose
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lb2: TLabel
    Left = 12
    Top = 4
    Width = 85
    Height = 13
    Caption = 'kanji message:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 11433472
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 17
    Top = 23
    Width = 47
    Height = 13
    Caption = 'There are'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbDue: TLabel
    Left = 67
    Top = 23
    Width = 44
    Height = 13
    Alignment = taCenter
    AutoSize = False
    Caption = '000000'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object Label4: TLabel
    Left = 17
    Top = 39
    Width = 126
    Height = 13
    Caption = 'in the long-term study list!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 114
    Top = 23
    Width = 46
    Height = 13
    Caption = 'due items'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lb1: TLabel
    Left = 6
    Top = 4
    Width = 6
    Height = 13
    Caption = 'z'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object tTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = tTimerTimer
    Left = 144
    Top = 8
  end
end
