object fImportQuery: TfImportQuery
  Left = 407
  Top = 551
  BorderStyle = bsDialog
  Caption = 'Dictionary import - Query'
  ClientHeight = 323
  ClientWidth = 634
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  DesignSize = (
    634
    323)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 32
    Width = 103
    Height = 13
    Caption = 'Word in dictionary'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 208
    Width = 79
    Height = 13
    Caption = 'New definition'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbMeaning: TLabel
    Left = 24
    Top = 224
    Width = 593
    Height = 13
    AutoSize = False
    Caption = 'Meaning:'
  end
  object lbFreqNew: TLabel
    Left = 128
    Top = 208
    Width = 55
    Height = 13
    Caption = 'Frequency:'
  end
  object lbFreqOri: TLabel
    Left = 128
    Top = 32
    Width = 55
    Height = 13
    Caption = 'Frequency:'
  end
  object lbLabels: TLabel
    Left = 24
    Top = 242
    Width = 601
    Height = 13
    AutoSize = False
    Caption = 'Labels:'
  end
  object Label7: TLabel
    Left = 80
    Top = 8
    Width = 474
    Height = 16
    Anchors = [akTop]
    Caption = 
      'The following word has different definitions in the current dict' +
      'ionary and in the file.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label3: TLabel
    Left = 280
    Top = 302
    Width = 343
    Height = 13
    Anchors = [akTop, akRight]
    Caption = 
      '* Currently the word frequency (word popularity) value is not up' +
      'dated.'
  end
  object pb: TZPaintBox
    Left = 8
    Top = 48
    Width = 617
    Height = 153
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
  end
  object BitBtn1: TBitBtn
    Left = 144
    Top = 264
    Width = 147
    Height = 25
    Caption = 'Replace selected meaning'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 1
    OnClick = BitBtn1Click
  end
  object BitBtn2: TBitBtn
    Left = 296
    Top = 264
    Width = 121
    Height = 25
    Caption = 'Add as new meaning'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 2
    OnClick = BitBtn2Click
  end
  object BitBtn3: TBitBtn
    Left = 424
    Top = 264
    Width = 65
    Height = 25
    Cancel = True
    Caption = 'Ignore'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 3
    OnClick = BitBtn3Click
  end
end
