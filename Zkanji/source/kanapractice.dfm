object fKana: TfKana
  Left = 629
  Top = 195
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Hiragana and Katakana practice settings'
  ClientHeight = 349
  ClientWidth = 503
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
  ShowHint = True
  OnPaint = FormPaint
  DesignSize = (
    503
    349)
  PixelsPerInch = 96
  TextHeight = 13
  object bv1: TBevel
    Left = 0
    Top = 317
    Width = 9999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 278
  end
  object bv2: TBevel
    Left = 0
    Top = 319
    Width = 9999
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 280
  end
  object Label1: TLabel
    Left = 7
    Top = 6
    Width = 488
    Height = 26
    Caption = 
      'Select the hiragana and katakana you would like to practice by c' +
      'hecking the boxes.  CTRL+Click on any checkbox to select/deselec' +
      't the whole group belonging to the same consonant or consonant p' +
      'air. '
    WordWrap = True
  end
  object btnRStart: TBitBtn
    Left = 143
    Top = 322
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Reading practice'
    Default = True
    DoubleBuffered = True
    Enabled = False
    ModalResult = 1
    ParentDoubleBuffered = False
    TabOrder = 0
  end
  object BitBtn2: TBitBtn
    Left = 424
    Top = 322
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = '&Close'
    DoubleBuffered = True
    ModalResult = 2
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 2
  end
  object tcKana: TTabControl
    Left = 6
    Top = 38
    Width = 492
    Height = 276
    TabOrder = 3
    Tabs.Strings = (
      'Hiragana ('#12402#12425#12364#12394')'
      'Katakana ('#12459#12479#12459#12490')')
    TabIndex = 0
    OnChange = tcKanaChange
    object Label50: TLabel
      Left = 12
      Top = 101
      Width = 8
      Height = 14
      Caption = 'T'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label51: TLabel
      Left = 12
      Top = 125
      Width = 8
      Height = 14
      Caption = 'N'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label52: TLabel
      Left = 12
      Top = 149
      Width = 8
      Height = 14
      Caption = 'H'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label53: TLabel
      Left = 12
      Top = 173
      Width = 9
      Height = 14
      Caption = 'M'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label54: TLabel
      Left = 12
      Top = 197
      Width = 8
      Height = 14
      Caption = 'Y'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label55: TLabel
      Left = 12
      Top = 221
      Width = 7
      Height = 14
      Caption = 'R'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label56: TLabel
      Left = 10
      Top = 245
      Width = 12
      Height = 14
      Caption = 'W'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label57: TLabel
      Left = 155
      Top = 224
      Width = 8
      Height = 14
      Caption = 'N'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label58: TLabel
      Left = 271
      Top = 32
      Width = 9
      Height = 14
      Caption = 'O'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label59: TLabel
      Left = 247
      Top = 32
      Width = 7
      Height = 14
      Caption = 'E'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label60: TLabel
      Left = 392
      Top = 173
      Width = 15
      Height = 14
      Caption = 'RY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label61: TLabel
      Left = 223
      Top = 32
      Width = 8
      Height = 14
      Caption = 'U'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label62: TLabel
      Left = 201
      Top = 32
      Width = 4
      Height = 14
      Caption = 'I'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label63: TLabel
      Left = 175
      Top = 32
      Width = 8
      Height = 14
      Caption = 'A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label64: TLabel
      Left = 156
      Top = 53
      Width = 8
      Height = 14
      Caption = 'G'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label65: TLabel
      Left = 156
      Top = 77
      Width = 7
      Height = 14
      Caption = 'Z'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label66: TLabel
      Left = 156
      Top = 101
      Width = 8
      Height = 14
      Caption = 'D'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label67: TLabel
      Left = 156
      Top = 149
      Width = 7
      Height = 14
      Caption = 'B'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label68: TLabel
      Left = 156
      Top = 173
      Width = 7
      Height = 14
      Caption = 'P'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label69: TLabel
      Left = 319
      Top = 32
      Width = 8
      Height = 14
      Caption = 'A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label70: TLabel
      Left = 343
      Top = 32
      Width = 8
      Height = 14
      Caption = 'U'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label71: TLabel
      Left = 31
      Top = 32
      Width = 8
      Height = 14
      Caption = 'A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label72: TLabel
      Left = 367
      Top = 32
      Width = 9
      Height = 14
      Caption = 'O'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label73: TLabel
      Left = 415
      Top = 32
      Width = 8
      Height = 14
      Caption = 'A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label74: TLabel
      Left = 439
      Top = 32
      Width = 8
      Height = 14
      Caption = 'U'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label75: TLabel
      Left = 463
      Top = 32
      Width = 9
      Height = 14
      Caption = 'O'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label76: TLabel
      Left = 296
      Top = 53
      Width = 15
      Height = 14
      Caption = 'KY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label77: TLabel
      Left = 392
      Top = 53
      Width = 16
      Height = 14
      Caption = 'GY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label78: TLabel
      Left = 296
      Top = 77
      Width = 15
      Height = 14
      Caption = 'SH'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label79: TLabel
      Left = 398
      Top = 77
      Width = 5
      Height = 14
      Caption = 'J'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label80: TLabel
      Left = 392
      Top = 101
      Width = 16
      Height = 14
      Caption = 'DY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label81: TLabel
      Left = 296
      Top = 101
      Width = 15
      Height = 14
      Caption = 'CH'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label82: TLabel
      Left = 57
      Top = 32
      Width = 4
      Height = 14
      Caption = 'I'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label83: TLabel
      Left = 296
      Top = 125
      Width = 16
      Height = 14
      Caption = 'HY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label84: TLabel
      Left = 392
      Top = 125
      Width = 15
      Height = 14
      Caption = 'BY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label85: TLabel
      Left = 392
      Top = 149
      Width = 15
      Height = 14
      Caption = 'PY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label86: TLabel
      Left = 296
      Top = 173
      Width = 16
      Height = 14
      Caption = 'NY'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label87: TLabel
      Left = 439
      Top = 208
      Width = 7
      Height = 14
      Caption = 'E'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label88: TLabel
      Left = 463
      Top = 208
      Width = 9
      Height = 14
      Caption = 'O'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label89: TLabel
      Left = 415
      Top = 208
      Width = 8
      Height = 14
      Caption = 'U'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label90: TLabel
      Left = 393
      Top = 208
      Width = 4
      Height = 14
      Caption = 'I'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label91: TLabel
      Left = 367
      Top = 208
      Width = 8
      Height = 14
      Caption = 'A'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object lbVowels: TLabel
      Left = 304
      Top = 227
      Width = 46
      Height = 16
      Caption = 'Vowels:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label93: TLabel
      Left = 79
      Top = 32
      Width = 8
      Height = 14
      Caption = 'U'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label94: TLabel
      Left = 103
      Top = 32
      Width = 7
      Height = 14
      Caption = 'E'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label95: TLabel
      Left = 127
      Top = 32
      Width = 9
      Height = 14
      Caption = 'O'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label96: TLabel
      Left = 12
      Top = 53
      Width = 7
      Height = 14
      Caption = 'K'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label97: TLabel
      Left = 12
      Top = 77
      Width = 7
      Height = 14
      Caption = 'S'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object sbMinus: TSpeedButton
      Left = 457
      Top = 1
      Width = 27
      Height = 26
      Glyph.Data = {
        4A090000424D4A09000000000000420000002800000044000000110000000100
        10000300000008090000120B0000120B00000000000000000000007C0000E003
        00001F000000DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        B349B349B349DF7AB349DF7AB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7AB349B349B349DF7AB349DF7AB349DF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7AB349B349B349DF7AB349DF7AB349DF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7AB349FF7FB349FF7FB349FF7FB349FF7FB349DF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7AB349726FB349726FB349726FB349726FB349DF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7AB349D77BB349D77BB349D77BB349D77BB349DF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7AB349FF7FFF7FFF7FB349FF7FB349FF7FB349
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AB349726F726F726FB349726FB349
        726FB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AB349D77BD77BD77BB349D77B
        B349D77BB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AB349FF7FB349FF7FB349
        FF7FB349FF7FB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AB349726FB349
        726FB349726FB349726FB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AB349D77B
        B349D77BB349D77BB349D77BB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AB349
        FF7FFF7FFF7FB349FF7FB349FF7FB349DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7AB349726F726F726FB349726FB349726FB349DF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7AB349D77BD77BD77BB349D77BB349D77BB349DF7ADF7A945294529452
        94529452945294528C31FF7F8C318C31FF7F8C31FF7F8C31DF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A9452
        9452945294529452945294528C31726F8C318C31726F8C31726F8C31DF7ADF7A
        94529452945294529452945294528C31D77B8C318C31D77B8C31D77B8C31DF7A
        A73DA73DA63DA639A6398639A63986398639E42085398539E3206539E3209452
        DF7AF75EF75EF75ED65AD65AD65AD65AB556B556B556B556B556945294529452
        DF7ADF7A824182418241824182418241824182418241E12882418241E1288241
        E1289452DF7A0756075627560756075607562756275627564435285628564535
        285645359452DF7A8B568F7B8E7B6E7B6E7B6D776D774D774D774D774C772C77
        2C732B7386399452DF7A1863186318631863F75EF75EF75EF75ED65AD65AD65A
        D65AD65AB556B556DF7ADF7A0131015E015E0162015E0162225E225E025E025E
        225E025E025E225E82419452DF7A643DE976E976E976E976E976E976E976E976
        E976EA76EA76EA76EA7607569452DF7AAB56B07BB07BB07B8F7B8F7B8F7B8F7B
        6E7B6E776E776D774D774D77A6399452DF7A3967396739673967186318631863
        18631863F75EF75EF75ED65AD65AD65ADF7ADF7A0031E05D0062E06100620062
        0062005E0062006200620162015E015E61419452DF7A6339C776C776C776C776
        C776C876C876C876C876C876C876C876C87606529452DF7AAC5AD17FD17FD17F
        D07FB07BB07BB07B907B8F7B8F7B8F7B6E7B8E7BA73D9452DF7A5A6B5A6B5A6B
        5A6B3967396739673967396718631863186318631863F75EDF7ADF7AE02CC059
        C059C059C05DC05DE05DE05DE05DE05DE05DE05DE05DE05D60419452DF7A4339
        A576A576A576A676A676A676A676A676A676A676A676A676A776E5519452DF7A
        AD5AAD5AAC5AAC5AAC5AAC5AAC5AAC56AC56AB56AB568B568B568B56C83DDF7A
        DF7A5A6B5A6B5A6B5A6B5A6B5A6B5A6B5A6B3967396739673967396739671863
        DF7ADF7AE02CE02CE02CE02CE02CE02CE02CE02CE02CE02CE02CE02CE02CE02C
        403DDF7ADF7A4239423942394239423942394239423942394239423942394239
        4239C451DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7A}
      NumGlyphs = 4
      OnClick = sbPlusClick
    end
    object sbPlus: TSpeedButton
      Left = 430
      Top = 1
      Width = 27
      Height = 26
      Glyph.Data = {
        4A090000424D4A09000000000000420000002800000044000000110000000100
        10000300000008090000120B0000120B00000000000000000000007C0000E003
        00001F000000DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A94529452
        AD35AD35AD35DF7AD44DDF7AD44DDF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        94529452AD35AD35AD35DF7AD44DDF7AD44DDF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7A94529452AD35AD35AD35DF7AD44DDF7AD44DDF7ADF7ADF7ADF7ADF7ADF7A
        DF7A64354435E320FF7FC320FF7FD44DFF7FD44DFF7FD44DDF7ADF7ADF7ADF7A
        DF7ADF7A9452945294529452734E734EDF7A734EDF7A734EDF7ADF7ADF7ADF7A
        DF7ADF7ADF7A824182410129726F0129726FD44D726FD44D726FD44DDF7ADF7A
        DF7ADF7ADF7ADF7A285628566535D77B6535D77BD44DD77BD44DD77BD44DDF7A
        DF7ADF7ADF7ADF7ADF7A074EC96EC645FF7FFF7FFF7FD44DFF7FD44DFF7FD44D
        DF7ADF7ADF7ADF7ADF7ADF7AB556B5569452945294529452DF7A9452DF7A734E
        DF7ADF7ADF7ADF7ADF7ADF7ADF7A2231235E6339726F726F726FD44D726FD44D
        726FD44DDF7ADF7ADF7ADF7ADF7ADF7A863D0B7BE74DD77BD77BD77BD44DD77B
        D44DD77BD44DDF7ADF7ADF7ADF7ADF7ADF7A074EEA72C649FF7FE324FF7FD44D
        FF7FD44DFF7FD44DDF7ADF7ADF7ADF7ADF7ADF7AB556B556B556B556B5569452
        DF7A9452DF7A9452DF7ADF7ADF7ADF7ADF7ADF7ADF7A0131235E6239726F0129
        726FD44D726FD44D726FD44DDF7ADF7ADF7ADF7ADF7ADF7A853D0B7BE74DD77B
        6535D77BD44DD77BD44DD77BD44DDF7ADF7ADF7ADF7ADF7ADF7A28520B73E749
        FF7FFF7FFF7FD44DFF7FD44DFF7FD44DDF7ADF7ADF7ADF7ADF7ADF7AD65AD65A
        D65AB556B556B556DF7AB556DF7A9452DF7ADF7ADF7ADF7ADF7ADF7ADF7A0131
        235E6239726F726F726FD44D726FD44D726FD44DDF7ADF7ADF7ADF7ADF7ADF7A
        853DEA7AE649D77BD77BD77BD44DD77BD44DD77BD44DDF7ADF7A945294529452
        945249522C770C77E849FF7FAD35AD35FF7FAD35FF7FAD35DF7ADF7ADF7ADF7A
        DF7ADF7AF75EF75ED65AD65AD65ADF7ADF7AB556DF7AB556DF7ADF7ADF7A9452
        9452945294520131025E225E6139726FAD35AD35726FAD35726FAD35DF7ADF7A
        9452945294529452653DEA76EA76C649D77BAD35AD35D77BAD35D77BAD35DF7A
        C73DC73DC73DA73DA73D49524D774D772C770425A639863904258539E3249452
        DF7A39673967186318631863F75EF75EF75EF75EF75EF75ED65AD65AD65AD65A
        DF7ADF7A614161416141614161410031025E015E025EE12861416141E1288141
        01299452DF7A06520652065206520652643DE976E976E9764435075207564435
        075644359452DF7A8B568F7B8F7B8F7B8E7B6E776E776D776D774D774D772C77
        2C772C7786399452DF7A5A6B39673967396739671863186318631863F75EF75E
        F75EF75EF75ED65ADF7ADF7A0031005E015E006200620162015E015E015E015E
        0162015E025E025E61419452DF7A643DC876C876C876C876C876C876C876C976
        C976E976E976E976E97606529452DF7AAB56B07BB07BAF7B8F7B8F7B8F7B8E77
        6E7B6E776E776D774D774D77A6399452DF7A5A6B5A6B5A6B5A6B396739673967
        39671863186318631863F75EF75EF75EDF7ADF7A003100620062006200620062
        0062005E00620062006201620162015E61419452DF7A633DC776C776C776C776
        C776C876C876C876C876C876C876C876C87606529452DF7AAC5AD17FD17BD07F
        B07BB07BAF7B8F7B8F7B6F7B6F7B8E776E776E77A7399452DF7A7B6F7B6F5A6B
        5A6B5A6B5A6B5A6B39673967396739673967186318631863DF7ADF7AE02CE05D
        E05DE05DE05DE05DE05DE05D00620062006200620062006260419452DF7A4339
        A676A676A676A676A776C776C776C776C776C776C776C876C876E5519452DF7A
        AC5AAC5AAC5AAC5AAC56AC56D07FB07BB07B8B568B566A566A568A56C73DDF7A
        DF7A7B6F7B6F7B6F7B6F7B6F5A6B5A6B5A6B5A6B5A6B39673967396739673967
        DF7ADF7AE02CE02CE02CE02CE02CE02CE05DE05DE05DE02C002D003100310031
        6041DF7ADF7A433943394339433943394339A676A676A67643396339633D633D
        633DE551DF7ADF7ADF7ADF7ADF7ADF7ADF7AAC5AD17FD17FD17BE83D9452DF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A7B6F7B6F5A6B5A6B5A6BDF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AE02CC059C059C05D4041
        9452DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A4339A576A576A676
        C4519452DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AAC5AD17FD17F
        D17FE83D9452DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A7B6F7B6F
        7B6F7B6F7B6FDF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AE02C
        C059C059C059403D9452DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        4239857685768576C3519452DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7AAD5AD27FD27FD27FE93D9452DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7A7B6F7B6F7B6F7B6F7B6FDF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7AE02CC055A059C059403D9452DF7ADF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7A4239847684768476C3519452DF7ADF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7AAD5AD37FD37FD27FE93D9452DF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7A7B6F7B6F7B6F7B6F7B6FDF7ADF7ADF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7AE028A055A055A055203D9452DF7ADF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A4239647684768476C3519452DF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AAD5AAD5AAD5AAD5AE93DDF7ADF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A7B6F7B6F7B6F7B6F7B6FDF7A
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7AE028E028C028C0282039
        DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A4239423942394239
        A251DF7ADF7ADF7ADF7ADF7ADF7A}
      NumGlyphs = 4
      OnClick = sbPlusClick
    end
    object lbInstruction: TLabel
      Left = 229
      Top = 255
      Width = 251
      Height = 13
      Alignment = taRightJustify
      Caption = 'Select at least 2 syllables to start practicing!'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object lbPage: TLabel
      Left = 193
      Top = 197
      Width = 83
      Height = 44
      Caption = 'Hiragana syllables'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -19
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      WordWrap = True
    end
    object CheckBox105: TCheckBox
      Tag = 1
      Left = 29
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KA'
      TabOrder = 0
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox106: TCheckBox
      Tag = 2
      Left = 29
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SA'
      TabOrder = 5
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox107: TCheckBox
      Tag = 27
      Left = 461
      Top = 227
      Width = 15
      Height = 17
      Hint = 'O'
      TabOrder = 103
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox108: TCheckBox
      Tag = 27
      Left = 437
      Top = 227
      Width = 15
      Height = 17
      Hint = 'E'
      TabOrder = 102
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox109: TCheckBox
      Tag = 27
      Left = 413
      Top = 227
      Width = 15
      Height = 17
      Hint = 'U'
      TabOrder = 101
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox110: TCheckBox
      Tag = 27
      Left = 389
      Top = 227
      Width = 15
      Height = 17
      Hint = 'I'
      TabOrder = 100
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox111: TCheckBox
      Tag = 27
      Left = 365
      Top = 227
      Width = 15
      Height = 17
      Hint = 'A'
      TabOrder = 99
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox112: TCheckBox
      Tag = 3
      Left = 125
      Top = 99
      Width = 15
      Height = 17
      Hint = 'TO'
      TabOrder = 14
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox113: TCheckBox
      Tag = 3
      Left = 101
      Top = 99
      Width = 15
      Height = 17
      Hint = 'TE'
      TabOrder = 13
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox114: TCheckBox
      Tag = 3
      Left = 77
      Top = 99
      Width = 15
      Height = 17
      Hint = 'TSU'
      TabOrder = 12
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox115: TCheckBox
      Tag = 3
      Left = 53
      Top = 99
      Width = 15
      Height = 17
      Hint = 'CHI'
      TabOrder = 11
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox116: TCheckBox
      Tag = 3
      Left = 29
      Top = 99
      Width = 15
      Height = 17
      Hint = 'TA'
      TabOrder = 10
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox117: TCheckBox
      Tag = 4
      Left = 125
      Top = 123
      Width = 15
      Height = 17
      Hint = 'NO'
      TabOrder = 19
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox118: TCheckBox
      Tag = 4
      Left = 101
      Top = 123
      Width = 15
      Height = 17
      Hint = 'NE'
      TabOrder = 18
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox119: TCheckBox
      Tag = 4
      Left = 77
      Top = 123
      Width = 15
      Height = 17
      Hint = 'NU'
      TabOrder = 17
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox120: TCheckBox
      Tag = 4
      Left = 53
      Top = 123
      Width = 15
      Height = 17
      Hint = 'NI'
      TabOrder = 16
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox121: TCheckBox
      Tag = 1
      Left = 53
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KI'
      TabOrder = 1
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox122: TCheckBox
      Tag = 4
      Left = 29
      Top = 123
      Width = 15
      Height = 17
      Hint = 'NA'
      TabOrder = 15
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox123: TCheckBox
      Tag = 5
      Left = 125
      Top = 147
      Width = 15
      Height = 17
      Hint = 'HO'
      TabOrder = 24
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox124: TCheckBox
      Tag = 5
      Left = 101
      Top = 147
      Width = 15
      Height = 17
      Hint = 'HE'
      TabOrder = 23
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox125: TCheckBox
      Tag = 5
      Left = 77
      Top = 147
      Width = 15
      Height = 17
      Hint = 'FU'
      TabOrder = 22
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox126: TCheckBox
      Tag = 5
      Left = 53
      Top = 147
      Width = 15
      Height = 17
      Hint = 'HI'
      TabOrder = 21
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox127: TCheckBox
      Tag = 5
      Left = 29
      Top = 147
      Width = 15
      Height = 17
      Hint = 'HA'
      TabOrder = 20
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox128: TCheckBox
      Tag = 6
      Left = 125
      Top = 171
      Width = 15
      Height = 17
      Hint = 'MO'
      TabOrder = 29
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox129: TCheckBox
      Tag = 6
      Left = 101
      Top = 171
      Width = 15
      Height = 17
      Hint = 'ME'
      TabOrder = 28
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox130: TCheckBox
      Tag = 6
      Left = 77
      Top = 171
      Width = 15
      Height = 17
      Hint = 'MU'
      TabOrder = 27
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox131: TCheckBox
      Tag = 6
      Left = 53
      Top = 171
      Width = 15
      Height = 17
      Hint = 'MI'
      TabOrder = 26
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox132: TCheckBox
      Tag = 1
      Left = 77
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KU'
      TabOrder = 2
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox133: TCheckBox
      Tag = 6
      Left = 29
      Top = 171
      Width = 15
      Height = 17
      Hint = 'MA'
      TabOrder = 25
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox134: TCheckBox
      Tag = 7
      Left = 125
      Top = 195
      Width = 15
      Height = 17
      Hint = 'YO'
      TabOrder = 32
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox135: TCheckBox
      Tag = 11
      Left = 173
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GA'
      TabOrder = 41
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox136: TCheckBox
      Tag = 7
      Left = 77
      Top = 195
      Width = 15
      Height = 17
      Hint = 'YU'
      TabOrder = 31
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox137: TCheckBox
      Tag = 11
      Left = 197
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GI'
      TabOrder = 42
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox138: TCheckBox
      Tag = 7
      Left = 29
      Top = 195
      Width = 15
      Height = 17
      Hint = 'YA'
      TabOrder = 30
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox139: TCheckBox
      Tag = 8
      Left = 125
      Top = 219
      Width = 15
      Height = 17
      Hint = 'RO'
      TabOrder = 37
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox140: TCheckBox
      Tag = 8
      Left = 101
      Top = 219
      Width = 15
      Height = 17
      Hint = 'RE'
      TabOrder = 36
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox141: TCheckBox
      Tag = 8
      Left = 77
      Top = 219
      Width = 15
      Height = 17
      Hint = 'RU'
      TabOrder = 35
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox142: TCheckBox
      Tag = 8
      Left = 53
      Top = 219
      Width = 15
      Height = 17
      Hint = 'RI'
      TabOrder = 34
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox143: TCheckBox
      Tag = 1
      Left = 101
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KE'
      TabOrder = 3
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox144: TCheckBox
      Tag = 8
      Left = 29
      Top = 219
      Width = 15
      Height = 17
      Hint = 'RA'
      TabOrder = 33
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox145: TCheckBox
      Tag = 9
      Left = 125
      Top = 243
      Width = 15
      Height = 17
      Hint = 'WO'
      TabOrder = 39
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox146: TCheckBox
      Tag = 11
      Left = 221
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GU'
      TabOrder = 43
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox147: TCheckBox
      Tag = 11
      Left = 245
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GE'
      TabOrder = 44
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox148: TCheckBox
      Tag = 11
      Left = 269
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GO'
      TabOrder = 45
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox149: TCheckBox
      Tag = 9
      Left = 29
      Top = 243
      Width = 15
      Height = 17
      Hint = 'WA'
      TabOrder = 38
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox150: TCheckBox
      Tag = 10
      Left = 153
      Top = 243
      Width = 15
      Height = 17
      Hint = 'N'#39' (watch the '#39' !)'
      TabOrder = 40
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox151: TCheckBox
      Tag = 12
      Left = 269
      Top = 75
      Width = 15
      Height = 17
      Hint = 'ZO'
      TabOrder = 50
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox152: TCheckBox
      Tag = 12
      Left = 245
      Top = 75
      Width = 15
      Height = 17
      Hint = 'ZE'
      TabOrder = 49
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox153: TCheckBox
      Tag = 12
      Left = 221
      Top = 75
      Width = 15
      Height = 17
      Hint = 'ZU'
      TabOrder = 48
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox154: TCheckBox
      Tag = 1
      Left = 125
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KO'
      TabOrder = 4
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox155: TCheckBox
      Tag = 12
      Left = 197
      Top = 75
      Width = 15
      Height = 17
      Hint = 'JI'
      TabOrder = 47
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox156: TCheckBox
      Tag = 12
      Left = 173
      Top = 75
      Width = 15
      Height = 17
      Hint = 'ZA'
      TabOrder = 46
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox157: TCheckBox
      Tag = 13
      Left = 269
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DO'
      TabOrder = 55
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox158: TCheckBox
      Tag = 13
      Left = 245
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DE'
      TabOrder = 54
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox159: TCheckBox
      Tag = 13
      Left = 221
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DU (ZU)'
      TabOrder = 53
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox160: TCheckBox
      Tag = 13
      Left = 197
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DI (JI)'
      TabOrder = 52
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox161: TCheckBox
      Tag = 13
      Left = 173
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DA'
      TabOrder = 51
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox162: TCheckBox
      Tag = 14
      Left = 269
      Top = 147
      Width = 15
      Height = 17
      Hint = 'BO'
      TabOrder = 60
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox163: TCheckBox
      Tag = 14
      Left = 245
      Top = 147
      Width = 15
      Height = 17
      Hint = 'BE'
      TabOrder = 59
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox164: TCheckBox
      Tag = 14
      Left = 221
      Top = 147
      Width = 15
      Height = 17
      Hint = 'BU'
      TabOrder = 58
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox165: TCheckBox
      Tag = 2
      Left = 125
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SO'
      TabOrder = 9
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox166: TCheckBox
      Tag = 14
      Left = 197
      Top = 147
      Width = 15
      Height = 17
      Hint = 'BI'
      TabOrder = 57
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox167: TCheckBox
      Tag = 14
      Left = 173
      Top = 147
      Width = 15
      Height = 17
      Hint = 'BA'
      TabOrder = 56
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox168: TCheckBox
      Tag = 15
      Left = 269
      Top = 171
      Width = 15
      Height = 17
      Hint = 'PO'
      TabOrder = 65
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox169: TCheckBox
      Tag = 15
      Left = 245
      Top = 171
      Width = 15
      Height = 17
      Hint = 'PE'
      TabOrder = 64
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox170: TCheckBox
      Tag = 15
      Left = 221
      Top = 171
      Width = 15
      Height = 17
      Hint = 'PU'
      TabOrder = 63
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox171: TCheckBox
      Tag = 15
      Left = 197
      Top = 171
      Width = 15
      Height = 17
      Hint = 'PI'
      TabOrder = 62
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox172: TCheckBox
      Tag = 15
      Left = 173
      Top = 171
      Width = 15
      Height = 17
      Hint = 'PA'
      TabOrder = 61
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox173: TCheckBox
      Tag = 16
      Left = 317
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KYA'
      TabOrder = 66
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox174: TCheckBox
      Tag = 16
      Left = 341
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KYU'
      TabOrder = 67
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox175: TCheckBox
      Tag = 16
      Left = 365
      Top = 51
      Width = 15
      Height = 17
      Hint = 'KYO'
      TabOrder = 68
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox176: TCheckBox
      Tag = 2
      Left = 101
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SE'
      TabOrder = 8
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox177: TCheckBox
      Tag = 17
      Left = 365
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SHO'
      TabOrder = 71
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox178: TCheckBox
      Tag = 17
      Left = 341
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SHU'
      TabOrder = 70
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox179: TCheckBox
      Tag = 17
      Left = 317
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SHA'
      TabOrder = 69
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox180: TCheckBox
      Tag = 18
      Left = 317
      Top = 99
      Width = 15
      Height = 17
      Hint = 'CHA'
      TabOrder = 72
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox181: TCheckBox
      Tag = 18
      Left = 341
      Top = 99
      Width = 15
      Height = 17
      Hint = 'CHU'
      TabOrder = 73
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox182: TCheckBox
      Tag = 18
      Left = 365
      Top = 99
      Width = 15
      Height = 17
      Hint = 'CHO'
      TabOrder = 74
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox183: TCheckBox
      Tag = 21
      Left = 413
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GYA'
      TabOrder = 81
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox184: TCheckBox
      Tag = 21
      Left = 437
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GYU'
      TabOrder = 82
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox185: TCheckBox
      Tag = 21
      Left = 461
      Top = 51
      Width = 15
      Height = 17
      Hint = 'GYO'
      TabOrder = 83
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox186: TCheckBox
      Tag = 22
      Left = 461
      Top = 75
      Width = 15
      Height = 17
      Hint = 'JO'
      TabOrder = 86
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox187: TCheckBox
      Tag = 2
      Left = 77
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SU'
      TabOrder = 7
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox188: TCheckBox
      Tag = 22
      Left = 437
      Top = 75
      Width = 15
      Height = 17
      Hint = 'JU'
      TabOrder = 85
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox189: TCheckBox
      Tag = 22
      Left = 413
      Top = 75
      Width = 15
      Height = 17
      Hint = 'JA'
      TabOrder = 84
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox190: TCheckBox
      Tag = 23
      Left = 413
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DYA (JA)'
      TabOrder = 87
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox191: TCheckBox
      Tag = 23
      Left = 437
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DYU (JU)'
      TabOrder = 88
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox192: TCheckBox
      Tag = 23
      Left = 461
      Top = 99
      Width = 15
      Height = 17
      Hint = 'DYO (JO)'
      TabOrder = 89
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox193: TCheckBox
      Tag = 24
      Left = 413
      Top = 123
      Width = 15
      Height = 17
      Hint = 'BYA'
      TabOrder = 90
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox194: TCheckBox
      Tag = 24
      Left = 437
      Top = 123
      Width = 15
      Height = 17
      Hint = 'BYU'
      TabOrder = 91
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox195: TCheckBox
      Tag = 24
      Left = 461
      Top = 123
      Width = 15
      Height = 17
      Hint = 'BYO'
      TabOrder = 92
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox196: TCheckBox
      Tag = 25
      Left = 461
      Top = 147
      Width = 15
      Height = 17
      Hint = 'PYO'
      TabOrder = 95
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox197: TCheckBox
      Tag = 25
      Left = 437
      Top = 147
      Width = 15
      Height = 17
      Hint = 'PYU'
      TabOrder = 94
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox198: TCheckBox
      Tag = 2
      Left = 53
      Top = 75
      Width = 15
      Height = 17
      Hint = 'SHI'
      TabOrder = 6
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox199: TCheckBox
      Tag = 25
      Left = 413
      Top = 147
      Width = 15
      Height = 17
      Hint = 'PYA'
      TabOrder = 93
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox200: TCheckBox
      Tag = 26
      Left = 413
      Top = 171
      Width = 15
      Height = 17
      Hint = 'RYA'
      TabOrder = 96
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox201: TCheckBox
      Tag = 26
      Left = 437
      Top = 171
      Width = 15
      Height = 17
      Hint = 'RYU'
      TabOrder = 97
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox202: TCheckBox
      Tag = 26
      Left = 461
      Top = 171
      Width = 15
      Height = 17
      Hint = 'RYO'
      TabOrder = 98
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox203: TCheckBox
      Tag = 19
      Left = 317
      Top = 123
      Width = 15
      Height = 17
      Hint = 'HYA'
      TabOrder = 75
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox204: TCheckBox
      Tag = 19
      Left = 341
      Top = 123
      Width = 15
      Height = 17
      Hint = 'HYU'
      TabOrder = 76
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox205: TCheckBox
      Tag = 19
      Left = 363
      Top = 123
      Width = 15
      Height = 17
      Hint = 'HYO'
      TabOrder = 77
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox206: TCheckBox
      Tag = 20
      Left = 317
      Top = 171
      Width = 15
      Height = 17
      Hint = 'NYA'
      TabOrder = 78
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox207: TCheckBox
      Tag = 20
      Left = 341
      Top = 171
      Width = 15
      Height = 17
      Hint = 'NYU'
      TabOrder = 79
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
    object CheckBox208: TCheckBox
      Tag = 20
      Left = 363
      Top = 171
      Width = 15
      Height = 17
      Hint = 'NYO'
      TabOrder = 80
      OnClick = CheckBox1Click
      OnMouseDown = CheckBox1MouseDown
    end
  end
  object btnWStart: TBitBtn
    Left = 249
    Top = 322
    Width = 105
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Writing practice'
    Default = True
    DoubleBuffered = True
    Enabled = False
    ModalResult = 6
    ParentDoubleBuffered = False
    TabOrder = 1
  end
end
