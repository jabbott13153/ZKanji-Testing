object fWriter: TfWriter
  Left = 290
  Top = 527
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Kanji writing test'
  ClientHeight = 401
  ClientWidth = 567
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyPress = FormKeyPress
  DesignSize = (
    567
    401)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 400
    Top = 16
    Width = 42
    Height = 13
    Caption = 'Element:'
  end
  object Label2: TLabel
    Left = 400
    Top = 56
    Width = 35
    Height = 13
    Caption = 'Stroke:'
  end
  object btnLockStroke: TSpeedButton
    Left = 480
    Top = 72
    Width = 41
    Height = 22
    AllowAllUp = True
    GroupIndex = 1
    Caption = 'Lock'
  end
  object pb: TZPaintBox
    Left = 8
    Top = 8
    Width = 385
    Height = 385
    TabOrder = 0
  end
  object btnClose: TBitBtn
    Left = 483
    Top = 368
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Close'
    DoubleBuffered = True
    ModalResult = 2
    ParentDoubleBuffered = False
    TabOrder = 1
  end
  object edElem: TEdit2
    Left = 400
    Top = 32
    Width = 76
    Height = 21
    TabOrder = 2
    Text = '1'
  end
  object pbChar: TZPaintBox
    Left = 400
    Top = 112
    Width = 38
    Height = 38
    BorderStyle = bsNone
    HorzScrollBar.Visible = False
    TabOrder = 3
    VertScrollBar.Visible = False
    OnPaint = pbCharPaint
  end
  object edStroke: TEdit2
    Left = 399
    Top = 72
    Width = 76
    Height = 21
    TabOrder = 4
    Text = '1'
  end
  object memo: TMemo
    Left = 400
    Top = 160
    Width = 158
    Height = 201
    TabOrder = 5
  end
  object cbDebug: TCheckBox
    Left = 448
    Top = 112
    Width = 106
    Height = 17
    Caption = 'Show debug data'
    TabOrder = 6
    OnClick = cbDebugClick
  end
  object cbHint: TCheckBox
    Left = 448
    Top = 132
    Width = 97
    Height = 17
    Caption = 'Next stroke hint'
    TabOrder = 7
    OnClick = cbHintClick
  end
end
