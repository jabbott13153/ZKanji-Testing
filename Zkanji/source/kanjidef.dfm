object fDef: TfDef
  Left = 1001
  Top = 558
  ActiveControl = edMeaning
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Kanji definition'
  ClientHeight = 345
  ClientWidth = 669
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000040030000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000A4A0A0A4A0A0000000A4A0A0A4A0A0A4A0A0
    A4A0A0A4A0A0000000A4A0A0A4A0A0A4A0A0A4A0A0A4A0A0000000A4A0A00000
    00000000A4A0A0000000000000000000000000000000000000000000000000A4
    A0A0000000000000A4A0A0000000000000000000A4A0A0000000A4A0A0000000
    808080808080000000000000A4A0A0000000A4A0A0000000A4A0A00000000000
    00000000A4A0A0000000808080C0C0C0F2F2F2C0C0C07A7A7AA4A0A000000000
    0000A4A0A0000000A4A0A0000000000000000000A4A0A0808080F2F2F2FFFFFF
    FFFFFFF2F2F2C0C0C07A7A7A000000A4A0A0A4A0A0000000A4A0A00000000000
    00000000A4A0A0C0C0C0FFFFFFFFFFFFFFFFFFFFFFFFC0C0C0C0C0C07A7A7A00
    0000000000000000A4A0A0000000000000000000000000C0C0C0FFFFFFFFFFFF
    FFFFFFFFFFFFDADADA4848FF7A7A7A0000000000000000000000000000000000
    00000000000000000000C0C0C0FFFFFFFFFFFFDADADAFF8F6BFF8F6B4848FF7A
    7A7A000000000000000000000000000000000000000000000000C0C0C0DADADA
    FF8F6BFF8F6BFFC7B1FF8F6B4848FF7A7A7A0000000000000000000000000000
    00000000000000000000000000FF8F6BFFC7B1FFC7B1FFC7B1FFC7B1FF8F6B48
    48FF7A7A7A0000000000000000000000000000000000000000000000007A7A7A
    FFC7B1FFC7B1FFC7B1FFC7B1FFC7B14848FF7A7A7A0000000000000000000000
    00000000000000000000000000000000FF8F6BFFC7B1FFC7B1FFC7B1FF8F6BFF
    8F6BA4A0A0000000000000000000000000000000000000000000000000000000
    7A7A7AFFC7B1FF8F6BFF8F6BFFFFFFFFFFFFA4A0A00000000000000000000000
    00000000000000000000000000000000000000FF8F6BFFFFFFFFFFFFA4A0A0A4
    A0A0000000000000000000000000000000000000000000000000000000000000
    000000000000A4A0A0A4A0A0000000000000000000000000000000000000FFFF
    0000208200000380000084900000903000008020000000000000E01E0000F00E
    0000F00E0000F8070000F8070000FC070000FC070000FE0F0000FF3F0000}
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    669
    345)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 115
    Top = 68
    Width = 189
    Height = 13
    Caption = 'Enter new definition for this kanji:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 115
    Top = 7
    Width = 192
    Height = 13
    Caption = 'Original definition from dictionary:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object bvleft: TBevel
    Left = 110
    Top = 0
    Width = 2
    Height = 112
    Shape = bsLeftLine
  end
  object bv6: TBevel
    Left = 0
    Top = 315
    Width = 99990
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object lbOrig: TLabel
    Left = 116
    Top = 30
    Width = 548
    Height = 33
    AutoSize = False
    Caption = 'lbOrig'
    Color = clWhite
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentColor = False
    ParentFont = False
    Transparent = False
    WordWrap = True
  end
  object bv1: TBevel
    Left = 111
    Top = 28
    Width = 9999
    Height = 2
    Shape = bsTopLine
  end
  object bv2: TBevel
    Left = 111
    Top = 63
    Width = 9999
    Height = 2
    Shape = bsTopLine
  end
  object bv3: TBevel
    Left = 111
    Top = 84
    Width = 9999
    Height = 2
    Shape = bsTopLine
  end
  object bv4: TBevel
    Left = 111
    Top = 104
    Width = 9999
    Height = 2
    Shape = bsTopLine
  end
  object bv5: TBevel
    Left = 0
    Top = 110
    Width = 9999
    Height = 1
    Shape = bsTopLine
  end
  object bv7: TBevel
    Left = 0
    Top = 317
    Width = 99990
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object pUC: TPanel
    Left = 408
    Top = 3
    Width = 105
    Height = 22
    Alignment = taLeftJustify
    BevelInner = bvRaised
    BevelOuter = bvLowered
    Caption = ' Unicode:'
    Color = 15792890
    ParentBackground = False
    TabOrder = 7
    object lbUC: TLabel
      Left = 56
      Top = 4
      Width = 41
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'lbUC'
    end
  end
  object edMeaning: TEdit2
    Left = 114
    Top = 87
    Width = 551
    Height = 16
    Anchors = [akLeft, akTop, akRight]
    AutoSize = False
    BorderStyle = bsNone
    TabOrder = 1
    Text = 'edMeaning'
    OnChange = edMeaningChange
    OnKeyDown = edMeaningKeyDown
  end
  object btnOk: TButton
    Left = 521
    Top = 320
    Width = 72
    Height = 23
    Anchors = [akRight, akBottom]
    Caption = 'Ok'
    TabOrder = 5
    OnClick = btnOkClick
  end
  object btnCancel: TBitBtn
    Left = 594
    Top = 320
    Width = 72
    Height = 23
    Anchors = [akRight, akBottom]
    Caption = 'Close'
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
    ParentDoubleBuffered = False
    TabOrder = 6
    OnClick = btnCancelClick
  end
  object btnNext: TButton
    Left = 120
    Top = 320
    Width = 101
    Height = 23
    Anchors = [akLeft, akBottom]
    Caption = 'Next >>'
    Default = True
    TabOrder = 3
    OnClick = btnNextClick
  end
  object cbSkip: TCheckBox
    Left = 229
    Top = 323
    Width = 81
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = 'Skip defined'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object pbWords: TZPaintBox
    Top = 111
    Width = 669
    Height = 185
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    InvalidateOnScroll = True
    TabOrder = 2
    OnKeyDown = pbWordsKeyDown
    OnKeyPress = pbWordsKeyPress
    OnEnter = pbWordsEnter
    OnExit = pbWordsExit
    OnMouseDown = pbWordsMouseDown
    OnMouseUp = pbWordsMouseUp
  end
  object cbDictionary: TComboBox
    Left = 312
    Top = 3
    Width = 93
    Height = 21
    Style = csDropDownList
    TabOrder = 0
    OnChange = cbDictionaryChange
  end
  object sbInfo: TStatusBar
    AlignWithMargins = True
    Left = 0
    Top = 296
    Width = 669
    Height = 19
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 30
    Panels = <
      item
        Alignment = taRightJustify
        Bevel = pbNone
        Text = 'Kanji in list:'
        Width = 65
      end
      item
        Alignment = taRightJustify
        Bevel = pbNone
        Width = 40
      end
      item
        Bevel = pbNone
        Text = ':'
        Width = 7
      end
      item
        Bevel = pbNone
        Width = 55
      end
      item
        Bevel = pbNone
        Text = 'Defined:'
        Width = 45
      end
      item
        Bevel = pbNone
        Width = 50
      end>
  end
  object btnBack: TButton
    Left = 3
    Top = 320
    Width = 113
    Height = 23
    Anchors = [akLeft, akBottom]
    Caption = '<< Back (no saving)'
    Enabled = False
    TabOrder = 9
    OnClick = btnBackClick
  end
  object pbKanji: TZPaintBox
    Width = 110
    Height = 110
    BorderStyle = bsNone
    HorzScrollBar.Visible = False
    TabOrder = 10
    VertScrollBar.Visible = False
  end
end
