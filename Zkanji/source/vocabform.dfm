object fAddVocab: TfAddVocab
  Left = 1181
  Top = 421
  HorzScrollBar.Visible = False
  VertScrollBar.Visible = False
  BorderIcons = [biSystemMenu]
  Caption = 'Add word to dictionary or group'
  ClientHeight = 321
  ClientWidth = 582
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    000001001800000000004003000000000000000000000000000000000000D4CC
    C8D5E6F1B36A36C77932D6E3ECA85D15C39175CBE4F3B86926CD803ED2E6F2B5
    9A91A25815B2AAA5D5DBDEF2F2F2AC640BE6BA85D5DADBA25B00DA9646AB6800
    CF7B00E7D4BBBCB4AC9D4B00DDBE94C69B6E9A3B00DFCBB1F0F5F9EDEDEDC7B2
    95B65F00DBC3A1C6985CB76500D5CCBCC1A178AB4800CFC2B2E0A352AC5100BE
    7700E4AA5BF2F3F5F0F2F4EFEFEFB1D8F3DD8F0CC07A00B6C4D0C67900DB8A00
    CD8000C27700F2E6D3F2F4F6F0E5D4F2F8FBF3F8FEF1F3F5EFEFEFF6F8F7F1D5
    ABD17F00D98500F4FAFFF2D8B0F2B14EF2E0C8F2F5F8F3F7FAF6F9FAF3F6F9F0
    F1F4F8FCFCF0F4F7AEB3B3DBD3D8F5A015F9AA32F4E8DBF3F8FDF3F7FCF9FCFF
    F7FCFFF6F9FBF7FAFED1DDE4B1BEC8ACB8C0C5A99BDDB3A3C6D8DE9D4C3BF4FA
    FFF7FBFFF7FBFEF6F7F7FAFDFFDEE2E7C6C8C8F8FAFDE2CFBCAB6A1ECB9367EA
    EEF1BA9B90861C00E3DCD6C68C5BF7F9FAFCFEFFCBD4DEB9BEC2F6E9D5CBBFB2
    9AB3CDC69D74A54F00C57C1AB76E00C07500EBEFF0CB8312C27700D88E1BF8F8
    F7F3CB8CC9B59B9FB4C5B36300B97000B3C2D2CC9F63C17500D2E3F0A2C0D8C0
    7800E29000B9B7B2AF5C00ECC790F8F6F4B95D00C47600B9BCBCE09E46CD7A00
    E08E00E9E8E1C36F00F6D193E3F6FFC8AF8DC26300CACCCCECB565C57215FDFF
    FFF5AF40D37F00EA9000E4A652ED980AD18000F9BA58F7E0BACB6D00F9B23CE1
    8900D07A00FCFCFDFBFDFEF9E2C1FDFFFFFAE7CCEB8E00F4CC93DA8100EC8E00
    E2DAD3DC8000FBCB85FAF4EBF9C477FACE8BFBF7F3FFFFFFFFFFFFFEFFFFFBFD
    FFFBFCFDE37D00F9D5A7FBDDB4E47D00FEF6EBFCFAF9FCF0DDFFFFFFFFFFFFFF
    FFFFFFFFFFE4E5E6DADCDCFFFFFFFCFCFCFDFFFFFFC26EFFD89DFFFFFFFFFFFE
    FEFFFFFEFFFFFFFFFFE7EDF0C2C9CBD7D8DAFCF9F9E0E6E99A989DA9575AFCFC
    FCFEFEFFFFFFFFFFFFFFFDFEFFEFF3F4F9FBFCFFFFFFDAD8DA968984D7DBE2C2
    CED97B2400DDA583D0DAE298480BFEFEFEFBFBFBDADADBF8F8F8FCFCFC9E9EA0
    C2C9CFEBC28DA66615AA6C15D18415F9F1E7D0B395AB6715E6BE8FC386290000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000}
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnCloseQuery = FormCloseQuery
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    582
    321)
  PixelsPerInch = 96
  TextHeight = 13
  object lbInfo: TLabel
    Left = 19
    Top = 146
    Width = 366
    Height = 27
    AutoSize = False
    Caption = 
      '*The word might have more than one definition in the dictionary.' +
      ' In that case only the selected meaning is added.'
    WordWrap = True
  end
  object Bevel2: TBevel
    Left = 38
    Top = 8
    Width = 2
    Height = 41
    Shape = bsRightLine
  end
  object Label1: TLabel
    Left = 277
    Top = 58
    Width = 64
    Height = 13
    Alignment = taRightJustify
    Caption = 'Select &group:'
    FocusControl = cbGroups
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object b0: TBevel
    Left = 0
    Top = 7
    Width = 99999
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label3: TLabel
    Left = 26
    Top = 85
    Width = 82
    Height = 13
    Alignment = taRightJustify
    Caption = '*Select &meaning:'
    FocusControl = cbMeaning
  end
  object b3: TBevel
    Left = 0
    Top = 108
    Width = 99999
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label4: TLabel
    Left = 0
    Top = 9
    Width = 40
    Height = 19
    Alignment = taCenter
    AutoSize = False
    Caption = 'Kanji:'
    Color = clWhite
    ParentColor = False
    Transparent = False
    Layout = tlCenter
  end
  object Label5: TLabel
    Left = 0
    Top = 30
    Width = 40
    Height = 19
    Alignment = taCenter
    AutoSize = False
    Caption = 'Kana:'
    Color = clWhite
    ParentColor = False
    Transparent = False
    Layout = tlCenter
  end
  object b1: TBevel
    Left = 0
    Top = 28
    Width = 99999
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label7: TLabel
    Left = 25
    Top = 58
    Width = 83
    Height = 13
    Alignment = taRightJustify
    Caption = 'Select &dictionary:'
    FocusControl = cbCollection
  end
  object b2: TBevel
    Left = 0
    Top = 49
    Width = 99999
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object lbWarning: TLabel
    Left = 214
    Top = 147
    Width = 188
    Height = 26
    AutoSize = False
    Caption = 'A new definition will be added to the word if you continue!'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
    WordWrap = True
  end
  object b4: TBevel
    Left = 0
    Top = 142
    Width = 99999
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object pbKanji: TPaintBox
    Left = 40
    Top = 9
    Width = 542
    Height = 19
    Anchors = [akLeft, akTop, akRight]
    Color = clWhite
    ParentColor = False
    OnPaint = pbKanjiPaint
  end
  object pbKana: TPaintBox
    Left = 40
    Top = 30
    Width = 542
    Height = 19
    Anchors = [akLeft, akTop, akRight]
    Color = clWhite
    ParentColor = False
    OnPaint = pbKanjiPaint
  end
  object b5: TBevel
    Left = 0
    Top = 177
    Width = 99999
    Height = 1
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object lbMeaning: TLabel
    Left = 25
    Top = 119
    Width = 83
    Height = 13
    Alignment = taRightJustify
    Caption = '&Custom meaning:'
    FocusControl = edMeaning
  end
  object cbGroups: TComboBox
    Left = 346
    Top = 55
    Width = 136
    Height = 21
    TabOrder = 1
    OnChange = cbGroupsChange
  end
  object edMeaning: TEdit2
    Left = 112
    Top = 115
    Width = 462
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 3
    OnChange = edMeaningExit
    OnExit = edMeaningExit
  end
  object btnOk: TButton
    Left = 422
    Top = 148
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'OK'
    Default = True
    Enabled = False
    ModalResult = 1
    TabOrder = 4
  end
  object btnCancel: TButton
    Left = 497
    Top = 148
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 5
  end
  object cbMeaning: TComboBox
    Left = 112
    Top = 82
    Width = 462
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 2
    OnChange = cbMeaningChange
  end
  object cbCollection: TComboBox
    Left = 112
    Top = 55
    Width = 136
    Height = 21
    TabOrder = 0
    OnChange = cbCollectionChange
  end
  object pbMeanings: TZPaintBox
    Top = 178
    Width = 582
    Height = 124
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    HorzScrollBar.SmallChange = 10
    HorzScrollBar.LargeChange = 60
    InvalidateOnScroll = True
    TabOrder = 7
    TabStop = False
    VertScrollBar.LargeChange = 40
    OnMouseDown = pbMeaningsMouseDown
    OnMouseUp = pbMeaningsMouseUp
  end
  object cbDefinition: TCheckBox
    Left = 8
    Top = 152
    Width = 170
    Height = 17
    Hint = 
      'The selected meaning from the list below will be added to the gr' +
      'oup, instead of a meaning copied from a different dictionary.'
    Caption = '&Add selected definition to group'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 6
    Visible = False
    OnClick = cbDefinitionClick
  end
  object sbInfo: TStatusBar
    Left = 0
    Top = 302
    Width = 582
    Height = 19
    Panels = <
      item
        Alignment = taRightJustify
        Bevel = pbNone
        Text = '0 :'
        Width = 30
      end
      item
        Bevel = pbNone
        Text = '0'
        Width = 25
      end
      item
        Bevel = pbNone
        Text = 'Groups: '
        Width = 45
      end
      item
        Bevel = pbNone
        Text = '-'
        Width = 9999
      end>
    SimpleText = 'Groups:'
    SizeGrip = False
  end
end
