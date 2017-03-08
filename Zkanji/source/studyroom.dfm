object fWStudy: TfWStudy
  Left = 860
  Top = 365
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Study vocabulary'
  ClientHeight = 461
  ClientWidth = 608
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnKeyDown = FormKeyDown
  OnKeyPress = FormKeyPress
  OnPaint = FormPaint
  OnShow = FormShow
  DesignSize = (
    608
    461)
  PixelsPerInch = 96
  TextHeight = 13
  object bChoices: TBevel
    Left = 80
    Top = 232
    Width = 498
    Height = 152
  end
  object lbChoices: TLabel
    Left = 7
    Top = 242
    Width = 66
    Height = 44
    Caption = 'Click the answer!'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Trebuchet MS'
    Font.Style = [fsBold]
    ParentFont = False
    WordWrap = True
  end
  object lbTitleMeaning: TLabel
    Left = 5
    Top = 31
    Width = 77
    Height = 18
    Caption = 'Meaning:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object pbKanji: TPaintBox
    Left = 0
    Top = 117
    Width = 599
    Height = 48
    OnPaint = pbKanjiPaint
  end
  object pbKana: TPaintBox
    Left = 0
    Top = 186
    Width = 599
    Height = 31
    OnPaint = pbKanaPaint
  end
  object lbTitleKanji: TLabel
    Left = 5
    Top = 95
    Width = 50
    Height = 18
    Caption = 'Kanji:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbTitleKana: TLabel
    Left = 5
    Top = 166
    Width = 49
    Height = 18
    Caption = 'Kana:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object b7: TBevel
    Left = 0
    Top = 387
    Width = 99999
    Height = 4
    Shape = bsTopLine
  end
  object b8: TBevel
    Left = 0
    Top = 227
    Width = 99999
    Height = 2
    Shape = bsTopLine
  end
  object pbMeaning: TPaintBox
    Left = 0
    Top = 50
    Width = 599
    Height = 42
    OnPaint = pbMeaningPaint
  end
  object b9: TBevel
    Left = 0
    Top = 288
    Width = 99999
    Height = 2
    Shape = bsTopLine
  end
  object lbInstruction: TLabel
    Left = 46
    Top = 5
    Width = 516
    Height = 16
    Anchors = [akTop]
    Caption = 
      'Try to tell the missing meaning, kanji or kana reading of a word' +
      ' from the given information!'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    ExplicitLeft = 48
  end
  object b11: TBevel
    Left = 0
    Top = 420
    Width = 99999
    Height = 2
    Shape = bsTopLine
  end
  object pbInfo: TPaintBox
    Left = 0
    Top = 422
    Width = 600
    Height = 31
    OnPaint = pbInfoPaint
  end
  object btnSuspend: TSpeedButton
    Left = 512
    Top = 392
    Width = 81
    Height = 25
    Caption = 'Suspend'
    Glyph.Data = {
      42040000424D4204000000000000420000002800000020000000100000000100
      10000300000000040000120B0000120B00000000000000000000007C0000E003
      00001F00000038539452EF3DCE394A2929254A294A294A296B2D4A298C318C31
      8C31314694523853385338533853385338533853385338533853385338533853
      385338533853426842684264426042584258214C214C214C2148214821482148
      2148425838535A6B5A6B7B6F7B6FBD77BD77FF7FFF7FFF7FFF7FFF7FFF7FFF7F
      FF7FBD773853427C427C427C427C427C427C427C427C427C427C427C427C427C
      427C427C38539452945294529452945294529452945294529452945294529452
      945294523853385338533853427C4278292538533853427C427C8C3138533853
      3853385338533853385338539452B55638533853385394529452385338533853
      385338533853385338533853427C426C4A2938533853427C42708C3138533853
      3853385338533853385338539452396738533853385394521863385338533853
      385338533853385338533853427C425C6B2D38533853427C4260AD3538533853
      38533853385338533853385394529C7338533853385394527B6F385338533853
      385338533853385338533853427C42546B2D38533853427C425CAD3538533853
      3853385338533853385338539452BD7738533853385394529C73385338533853
      385338533853385338533853427C42508C3138533853427C4258AD3538533853
      3853385338533853385338539452DE7B3853385338539452BD77385338533853
      385338533853385338533853427C42548C3138533853427C42584A29EF3D3146
      734E945238533853385338539452BD773853385338539452BD77385338533853
      385338533853385338533853427C4254CE3938533853427C4254425442544258
      4264945238533853385338539452BD773853385338539452BD77BD77BD77BD77
      7B6F38533853385338533853427C2150524A38533853427C427C427C427C427C
      427C385338533853385338539452DE7B38533853385394529452945294529452
      945238533853385338533853427C4260945238533853427C426C082138533853
      38533853385338533853385394527B6F38533853385394523967385338533853
      385338533853385338533853427C4260385338533853427C42644A2938533853
      38533853385338533853385394527B6F38533853385394527B6F385338533853
      38533853385338533853385338533853385338533853427C4260CE3938533853
      3853385338533853385338533853385338533853385394527B6F385338533853
      38533853385338533853385338533853385338533853427C425CCE3938533853
      3853385338533853385338533853385338533853385394529C73385338533853
      38533853385338533853385338533853385338533853427C425C385338533853
      3853385338533853385338533853385338533853385394529C73385338533853
      385338533853}
    NumGlyphs = 2
    OnClick = btnSuspendClick
  end
  object btnSolve: TSpeedButton
    Left = 248
    Top = 392
    Width = 121
    Height = 25
    Caption = 'Reveal the answer'
    OnClick = btnSolveClick
  end
  object btnMShow: TSpeedButton
    Left = 59
    Top = 30
    Width = 102
    Height = 21
    Caption = 'Show &meaning'
    Enabled = False
    Visible = False
    OnClick = btnMShowClick
  end
  object b1: TBevel
    Left = 0
    Top = 27
    Width = 99999
    Height = 2
    Shape = bsTopLine
  end
  object b12: TBevel
    Left = 0
    Top = 453
    Width = 99999
    Height = 2
    Shape = bsTopLine
  end
  object Bevel1: TBevel
    Left = 593
    Top = 0
    Width = 4
    Height = 461
    Align = alRight
    Shape = bsRightLine
    ExplicitLeft = 537
    ExplicitHeight = 417
  end
  object pKanji: TPanel
    Left = 33
    Top = 389
    Width = 437
    Height = 31
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 4
    Visible = False
    object Label2: TLabel
      Left = 5
      Top = 8
      Width = 93
      Height = 13
      Caption = 'Enter your answer:'
    end
    object btnAccept2: TSpeedButton
      Left = 376
      Top = 3
      Width = 59
      Height = 25
      Caption = 'Accept'
      OnClick = btnAcceptClick
    end
    object sbKanji: TSpeedButton
      Left = 111
      Top = 3
      Width = 26
      Height = 25
      AllowAllUp = True
      GroupIndex = 99
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
    end
    object edKanji: TZPaintBox
      Left = 139
      Top = 3
      Width = 235
      Height = 25
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Gothic'
      Font.Style = []
      HorzScrollBar.Visible = False
      ParentFont = False
      TabOrder = 0
      VertScrollBar.Visible = False
    end
  end
  object pKana: TPanel
    Left = 33
    Top = 389
    Width = 437
    Height = 31
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 3
    Visible = False
    object Label1: TLabel
      Left = 5
      Top = 8
      Width = 93
      Height = 13
      Caption = 'Enter your answer:'
    end
    object btnAccept: TSpeedButton
      Left = 376
      Top = 3
      Width = 59
      Height = 25
      Caption = 'Accept'
      OnClick = btnAcceptClick
    end
    object edKana: TZPaintBox
      Left = 112
      Top = 3
      Width = 262
      Height = 25
      Font.Charset = SHIFTJIS_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Gothic'
      Font.Style = []
      HorzScrollBar.Visible = False
      ParentFont = False
      TabOrder = 0
      VertScrollBar.Visible = False
    end
  end
  object pButtons: TPanel
    Left = 131
    Top = 389
    Width = 353
    Height = 31
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 0
    Visible = False
    object btnWrong: TSpeedButton
      Tag = 1
      Left = 8
      Top = 3
      Width = 105
      Height = 25
      Caption = '&Wrong answer'
      OnClick = btnCorrectClick
    end
    object btnCorrect: TSpeedButton
      Left = 240
      Top = 3
      Width = 105
      Height = 25
      Caption = '&Correct answer'
      OnClick = btnCorrectClick
    end
  end
  object pbChoices: TZPaintBox
    Tag = -1
    Left = 81
    Top = 233
    Width = 496
    Height = 150
    BorderStyle = bsNone
    HorzScrollBar.Visible = False
    ParentColor = True
    TabOrder = 1
    VertScrollBar.Visible = False
    OnPaint = pbChoicesPaint
    OnMouseDown = pbChoicesMouseDown
    OnMouseMove = pbChoicesMouseMove
    OnMouseLeave = pbChoicesMouseLeave
  end
  object pbTimer: TZPaintBox
    Left = 597
    Width = 11
    Height = 461
    Align = alRight
    BorderStyle = bsNone
    Color = clScrollBar
    HorzScrollBar.Visible = False
    TabOrder = 2
    VertScrollBar.Visible = False
    OnPaint = pbTimerPaint
  end
  object tTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = tTimerTimer
    Left = 568
    Top = 23
  end
end
