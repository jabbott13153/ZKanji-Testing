object fSelectElem: TfSelectElem
  Left = 308
  Top = 399
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Element Selector'
  ClientHeight = 512
  ClientWidth = 826
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000040030000000000000000000000000000000000000000
    000000000000009E9E9E00000000000000000000000000000000000000000000
    00000000000000000000000000000000000000009E9E9E0000009E9E9E000000
    0000000000000000000000000000000000000000000000000000000000000000
    00000000000000FFFFFF0000009E9E9E00000000000000000000000000000000
    00000000009E9E9E000000000000000000000000000000000000000000000000
    9E9E9E000000000000000000000000000000FFFFFF0000009E9E9E0000000000
    000000000000000000000000000000000000009E9E9E00000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000009E9E9E0000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000009E9E9E0000000000009E9E9E0000000000000000000000000000
    009E9E9E9E9E9E0000000000000000000000009E9E9E00000000000000000000
    00009E9E9E0000000000000000009E9E9EFAAB44FAAB449E9E9E0000005EE3FF
    5EE3FF0000000000000000000000000000000000009E9E9E9E9E9E9E9E9EFAAB
    44FFFFFFFEEEDAFAAB440000005EE3FF5EE3FF0000000000009E9E9E9E9E9E00
    0000000000000000000000000000FAAB44FBC072FEE6C7FAAB44000000000000
    0000000000009E9E9EFAAB44FAAB449E9E9E000000FFFFFFFFFFFF000000FAAB
    44FFFFFFFEEEDAFAAB44000000000000000000000000FAAB44FAAB44FFFFFFFA
    AB44000000FFFFFFFFFFFF000000000000FAAB44FAAB44000000000000000000
    000000000000FAAB44FAAB44FFFFFFFAAB440000000000000000000000000000
    00000000000000000000000000000000000000000000FAAB44FFFFFFFFFFFFFA
    AB44000000000000000000000000000000000000000000000000000000000000
    000000000000000000FAAB44FAAB44000000000000000000000000000000EFFF
    0000C7FF0000C3F30000E9E10000FCE70000FE2F0000FF9F0000FF0F00009867
    000000F0000000900000090000000F0000009F000000FF0F0000FF9F0000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  DesignSize = (
    826
    512)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 176
    Top = 25
    Width = 40
    Height = 13
    Caption = 'Pattern:'
  end
  object Label3: TLabel
    Left = 5
    Top = 4
    Width = 80
    Height = 13
    Caption = 'Main element:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
  end
  object Label2: TLabel
    Left = 5
    Top = 118
    Width = 82
    Height = 13
    Caption = 'Sub-elements:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
  end
  object b2: TBevel
    Left = 0
    Top = 113
    Width = 308
    Height = 2
    Shape = bsTopLine
  end
  object sbReset: TSpeedButton
    Left = 265
    Top = 1
    Width = 41
    Height = 17
    Caption = 'Reset'
    Flat = True
    OnClick = sbResetClick
  end
  object b3: TBevel
    Left = 0
    Top = 480
    Width = 9999
    Height = 2
    Shape = bsTopLine
  end
  object b1: TBevel
    Left = 0
    Top = 50
    Width = 308
    Height = 2
    Shape = bsTopLine
  end
  object cbStroke: TCheckBox
    Left = 21
    Top = 23
    Width = 112
    Height = 17
    Caption = 'Number of strokes:'
    TabOrder = 0
    OnClick = cbStrokeClick
  end
  object edStroke: TEdit2
    Left = 134
    Top = 22
    Width = 33
    Height = 21
    TabOrder = 1
    Text = '1'
  end
  object cbPattern: TComboBox
    Left = 221
    Top = 19
    Width = 43
    Height = 26
    AutoCloseUp = True
    Style = csOwnerDrawVariable
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ItemHeight = 20
    ItemIndex = 0
    ParentFont = False
    TabOrder = 2
    Text = '0'
    OnChange = cbPatternChange
    OnDrawItem = cbPattern1DrawItem
    OnMeasureItem = cbPattern1MeasureItem
    Items.Strings = (
      '0'
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10'
      '11'
      '12'
      '13'
      '14'
      '15'
      '16'
      '17'
      '18'
      '19'
      '20')
  end
  object p1: TPanel
    Left = 5
    Top = 136
    Width = 297
    Height = 81
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 3
    object Label7: TLabel
      Left = 200
      Top = 13
      Width = 40
      Height = 13
      Caption = 'Pattern:'
    end
    object Label8: TLabel
      Left = 8
      Top = 32
      Width = 21
      Height = 19
      Caption = '#1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label9: TLabel
      Left = 48
      Top = 30
      Width = 77
      Height = 13
      Caption = 'sub-patterns:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object cbStroke1: TCheckBox
      Left = 45
      Top = 11
      Width = 112
      Height = 17
      Caption = 'Number of strokes:'
      TabOrder = 0
      OnClick = cbStroke1Click
    end
    object edStroke1: TEdit2
      Left = 158
      Top = 10
      Width = 33
      Height = 21
      TabOrder = 1
      Text = '1'
    end
    object cbPattern1: TComboBox
      Left = 245
      Top = 7
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 2
      Text = '0'
      OnChange = cbPattern1Change
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS11: TComboBox
      Left = 45
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 3
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS12: TComboBox
      Left = 93
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 4
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS13: TComboBox
      Left = 141
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 5
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS14: TComboBox
      Left = 189
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 6
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cb1: TCheckBox
      Left = 5
      Top = 4
      Width = 17
      Height = 17
      TabOrder = 7
      OnClick = cb1Click
    end
  end
  object p2: TPanel
    Tag = 1
    Left = 6
    Top = 222
    Width = 297
    Height = 81
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 4
    object Label4: TLabel
      Left = 200
      Top = 13
      Width = 40
      Height = 13
      Caption = 'Pattern:'
    end
    object Label5: TLabel
      Left = 8
      Top = 32
      Width = 21
      Height = 19
      Caption = '#2'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label6: TLabel
      Left = 48
      Top = 30
      Width = 77
      Height = 13
      Caption = 'sub-patterns:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object cbStroke2: TCheckBox
      Left = 45
      Top = 11
      Width = 112
      Height = 17
      Caption = 'Number of strokes:'
      TabOrder = 0
      OnClick = cbStroke1Click
    end
    object edStroke2: TEdit2
      Left = 158
      Top = 10
      Width = 33
      Height = 21
      TabOrder = 1
      Text = '1'
    end
    object cbPattern2: TComboBox
      Left = 245
      Top = 7
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 2
      Text = '0'
      OnChange = cbPattern1Change
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS21: TComboBox
      Left = 45
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 3
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS22: TComboBox
      Left = 93
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 4
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS23: TComboBox
      Left = 141
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 5
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS24: TComboBox
      Left = 189
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 6
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cb2: TCheckBox
      Left = 5
      Top = 4
      Width = 17
      Height = 17
      TabOrder = 7
      OnClick = cb1Click
    end
  end
  object p3: TPanel
    Tag = 2
    Left = 6
    Top = 308
    Width = 297
    Height = 81
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 5
    object Label10: TLabel
      Left = 200
      Top = 13
      Width = 40
      Height = 13
      Caption = 'Pattern:'
    end
    object Label11: TLabel
      Left = 8
      Top = 32
      Width = 21
      Height = 19
      Caption = '#3'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label12: TLabel
      Left = 48
      Top = 30
      Width = 77
      Height = 13
      Caption = 'sub-patterns:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object cbStroke3: TCheckBox
      Left = 45
      Top = 11
      Width = 112
      Height = 17
      Caption = 'Number of strokes:'
      TabOrder = 0
      OnClick = cbStroke1Click
    end
    object edStroke3: TEdit2
      Left = 158
      Top = 10
      Width = 33
      Height = 21
      TabOrder = 1
      Text = '1'
    end
    object cbPattern3: TComboBox
      Left = 245
      Top = 7
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 2
      Text = '0'
      OnChange = cbPattern1Change
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS32: TComboBox
      Left = 93
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 3
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS33: TComboBox
      Left = 141
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 4
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS34: TComboBox
      Left = 189
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 5
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cb3: TCheckBox
      Left = 5
      Top = 4
      Width = 17
      Height = 17
      TabOrder = 6
      OnClick = cb1Click
    end
    object cbS31: TComboBox
      Left = 45
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 7
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
  end
  object p4: TPanel
    Tag = 3
    Left = 6
    Top = 394
    Width = 297
    Height = 81
    BevelInner = bvRaised
    BevelOuter = bvLowered
    TabOrder = 6
    object Label13: TLabel
      Left = 200
      Top = 13
      Width = 40
      Height = 13
      Caption = 'Pattern:'
    end
    object Label14: TLabel
      Left = 8
      Top = 32
      Width = 21
      Height = 19
      Caption = '#4'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label15: TLabel
      Left = 48
      Top = 30
      Width = 77
      Height = 13
      Caption = 'sub-patterns:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object cbStroke4: TCheckBox
      Left = 45
      Top = 11
      Width = 112
      Height = 17
      Caption = 'Number of strokes:'
      TabOrder = 0
      OnClick = cbStroke1Click
    end
    object edStroke4: TEdit2
      Left = 158
      Top = 10
      Width = 33
      Height = 21
      TabOrder = 1
      Text = '1'
    end
    object cbPattern4: TComboBox
      Left = 245
      Top = 7
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 2
      Text = '0'
      OnChange = cbPattern1Change
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS41: TComboBox
      Left = 45
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 3
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS42: TComboBox
      Left = 93
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 4
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS43: TComboBox
      Left = 141
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 5
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cbS44: TComboBox
      Left = 189
      Top = 47
      Width = 43
      Height = 26
      AutoCloseUp = True
      Style = csOwnerDrawVariable
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ItemHeight = 20
      ItemIndex = 0
      ParentFont = False
      TabOrder = 6
      Text = '0'
      OnChange = cbStroke1Click
      OnDrawItem = cbPattern1DrawItem
      OnMeasureItem = cbPattern1MeasureItem
      Items.Strings = (
        '0'
        '1'
        '2'
        '3'
        '4'
        '5'
        '6'
        '7'
        '8'
        '9'
        '10'
        '11'
        '12'
        '13'
        '14'
        '15'
        '16'
        '17'
        '18'
        '19')
    end
    object cb4: TCheckBox
      Left = 5
      Top = 4
      Width = 17
      Height = 17
      TabOrder = 7
      OnClick = cb1Click
    end
  end
  object pbGrid: TZPaintBox
    Left = 309
    Width = 517
    Height = 480
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    TabOrder = 7
    VertScrollBar.Range = 10000
    OnMouseDown = pbGridMouseDown
  end
  object btnOk: TBitBtn
    Left = 363
    Top = 483
    Width = 101
    Height = 28
    Anchors = [akLeft, akBottom]
    Caption = 'OK'
    Default = True
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    ModalResult = 1
    NumGlyphs = 2
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 8
    OnClick = btnOkClick
  end
  object BitBtn2: TBitBtn
    Left = 757
    Top = 484
    Width = 66
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 9
    OnClick = BitBtn2Click
  end
  object edMeaning: TEdit2
    Left = 72
    Top = 59
    Width = 89
    Height = 21
    TabOrder = 10
    OnChange = edMeaningChange
  end
  object edReading: TZPaintBox
    Left = 72
    Top = 84
    Width = 73
    Height = 23
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'MS Gothic'
    Font.Style = []
    HorzScrollBar.Visible = False
    ParentFont = False
    TabOrder = 11
    VertScrollBar.Visible = False
  end
  object edIndex: TEdit2
    Left = 253
    Top = 59
    Width = 49
    Height = 21
    TabOrder = 12
  end
  object cbOn: TCheckBox
    Left = 166
    Top = 88
    Width = 34
    Height = 15
    Caption = 'ON'
    Checked = True
    State = cbChecked
    TabOrder = 13
  end
  object cbKun: TCheckBox
    Left = 203
    Top = 88
    Width = 36
    Height = 15
    Caption = 'kun'
    Checked = True
    State = cbChecked
    TabOrder = 14
  end
  object cbOku: TCheckBox
    Left = 241
    Top = 88
    Width = 39
    Height = 15
    Caption = 'oku.'
    Checked = True
    State = cbChecked
    TabOrder = 15
  end
  object cbMeaning: TCheckBox
    Left = 6
    Top = 60
    Width = 64
    Height = 17
    Caption = '&Meaning:'
    TabOrder = 16
    OnClick = cbMeaningClick
  end
  object cbReading: TCheckBox
    Left = 6
    Top = 87
    Width = 62
    Height = 17
    Caption = '&Reading:'
    TabOrder = 17
    OnClick = cbReadingClick
  end
  object cbIndex: TCheckBox
    Left = 166
    Top = 60
    Width = 85
    Height = 17
    Caption = '&Parent index:'
    TabOrder = 18
    OnClick = cbIndexClick
  end
end
