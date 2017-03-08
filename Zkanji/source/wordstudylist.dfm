object fStudyList: TfStudyList
  Left = 900
  Top = 233
  ActiveControl = btnTest
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Long-term study list'
  ClientHeight = 498
  ClientWidth = 709
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000000000000000000000000000000000000000000000000
    0000000099999999999999999999999999999999999999999999999999999999
    9999999999999999000000000000000000CCCCCCD6D6CEE6E5D8EBE9DBEBE9DA
    EBE8DAEBE8DAEBE8D9EAE7D9EBE7D9EAE7D8DEDDD1C9C9C19999990000000000
    00CCCCCCE6E5D8EEEADEEEEADEEDEADDECEADD777777DBDAD5EDE9DCECE9DCEC
    E9DBEBE8DBDEDDD1999999000000000000CCCCCCEFEDE2EFEDE2EFECE1EEECE1
    888888333333555555EEEBDFEEEBDEEEEBDEEEEBDEEDEBDE9999990000000000
    00CCCCCCF1EFE5F0EFE5F1EEE4F0EEE4F0EEE3888888555555F0EDE2F0EEE2EF
    EDE2EFECE1EFECE1999999000000000000CCCCCCF3F1E8EFEFEE222222222222
    222222222222111111222222222222222222F1EEE4F0EFE49999990000000000
    00CCCCCCF4F2EAF4F2EAF3F2EAF4F2E9F3F1E9888888222222E5E4E2E5E4E2D7
    D7D4F2F0E8F2F1E7999999000000000000CCCCCCF5F4EDF6F4EDE2E2E1888888
    555555555555222222111111E6E5E4F4F2EBF4F2EAF3F2EA9999990000000000
    00CCCCCCF6F5F0F7F5EF222222666666999999999999999999999999888888DB
    DAD7F5F3EDF5F4EC999999000000000000CCCCCCF9F8F3F8F7F3E3E3E1333333
    555555222222222222555555444444555555F6F6F0F6F5EF9999990000000000
    00CCCCCCFAF9F5F9F9F4FAF8F4F9F8F4F9F9F3555555666666F8F7F3F8F7F3F9
    F7F2F8F7F2F7F7F2999999000000000000CCCCCCF5F5F0FAF9F7FBFAF7FBFAF6
    FAF9F6F1F1F0FAF9F6FAF9F5FAF9F5FAF8F5F9F9F5F5F4EB9999990000000000
    00CCCCCCF2F2E9F7F7F2595959000000FBFAF9FBFAF8FBFAF8595959000000FB
    FAF7FAFAF5EDEDE4999999000000000000000000CCCCCC474747000000CCCCCC
    CCCCCCCCCCCC474747000000CCCCCCCCCCCCCCCCCCCCCCCC0000000000000000
    00000000B6B6B65959590000007C7C7C000000B6B6B65959590000007C7C7C00
    0000B6B6B6000000000000000000000000000000000000B6B6B6595959000000
    B6B6B6000000B6B6B6595959000000B6B6B6000000000000000000000000C003
    AC418001AC418001AC418001AC418001AC418001AC418001AC418001AC418001
    AC418001AC418001AC418001AC418001AC41C003AC41C007AC41E10FAC41}
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    709
    498)
  PixelsPerInch = 96
  TextHeight = 13
  object b2: TBevel
    Left = 0
    Top = 468
    Width = 9999
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object Bevel1: TBevel
    Left = 0
    Top = 466
    Width = 9999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object tTabs: TTabControl
    Left = -1
    Top = 1
    Width = 9999
    Height = 29
    Images = ImageList1
    TabOrder = 0
    Tabs.Strings = (
      'Item list'
      'Global statistics')
    TabIndex = 0
    TabStop = False
    OnChange = tTabsChange
  end
  object pStats: TPanel
    AlignWithMargins = True
    Left = 0
    Top = 25
    Width = 709
    Height = 441
    Margins.Left = 0
    Margins.Top = 25
    Margins.Right = 0
    Margins.Bottom = 32
    Align = alClient
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 5
    Visible = False
    DesignSize = (
      709
      441)
    object bv7: TBevel
      Left = 0
      Top = 147
      Width = 99999
      Height = 1
      Shape = bsTopLine
    end
    object Label19: TLabel
      Left = 8
      Top = 128
      Width = 43
      Height = 13
      Caption = 'Graphs:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object sbG1: TSpeedButton
      Left = 59
      Top = 125
      Width = 86
      Height = 19
      GroupIndex = 2
      Caption = '1 - Results'
      OnClick = sbG1Click
    end
    object sbG2: TSpeedButton
      Tag = 1
      Left = 146
      Top = 125
      Width = 86
      Height = 19
      GroupIndex = 2
      Down = True
      Caption = '2 - Items'
      OnClick = sbG1Click
    end
    object sbG3: TSpeedButton
      Tag = 2
      Left = 233
      Top = 125
      Width = 86
      Height = 19
      GroupIndex = 2
      Caption = '3 - Time'
      OnClick = sbG1Click
    end
    object sbG4: TSpeedButton
      Tag = 3
      Left = 321
      Top = 125
      Width = 86
      Height = 19
      GroupIndex = 2
      Caption = '4 - Decks'
      OnClick = sbG1Click
    end
    object sbG5: TSpeedButton
      Tag = 4
      Left = 408
      Top = 125
      Width = 86
      Height = 19
      GroupIndex = 2
      Caption = '5 - Tries'
      OnClick = sbG1Click
    end
    object sbG6: TSpeedButton
      Tag = 5
      Left = 495
      Top = 125
      Width = 86
      Height = 19
      GroupIndex = 2
      Caption = '6 - Deck avg.'
      OnClick = sbG1Click
    end
    object bv6: TBevel
      Left = 0
      Top = 121
      Width = 99999
      Height = 1
      Shape = bsTopLine
    end
    object bv4: TBevel
      Left = 0
      Top = 7
      Width = 99999
      Height = 2
      Shape = bsTopLine
    end
    object bv5: TBevel
      Left = 0
      Top = 111
      Width = 99999
      Height = 2
      Shape = bsTopLine
    end
    object pbGlobalStats: TZPaintBox
      AlignWithMargins = True
      Top = 148
      Width = 709
      Height = 293
      Margins.Left = 0
      Margins.Top = 148
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alClient
      BorderStyle = bsNone
      TabOrder = 0
    end
    object Panel1: TPanel
      Left = 35
      Top = 11
      Width = 640
      Height = 98
      Anchors = [akTop]
      BevelEdges = [beLeft, beRight]
      BevelOuter = bvNone
      TabOrder = 1
      DesignSize = (
        640
        98)
      object sh1: TShape
        Left = 30
        Top = 43
        Width = 145
        Height = 17
        Brush.Color = clMedGray
        Pen.Style = psClear
      end
      object Label5: TLabel
        Left = 46
        Top = 28
        Width = 56
        Height = 13
        Caption = 'Items total:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label6: TLabel
        Left = 46
        Top = 60
        Width = 69
        Height = 13
        Caption = 'Tested words:'
      end
      object Label7: TLabel
        Left = 46
        Top = 76
        Width = 62
        Height = 13
        Caption = 'Tested kanji:'
      end
      object lbIDueTitle: TLabel
        Left = 46
        Top = 44
        Width = 83
        Height = 13
        Caption = 'Items due today:'
        Transparent = True
      end
      object Label11: TLabel
        Left = 238
        Top = 28
        Width = 47
        Height = 13
        Caption = 'First test:'
      end
      object Label12: TLabel
        Left = 238
        Top = 44
        Width = 46
        Height = 13
        Caption = 'Last test:'
      end
      object Label13: TLabel
        Left = 238
        Top = 60
        Width = 73
        Height = 13
        Caption = 'Days with test:'
      end
      object Label14: TLabel
        Left = 238
        Top = 76
        Width = 67
        Height = 13
        Caption = 'Skipped days:'
      end
      object Label15: TLabel
        Left = 430
        Top = 28
        Width = 61
        Height = 13
        Caption = 'Overall time:'
      end
      object Label16: TLabel
        Left = 430
        Top = 44
        Width = 68
        Height = 13
        Caption = 'Test average:'
      end
      object Label17: TLabel
        Left = 430
        Top = 60
        Width = 83
        Height = 13
        Caption = 'Answer average:'
      end
      object lbIWords: TLabel
        Left = 134
        Top = 60
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbIKanji: TLabel
        Left = 134
        Top = 76
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbIDue: TLabel
        Left = 134
        Top = 44
        Width = 4
        Height = 13
        Caption = '-'
        Transparent = True
      end
      object lbTFirst: TLabel
        Left = 326
        Top = 28
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbTLast: TLabel
        Left = 326
        Top = 44
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbTDays: TLabel
        Left = 326
        Top = 60
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbTSkipped: TLabel
        Left = 326
        Top = 76
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbTOverall: TLabel
        Left = 518
        Top = 28
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbTAverage: TLabel
        Left = 518
        Top = 44
        Width = 4
        Height = 13
        Caption = '-'
      end
      object lbTAAverage: TLabel
        Left = 518
        Top = 60
        Width = 4
        Height = 13
        Caption = '-'
      end
      object Bevel6: TBevel
        Left = 200
        Top = 24
        Width = 2
        Height = 50
        Shape = bsLeftLine
      end
      object Bevel7: TBevel
        Left = 392
        Top = 24
        Width = 2
        Height = 50
        Shape = bsLeftLine
      end
      object lbITotal: TLabel
        Left = 134
        Top = 28
        Width = 4
        Height = 13
        Caption = '-'
      end
      object Label10: TLabel
        Left = 230
        Top = 8
        Width = 35
        Height = 16
        Caption = 'Tests'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label18: TLabel
        Left = 422
        Top = 8
        Width = 29
        Height = 16
        Caption = 'Time'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label20: TLabel
        Left = 38
        Top = 8
        Width = 37
        Height = 16
        Caption = 'Items'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Bevel5: TBevel
        Left = 1
        Top = 8
        Width = 2
        Height = 82
        Shape = bsLeftLine
      end
      object Bevel8: TBevel
        Left = 638
        Top = 8
        Width = 2
        Height = 82
        Anchors = [akTop, akRight]
        Shape = bsLeftLine
      end
    end
  end
  object pList: TPanel
    AlignWithMargins = True
    Left = 0
    Top = 25
    Width = 709
    Height = 441
    Margins.Left = 0
    Margins.Top = 25
    Margins.Right = 0
    Margins.Bottom = 32
    Align = alClient
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 1
    DesignSize = (
      709
      441)
    object b1: TBevel
      Left = 0
      Top = 26
      Width = 9999
      Height = 1
      Shape = bsTopLine
    end
    object Label2: TLabel
      Left = 324
      Top = 5
      Width = 32
      Height = 13
      Caption = 'Filter:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object sbWordStats: TSpeedButton
      Left = 290
      Top = 0
      Width = 23
      Height = 23
      Hint = 'Show or hide statistics for items'
      AllowAllUp = True
      GroupIndex = 1
      Flat = True
      Glyph.Data = {
        4A090000424D4A09000000000000420000002800000044000000110000000100
        10000300000008090000120B0000120B00000000000000000000007C0000E003
        00001F000000DC7DDC7D9452DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D
        DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D
        DC7DDC7DDC7DDC7DDC7DDC7DDC7D9452DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D
        DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D9452DC7DDC7DDC7DDC7DDC7DDC7DDC7D
        DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D00009452945294529452945294529452
        94529452945294529452945294529452DC7D734EDC7DDC7DDC7DDC7DDC7DDC7D
        DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D000094529452945294529452
        9452945294529452945294529452945294529452DC7D00009452945294529452
        9452945294529452945294529452945294529452945200000000000000000000
        000000000000000000000000000000000000000000009452734E734E734E734E
        734E734E734E734E734E734E734E734E734E734E734E734EDC7D000000000000
        0000000000000000000000000000000000000000000000000000945200000000
        000000000000000000000000000000000000000000000000000000009452DC7D
        00006378215C215C5822973E3722973E371E763A161E763A161A763A161A9452
        DC7D734EB55694529452D65AF75ED65AF75ED65AF75ED65AF75ED65AD65AD65A
        DC7DDC7D0000215463702154782EB846782ED846782ED94A9932D94A9932D94A
        99329452DC7D00008A7D215C215CB82ED746972EB746972EB642962AB642762A
        B642762A9452DC7D0000D94684788478215CB8425822973E3722973E371E773A
        161E763A161E9452DC7D734EF75EB556B5569452F75ED65AF75ED65AF75ED65A
        F75ED65AF75ED65ADC7DDC7D00009742215463702154B746782EB846782ED846
        782ED84A9932D94A99329452DC7D0000194F8B7D8B7D215CD84AB832D746B72E
        D746972EB742962AB642762A9452DC7D0000DA4A792A8478215CD9425826215C
        5822983E3722973E371E973A371E9452DC7D734EF75ED65AB5569452F75ED65A
        9452D65AF75ED65AF75ED65AF75ED65ADC7DDC7D0000763E36262154636C9742
        572A636C582EB846782EB846782ED84A99329452DC7D00001A4FF936AB7D215C
        F94AD832215CB832D846B72ED746972EB742972A9452DC7D0000FB4A9A2EA578
        215CD946215CC678215CB8425822B83E36423642371E9452DC7D734E1863F75E
        B5569452F75E9452D65A9452F75ED65AF75ED65AD65AD65ADC7DDC7D0000763E
        36262154636C9642636C2154636CB742572AB846B231B231782E9452DC7D0000
        3B531A3BAB7D215C194F215CAC7D215CF84AB832D84636423642972E9452DC7D
        00001B4FBB2EFB4AC678215CE778DA46E778215C792636423642DC7D3642DC7D
        DC7D734E1863F75E1863B5569452D65AF75ED65A9452D65AD65AD65ADC7DD65A
        DC7DDC7D0000553A1522753E2154636C2154963E21546368572AB231B231DC7D
        B231DC7DDC7D00005B571B3F3B53AC7D215CCC7D1A4FCD7D215CD93636423642
        DC7D3642DC7DDC7D00003C53DC321B4FBB32E778BB2EFA4A0879215C3642DC7D
        3642DC7DDC7DDC7DDC7D734E1863F75E1863F75ED65AF75EF75ED65A9452D65A
        DC7DD65ADC7DDC7DDC7DDC7D0000553A1522553A152221543626763E21546368
        B231DC7DB231DC7DDC7DDC7DDC7D00007C5B3C435B571B3FCD7D1B3B3A53CD7D
        215C3642DC7D3642DC7DDC7DDC7DDC7D00005D53DC363C53DC321C4FBB321B4F
        2979215C3642DC7DDC7DDC7DDC7DDC7DDC7D734E1863F75E1863F75E1863F75E
        1863D65A9452D65ADC7DDC7DDC7DDC7DDC7DDC7D00003436F41D54361522553A
        1522753A21546368B231DC7DDC7DDC7DDC7DDC7DDC7D00007D5B5C437C5B3C43
        5C573B3F5B57ED7D215C3642DC7DDC7DDC7DDC7DDC7DDC7D00005D57FD365D57
        FD363C53364236424A79215C3642DC7DDC7D9452DC7DDC7DDC7D734E1863F75E
        1863F75E1863D65AD65AD65A9452D65ADC7DDC7DDC7DDC7DDC7DDC7D00003336
        F41D3436F41D5436B231B23121546368B231DC7DDC7D9452DC7DDC7DDC7D0000
        9D5F5D477D5B5D477C5B36423642EE7D215C3642DC7DDC7D9452DC7DDC7DDC7D
        00007E5B1E3B5E57FD3A5D573642DC7DDC7D6B79215CDC7DDC7D215C9452DC7D
        DC7D734E1863F75E1863F75E1863D65ADC7DDC7DD65A9452DC7DDC7D9452DC7D
        DC7DDC7D00001332D3193336F41D3436B231DC7DDC7D21546364DC7DDC7D6364
        9452DC7DDC7D00009E637E4B9E5F7D477D5F3642DC7DDC7D0E7E215CDC7DDC7D
        215C9452DC7DDC7D00007E5B1E3F36421E3B3642DC7DDC7DDC7D8C79215CDC7D
        215C215C9452DC7DDC7D734E18631863D65AF75ED65ADC7DDC7DDC7DD65A9452
        DC7D94529452DC7DDC7DDC7D00001332D319B231D319B231DC7DDC7DDC7D2154
        6364DC7D215463649452DC7DDC7D0000BE639E4B36427E4B3642DC7DDC7DDC7D
        0F7E215CDC7D215C215C9452DC7DDC7D00009F5F3642DC7D36423642DC7DDC7D
        DC7DDC7DAD79215CAD79215C9452DC7DDC7D734E1863D65ADC7DD65AD65ADC7D
        DC7DDC7DDC7DF75E9452F75E9452DC7DDC7DDC7D0000122EB231DC7DB231B231
        DC7DDC7DDC7DDC7D21546364636463649452DC7DDC7D0000BF633642DC7D3642
        3642DC7DDC7DDC7DDC7D2F7E215C307E215C9452DC7DDC7D00009F5F3642DC7D
        3642DC7DDC7DDC7DDC7DDC7D215CAD79D67E215C9452DC7DDC7D734E1863D65A
        DC7DD65ADC7DDC7DDC7DDC7DDC7D9452F75E18639452DC7DDC7DDC7D0000122E
        B231DC7DB231DC7DDC7DDC7DDC7DDC7D21546364CE6563649452DC7DDC7D0000
        DF673642DC7D3642DC7DDC7DDC7DDC7DDC7D215C307E187F215C9452DC7DDC7D
        00009F5F3642DC7DDC7DDC7DDC7DDC7DDC7D215CCE79F77EF77E215C9452DC7D
        DC7D734E1863D65ADC7DDC7DDC7DDC7DDC7DDC7D9452F75E186318639452DC7D
        DC7DDC7D0000122EB231DC7DDC7DDC7DDC7DDC7DDC7D21546364CE65CE656360
        9452DC7DDC7D0000DF673642DC7DDC7DDC7DDC7DDC7DDC7D215C307E187F187F
        215C9452DC7DDC7D00003642DC7DDC7DDC7DDC7DDC7DDC7DCE79CE79CE79CE79
        CE79215C9452DC7DDC7D734ED65ADC7DDC7DDC7DDC7DDC7DDC7DF75EF75EF75E
        F75EF75E9452DC7DDC7DDC7D0000B231DC7DDC7DDC7DDC7DDC7DDC7D21542154
        21542154215421549452DC7DDC7D00003642DC7DDC7DDC7DDC7DDC7DDC7D307E
        307E307E307E307E215C9452DC7D}
      NumGlyphs = 4
      OnClick = sbWordStatsClick
    end
    object sbList1: TSpeedButton
      Left = 3
      Top = 0
      Width = 96
      Height = 23
      Hint = 
        'List of items that were added to the long-term study list but ar' +
        'e not included in every day tests.'
      GroupIndex = 2
      Caption = '1 - Queued items'
      NumGlyphs = 4
      OnClick = sbList1Click
    end
    object Label1: TLabel
      Left = 423
      Top = 5
      Width = 37
      Height = 13
      Caption = '&Search:'
      FocusControl = pbFilter
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
    end
    object sbShowKanji: TSpeedButton
      Left = 360
      Top = 0
      Width = 18
      Height = 23
      Hint = 'Show items in the list where the written form is tested.'
      AllowAllUp = True
      GroupIndex = 3
      Down = True
      Caption = 'W'
      OnClick = sbShowKanjiClick
    end
    object sbShowKana: TSpeedButton
      Left = 378
      Top = 0
      Width = 18
      Height = 23
      Hint = 'Show items in the list where the kana form is tested.'
      AllowAllUp = True
      GroupIndex = 4
      Down = True
      Caption = 'K'
      OnClick = sbShowKanjiClick
    end
    object sbShowMeaning: TSpeedButton
      Left = 396
      Top = 0
      Width = 18
      Height = 23
      Hint = 'Show items in the list where the meaning is tested.'
      AllowAllUp = True
      GroupIndex = 5
      Down = True
      Caption = 'M'
      OnClick = sbShowKanjiClick
    end
    object Bevel3: TBevel
      Left = 317
      Top = 5
      Width = 2
      Height = 14
      Shape = bsLeftLine
    end
    object sbList2: TSpeedButton
      Tag = 1
      Left = 107
      Top = 0
      Width = 94
      Height = 23
      Hint = 'Items that will be included in tests when they are due.'
      GroupIndex = 2
      Caption = '2 - Studied items'
      NumGlyphs = 4
      OnClick = sbList1Click
    end
    object sbList3: TSpeedButton
      Tag = 2
      Left = 202
      Top = 0
      Width = 84
      Height = 23
      Hint = 'Items that were included in the test of the last test day.'
      GroupIndex = 2
      Caption = '3 - Last tested'
      NumGlyphs = 4
      OnClick = sbList1Click
    end
    object Bevel10: TBevel
      Left = 102
      Top = 5
      Width = 2
      Height = 14
      Shape = bsLeftLine
    end
    object pbFilter: TZPaintBox
      Left = 464
      Width = 161
      Height = 24
      Anchors = [akLeft, akTop, akRight]
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Gothic'
      Font.Style = []
      HorzScrollBar.Visible = False
      ParentFont = False
      TabOrder = 0
      VertScrollBar.Visible = False
      OnKeyDown = pbFilterKeyDown
    end
    object pWordStats: TPanel
      Left = 0
      Top = 288
      Width = 709
      Height = 153
      Align = alBottom
      BevelOuter = bvNone
      FullRepaint = False
      TabOrder = 1
      Visible = False
      object splitWordStats: TShape
        Left = -1
        Top = 0
        Width = 99999
        Height = 5
        Brush.Color = clMedGray
        Pen.Color = clGray
        OnMouseDown = splitWordStatsMouseDown
        OnMouseEnter = splitWordStatsMouseEnter
        OnMouseLeave = splitWordStatsMouseLeave
        OnMouseMove = splitWordStatsMouseMove
        OnMouseUp = splitWordStatsMouseUp
      end
      object bv3: TBevel
        Left = 0
        Top = 27
        Width = 99999
        Height = 1
        Shape = bsTopLine
      end
      object pbWordStats: TZPaintBox
        AlignWithMargins = True
        Top = 28
        Width = 709
        Height = 125
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alClient
        BorderStyle = bsNone
        HorzScrollBar.Visible = False
        TabOrder = 0
        VertScrollBar.Visible = False
      end
      object pWordStatButtons: TPanel
        AlignWithMargins = True
        Left = 0
        Top = 5
        Width = 709
        Height = 22
        Margins.Left = 0
        Margins.Top = 5
        Margins.Right = 0
        Margins.Bottom = 1
        Align = alTop
        BevelOuter = bvNone
        TabOrder = 1
        OnMouseDown = splitWordStatsMouseDown
        OnMouseEnter = splitWordStatsMouseEnter
        OnMouseLeave = splitWordStatsMouseLeave
        OnMouseMove = splitWordStatsMouseMove
        OnMouseUp = splitWordStatsMouseUp
        object Label4: TLabel
          Left = 8
          Top = 4
          Width = 74
          Height = 13
          Caption = 'Item history:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          OnMouseDown = splitWordStatsMouseDown
          OnMouseMove = splitWordStatsMouseMove
          OnMouseUp = splitWordStatsMouseUp
          OnMouseEnter = splitWordStatsMouseEnter
          OnMouseLeave = splitWordStatsMouseLeave
        end
        object sbWordDeck: TSpeedButton
          Left = 87
          Top = 1
          Width = 49
          Height = 19
          GroupIndex = 1
          Down = True
          Caption = 'Deck'
          OnClick = sbWordDeckClick
        end
        object sbWordScore: TSpeedButton
          Tag = 1
          Left = 137
          Top = 1
          Width = 49
          Height = 19
          GroupIndex = 1
          Caption = 'Score'
          OnClick = sbWordDeckClick
        end
      end
    end
    object pWordList: TPanel
      AlignWithMargins = True
      Left = 0
      Top = 27
      Width = 709
      Height = 261
      Margins.Left = 0
      Margins.Top = 27
      Margins.Right = 0
      Margins.Bottom = 0
      Align = alClient
      BevelOuter = bvNone
      FullRepaint = False
      TabOrder = 2
      object pbList: TZPaintBox
        Width = 709
        Height = 215
        Margins.Left = 0
        Margins.Top = 27
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alClient
        BorderStyle = bsNone
        InvalidateOnScroll = True
        TabOrder = 0
        OnKeyDown = pbListKeyDown
        OnKeyPress = pbListKeyPress
        OnMouseUp = pbListMouseUp
      end
      object sbItems: TStatusBar
        Left = 0
        Top = 215
        Width = 709
        Height = 19
        Panels = <
          item
            Alignment = taRightJustify
            Bevel = pbNone
            Text = '0  :'
            Width = 45
          end
          item
            Bevel = pbNone
            Text = '0'
            Width = 65
          end
          item
            Bevel = pbNone
            Text = '-'
            Width = 9999
          end>
      end
      object pMeaning: TPanel
        Left = 0
        Top = 234
        Width = 709
        Height = 27
        Align = alBottom
        BevelOuter = bvNone
        FullRepaint = False
        TabOrder = 2
        OnMouseDown = splitWordStatsMouseDown
        OnMouseEnter = splitWordStatsMouseEnter
        OnMouseLeave = splitWordStatsMouseLeave
        OnMouseMove = splitWordStatsMouseMove
        OnMouseUp = splitWordStatsMouseUp
        DesignSize = (
          709
          27)
        object Bevel4: TBevel
          Left = 0
          Top = 0
          Width = 99999
          Height = 1
          Shape = bsTopLine
        end
        object Label3: TLabel
          Left = 6
          Top = 7
          Width = 98
          Height = 13
          Caption = 'Studied &meaning:'
          FocusControl = edMeaning
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          OnMouseDown = splitWordStatsMouseDown
          OnMouseMove = splitWordStatsMouseMove
          OnMouseUp = splitWordStatsMouseUp
          OnMouseEnter = splitWordStatsMouseEnter
          OnMouseLeave = splitWordStatsMouseLeave
        end
        object btnSave: TSpeedButton
          Left = 604
          Top = 2
          Width = 47
          Height = 23
          Hint = 
            'Save as the word'#39's meaning. Unsaved changes will be lost when an' +
            'other item is selected or when the window closes.'
          Anchors = [akTop, akRight]
          Caption = 'Save'
          Enabled = False
          OnClick = btnSaveClick
          ExplicitLeft = 578
        end
        object btnReset: TSpeedButton
          Left = 657
          Top = 2
          Width = 49
          Height = 23
          Hint = 'Restore word meaning from dictionary'
          Anchors = [akTop, akRight]
          Caption = 'Reset'
          Enabled = False
          OnClick = btnResetClick
          ExplicitLeft = 631
        end
        object Bevel9: TBevel
          Left = 653
          Top = 5
          Width = 2
          Height = 18
          Anchors = [akTop, akRight]
          Shape = bsLeftLine
          ExplicitLeft = 627
        end
        object edMeaning: TEdit2
          Left = 110
          Top = 3
          Width = 491
          Height = 22
          Anchors = [akLeft, akTop, akRight]
          Enabled = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnChange = edMeaningChange
        end
      end
    end
    object Panel2: TPanel
      Left = 629
      Top = 0
      Width = 78
      Height = 23
      Anchors = [akTop, akRight]
      BevelOuter = bvNone
      FullRepaint = False
      TabOrder = 3
      object Bevel2: TBevel
        Left = 0
        Top = 5
        Width = 2
        Height = 14
        Shape = bsLeftLine
      end
      object sbAddItem: TSpeedButton
        Left = 5
        Top = 0
        Width = 23
        Height = 23
        Hint = 'Add other items with the same word but different question'
        Enabled = False
        Flat = True
        Glyph.Data = {
          4A090000424D4A09000000000000420000002800000044000000110000000100
          10000300000008090000120B0000120B00000000000000000000007C0000E003
          00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F94529452
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F843C843C94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7F734E734EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7F8044804494529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FA150A15094529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7F843CE77CE77C843C94529452FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7F734E734E734E734EFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F6044A064A064804494529452FF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FA150E274E274A15094529452FF7FFF7FFF7F
          FF7FFF7FFF7FFF7F945294529452843C077D077D077DE77C843C94529452FF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452945294529452734E734EFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F9452945294526044806480648064806460449452
          9452FF7FFF7FFF7FFF7FFF7FFF7F945294529452A250E274E274E274E274A250
          94529452FF7FFF7FFF7FFF7FFF7FC83DC83DC83DA43C277D277D277D277D077D
          077D843C94529452FF7FFF7FFF7FFF7FB556B556B556B556B556B55694529452
          945294529452FF7FFF7FFF7FFF7FFF7FFF7FC849C849C8494048806480648064
          806480648064604894529452FF7FFF7FFF7FFF7F2B562B564B568250E370C374
          E374E374E374E374A25094529452FF7FFF7FFF7FFF7F8C5A917FC83D677D477D
          477D477D477D277D277D277D843C9452FF7FFF7FFF7FFF7FD65AB556B556B556
          B556B556B556B556B556945294529452FF7FFF7FFF7FFF7FFF7F45358A66C749
          6064606460646064606460646064606440449452FF7FFF7FFF7FFF7F873D0F7B
          2A56C470C370C370C370C370C370C370C37482509452FF7FFF7F94529452AC5A
          B17FC83D055905590559677D477DA43CE558E558E558E558FF7FFF7FFF7FFF7F
          D65AD65AD65AD65AD65AD65AB556B556B556B556B556B556B556FF7FFF7F9452
          945244356966C7492130213020306164616440442030203020302030FF7FFF7F
          94529452873D0D7B2A56623862386238C470C47083506238623862386238FF7F
          E83DE83DE83DAC5AD17FE83DE83DC83DC83D877D877DC43C9452FF7FFF7FFF7F
          FF7FF75EF75EF75EF75EF75EF75ED65AD65AD65AD65AD65AD65AFF7FFF7FFF7F
          FF7FFF7FA549A549A54924356866A649A649A649C6494160416041449452FF7F
          FF7FFF7FFF7F075608560856663DEC7A0956095609562956A470A470834C9452
          FF7FFF7FFF7FFF7FCC5AF17FF17FF17FD17FD17FD17FD17FE83DA77DA77DC43C
          9452FF7FFF7FFF7FFF7F186318631863F75EF75EF75EF75EF75EF75EF75EF75E
          D65AFF7FFF7FFF7FFF7FFF7F23354666466646664766476647664866A6494260
          426021449452FF7FFF7FFF7FFF7F653DCA76CA76CA76CB76CB76EB7AEC7A0956
          A56CA56C644C9452FF7FFF7FFF7FFF7FCC56CC56CC56CC56F17FCC5ACC5AAC5A
          E83DC77DC77DE43C9452FF7FFF7FFF7FFF7F186318631863186318631863F75E
          F75EF75EF75EF75EF75EFF7FFF7FFF7FFF7FFF7F023502352235233525662335
          23352335A5492360236022449452FF7FFF7FFF7FFF7F443D443D443D643DA976
          653D653D653DE755866C866C644C9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FCC56
          F17BE83D9452FF7F6559077E077EE43C9452FF7FFF7FFF7FFF7FFF7FFF7FFF7F
          396718631863FF7FFF7F186318631863F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F0235246683491042FF7F0230245C235C22409452FF7FFF7FFF7FFF7FFF7F
          FF7FFF7F443DA876E6519452FF7F4334866C866C644C9452FF7FFF7FFF7FFF7F
          FF7FFF7FFF7FCC56F17BE83D9452FF7F8559277E277E043D9452FF7FFF7FFF7F
          FF7FFF7FFF7FFF7F396739673967FF7FFF7F3967396718631863FF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7F0235046663491042FF7F0230045C045C03409452FF7F
          FF7FFF7FFF7FFF7FFF7FFF7F43398776C5519452FF7F44346768676845489452
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FCC52CC52E83DFF7FFF7FA559477E277E243D
          9452FF7FFF7FFF7FFF7FFF7FFF7FFF7F5A6B5A6B3967FF7FFF7F396739673967
          3967FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F013501356249FF7FFF7F022C055C
          055C03409452FF7FFF7FFF7FFF7FFF7FFF7FFF7F43394339C451FF7FFF7F2434
          6868686845489452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          A559477E477E243D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F5A6B396739673967FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7F032C0658055804409452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7F24344868486845489452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FC559877E677E443D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7F5A6B5A6B5A6B5A6BFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F032C06580658043C9452FF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7F24344964496446489452FF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FC559877E877E443D9452FF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F5A6B5A6B5A6B5A6BFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F032C07540754053C9452FF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F24344964496426489452
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FC559C559C559443D
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F5A6B5A6B5A6B
          5A6BFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F032C032C
          032C053CFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F2530
          253025302648FF7FFF7FFF7FFF7F}
        NumGlyphs = 4
        OnClick = sbAddItemClick
      end
      object sbRemoveItem: TSpeedButton
        Left = 28
        Top = 0
        Width = 23
        Height = 23
        Hint = 'Remove item from long-term study list'
        Enabled = False
        Flat = True
        Glyph.Data = {
          4A090000424D4A09000000000000420000002800000044000000110000000100
          10000300000008090000120B0000120B00000000000000000000007C0000E003
          00001F000000DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
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
          DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
          DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
          DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
          DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A
          DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A945294529452
          945294529452945294529452945294529452945294529452DF7ADF7ADF7ADF7A
          DF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7ADF7A9452
          94529452945294529452945294529452945294529452945294529452DF7ADF7A
          945294529452945294529452945294529452945294529452945294529452DF7A
          A73DA73DA63DA639A6398639A639863986398639853985398539653965399452
          DF7AF75EF75EF75ED65AD65AD65AD65AB556B556B556B556B556945294529452
          DF7ADF7A82418241824182418241824182418241824182418241824182418241
          82419452DF7A0756075627560756075607562756275627562756285628562856
          285628569452DF7A8B568F7B8E7B6E7B6E7B6D776D774D774D774D774C772C77
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
        OnClick = sbRemoveItemClick
        OnMouseDown = sbRemoveItemMouseDown
      end
      object sbToQueue: TSpeedButton
        Left = 51
        Top = 0
        Width = 23
        Height = 23
        Hint = 
          'Move selected items back to the queue and delete their study dat' +
          'a.'
        Enabled = False
        Flat = True
        Glyph.Data = {
          4A090000424D4A09000000000000420000002800000044000000110000000100
          10000300000008090000120B0000120B00000000000000000000007C0000E003
          00001F0000003C793C793C7903563C793C793C793C793C793C793C793C793C79
          3C793C793C793C793C793C793C79B5563C793C793C793C793C793C793C793C79
          3C793C793C793C793C793C793C793C79E0513C793C793C793C793C793C793C79
          3C793C793C793C793C793C793C793C793C79685A3C793C793C793C793C793C79
          3C793C793C793C793C793C793C793C793C79435EC3763C793C793C793C793C79
          3C793C799452945294529452945294523C793C79D65AD65A3C793C793C793C79
          3C793C793C793C793C793C793C793C793C793C793C792752C96E3C793C793C79
          3C793C793C793C799452945294529452945294523C793C79485A2B7B3C793C79
          3C793C793C793C793C799452945294529452945294523C796662067F047F4362
          435E235A235A03563C79E73CE73CE63CC63CC63CC638C6383C79D65AD65AD65A
          D65AD65AD65AD65AD65A3C79B556B556B556B4529452945294523C79064EC96A
          C96A275227522752275227523C79C648C648C64CC64CC64CC64CC64C3C79475A
          0A770A77485A485A485A485A485A3C79E750E750E750E7500755085508558962
          4C7F4B7F297F287F067F047FE47AE3763C796B5DCD79AD75AD75AC71AC718C71
          F75ED65AD65AD65AD65AD65AD65AD65AD65A3C79B556B556B556B556B556B556
          B556E5498766A866A86AA86AC96AC96EC96EC96E3C798434E760086508650865
          086508652756E972E9720A770A770A770A772B7B2B7B3C79A53C296D4A6D4A6D
          4A6D4A6D4A6D89624C7F4C7F4C7F4C7F4B7F297F287F067F3C798C61EF7DEE79
          CE79CD79AD75AD75F75EF75EF75EF75EF75ED65AD65AD65AD65A3C79D65AD65A
          D65AD656B556B556B556C4456662876287668866A866A86AA96AC96A3C798434
          E760E760E7600865086508650652C86EC972E972E972EA760A770A770A773C79
          A538296D296D296D4A6D4A6D4A6D3C7989624C7F4C7F89628962896289628862
          3C798C61EF7DEF7DEF7DEF7DEE79CE793C791863F75EF75EF75EF75EF75EF75E
          F75E3C79D65AD65AD65AD65AD65AD65AD65A3C79A445455E665EC549E549E54D
          E64D064E3C798434E760E760E760E760E76008653C790552A86EC86E26562756
          2756475647563C79A53829692969296D296D296D4A6D3C793C7989624C7F3C79
          3C793C793C793C793C798C618C618C618C618C618C618C613C793C7918631863
          3C793C793C793C793C793C79F75EF75EF75EF75ED65AD65AD65A3C793C79A341
          455A3C793C793C793C793C793C7963348434843484348434843484343C793C79
          E54DA76A3C793C793C793C793C793C79A538A538A538A538A538A53CA53C3C79
          9452945289629452945294529452945294529452945294529452945294523C79
          3C793C793C7918633C793C793C793C793C793C793C793C793C793C793C793C79
          3C793C7994529452A34194529452945294529452945294529452945294529452
          94523C793C7994529452E54D9452945294529452945294529452945294529452
          945294523C791042104210421042104210421042104210421042104210421042
          1042104294523C79396739673967396739673967396718631863186318631863
          18631863F7623C793C79CE39CE39EF3DEF3DEF3DEF3DEF3DEF3D104210421042
          104210421042314694523C79323E323E32423242524252425242524253425346
          5346534653465346734694523C791863FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7F104294523C795A6B5A6B5A6B39673967396739673967
          39673967396739631863186318633C793C794A29734E734E734E945294529452
          94529452B556B556B556B556D55A104294523C798D2DD752F852F856F856F856
          F856F8561957195B195B195B195B195B534694523C791863FF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F104294523C795A6B5A6B5A6B5A6B
          5A6B5A6B5A6B596B39673967396739673967396739673C793C794A29524A734E
          734E734E734E94529452945294529452B556B556B556104294523C798D2DD752
          D752D752F856F856F856F856F856F8561957195B195B195B534694523C791863
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F104294523C79
          7B6F7B6F5A6B5A6B5A6B5A6B5A6B5A6B5A6B5A6B5A6B5A673967396739673C79
          3C794A29524A524A734E734E734E734E734E94529452945294529452B5561042
          94523C798D2DD752D752D752D752D752F852F856F856F856F856F856195B195B
          534294523C791863186318631863186318631863186318631863186318631863
          186310423C793C797B6F7B6F7B6F7B6F7B6F7A6F5A6B5A6B5A6B5A6B5A6B5A6B
          5A6B5A6B5A6B3C793C794A294A294A294A294A294A294A294A294A296B2D6B2D
          6B2D6B2D6B2DEF3D3C793C798C2D8C2D8D2D8D2D8D2D8D2D8D2D8D2D8D2DAD2D
          AD2DAD31AD31AD3152423C793C793C7994529452945294529452945294529452
          94529452945294529452945294523C793C793C793C793C793C793C793C793C79
          3C793C793C793C793C793C793C793C793C793C79945294529452945294529452
          9452945294529452945294529452945294523C793C7994529452945294529452
          94529452945294529452945294529452945294523C7910421042104210421042
          104210421042104210421042104210421042104294523C797B6F7B6F7B6F7B6F
          7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7A6B3C793C79AD35AD35AD35
          AD35AD35AD35AD35CE39CE39CE39CE39CE39CE39EE39EF3D94523C79113A113A
          113A113A113A113E113E113E313E313E323E323E323E323E324294523C79FF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F104294523C79
          7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F3C79
          3C7931463146314631463146524A524A524A524A524A524A734E734E734ECE39
          94523C79B64EB64EB64EB64EB64EB64ED64ED74ED752D752D752D752D752D752
          323E94523C79FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F104294523C797B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F7B6F
          7B6F7B6F7B6F3C793C7931463146314631463146524A524A524A524A524A524A
          734E734E734ECE3994523C79B64EB64EB64EB64EB64EB64ED64ED74ED752D752
          D752D752D752D752323E94523C79}
        NumGlyphs = 4
        OnClick = sbToQueueClick
      end
    end
  end
  object BitBtn1: TBitBtn
    Left = 658
    Top = 471
    Width = 48
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Close'
    DoubleBuffered = True
    ModalResult = 2
    ParentDoubleBuffered = False
    TabOrder = 4
  end
  object btnTest: TButton
    Left = 298
    Top = 471
    Width = 114
    Height = 25
    Anchors = [akBottom]
    Caption = 'Start the test'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
    OnClick = btnTestClick
  end
  object btnSettings: TBitBtn
    Left = 570
    Top = 471
    Width = 87
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Settings'
    DoubleBuffered = True
    Glyph.Data = {
      A6020000424DA602000000000000420000002800000011000000110000000100
      10000300000064020000120B0000120B00000000000000000000007C0000E003
      00001F000000DC7D744EDC7D744E104210421042104210421042104210421042
      1042744EDC7DDC7D0000DC7DDC7D744EDC7D744E744E744E744E406E80498049
      80490000EF3D4A29DC7DDC7D0000DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D406E
      517F517F8049EF3DF75EF75E734EDC7D0000DC7DDC7DDC7DDC7DDC7DDC7DDC7D
      DC7D406EC97E8049406EEF3D734E734E1042DC7D0000DC7DDC7DDC7DDC7DDC7D
      DC7DDC7DDC7D406EA07EA07E4A299C7310421042DC7DDC7D0000DC7DDC7DDC7D
      DC7DDC7DDC7DDC7DDC7D406EA07E4A299C731042DC7DDC7DDC7DDC7D0000DC7D
      DC7DDC7DDC7DDC7DDC7DDC7DDC7DA07E4A299C73F75EDC7DDC7DDC7DDC7DDC7D
      0000DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D4A299C73F75E8049DC7DDC7DDC7D
      DC7DDC7D0000DC7DDC7DDC7DDC7DDC7DDC7D20392039F75E1042767F8049DC7D
      DC7DDC7DDC7DDC7D0000DC7DDC7DDC7DDC7DDC7D20390758D67E2039C97E8049
      406EDC7DDC7DDC7DDC7DDC7D0000DC7DDC7DDC7DDC7D2039B17DD67EB17E2039
      BA7F767F8049DC7DDC7DDC7DDC7DDC7D0000DC7DDC7DDC7D2039D67E206DB17E
      2039A07EBA7FBA7F8049DC7DDC7DDC7DDC7DDC7D0000DC7DDC7D20390758007C
      B17E2039DC7D517FC97E8049406EDC7DDC7DDC7DDC7DDC7D0000DC7DDC7D2039
      007CB17E2039DC7DDC7DA07EBA7FBA7F8049DC7DDC7DDC7DDC7DDC7D0000DC7D
      DC7DDC7D20392039DC7DDC7DDC7DA07EBA7FBA7F8049DC7DDC7DDC7DDC7DDC7D
      0000DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DC97E517F517F8049DC7DDC7DDC7D
      DC7DDC7D0000DC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7DDC7D
      DC7DDC7DDC7DDC7D0000}
    ParentDoubleBuffered = False
    TabOrder = 3
    OnClick = btnSettingsClick
  end
  object pHack: TPanel
    Left = 2
    Top = 472
    Width = 108
    Height = 22
    Anchors = [akLeft, akBottom]
    BevelOuter = bvNone
    TabOrder = 6
    Visible = False
    object btnHack: TButton
      Left = 1
      Top = 1
      Width = 106
      Height = 21
      Caption = 'Remove last test'
      TabOrder = 0
      OnClick = btnHackClick
    end
  end
  object pmList: TPopupMenu
    Left = 480
    Top = 9
  end
  object pmItems: TPopupMenu
    Left = 512
    Top = 9
    object miPriority: TMenuItem
      Caption = 'Priority'
      Hint = 'Set importance of item for inclusion in next test'
      object N101: TMenuItem
        Tag = 9
        Caption = 'Highest'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8249
        OnClick = N11Click
      end
      object N91: TMenuItem
        Tag = 8
        Caption = 'Very High'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8248
        OnClick = N11Click
      end
      object N81: TMenuItem
        Tag = 7
        Caption = 'High'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8247
        OnClick = N11Click
      end
      object N61: TMenuItem
        Tag = 6
        Caption = 'Higher'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8246
        OnClick = N11Click
      end
      object N51: TMenuItem
        Tag = 5
        Caption = 'Normal'
        Checked = True
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8245
        OnClick = N11Click
      end
      object N41: TMenuItem
        Tag = 4
        Caption = 'Lower'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8244
        OnClick = N11Click
      end
      object N31: TMenuItem
        Tag = 3
        Caption = 'Low'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8243
        OnClick = N11Click
      end
      object N21: TMenuItem
        Tag = 2
        Caption = 'Very Low'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8242
        OnClick = N11Click
      end
      object N11: TMenuItem
        Tag = 1
        Caption = 'Lowest'
        GroupIndex = 1
        RadioItem = True
        ShortCut = 8241
        OnClick = N11Click
      end
    end
    object N5: TMenuItem
      Caption = '-'
    end
    object miHint: TMenuItem
      Caption = 'Main hint'
      Hint = 'Select which part of the word should be shown for this item'
      object miHintDefault: TMenuItem
        Tag = 3
        Caption = 'Default'
        Checked = True
        GroupIndex = 1
        OnClick = miHintDefaultClick
      end
      object miHintWritten: TMenuItem
        Caption = 'Written'
        GroupIndex = 1
        OnClick = miHintDefaultClick
      end
      object miHintKana: TMenuItem
        Tag = 1
        Caption = 'Kana'
        GroupIndex = 1
        OnClick = miHintDefaultClick
      end
      object miHintMeaning: TMenuItem
        Tag = 2
        Caption = 'Meaning'
        GroupIndex = 1
        OnClick = miHintDefaultClick
      end
    end
    object miAddItem: TMenuItem
      Caption = 'Add question...'
      Hint = 'Add other items with the same word but different part asked'
      OnClick = miAddItemClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object miRemoveItem: TMenuItem
      Caption = 'Remove'
      Hint = 'Remove item from long-term study list'
      OnClick = miRemoveItemClick
    end
    object miToQueue: TMenuItem
      Caption = 'Back to queue'
      Hint = 
        'Moves the selected items back to the queued list and resets thei' +
        'r data.'
      OnClick = miToQueueClick
    end
    object N2: TMenuItem
      Caption = '-'
    end
    object miStudyOptions: TMenuItem
      Caption = 'Study options'
      object miDeckInc: TMenuItem
        Tag = 1
        Caption = 'Deck+'
        OnClick = miDeckIncClick
      end
      object miDeckDec: TMenuItem
        Tag = -1
        Caption = 'Deck-'
        OnClick = miDeckIncClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object miResetData: TMenuItem
        Caption = 'Reset study data'
        OnClick = miResetDataClick
      end
    end
    object N4: TMenuItem
      Caption = '-'
    end
    object miToGroup: TMenuItem
      Caption = 'Add to word group...'
      OnClick = miToGroupClick
    end
  end
  object ImageList1: TImageList
    Height = 15
    Width = 15
    Left = 296
    Top = 108
    Bitmap = {
      494C01010200080000010F000F00FFFFFFFFFF10FFFFFFFFFFFFFFFF424D3600
      00000000000036000000280000003C0000000F0000000100200000000000100E
      00000000000000000000000000000000000000000000C4C4C400A9A9A900A9A9
      A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9
      A900A9A9A900A9A9A900A9A9A90000000000C4C4C400A9A9A900A9A9A900A9A9
      A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9A900A9A9
      A900A9A9A900A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFD2BC00DFD1BB00DFD1B900DFD0B800DFD0B700DFCFB500DFCFB400DFCE
      B300DFCEB200DFCEB000DFCDAF00DFCDAE00DFCCAD00DFCCAC00A9A9A9000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000A9A9A900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A195DF00FFF3DE00DFD2BC00FFF2
      DB00FFF1DA00FFF1D900FFF0D700FFF0D600FFF0D500FFEFD300DFCEB200FFEE
      D100A190D000FFEDCF00A9A9A900FFFFFF00FFFFFF00DA9F3E00966C2A00D0D0
      D0005252FF001818F800D0D0D000DA9C3600966A2400D0D0D000DA9B3200966B
      2200D0D0D000A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFD4C200DFD3C100DFD3BF00DFD2BE00DFD2BC00DFD1BB00DFD1BA00DFD0
      B800DFD0B700DFCFB600DFCFB400DFCEB300DFCEB200DFCEB100A9A9A9000000
      0000E3E3E300FFCC7D00BF8A3A00ABABAB009E9EFF003E3EFF00ABABAB00FFCA
      7700BF8B3400ABABAB00FFCA7300BF893000ABABAB00A9A9A900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A197E600FFF5E400DFD4C200FFF4
      E100FFF3E000FFF3DE00FFF2DD00FFF2DC00FFF1DA00FFF1D900DFD0B700A192
      D600FFF0D500A191D400A9A9A900FFFFFF00FFFFFF00FFCD8200BF8D4000C0C0
      C000A0A0FF004242FF00C0C0C000FFE1B300FFBF5C00D0D0D000FFCA7900BF8B
      3600C0C0C000A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFD6C800DFD5C700DFD5C500DFD4C400DFD4C300DFD3C100DFD3C000DFD2
      BE00DFD2BD00DFD1BC00DFD1BA00DFD1B800DFD0B800DFCFB600A9A9A9000000
      0000E3E3E300FFD18800BF8E4600ABABAB00A2A2FF004747FF00ABABAB00CCCC
      CC00D5D5D500D5D5D500FFCD7F00BF8B3C00ABABAB00A9A9A900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A199EB00FFF6EA00DFD6C800FFF6
      E700FFF5E600FFF5E400FFF4E300FFF4E200FFF3E000FFF3DF00DFD2BD00FFF2
      DC00A194DB00FFF1DA00A9A9A900FFFFFF00FFFFFF00FFD28D00BF8F4C00C0C0
      C000A6A6FF004B4BFF00C0C0C000EAEAEA00FFFFFF00FFFFFF00FFCD8400BF8E
      4100C0C0C000A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFD7CD00DFD7CC00DFD7CB00DFD6C900DFD6C800DFD5C700DFD5C500DFD4
      C400DFD4C300DFD3C100DFD3C000DFD2BE00DFD2BD00DFD2BC00A9A9A9000000
      0000E3E3E300FFD29200BF925000ABABAB00AAAAFF004C4CFF00ABABAB00D0D0
      D000E3E3E300E3E3E300FFD08900BF914600ABABAB00A9A9A900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A19BF000FFF8F000DFD7CE00FFF8
      ED00FFF7EB00FFF7EA00FFF6E900FFF6E700FFF5E600FFF5E500DFD4C300FFF4
      E200A195E100FFF3DF00A9A9A900FFFFFF00FFFFFF00FFD19700BF935500C0C0
      C000ACACFF004F4FFF00C0C0C000EAEAEA00FFFFFF00FFFFFF00FFD38E00BF91
      4C00C0C0C000A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFD9D200DFD9D100DFD9D000DFD8CF00DFD8CE00DFD7CC00DFD7CB00DFD6
      CA00DFD6C900DFD5C700DFD5C600DFD5C500DFD4C300DFD4C200A9A9A9000000
      0000E3E3E300FFE5C500FFC98300B9B9B900B2B2FF005353FF00ABABAB00D0D0
      D000E3E3E300E3E3E300FFD39300BF915100ABABAB00A9A9A900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000C699B500FED0B400EABC9F00FECF
      B200FECFB100FECFB000FECEAE00FECEAD00FECDAC00FECDAA00EAB99500C695
      A800FECCA600FECBA500A9A9A900FFFFFF00FFFFFF00FAFAFA00EFEFEF00EFEF
      EF00B0B0FF005353FF00C0C0C000EAEAEA00FFFFFF00FFFFFF00FFD39800BF94
      5600C0C0C000A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFDBD700DFDBD600DFDAD500DFDAD300DFD9D200DFD9D200DFD9D000DFD8
      CF00DFD8CE00DFD7CD00DFD7CC00DFD7CA00DFD6C900DFD6C800A9A9A9000000
      0000E3E3E300E3E3E300E3E3E300E3E3E300B2B2FF005656FF00ABABAB00D0D0
      D000E3E3E300E3E3E300FFE6C600FFC78400B9B9B900A9A9A900000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A19EF800FFFCF800DFDBD700FFFB
      F600FFFAF500FFFAF400FFFAF300FFF9F200FFF9F100FFF8F000DFD8CE00FFF8
      ED00A199EC00FFF7EB00A9A9A900FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00B2B2FF005151FF00C0C0C000EAEAEA00FFFFFF00FFFFFF00FAFAFA00EFEF
      EF00EFEFEF00A9A9A90000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000DFDBD900DFDBD800DFDBD800DFDBD700DFDBD700DFDBD600DFDAD500DFDA
      D400DFD9D300DFD9D200DFD9D100DFD8CF00DFD8CE00DFD7CD00C4C4C4000000
      0000E3E3E300E3E3E300E3E3E300E3E3E300B2B2FF005555FF00ABABAB00D0D0
      D000E3E3E300E3E3E300E3E3E300E3E3E300E3E3E300C4C4C400000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000A19EF900FFFCF900DFDBD900FFFC
      F900FFFCF800FFFCF800FFFBF700FFFBF600FFFBF500FFFAF400DFD9D300A19C
      F300FFF9F100A19BF00000000000FFFFFF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00D2D2FF009F9FFF00D0D0D000EFEFEF00FFFFFF00FFFFFF00FFFFFF00FFFF
      FF00FFFFFF000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000424D3E000000000000003E000000280000003C0000000F00000001000100
      00000000780000000000000000000000000000000000000000000000FFFFFF00
      8001000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000020004000000000000000000000000
      0000000000000000000000000000}
  end
end
