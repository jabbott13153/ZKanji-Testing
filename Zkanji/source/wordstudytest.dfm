object fTest: TfTest
  Left = 1008
  Top = 463
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Testing in progress...'
  ClientHeight = 338
  ClientWidth = 740
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  OnKeyDown = FormKeyDown
  OnKeyPress = FormKeyPress
  OnMouseLeave = FormMouseLeave
  OnPaint = FormPaint
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object lbKanji: TLabel
    Left = 16
    Top = 16
    Width = 93
    Height = 25
    Caption = 'Written:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -21
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbKana: TLabel
    Left = 16
    Top = 64
    Width = 66
    Height = 25
    Caption = 'Kana:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -21
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbMeaning: TLabel
    Left = 16
    Top = 120
    Width = 104
    Height = 25
    Caption = 'Meaning:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -21
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object bv: TBevel
    Left = 128
    Top = 14
    Width = 1
    Height = 185
    Shape = bsLeftLine
  end
  object lbKanjiQ: TLabel
    Left = 136
    Top = 10
    Width = 15
    Height = 31
    Caption = '?'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -27
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbKanaQ: TLabel
    Left = 136
    Top = 58
    Width = 15
    Height = 31
    Caption = '?'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -27
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbMeaningQ: TLabel
    Left = 136
    Top = 114
    Width = 15
    Height = 31
    Caption = '?'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -27
    Font.Name = 'Georgia'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object lbNew: TLabel
    Left = 658
    Top = 47
    Width = 54
    Height = 29
    Caption = 'New'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 11434831
    Font.Height = -24
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object lbFailed: TLabel
    Left = 654
    Top = 47
    Width = 65
    Height = 29
    Caption = 'Retry'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 8400
    Font.Height = -24
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Visible = False
  end
  object pSuspended: TPanel
    Left = 0
    Top = 0
    Width = 740
    Height = 210
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 8
    Visible = False
    object Label2: TLabel
      Left = 56
      Top = 40
      Width = 261
      Height = 25
      Caption = 'The test was suspended.'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -21
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Bevel1: TBevel
      Left = 0
      Top = 69
      Width = 99999
      Height = 2
      Shape = bsTopLine
    end
    object Bevel2: TBevel
      Left = 0
      Top = 238
      Width = 99999
      Height = 2
      Shape = bsTopLine
    end
    object btnResumeSuspend: TBitBtn
      Left = 149
      Top = 112
      Width = 442
      Height = 33
      Cancel = True
      Caption = '1. Continue testing'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      Glyph.Data = {
        22020000424D220200000000000042000000280000000F0000000F0000000100
        100003000000E0010000120B0000120B00000000000000000000007C0000E003
        00001F000000FF7FFF7FFF7F00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000000000000000FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000000000000000FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000000000000000FF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000000000000000FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000000000000000FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000}
      Margin = 5
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      OnClick = btnResumeSuspendClick
    end
    object btnQuitSuspend: TBitBtn
      Left = 149
      Top = 248
      Width = 442
      Height = 33
      Caption = '3. Quit the test'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
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
      Margin = 5
      ModalResult = 2
      NumGlyphs = 2
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 1
    end
  end
  object pLimit: TPanel
    Left = 0
    Top = 0
    Width = 740
    Height = 210
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 5
    Visible = False
    object Label1: TLabel
      Left = 56
      Top = 40
      Width = 529
      Height = 25
      Caption = 'You have reached the set limit. Would you like to:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -21
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object bv1: TBevel
      Left = 0
      Top = 69
      Width = 99999
      Height = 2
      Shape = bsTopLine
    end
    object bv2: TBevel
      Left = 0
      Top = 238
      Width = 99999
      Height = 2
      Shape = bsTopLine
    end
    object btnLimit1: TBitBtn
      Left = 149
      Top = 104
      Width = 442
      Height = 33
      Caption = '1. Continue testing until the limit is reached again?'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      Glyph.Data = {
        22020000424D220200000000000042000000280000000F0000000F0000000100
        100003000000E0010000120B0000120B00000000000000000000007C0000E003
        00001F000000FF7FFF7FFF7F00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000000000000000FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000000000000000FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000000000000000FF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000000000000000FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000000000000000FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000}
      Margin = 5
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 0
      OnClick = btnLimit1Click
    end
    object btnLimit2: TBitBtn
      Left = 149
      Top = 160
      Width = 442
      Height = 33
      Caption = '2. Continue testing, but don'#39't stop this time?'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      Glyph.Data = {
        22020000424D220200000000000042000000280000000F0000000F0000000100
        100003000000E0010000120B0000120B00000000000000000000007C0000E003
        00001F000000FF7FFF7FFF7F00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000000000000000FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000000000000000FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000000000000000FF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000000000000000FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000000000000000FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000000000000000FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000000000000000FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F0000000000000000FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F000000000000FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7F00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000}
      Margin = 5
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 1
      OnClick = btnLimit2Click
    end
    object btnLimit3: TBitBtn
      Left = 149
      Top = 248
      Width = 442
      Height = 33
      Caption = '3. Quit the test?'
      DoubleBuffered = True
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
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
      Margin = 5
      ModalResult = 2
      NumGlyphs = 2
      ParentDoubleBuffered = False
      ParentFont = False
      TabOrder = 2
    end
  end
  object pInput1: TPanel
    Left = 0
    Top = 210
    Width = 740
    Height = 109
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 4
    object btnShowAnswer: TButton
      Left = 33
      Top = 11
      Width = 211
      Height = 43
      Caption = 'Show the answer'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = btnShowAnswerClick
    end
  end
  object pInput2: TPanel
    Left = 0
    Top = 210
    Width = 740
    Height = 109
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 3
    object Label8: TLabel
      Left = 16
      Top = 8
      Width = 194
      Height = 25
      Caption = 'Enter the answer:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -21
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label9: TLabel
      Left = 552
      Top = 80
      Width = 169
      Height = 18
      Caption = '*or press Escape to skip'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Georgia'
      Font.Style = []
      ParentFont = False
    end
    object edRomaji: TEdit2
      Left = 42
      Top = 48
      Width = 607
      Height = 21
      TabOrder = 0
      OnKeyDown = edRomajiKeyDown
      OnKeyPress = edRomajiKeyPress
    end
    object btnRomajiAccept: TButton
      Left = 656
      Top = 46
      Width = 75
      Height = 25
      Caption = 'Accept'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = btnRomajiAcceptClick
    end
  end
  object pInput3: TPanel
    Left = 0
    Top = 210
    Width = 740
    Height = 109
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 2
    object Label6: TLabel
      Left = 16
      Top = 8
      Width = 194
      Height = 25
      Caption = 'Enter the answer:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -21
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object sbRecognize: TSpeedButton
      Left = 13
      Top = 45
      Width = 27
      Height = 27
      AllowAllUp = True
      GroupIndex = 1
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
      NumGlyphs = 2
      OnClick = sbRecognizeClick
    end
    object Label10: TLabel
      Left = 552
      Top = 72
      Width = 169
      Height = 18
      Caption = '*or press Escape to skip'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'Georgia'
      Font.Style = []
      ParentFont = False
    end
    object pbKanji: TZPaintBox
      Left = 42
      Top = 47
      Width = 607
      Height = 23
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Gothic'
      Font.Style = []
      HorzScrollBar.Visible = False
      ParentFont = False
      TabOrder = 0
      VertScrollBar.Visible = False
      OnKeyDown = pbKanjiKeyDown
      OnKeyPress = pbKanjiKeyPress
    end
    object btnKanjiAccept: TButton
      Left = 656
      Top = 46
      Width = 75
      Height = 25
      Caption = 'Accept'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = btnKanjiAcceptClick
    end
  end
  object sbInfo: TStatusBar
    Left = 0
    Top = 319
    Width = 740
    Height = 19
    Panels = <
      item
        Bevel = pbNone
        Text = 'Due:'
        Width = 34
      end
      item
        Bevel = pbNone
        Width = 60
      end
      item
        Bevel = pbNone
        Text = 'Tested:'
        Width = 48
      end
      item
        Bevel = pbNone
        Width = 60
      end
      item
        Bevel = pbNone
        Text = 'ETA:'
        Width = 34
      end
      item
        Bevel = pbNone
        Text = '00:00:00'
        Width = 50
      end
      item
        Bevel = pbNone
        Text = 'Time passed:'
        Width = 88
      end
      item
        Bevel = pbNone
        Text = '00:00:00'
        Width = 50
      end>
  end
  object pHint: TPanel
    Left = 608
    Top = 216
    Width = 129
    Height = 30
    BevelOuter = bvNone
    TabOrder = 6
    object sbHint: TSpeedButton
      Left = 4
      Top = 4
      Width = 121
      Height = 23
      Caption = 'Show hint [Ctrl+H]'
      OnClick = sbHintClick
    end
  end
  object pOptions: TPanel
    Left = 651
    Top = 50
    Width = 75
    Height = 26
    BevelOuter = bvNone
    TabOrder = 0
    Visible = False
    object btnOptions: TBitBtn
      Left = 0
      Top = -1
      Width = 72
      Height = 25
      Caption = 'Options'
      DoubleBuffered = True
      Glyph.Data = {
        22020000424D220200000000000042000000280000000F0000000F0000000100
        100003000000E0010000C30E0000C30E00000000000000000000007C0000E003
        00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FA514FF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7F6B2D00006B2DFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FA5140000A514FF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7F29250000000000006B2DFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FA514000000000000A514FF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7F6B2D000000000000000000006B2DFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FCE39CE39CE39CE39CE39CE39CE39FF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0000}
      Layout = blGlyphRight
      Margin = 2
      ParentDoubleBuffered = False
      TabOrder = 0
      OnClick = btnOptionsClick
    end
  end
  object pUndo: TPanel
    Left = 332
    Top = 227
    Width = 167
    Height = 18
    BevelOuter = bvNone
    TabOrder = 9
    Visible = False
    object Label3: TLabel
      Left = 2
      Top = 2
      Width = 81
      Height = 13
      Caption = 'Previous item:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object sbChange: TSpeedButton
      Left = 89
      Top = 0
      Width = 75
      Height = 18
      Caption = 'Incorrect'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Tahoma'
      Font.Style = []
      Glyph.Data = {
        D6010000424DD60100000000000036000000280000000A0000000D0000000100
        180000000000A001000000000000000000000000000000000000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFF000000FFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFF000000000000000000FFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFF000000000000FFFFFF000000000000FFFFFFFFFFFF0000FFFFFFFFFFFF
        000000000000FFFFFFFFFFFFFFFFFF000000000000FFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000FFFFFFFFFFFF
        FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0000}
      Layout = blGlyphRight
      Margin = 6
      ParentFont = False
      Spacing = 5
      OnClick = sbChangeClick
    end
  end
  object pPause: TPanel
    Left = 649
    Top = 14
    Width = 75
    Height = 28
    BevelOuter = bvNone
    TabOrder = 10
    object btnPause: TButton
      Left = 2
      Top = 2
      Width = 72
      Height = 25
      Caption = 'Pause test'
      TabOrder = 0
      OnClick = btnPauseClick
    end
  end
  object pAnswer: TPanel
    Left = 0
    Top = 210
    Width = 740
    Height = 109
    Align = alBottom
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 1
    ExplicitTop = 211
    object lb1: TLabel
      Left = 239
      Top = 57
      Width = 108
      Height = 14
      Alignment = taCenter
      AutoSize = False
      Caption = 'Few minutes'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object lb2: TLabel
      Left = 418
      Top = 57
      Width = 108
      Height = 14
      Alignment = taCenter
      AutoSize = False
      Caption = '*'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object lb0: TLabel
      Left = 634
      Top = 9
      Width = 93
      Height = 13
      Alignment = taCenter
      AutoSize = False
      Caption = 'Few minutes'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object lb3: TLabel
      Left = 634
      Top = 57
      Width = 93
      Height = 14
      Alignment = taCenter
      AutoSize = False
      Caption = '*'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -12
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label5: TLabel
      Left = 16
      Top = 69
      Width = 99
      Height = 25
      Caption = 'Pick one:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -21
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbAnswer: TLabel
      Left = 16
      Top = 31
      Width = 97
      Height = 16
      Caption = 'Your answer:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbWordTypes: TLabel
      Left = 125
      Top = 3
      Width = 75
      Height = 16
      Caption = 'lbWordTypes'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object Label4: TLabel
      Left = 27
      Top = 3
      Width = 86
      Height = 16
      Caption = 'Word types:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlack
      Font.Height = -13
      Font.Name = 'Georgia'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object btn1: TButton
      Tag = 1
      Left = 236
      Top = 73
      Width = 113
      Height = 25
      Caption = 'Couldn'#39't recall [1]'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = btn1Click
    end
    object btn0: TButton
      Left = 632
      Top = 25
      Width = 97
      Height = 25
      Caption = 'Try again [0]'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = btn1Click
    end
    object btn2: TButton
      Tag = 2
      Left = 416
      Top = 73
      Width = 113
      Height = 25
      Caption = 'Remembered [2]'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -13
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = btn1Click
    end
    object btn3: TButton
      Tag = 3
      Left = 632
      Top = 73
      Width = 97
      Height = 25
      Caption = 'Too easy [3]'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      OnClick = btn1Click
    end
    object pbAnswer: TZPaintBox
      Left = 120
      Top = 23
      Width = 503
      Height = 29
      BorderStyle = bsNone
      HorzScrollBar.Visible = False
      TabOrder = 4
      VertScrollBar.Visible = False
      OnPaint = pbAnswerPaint
    end
  end
  object tTime: TTimer
    Enabled = False
    Interval = 500
    OnTimer = tTimeTimer
    Left = 680
    Top = 136
  end
  object pmOptions: TPopupMenu
    AutoPopup = False
    Left = 608
    Top = 24
    object miGroup: TMenuItem
      Caption = 'Add to group...'
      OnClick = miGroupClick
    end
    object miQuestion: TMenuItem
      Caption = 'Add question...'
      OnClick = miQuestionClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object miSites: TMenuItem
      Caption = 'Search online'
      Visible = False
    end
  end
  object pmChange: TPopupMenu
    AutoPopup = False
    Left = 536
    Top = 284
    object miRetry: TMenuItem
      Caption = 'Retry'
      OnClick = miRetryClick
    end
    object N3: TMenuItem
      Caption = '-'
    end
    object miHard: TMenuItem
      Tag = 1
      Caption = 'Incorrect'
      OnClick = miRetryClick
    end
    object miGood: TMenuItem
      Tag = 2
      Caption = 'Correct'
      OnClick = miRetryClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object miEasy: TMenuItem
      Tag = 3
      Caption = 'Easy'
      OnClick = miRetryClick
    end
  end
end
