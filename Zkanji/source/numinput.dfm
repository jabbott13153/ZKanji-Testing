object fNumInput: TfNumInput
  Left = 673
  Top = 789
  ActiveControl = edNum
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'fNumInput'
  ClientHeight = 77
  ClientWidth = 463
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  DesignSize = (
    463
    77)
  PixelsPerInch = 96
  TextHeight = 13
  object lbMsg: TLabel
    Left = 16
    Top = 6
    Width = 431
    Height = 38
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 
      'lbMsg aaaaaaaaaaa bbbbbbbbbbb ccccccccccccceeeeee dddddd gggggg ' +
      'eeeeeee aaaaaaaa ggggggggg aaaaaa:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Transparent = True
    Layout = tlCenter
    WordWrap = True
  end
  object Panel1: TPanel
    Left = 0
    Top = 47
    Width = 463
    Height = 30
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 0
    DesignSize = (
      463
      30)
    object lbInterval: TLabel
      Left = 16
      Top = 5
      Width = 143
      Height = 16
      Anchors = [akLeft, akBottom]
      Caption = 'Valid range: 0000 - 0000'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      Transparent = True
      Layout = tlCenter
    end
    object BitBtn1: TBitBtn
      Left = 307
      Top = 1
      Width = 75
      Height = 25
      Caption = 'OK'
      Default = True
      DoubleBuffered = True
      ModalResult = 1
      NumGlyphs = 2
      ParentDoubleBuffered = False
      TabOrder = 0
    end
    object BitBtn2: TBitBtn
      Left = 384
      Top = 1
      Width = 75
      Height = 25
      Cancel = True
      Caption = 'Cancel'
      DoubleBuffered = True
      ModalResult = 2
      NumGlyphs = 2
      ParentDoubleBuffered = False
      TabOrder = 1
    end
    object edNum: TEdit2
      Left = 220
      Top = 3
      Width = 85
      Height = 21
      TabOrder = 2
      Text = '0'
    end
  end
end
