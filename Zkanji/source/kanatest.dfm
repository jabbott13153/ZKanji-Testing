object fKanaTest: TfKanaTest
  Left = 1056
  Top = 47
  ActiveControl = pbType
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Hiragana and Katakana practice'
  ClientHeight = 405
  ClientWidth = 531
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000040030000000000000000000000000000000000000000
    00000000A59C9CA59C9CA59C9CA59C9CA59C9CA59C9CA59C9CA59C9CA59C9CA5
    9C9CA59C9CA59C9C000000000000000000A59C9CDEDEDEDEDEDEDEDEDEDEDEDE
    DEDEDEDEDEDEDEDEDED6D6D6D6D6D6D6D6D6D6D6D6D6D6D6A59C9C000000A59C
    9CE6E6E6B5B5B55A5A5AA5A5A5B5B5B5D6D6D67B7B7B4A4A4AB5B5B5DEDEDEDE
    DEDEDEDEDEDEDEDEDEDEDEA59C9CA59C9CEFEFEF4242427B7B7B424242101010
    BDBDBDE6E6E6C5C5C5313131B5B5B5DEDEDEDEDEDEDEDEDEDEDEDEA59C9CA59C
    9CEFEFEF9C9C9C636363EFEFEF000000636363EFEFEFEFEFEF7B7B7B7B7B7BE6
    E6E6E6E6E6E6E6E6E6E6E6A59C9CA59C9CF7F7F7F7F7F7949494525252212121
    525252A5A5A5EFEFEF7373737B7B7BEFEFEFEFEFEFE6E6E6E6E6E6A59C9CA59C
    9CF7F7F7F7F7F7F7F7F7C5C5C5101010212121101010525252424242E6E6E6EF
    EFEFEFEFEFEFEFEFEFEFEFA59C9CA59C9CF7F7F7F7F7F7F7F7F7F7F7F7000000
    F7F7F7525252D6D6D6F7F7F7F7F7F7F7F7F7EFEFEFEFEFEFEFEFEFA59C9CA59C
    9CF7F7F7F7F7F7636363212121000000636363848484E6E6E6F7F7F7F7F7F7F7
    F7F7F7F7F7F7F7F7F7F7F7A59C9CA59C9CF7F7F7F7F7F7E6E6E6F7F7F7424242
    848484636363C5C5C5F7F7F7848484000000000000848484F7F7F7A59C9CA59C
    9CEFEFEFEFEFEFEFEFEFF7F7F7848484848484F7F7F7F7F7F7848484848484F7
    F7F7848484848484F7F7F7A59C9CA59C9CE6E6E6E6E6E6E6E6E6EFEFEFA5A5A5
    E6E6E6F7F7F7F7F7F7F7F7F7848484000000000000848484F7F7F7A59C9CA59C
    9CDEDEDEE6E6E6E6E6E6E6E6E6EFEFEFEFEFEFEFEFEFF7F7F7F7F7F7F7F7F7F7
    F7F7848484848484F7F7F7A59C9CA59C9CD6D6D6DEDEDEDEDEDEE6E6E6E6E6E6
    E6E6E6EFEFEFEFEFEFEFEFEF848484000000848484F7F7F7F7F7F7A59C9C0000
    00A59C9CD6D6D6D6D6D6DEDEDEDEDEDEDEDEDEE6E6E6E6E6E6E6E6E6EFEFEFEF
    EFEFEFEFEFF7F7F7A59C9C000000000000000000A59C9CA59C9CA59C9CA59C9C
    A59C9CA59C9CA59C9CA59C9CA59C9CA59C9CA59C9CA59C9C000000000000C003
    0000800100000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000080010000C0030000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  DesignSize = (
    531
    405)
  PixelsPerInch = 96
  TextHeight = 13
  object bv1: TBevel
    Left = 0
    Top = 39
    Width = 99999
    Height = 1
    Shape = bsTopLine
  end
  object bv3: TBevel
    Left = 0
    Top = 371
    Width = 9999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 331
  end
  object Label1: TLabel
    Left = 6
    Top = 4
    Width = 517
    Height = 28
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    Caption = 
      'Type the current kana with roomaji! You can try each syllable tw' +
      'ice, but you will have to start the test all over after the seco' +
      'nd mistake!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object bv4: TBevel
    Left = 0
    Top = 373
    Width = 9999
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 333
  end
  object lbType: TLabel
    Left = 142
    Top = 314
    Width = 81
    Height = 39
    Anchors = [akBottom]
    Caption = 'Type:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -32
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object lbDone: TLabel
    Left = 164
    Top = 309
    Width = 203
    Height = 52
    Anchors = [akBottom]
    Caption = 'Well done!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGreen
    Font.Height = -43
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object lbMin: TLabel
    Left = 30
    Top = 336
    Width = 30
    Height = 24
    Alignment = taRightJustify
    Anchors = [akLeft, akBottom]
    Caption = '00:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object Label2: TLabel
    Left = 10
    Top = 314
    Width = 59
    Height = 16
    Anchors = [akLeft, akBottom]
    Caption = 'Your time:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object lbSec: TLabel
    Left = 61
    Top = 336
    Width = 24
    Height = 24
    Anchors = [akLeft, akBottom]
    Caption = '00'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object lbError: TLabel
    Left = 457
    Top = 324
    Width = 64
    Height = 25
    Alignment = taRightJustify
    Anchors = [akRight, akBottom]
    Caption = 'lbError'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -21
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    Visible = False
  end
  object lbMis: TLabel
    Left = 430
    Top = 344
    Width = 89
    Height = 18
    Alignment = taRightJustify
    Anchors = [akRight, akBottom]
    Caption = 'Mistakes: 48'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object pb: TZPaintBox
    AlignWithMargins = True
    Top = 40
    Width = 531
    Height = 265
    Margins.Left = 0
    Margins.Top = 24
    Margins.Right = 0
    Margins.Bottom = 0
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    HorzScrollBar.Visible = False
    TabOrder = 0
    TabStop = False
    VertScrollBar.Visible = False
    OnPaint = pbPaint
  end
  object BitBtn1: TBitBtn
    Left = 426
    Top = 377
    Width = 103
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Quit practice'
    DoubleBuffered = True
    Glyph.Data = {
      CA010000424DCA0100000000000042000000280000000D0000000E0000000100
      10000300000088010000120B0000120B00000000000000000000007C0000E003
      00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
      00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F00000000FF7F00000000
      FF7FFF7FFF7FFF7F00000000FF7FFF7FFF7FFF7F0000FF7F00000000FF7F0000
      0000FF7FFF7F0000FF7FFF7FFF7FFF7F0000FF7F00000000FF7FFF7FFF7F0000
      FF7F0000FF7FFF7FFF7FFF7F0000FF7F00000000FF7FFF7FFF7FFF7F00000000
      FF7FFF7FFF7FFF7F0000FF7F00000000FF7F0000000000000000000000000000
      0000FF7F0000FF7F00000000FF7FFF7FFF7FFF7FFF7F0000FF7FFF7FFF7FFF7F
      0000FF7F00000000FF7FFF7FFF7FFF7FFF7F0000FF7FFF7FFF7FFF7F0000FF7F
      000000000000000000000000FF7FFF7F00000000000000000000FF7F00000000
      FF7FFF7FFF7F0000FF7FFF7F0000FF7FFF7FFF7F0000FF7F0000000000000000
      00000000FF7FFF7F00000000000000000000FF7F00000000FF7FFF7FFF7F0000
      FF7FFF7F0000FF7FFF7FFF7F0000FF7F000000000000000000000000FF7FFF7F
      00000000000000000000FF7F0000}
    ModalResult = 2
    ParentDoubleBuffered = False
    TabOrder = 3
  end
  object btnGen: TBitBtn
    Left = 2
    Top = 377
    Width = 183
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = '&Generate random kana text'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 1
    OnClick = btnGenClick
  end
  object btnSet: TBitBtn
    Left = 189
    Top = 377
    Width = 148
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = '&Change test settings'
    DoubleBuffered = True
    Glyph.Data = {
      A6020000424DA602000000000000420000002800000011000000110000000100
      10000300000064020000120B0000120B00000000000000000000007C0000E003
      00001F000000DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D
      DC7DDC7DDC7DDC7D0000DC7DDC7D744E744E744E744E744E744E744E744E744E
      744E744E744EDC7DDC7DDC7D0000DC7D744E7B6F7B6F7B6F7B6F7B6F7B6F7B6F
      5A6B5A6B5A6B5A6B5A6B744EDC7DDC7D0000744E9C73D65A6B2D9452D65A5A6B
      EF3D2925D65A7B6F7B6F7B6F7B6F7B6F744EDC7D0000744EBD770821EF3D0821
      4208F75E9C731863C618D65A7B6F7B6F7B6F7B6F744EDC7D0000744EBD77734E
      8C31BD7700008C31BD77BD77EF3DEF3D9C739C739C739C73744EDC7D0000744E
      DE7BDE7B524A4A2984104A299452BD77CE39EF3DBD77BD779C739C73744EDC7D
      0000744EDE7BDE7BDE7B18634208841042084A2908219C73BD77BD77BD77BD77
      744EDC7D0000744EDE7BDE7BDE7BDE7B0000DE7B4A295A6BDE7BDE7BDE7BBD77
      BD77BD77744EDC7D0000744EDE7BDE7B8C31841000008C3110429C73DE7BDE7B
      DE7BDE7BDE7BDE7B744EDC7D0000744EDE7BDE7B9C73DE7B082110428C311863
      DE7B1042000000001042DE7B744EDC7D0000744EBD77BD77BD77DE7B10421042
      DE7BDE7B10421042DE7B10421042DE7B744EDC7D0000744E9C739C739C73BD77
      94529C73DE7BDE7BDE7B1042000000001042DE7B744EDC7D0000744E7B6F9C73
      9C739C73BD77BD77BD77DE7BDE7BDE7BDE7B10421042DE7B744EDC7D0000744E
      5A6B7B6F7B6F9C739C739C73BD77BD77BD77104200001042DE7BDE7B744EDC7D
      0000DC7D744E5A6B5A6B7B6F7B6F7B6F9C739C739C73BD77BD77BD77DE7B744E
      DC7DDC7D0000DC7DDC7D744E744E744E744E744E744E744E744E744E744E744E
      744EDC7DDC7DDC7D0000}
    ModalResult = 1
    ParentDoubleBuffered = False
    TabOrder = 2
  end
  object pbType: TZPaintBox
    Left = 232
    Top = 310
    Width = 78
    Height = 55
    Anchors = [akBottom]
    BorderStyle = bsNone
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -32
    Font.Name = 'Tahoma'
    Font.Style = []
    HorzScrollBar.Visible = False
    ParentFont = False
    TabOrder = 4
    VertScrollBar.Visible = False
    OnPaint = pbTypePaint
    OnKeyDown = pbTypeKeyDown
    OnKeyPress = pbTypeKeyPress
    OnEnter = pbTypeEnter
    OnExit = pbTypeExit
  end
  object t: TTimer
    Enabled = False
    Interval = 2000
    OnTimer = tTimer
    Left = 8
    Top = 48
  end
  object tTime: TTimer
    Enabled = False
    Interval = 200
    OnTimer = tTimeTimer
    Left = 8
    Top = 256
  end
end
