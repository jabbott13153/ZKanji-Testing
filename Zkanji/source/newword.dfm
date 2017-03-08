object fNewWord: TfNewWord
  Left = 783
  Top = 365
  ActiveControl = edReading
  BorderStyle = bsSingle
  Caption = 'New word definition'
  ClientHeight = 389
  ClientWidth = 592
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000040030000000000000000000000000000000000000000
    00000000A4A0A0000000000000000000A4A0A0000000000000A4A0A000000000
    0000000000000000A4A0A0000000000000000000000000000000000000000000
    A4A0A0000000000000000000A4A0A0000000000000A4A0A00000000000000000
    00000000000000A4A0A0A4A0A0000000A4A0A000000000000000000000000000
    0000A4A0A0000000000000000000000000000000000000000000000000000000
    A4A0A00000000000000000000000000000000000000000000000000000000000
    00000000000000000000000000A4A0A0000000000000000000000000A4A0A000
    0000A4A0A0000000000000000000000000A4A0A0A4A0A0000000A4A0A0C7B1FF
    A4A0A0000000A4A0A0000000000000000000000000A4A0A00000000000000000
    00C7B1FFC7B1FFA4A0A0C7B1FF5725FFC7B1FFA4A0A0C7B1FFC7B1FFA4A0A000
    0000000000000000000000000000000000C7B1FF5725FFC7B1FFC7B1FF8EFFFF
    C7B1FFC7B1FF5725FFC7B1FF000000000000A4A0A0A4A0A0A4A0A00000000000
    00A4A0A0C7B1FF8EFFFFC7B1FF8EFFFFC7B1FF8EFFFFC7B1FFA4A0A000000000
    0000000000000000000000A4A0A0A4A0A0C7B1FFC7B1FF8EFFFF8EFFFF8EFFFF
    8EFFFFC7B1FFC7B1FFC7B1FFA4A0A0000000000000A4A0A0000000000000C7B1
    FF5725FF8EFFFF8EFFFFFFFFFFFFFFFF8EFFFF8EFFFF8EFFFF5725FFC7B1FF00
    0000000000A4A0A0A4A0A0000000000000C7B1FFC7B1FF8EFFFF8EFFFF8EFFFF
    8EFFFF8EFFFFC7B1FFA4A0A00000000000000000000000000000000000000000
    00A4A0A0C7B1FF8EFFFFC7B1FF8EFFFFC7B1FFC7B1FF5725FFC7B1FFA4A0A000
    0000000000A4A0A0000000000000000000C7B1FF5725FFC7B1FFC7B1FF8EFFFF
    C7B1FFA4A0A0C7B1FFC7B1FF0000000000000000000000000000000000000000
    00C7B1FFC7B1FF000000C7B1FF5725FFC7B1FF00000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000C7B1FF
    000000000000000000000000000000000000000000000000000000000000C185
    0000D9810000C1C10000E1E30000FBC7000091030000800F0000803100008030
    0000001200000010000080310000801300008037000091FF0000FBFF0000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  DesignSize = (
    592
    389)
  PixelsPerInch = 96
  TextHeight = 13
  object b1: TBevel
    Left = -1
    Top = 90
    Width = 594
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label2: TLabel
    Left = 302
    Top = 40
    Width = 28
    Height = 13
    Alignment = taRightJustify
    Caption = '&Kana:'
    FocusControl = edReading
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object b2: TBevel
    Left = -1
    Top = 122
    Width = 594
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label4: TLabel
    Left = 17
    Top = 67
    Width = 44
    Height = 13
    Alignment = taRightJustify
    Caption = '&Meaning:'
    FocusControl = edMeaning
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Label8: TLabel
    Left = 17
    Top = 100
    Width = 88
    Height = 13
    Alignment = taRightJustify
    Caption = '&Frequency of use:'
    FocusControl = cbFrequency
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object sbRecognize: TSpeedButton
    Left = 70
    Top = 36
    Width = 23
    Height = 22
    AllowAllUp = True
    GroupIndex = 7
    Flat = True
    Glyph.Data = {
      52050000424D5205000000000000420000002800000024000000120000000100
      10000300000010050000C30E0000C30E00000000000000000000007C0000E003
      00001F000000FF7D7B6F7B6F7B6F7B6F9C73F75E6B2D10427B6F9C739C739C73
      7B6F1863F75E5A6768517B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F9C739C73
      9C739C739C739C739C739C739C737B6F7B6F7B6F7B6F9C733967AD3508214A29
      D65A9C739C739C735A6BF75E185F6851D06A7B6F5A6B5A6B7B6F9C737B6F7B6F
      9C739C739C739C739C739C739C739C739C739C739C73BD77BD77BD77BD779C73
      524AEF3D734E10428C313967BD779C733967195F6851D16AD06A3967F75ED65A
      F75E7B6F9C739C739C739C739C739C739C739C73BD77BD77BD77BD77BD77DE7B
      DE7BDE7BDE7B9C7318637B6FDE7BF75EE71CB556FF7FBD770000A955D16AD16A
      674D1863D65AF75ED65AF75E7B6FDE7BDE7BDE7BDE7BDE7BDE7BDE7BDE7BBD77
      5A6B7B6FDE7B9C73BD77BD77BD77BD77BD77DE7BDE7BD65AC6183146FF7F0000
      0000CB5DF26A674DFF7F7B6F1863D65AD65AD65AD65A5A6BBD77DE7BDE7BDE7B
      DE7BDE7B7B6FF75EF75E1863FF7F1042734EF75E5A6B9C73BD77DE7BBD773146
      42083146FF7F0000000000008855FF7FDE7B9C739C735A6BF75ED65AD65AD65A
      3967BD77BD77BD77BD775A6BF75EF75EF75E7B6FDE7B42082104841008218C31
      10423146734E4A292104734E0000000000000000FF7FDE7BDE7B9C739C739C73
      7B6F1863D65AD65AD65A7B6FBD77BD775A6BF75EF75EF75E7B6FBD77DE7B524A
      CE396B2D0821A51484108410E71C42082104DE7B00000000DE7B39675A6BDE7B
      7B6FBD77BD77BD77BD779C733967F75ED65AD65A7B6F7B6FF75EF75E18639C73
      DE7BDE7BDE7BDE7BBD77BD777B6FF75ED65A734E4A2984108C31314694521863
      D65AE71CEF3D7B6F734EBD77BD77DE7BDE7BDE7BDE7B7B6FF75EF75ED65AF75E
      F75E18639C73DE7BFF7FFF7FFF7FBD77BD77BD77DE7BFF7FDE7B10420000C618
      F75EFF7FFF7FDE7B9C73F75E1863FF7F7B6FBD77BD77BD77DE7BDE7BDE7BDE7B
      7B6FF75EF75ED65AF75E9C73DE7BDE7BDE7BDE7BDE7B9C739C739C739C735A6B
      7B6F5A6B4A29000084109452BD77FF7FDE7BFF7FFF7FDE7BFF7F9C739C739C73
      BD77BD77DE7BBD773967F75EF75EF75EF75E9C73DE7BDE7BDE7BDE7BDE7B9C73
      9C739C735A6BCE398C31CE39EF3D2925630C2104CE399C739C737B6FBD77FF7F
      FF7F7B6F9C739C739C739C739C733967F75E18633967F75EF75E1863BD77DE7B
      DE7BDE7BDE7B9C737B6FF75E39671863524AAD350821C618E71CC618E71C9C73
      BD77524AAD35B556DE7B9C739C739C739C739C733967186339679C73DE7B9C73
      1863F75E1863BD77DE7BDE7BDE7B7B6F524AEF3D18637B6F5A6B1863D65A9452
      CE394A29524AF75EF75E734EE71C84104A29FF7FBD77BD779C73396718637B6F
      BD77DE7BBD77BD777B6F1863F75E3967BD779C739C73CE39630C29254A292925
      292529250821E71CC618C618A51484108410630C420800000000FF7FDE7BBD77
      5A6B5A6BBD77FF7FFF7FDE7BFF7FDE7BDE7BBD775A6B18637B6F9C739C73CE39
      082129254A294A296B2D8C316B2DA514630C8C31EF3D10423146524A94529452
      524AFF7FDE7B9C737B6FDE7BFF7FFF7FFF7FFF7FFF7FDE7BDE7BDE7BBD777B6F
      5A6BBD77BD779C737B6F7B6F7B6F9C739C739C73D65A630C0000734EDE7BDE7B
      DE7BDE7BFF7FFF7FFF7FFF7FDE7BDE7BDE7BFF7FFF7FFF7FFF7FFF7FFF7FFF7F
      FF7FFF7FFF7FFF7FDE7BDE7BFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F39674208
      0000734EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
      FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F}
    Layout = blGlyphRight
    NumGlyphs = 2
    Spacing = 0
  end
  object Label1: TLabel
    Left = 18
    Top = 40
    Width = 40
    Height = 13
    Alignment = taRightJustify
    Caption = '&Written:'
    FocusControl = edKanji
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object b0: TBevel
    Left = -2
    Top = 30
    Width = 594
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label3: TLabel
    Left = 17
    Top = 8
    Width = 58
    Height = 13
    Alignment = taRightJustify
    Caption = '&Destination:'
    FocusControl = cbDict
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object b3: TBevel
    Left = -2
    Top = 355
    Width = 594
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object edReading: TZPaintBox
    Left = 336
    Top = 35
    Width = 249
    Height = 24
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Gothic'
    Font.Style = []
    HorzScrollBar.Range = 5
    HorzScrollBar.Visible = False
    ParentFont = False
    TabOrder = 2
    VertScrollBar.Range = 5
    VertScrollBar.Visible = False
  end
  object btnNew: TButton
    Left = 227
    Top = 361
    Width = 137
    Height = 25
    Caption = 'Add word to dictionary'
    Default = True
    Enabled = False
    ModalResult = 1
    TabOrder = 6
  end
  object pBoxes: TPanel
    Left = 8
    Top = 128
    Width = 576
    Height = 223
    Anchors = [akLeft, akTop, akRight]
    BevelOuter = bvLowered
    TabOrder = 5
    object bvH: TBevel
      Left = 1
      Top = 112
      Width = 456
      Height = 2
      Shape = bsTopLine
    end
    object bvV: TBevel
      Left = 457
      Top = 1
      Width = 2
      Height = 221
      Shape = bsLeftLine
    end
    object Label5: TLabel
      Left = 470
      Top = 9
      Width = 29
      Height = 13
      Caption = 'F&ield:'
      FocusControl = cb45
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label6: TLabel
      Left = 6
      Top = 6
      Width = 62
      Height = 13
      Caption = 'Word &type:'
      FocusControl = cb1
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label7: TLabel
      Left = 6
      Top = 118
      Width = 38
      Height = 13
      Caption = '&Usage:'
      FocusControl = cb24
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object cb1: TCheckBox
      Left = 120
      Top = 8
      Width = 49
      Height = 17
      Caption = 'noun'
      TabOrder = 0
    end
    object cb2: TCheckBox
      Left = 232
      Top = 8
      Width = 73
      Height = 17
      Caption = 'takes -suru'
      TabOrder = 1
    end
    object cb3: TCheckBox
      Left = 344
      Top = 8
      Width = 79
      Height = 17
      Caption = 'Godan verb'
      TabOrder = 2
    end
    object cb4: TCheckBox
      Left = 8
      Top = 24
      Width = 74
      Height = 17
      Caption = 'expression'
      TabOrder = 3
    end
    object cb5: TCheckBox
      Left = 120
      Top = 24
      Width = 84
      Height = 17
      Caption = '-na adjective'
      TabOrder = 4
    end
    object cb6: TCheckBox
      Left = 232
      Top = 24
      Width = 89
      Height = 17
      Caption = 'transitive verb'
      TabOrder = 5
    end
    object cb7: TCheckBox
      Left = 344
      Top = 24
      Width = 83
      Height = 17
      Caption = 'Ichidan verb'
      TabOrder = 6
    end
    object cb8: TCheckBox
      Left = 8
      Top = 40
      Width = 96
      Height = 17
      Caption = 'intransitive verb'
      TabOrder = 7
    end
    object cb9: TCheckBox
      Left = 120
      Top = 40
      Width = 56
      Height = 17
      Caption = 'adverb'
      TabOrder = 8
    end
    object cb10: TCheckBox
      Left = 232
      Top = 40
      Width = 65
      Height = 17
      Caption = 'adjective'
      TabOrder = 9
    end
    object cb11: TCheckBox
      Left = 344
      Top = 40
      Width = 84
      Height = 17
      Caption = 'may take -no'
      TabOrder = 10
    end
    object cb12: TCheckBox
      Left = 8
      Top = 56
      Width = 48
      Height = 17
      Caption = 'suffix'
      TabOrder = 11
    end
    object cb13: TCheckBox
      Left = 120
      Top = 56
      Width = 91
      Height = 17
      Caption = 'Taru adjective'
      TabOrder = 12
    end
    object cb14: TCheckBox
      Left = 232
      Top = 56
      Width = 49
      Height = 17
      Caption = 'prefix'
      TabOrder = 13
    end
    object cb15: TCheckBox
      Left = 344
      Top = 56
      Width = 69
      Height = 17
      Caption = 'suru verb'
      TabOrder = 14
    end
    object cb16: TCheckBox
      Left = 8
      Top = 72
      Width = 88
      Height = 17
      Caption = 'pre -noun adj.'
      TabOrder = 15
    end
    object cb17: TCheckBox
      Left = 120
      Top = 72
      Width = 80
      Height = 17
      Caption = 'conjunction'
      TabOrder = 16
    end
    object cb18: TCheckBox
      Left = 232
      Top = 72
      Width = 73
      Height = 17
      Caption = 'interjection'
      TabOrder = 17
    end
    object cb19: TCheckBox
      Left = 344
      Top = 72
      Width = 67
      Height = 17
      Caption = '-iku verb'
      TabOrder = 18
    end
    object cb20: TCheckBox
      Left = 8
      Top = 88
      Width = 84
      Height = 17
      Caption = 'auxiliary verb'
      TabOrder = 19
    end
    object cb21: TCheckBox
      Left = 120
      Top = 88
      Width = 72
      Height = 17
      Caption = '-kuru verb'
      TabOrder = 20
    end
    object cb22: TCheckBox
      Left = 232
      Top = 88
      Width = 103
      Height = 17
      Caption = 'auxiliary adjective'
      TabOrder = 21
    end
    object cb23: TCheckBox
      Left = 344
      Top = 88
      Width = 69
      Height = 17
      Caption = '-aru verb'
      TabOrder = 22
    end
    object cb24: TCheckBox
      Left = 120
      Top = 120
      Width = 68
      Height = 17
      Caption = 'sensitivity'
      TabOrder = 23
    end
    object cb25: TCheckBox
      Left = 232
      Top = 120
      Width = 81
      Height = 17
      Caption = 'abbreviation'
      TabOrder = 24
    end
    object cb26: TCheckBox
      Left = 344
      Top = 120
      Width = 62
      Height = 17
      Caption = 'honorifix'
      TabOrder = 25
    end
    object cb27: TCheckBox
      Left = 8
      Top = 136
      Width = 105
      Height = 17
      Caption = 'humble language'
      TabOrder = 26
    end
    object cb28: TCheckBox
      Left = 120
      Top = 136
      Width = 83
      Height = 17
      Caption = 'colloquialism'
      TabOrder = 27
    end
    object cb29: TCheckBox
      Left = 232
      Top = 136
      Width = 48
      Height = 17
      Caption = 'polite'
      TabOrder = 28
    end
    object cb30: TCheckBox
      Left = 344
      Top = 136
      Width = 49
      Height = 17
      Caption = 'slang'
      TabOrder = 29
    end
    object cb31: TCheckBox
      Left = 8
      Top = 152
      Width = 102
      Height = 17
      Caption = 'familiar language'
      TabOrder = 30
    end
    object cb32: TCheckBox
      Left = 120
      Top = 152
      Width = 45
      Height = 17
      Caption = 'male'
      TabOrder = 31
    end
    object cb33: TCheckBox
      Left = 232
      Top = 152
      Width = 54
      Height = 17
      Caption = 'female'
      TabOrder = 32
    end
    object cb34: TCheckBox
      Left = 344
      Top = 152
      Width = 79
      Height = 17
      Caption = 'kana usage'
      TabOrder = 33
    end
    object cb35: TCheckBox
      Left = 8
      Top = 168
      Width = 93
      Height = 17
      Caption = 'only kanji used'
      TabOrder = 34
    end
    object cb36: TCheckBox
      Left = 120
      Top = 168
      Width = 65
      Height = 17
      Caption = 'archaism'
      TabOrder = 35
    end
    object cb37: TCheckBox
      Left = 232
      Top = 168
      Width = 42
      Height = 17
      Caption = 'rare'
      TabOrder = 36
    end
    object cb38: TCheckBox
      Left = 344
      Top = 168
      Width = 86
      Height = 17
      Caption = 'obsolete term'
      TabOrder = 37
    end
    object cb39: TCheckBox
      Left = 8
      Top = 184
      Width = 53
      Height = 17
      Caption = 'vulgar'
      TabOrder = 38
    end
    object cb40: TCheckBox
      Left = 120
      Top = 184
      Width = 57
      Height = 17
      Caption = 'X rated'
      TabOrder = 39
    end
    object cb41: TCheckBox
      Left = 232
      Top = 184
      Width = 73
      Height = 17
      Caption = 'derogatory'
      TabOrder = 40
    end
    object cb42: TCheckBox
      Left = 344
      Top = 184
      Width = 65
      Height = 17
      Caption = 'obscure'
      TabOrder = 41
    end
    object cb43: TCheckBox
      Left = 8
      Top = 200
      Width = 65
      Height = 17
      Caption = 'idiomatic'
      TabOrder = 42
    end
    object cb44: TCheckBox
      Left = 120
      Top = 200
      Width = 81
      Height = 17
      Caption = 'manga slang'
      TabOrder = 43
    end
    object cb45: TCheckBox
      Left = 472
      Top = 29
      Width = 46
      Height = 17
      Caption = 'math'
      TabOrder = 44
    end
    object cb46: TCheckBox
      Left = 472
      Top = 53
      Width = 68
      Height = 17
      Caption = 'linguistics'
      TabOrder = 45
    end
    object cb47: TCheckBox
      Left = 472
      Top = 77
      Width = 73
      Height = 17
      Caption = 'computers'
      TabOrder = 46
    end
    object cb48: TCheckBox
      Left = 472
      Top = 101
      Width = 44
      Height = 17
      Caption = 'food'
      TabOrder = 47
    end
    object cb49: TCheckBox
      Left = 472
      Top = 125
      Width = 73
      Height = 17
      Caption = 'martial arts'
      TabOrder = 48
    end
    object cb50: TCheckBox
      Left = 472
      Top = 149
      Width = 66
      Height = 17
      Caption = 'geometry'
      TabOrder = 49
    end
    object cb51: TCheckBox
      Left = 472
      Top = 173
      Width = 57
      Height = 17
      Caption = 'physics'
      TabOrder = 50
    end
    object cb52: TCheckBox
      Left = 472
      Top = 197
      Width = 89
      Height = 17
      Caption = 'Buddhist term'
      TabOrder = 51
    end
  end
  object edMeaning: TEdit2
    Left = 66
    Top = 64
    Width = 518
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    OnChange = edMeaningClick
  end
  object cbFrequency: TComboBox
    Left = 118
    Top = 96
    Width = 116
    Height = 21
    Style = csDropDownList
    DropDownCount = 9
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ItemIndex = 8
    ParentFont = False
    TabOrder = 4
    Text = 'zero (red)'
    Items.Strings = (
      'overused (orange)'
      'favorite (orange)'
      'popular (orange)'
      'average (gray)'
      'mild (gray)'
      'weak (gray)'
      'unpopular (red)'
      'very rare (red)'
      'zero (red)')
  end
  object BitBtn1: TBitBtn
    Left = 504
    Top = 361
    Width = 84
    Height = 25
    Cancel = True
    Caption = 'Cancel'
    DoubleBuffered = True
    ModalResult = 2
    ParentDoubleBuffered = False
    TabOrder = 7
  end
  object edKanji: TZPaintBox
    Left = 96
    Top = 35
    Width = 193
    Height = 24
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'MS Gothic'
    Font.Style = []
    HorzScrollBar.Range = 5
    HorzScrollBar.Visible = False
    ParentFont = False
    TabOrder = 1
    VertScrollBar.Range = 5
    VertScrollBar.Visible = False
  end
  object cbDict: TComboBox
    Left = 86
    Top = 5
    Width = 145
    Height = 21
    Style = csDropDownList
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
  end
end
