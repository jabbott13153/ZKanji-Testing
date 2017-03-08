object fRadicals: TfRadicals
  Left = 852
  Top = 85
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Kanji radicals'
  ClientHeight = 558
  ClientWidth = 729
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000040030000000000000000000000000000000000000000
    00000000808080808080808080000000000000000000000000000000C0C0C0C0
    C0C0000000000000000000000000000000000000003DB9003DB9808080000000
    000000000000000000000000000000C0C0C00000000000000000000000000000
    000000008EABFF8EABFF8080800000000000000000000000000000003E3E3EC0
    C0C00000000000000000000000000000000000008E8EFF8EABFF808080C0C0C0
    000000000000000000000000565656C0C0C0C0C0C0C0C0C00000000000000000
    000000006B6BFF8EABFF8080803E3E3EC0C0C00000003E3E3E6E6E6E62626256
    56563E3E3EC0C0C00000000000000000000000006B6BFF8E8EFF808080000000
    565656C0C0C00000000000006E6E6EC0C0C00000000000003E3E3EC0C0C00000
    000000006B6BFF6B6BFF8080800000000000006E6E6EC0C0C00000006E6E6EC0
    C0C0000000565656C0C0C0000000003DB90000000000FF6B6BFF808080000000
    0000000000006E6E6EC0C0C06E6E6EC0C0C06E6E6EC0C0C0000000000000003D
    B9003DB90000FF6B6BFF8080800000000000000000000000006E6E6E6E6E6E6E
    6E6EC0C0C0000000000000000000000000003DB9003DB96B6BFF808080C0C0C0
    C0C0C0C0C0C0C0C0C0C0C0C06E6E6EC0C0C0C0C0C0C0C0C0C0C0C0C0C0C00000
    00000000003DB9003DB98080800000000000000000000000007A7A7A7A7A7A6E
    6E6E6E6E6E6E6E6E6E6E6E000000000000000000000000003DB9003DB9808080
    0000000000000000000000007A7A7A0000000000000000000000000000000000
    00000000000000000000003DB9003DB980808000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000003DB9
    8080800000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000C7CF
    0000C7CF0000C7CF0000C3C30000C1020000C4CC0000C6490000470300000787
    000080000000C0000000E3DF0000F1DF0000F9DF0000FFFF0000FFFF0000}
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnHide = FormHide
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object pbRad: TZPaintBox
    Top = 24
    Width = 729
    Height = 466
    Align = alClient
    BorderStyle = bsNone
    Font.Charset = SHIFTJIS_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    InvalidateOnScroll = True
    ParentFont = False
    TabOrder = 0
    OnPaint = pbRadPaint
    OnKeyDown = pbRadKeyDown
    OnKeyPress = pbRadKeyPress
    OnMouseDown = pbRadMouseDown
    OnMouseMove = pbRadMouseMove
    OnMouseLeave = pbRadMouseLeave
  end
  object Panel1: TPanel
    Left = 0
    Top = 520
    Width = 729
    Height = 38
    Align = alBottom
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 1
    DesignSize = (
      729
      38)
    object lbInfo: TLabel
      Left = 207
      Top = 1
      Width = 521
      Height = 37
      Anchors = [akLeft, akRight, akBottom]
      AutoSize = False
      Caption = 
        'Hold the CTRL or SHIFT key while selecting radical, to select mo' +
        're than one.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      Layout = tlCenter
      WordWrap = True
      ExplicitTop = 22
    end
    object Bevel1: TBevel
      Left = 153
      Top = 8
      Width = 9
      Height = 25
      Anchors = [akLeft, akBottom]
      Shape = bsLeftLine
      ExplicitTop = 30
    end
    object b1: TBevel
      Left = 0
      Top = 1
      Width = 99999
      Height = 2
      Anchors = [akLeft, akBottom]
      Shape = bsTopLine
      Style = bsRaised
    end
    object b0: TBevel
      Left = 0
      Top = 0
      Width = 99999
      Height = 1
      Anchors = [akLeft, akBottom]
      Shape = bsTopLine
      ExplicitTop = 21
    end
    object Bevel5: TBevel
      Left = 0
      Top = 0
      Width = 99999
      Height = 1
      Shape = bsTopLine
    end
    object btnCancel: TButton
      Left = 78
      Top = 7
      Width = 72
      Height = 25
      Anchors = [akLeft, akBottom]
      Cancel = True
      Caption = 'Cancel'
      ModalResult = 2
      TabOrder = 0
    end
    object btnClear: TButton
      Left = 157
      Top = 7
      Width = 43
      Height = 24
      Anchors = [akLeft, akBottom]
      Caption = 'Reset'
      TabOrder = 1
      OnClick = btnClearClick
    end
    object btnOk: TButton
      Left = 6
      Top = 7
      Width = 72
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'OK'
      Default = True
      ModalResult = 1
      TabOrder = 2
    end
  end
  object pKanji: TPanel
    Left = 0
    Top = 490
    Width = 729
    Height = 30
    Align = alBottom
    BevelOuter = bvNone
    TabOrder = 2
    Visible = False
    ExplicitTop = 389
    DesignSize = (
      729
      30)
    object Bevel4: TBevel
      Left = 0
      Top = 0
      Width = 729
      Height = 1
      Align = alTop
      Shape = bsBottomLine
      ExplicitWidth = 712
    end
    object Label1: TLabel
      Left = 136
      Top = 9
      Width = 31
      Height = 13
      Caption = 'Name:'
    end
    object btnGroup: TSpeedButton
      Left = 3
      Top = 4
      Width = 38
      Height = 22
      Hint = 'Group the same radicals in searches unregarding their position'
      AllowAllUp = True
      GroupIndex = 1
      Caption = 'Group'
      OnClick = btnGroupClick
    end
    object btnExact: TSpeedButton
      Left = 43
      Top = 4
      Width = 38
      Height = 22
      Hint = 'Show only results with exactly matching name'
      AllowAllUp = True
      GroupIndex = 2
      Caption = 'Exact'
      OnClick = btnGroupClick
    end
    object btnNames: TSpeedButton
      Left = 83
      Top = 4
      Width = 46
      Height = 22
      Hint = 'Show or hide radical names'
      AllowAllUp = True
      GroupIndex = 3
      Caption = 'Names'
      OnClick = btnNamesClick
    end
    object Label2: TLabel
      Left = 294
      Top = 9
      Width = 40
      Height = 13
      Caption = 'Strokes:'
    end
    object Bevel2: TBevel
      Left = 0
      Top = 1
      Width = 729
      Height = 1
      Align = alTop
      Shape = bsTopLine
      ExplicitLeft = -99270
      ExplicitTop = 20
      ExplicitWidth = 99999
    end
    object edKanji: TZPaintBox
      Left = 169
      Top = 4
      Width = 120
      Height = 24
      Hint = 'Enter radical name'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Gothic'
      Font.Style = []
      HorzScrollBar.Visible = False
      ParentFont = False
      TabOrder = 0
      VertScrollBar.Visible = False
      OnKeyPress = edKanjiKeyPress
    end
    object pbSelected: TZPaintBox
      Left = 436
      Top = 5
      Width = 287
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      BorderStyle = bsNone
      HorzScrollBar.Visible = False
      TabOrder = 1
      TabStop = False
      VertScrollBar.Visible = False
      OnPaint = pbSelectedPaint
    end
    object btnAdd: TButton
      Left = 379
      Top = 3
      Width = 53
      Height = 25
      Caption = 'Add'
      Enabled = False
      TabOrder = 2
      OnClick = btnAddClick
    end
    object edStrokes: TEdit2
      Left = 336
      Top = 6
      Width = 39
      Height = 21
      Hint = 'Enter number of strokes in radical. You can also leave it empty.'
      TabOrder = 3
      OnChange = edStrokesChange
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 0
    Width = 729
    Height = 24
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 3
    object tcSelect: TTabControl
      Left = -1
      Top = 0
      Width = 65535
      Height = 25
      MultiLine = True
      TabOrder = 0
      Tabs.Strings = (
        'Base'
        'Classical'
        'Classical 2')
      TabIndex = 0
      OnChange = tcSelectClick
    end
  end
end
