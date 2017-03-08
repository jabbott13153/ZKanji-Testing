object frmJLPTEdit: TfrmJLPTEdit
  Left = 505
  Top = 341
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'JLPT vocabulary list editor'
  ClientHeight = 472
  ClientWidth = 742
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  Menu = main
  OldCreateOrder = False
  Position = poScreenCenter
  OnCloseQuery = FormCloseQuery
  OnKeyDown = FormKeyDown
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    742
    472)
  PixelsPerInch = 96
  TextHeight = 13
  object b1: TBevel
    Left = 0
    Top = 440
    Width = 99999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 421
  end
  object bt: TBevel
    Left = 0
    Top = 28
    Width = 999999
    Height = 1
    Shape = bsTopLine
  end
  object pb: TZPaintBox
    Top = 80
    Width = 742
    Height = 293
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    TabOrder = 1
    OnKeyPress = pbKeyPress
  end
  object btnClose: TBitBtn
    Left = 334
    Top = 445
    Width = 75
    Height = 25
    Anchors = [akBottom]
    Cancel = True
    Caption = 'Close'
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    NumGlyphs = 2
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 2
    OnClick = btnCloseClick
  end
  object sbInfo: TStatusBar
    Left = 0
    Top = 373
    Width = 65535
    Height = 19
    Align = alNone
    Anchors = [akLeft, akBottom]
    Panels = <
      item
        Alignment = taRightJustify
        Bevel = pbNone
        Text = '0'
        Width = 50
      end
      item
        Bevel = pbNone
        Text = ':  0'
        Width = 50
      end
      item
        Width = 99999
      end>
    SizeGrip = False
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 742
    Height = 28
    Anchors = [akLeft, akTop, akRight]
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 0
    DesignSize = (
      742
      28)
    object Label2: TLabel
      Left = 46
      Top = 8
      Width = 11
      Height = 13
      Caption = 'N:'
    end
    object sbN5: TSpeedButton
      Left = 62
      Top = 3
      Width = 16
      Height = 22
      AllowAllUp = True
      GroupIndex = 1
      Down = True
      Caption = '5'
      OnClick = sbN5Click
    end
    object sbN4: TSpeedButton
      Left = 78
      Top = 3
      Width = 16
      Height = 22
      AllowAllUp = True
      GroupIndex = 2
      Down = True
      Caption = '4'
      OnClick = sbN5Click
    end
    object sbN3: TSpeedButton
      Left = 94
      Top = 3
      Width = 16
      Height = 22
      AllowAllUp = True
      GroupIndex = 3
      Down = True
      Caption = '3'
      OnClick = sbN5Click
    end
    object sbN2: TSpeedButton
      Left = 110
      Top = 3
      Width = 16
      Height = 22
      AllowAllUp = True
      GroupIndex = 4
      Down = True
      Caption = '2'
      OnClick = sbN5Click
    end
    object sbN1: TSpeedButton
      Left = 126
      Top = 3
      Width = 16
      Height = 22
      AllowAllUp = True
      GroupIndex = 5
      Down = True
      Caption = '1'
      OnClick = sbN5Click
    end
    object Label3: TLabel
      Left = 148
      Top = 8
      Width = 27
      Height = 13
      Caption = 'Area:'
    end
    object sbAreaFixed: TSpeedButton
      Left = 178
      Top = 3
      Width = 29
      Height = 22
      AllowAllUp = True
      GroupIndex = 6
      Down = True
      Caption = 'Fx'
      OnClick = sbN5Click
    end
    object sbAreaFound: TSpeedButton
      Left = 207
      Top = 3
      Width = 29
      Height = 22
      AllowAllUp = True
      GroupIndex = 7
      Down = True
      Caption = 'Fnd'
      OnClick = sbN5Click
    end
    object sbAreaMatch: TSpeedButton
      Left = 236
      Top = 3
      Width = 29
      Height = 22
      AllowAllUp = True
      GroupIndex = 8
      Down = True
      Caption = 'Mch'
      OnClick = sbN5Click
    end
    object Label4: TLabel
      Left = 347
      Top = 8
      Width = 37
      Height = 13
      Caption = '&Search:'
      FocusControl = edKanji
    end
    object Bevel1: TBevel
      Left = 339
      Top = 5
      Width = 2
      Height = 19
      Shape = bsLeftLine
    end
    object SpeedButton1: TSpeedButton
      Left = 3
      Top = 3
      Width = 38
      Height = 22
      Caption = 'reset'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentFont = False
      OnClick = SpeedButton1Click
    end
    object Label6: TLabel
      Left = 272
      Top = 8
      Width = 24
      Height = 13
      Caption = 'Klen:'
    end
    object edKanji: TZPaintBox
      Left = 388
      Top = 2
      Width = 352
      Height = 24
      Cursor = crIBeam
      Anchors = [akLeft, akTop, akRight]
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -16
      Font.Name = 'MS Gothic'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnKeyDown = edKanjiKeyDown
    end
    object edKLen: TEdit2
      Left = 299
      Top = 5
      Width = 36
      Height = 21
      TabOrder = 1
      OnChange = sbN5Click
    end
  end
  object pButtons: TPanel
    Left = 0
    Top = 411
    Width = 742
    Height = 29
    Anchors = [akLeft, akRight, akBottom]
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 4
    ExplicitTop = 392
    ExplicitWidth = 769
    DesignSize = (
      742
      29)
    object Bevel2: TBevel
      Left = 0
      Top = 0
      Width = 99999
      Height = 2
      Anchors = [akLeft, akBottom]
      Shape = bsTopLine
    end
    object lbMeaning: TLabel
      Left = 182
      Top = 8
      Width = 44
      Height = 13
      Caption = '&Meaning:'
      FocusControl = edMeaning
    end
    object btnSave: TSpeedButton
      Left = 545
      Top = 4
      Width = 36
      Height = 22
      Anchors = [akTop, akRight]
      Caption = 'Save'
      OnClick = btnSaveClick
      ExplicitLeft = 572
    end
    object btnOrig: TSpeedButton
      Left = 582
      Top = 4
      Width = 47
      Height = 22
      Anchors = [akTop, akRight]
      Caption = '&Original'
      OnClick = btnOrigClick
      ExplicitLeft = 609
    end
    object btnDict: TSpeedButton
      Left = 630
      Top = 4
      Width = 63
      Height = 22
      Anchors = [akTop, akRight]
      Caption = '&Dict. def.'
      OnClick = btnDictClick
      ExplicitLeft = 657
    end
    object Label1: TLabel
      Left = 106
      Top = 8
      Width = 11
      Height = 13
      Caption = '&N:'
      FocusControl = cbN
    end
    object sbPrev: TSpeedButton
      Tag = -1
      Left = 694
      Top = 4
      Width = 23
      Height = 22
      Anchors = [akTop, akRight]
      Flat = True
      Glyph.Data = {
        4A090000424D4A09000000000000420000002800000044000000110000000100
        10000300000008090000C30E0000C30E00000000000000000000007C0000E003
        00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452A2159452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F734E
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452
        A6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        9452E8299452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        9452A215A2159452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F734E734EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F9452A625A6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7F9452E729E7299452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7F9452C319C42AA2159452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7F9452734E734EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7F9452A5214936A6259452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F9452C6250D4FE7299452FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F9452E425E632C52EA2159452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F945294529452734EFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452851D28324936A6259452FF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452C625EC460D4BE7299452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452062E093FE636E52EC2199452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556B55694529452734E
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452851D062A282E4932
        A6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452C525CA3EEB42
        0C4BE7299452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F945209364C4B2A43
        083BE532C3199452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD65AB556
        B55694529452734EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452841D
        E525062A282E2832A6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452
        A521A836C93AEA42EC46C7299452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452
        2B3A6F5B4D4F2A47093FE636C3199452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FF75ED65AD65AB556B55694529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7F9452641DC421E525052A062E282EA5219452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F9452A51D8632A836C93ACA3EEB46C6299452FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7F4D429363905B6E534B4B2943083BC3199452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7F1863F75EF75ED65AB556B55694529452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7F6319C421C421E525E529062A282E85219452FF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FA41D852A862EA832A93ACA3EEB42C6259452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7F6E42B76FB467925F6F574C4F2A47083BC4219452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7F396718631863F75ED65AD65AB55694529452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F020DA319C319C421E425E525062A262E
        85219452FF7FFF7FFF7FFF7FFF7FFF7FFF7F6215642A652A852EA632A836C93A
        EA3EC6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6D42B56B9363905B6D53
        4B47293FC4219452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F39671863F75EF75E
        D65AB556B5569452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F020DC319C421
        C421E525052A062A85219452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6215642A
        852E8632A836C93ACA3EC6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        6D42B467915F6E574C4F2A43E4259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7F18631863F75ED65AD65AB5569452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F0211C419C421E425E529062A85219452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7F8315852A862EA632A836CA3AC6259452FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7F6D3E9363705B4D532B47E5259452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F1863F75ED65AD65AB556B556FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7F2211C421C421E525062A851D9452FF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F8315852E8632A836C93AC6259452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6C3E915F6E534C4BE5299452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EF75ED65AB556B556
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F2211C421E525052A
        851D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F83198632A836
        C93AC6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6B3A
        6F574D4F062A9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        F75ED65AD65AB556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7F2211E425E525851D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F8319A632A836C6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7F4A366E53062E9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FF75ED65AB556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F2311E525851D9452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FA419A836C5259452FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F4A3608329452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD65AD65AFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F2315851D9452FF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FA421C5259452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F4936FF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD65A
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        4315FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7F8319FF7FFF7FFF7FFF7FFF7F}
      NumGlyphs = 4
      OnMouseDown = sbNextMouseDown
      ExplicitLeft = 721
    end
    object sbNext: TSpeedButton
      Tag = 1
      Left = 717
      Top = 4
      Width = 23
      Height = 22
      Anchors = [akTop, akRight]
      Flat = True
      Glyph.Data = {
        4A090000424D4A09000000000000420000002800000044000000110000000100
        10000300000008090000C30E0000C30E00000000000000000000007C0000E003
        00001F000000FF7FFF7FFF7FFF7F6B3E94529452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD65AFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F031194529452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F841D94529452FF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6C42E72D94529452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD65AD65AFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F031187219452
        9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F8419082E
        94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6D42
        4C4FE62994529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        F75ED65AB556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7F0211C521872594529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F6319872E082E94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7F8D464D532B4BC52594529452FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FF75ED65AD65AB556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7F0211A421C521A72594529452FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F6319662E8732082E94529452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7F8E464F572C4B0A43C52194529452FF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75ED65AD65AB556B556FF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F0211A41DC521E525A72594529452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6319662E8732A83609329452
        9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F8F4A705B4D4F2B47093F
        C42194529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EF75ED65AB556
        B5569452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F0211A41DC421
        C521E625A82994529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6319662A
        862E8732A836293694529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F8F4A
        715B4E572C4B0943E83AA41D94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        1863F75ED65AD65AB556B5569452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7F0211A41DA41DC521E525E629A82994529452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F6315652A662E8732A836A93A2A3694529452FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FB04E725F6F574D4F2A47083FE736A31994529452FF7FFF7FFF7F
        FF7FFF7FFF7FFF7F1863F75EF75ED65AB556B55694529452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FE210A41DA41DC521C525E625072AA82994529452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7F63156526662A872E8836A93ACA3E2A3694529452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FB14E9363715B4E532B4B0943E736C632A319
        9452FF7FFF7FFF7FFF7FFF7FFF7FFF7F18631863F75ED65AD65AB55694529452
        524AFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FE210A319A41DC421C521E625062A
        072EC82D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7F63154526652A862E8732A836
        C93ACB422B3A9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FB14E9467725F6F574C4F
        2A43083BA41D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F39671863F75EF75E
        D65AB556B5569452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FE20CA319A41D
        A41DC521E525E62544199452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F42154426
        652A662E8732A836A93AA6259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB252
        956B9363705B4D532B4BC5259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        396718631863F75ED65AD65AB556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FE20C8319A419A41DC421C52524199452FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F421544266526662A862E8732A5219452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FB252B66B9463725F4F57E72D9452FF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7F396718631863F75ED65AD65AFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FE20C8319A319A41DA41D23159452FF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F421544264526652A662E841D9452FF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FB352B76F9567725F09369452FF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F396739671863F75EF75EFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FE10C8319A319A41D02119452FF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F421124224426652684199452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD356B86FB66B2C3E9452
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F3967396718631863
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FE10C83158319
        02119452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F42112422
        442663199452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FD356
        B9732E429452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        396739673967FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FE10C8315E20C9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7F4211242262159452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FD4564F469452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7F5A6B3967FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FE10CE10C9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7F421142119452FF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FD456FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F5A6BFF7FFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FE10CFF7FFF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F4211FF7FFF7FFF7FFF7FFF7F
        FF7FFF7FFF7FFF7FFF7FFF7FFF7F}
      NumGlyphs = 4
      OnMouseDown = sbNextMouseDown
      ExplicitLeft = 744
    end
    object Bevel4: TBevel
      Left = 158
      Top = 5
      Width = 2
      Height = 19
      Shape = bsLeftLine
    end
    object btnDelMeaning: TSpeedButton
      Left = 162
      Top = 5
      Width = 17
      Height = 20
      Caption = 'X'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      OnClick = btnDelMeaningClick
    end
    object Bevel3: TBevel
      Left = 100
      Top = 5
      Width = 2
      Height = 19
      Shape = bsLeftLine
    end
    object btnAlt: TSpeedButton
      Left = 58
      Top = 4
      Width = 39
      Height = 21
      Caption = 'Alt'
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
      Spacing = -1
      OnClick = btnAltClick
    end
    object Bevel10: TBevel
      Left = 54
      Top = 5
      Width = 2
      Height = 19
      Shape = bsLeftLine
    end
    object btnL: TSpeedButton
      Left = 2
      Top = 4
      Width = 16
      Height = 21
      Hint = 'Mark for level change'
      AllowAllUp = True
      GroupIndex = 9
      Caption = 'L'
      OnClick = btnLClick
    end
    object btnK: TSpeedButton
      Left = 18
      Top = 4
      Width = 16
      Height = 21
      Hint = 'Mark for kanji variant create'
      AllowAllUp = True
      GroupIndex = 9
      Caption = 'K'
      OnClick = btnLClick
    end
    object btnF: TSpeedButton
      Left = 34
      Top = 4
      Width = 16
      Height = 21
      Hint = 'Mark for kanji variant create'
      AllowAllUp = True
      GroupIndex = 9
      Caption = 'F'
      OnClick = btnLClick
    end
    object edMeaning: TEdit2
      Left = 232
      Top = 5
      Width = 309
      Height = 21
      Anchors = [akLeft, akTop, akRight]
      TabOrder = 0
      OnChange = edMeaningChange
      OnKeyDown = edKanjiKeyDown
      OnKeyPress = edMeaningKeyPress
    end
    object cbN: TComboBox
      Left = 121
      Top = 4
      Width = 34
      Height = 21
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 1
      TabStop = False
      Text = '5'
      OnChange = cbNChange
      Items.Strings = (
        '5'
        '4'
        '3'
        '2'
        '1')
    end
  end
  object Panel2: TPanel
    Left = 0
    Top = 441
    Width = 217
    Height = 30
    Anchors = [akLeft, akBottom]
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 5
    DesignSize = (
      217
      30)
    object Bevel5: TBevel
      Left = 45
      Top = 15
      Width = 166
      Height = 14
      Anchors = [akTop, akRight]
    end
    object lbN5: TLabel
      Left = 182
      Top = 15
      Width = 24
      Height = 13
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '6666'
      ExplicitLeft = 134
    end
    object lb5: TLabel
      Left = 187
      Top = 4
      Width = 14
      Height = 11
      Anchors = [akTop, akRight]
      Caption = 'N5'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitLeft = 139
    end
    object lb3: TLabel
      Left = 123
      Top = 4
      Width = 14
      Height = 11
      Anchors = [akTop, akRight]
      Caption = 'N3'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitLeft = 75
    end
    object lb4: TLabel
      Left = 155
      Top = 4
      Width = 14
      Height = 11
      Anchors = [akTop, akRight]
      Caption = 'N4'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitLeft = 107
    end
    object lb2: TLabel
      Left = 90
      Top = 4
      Width = 14
      Height = 11
      Anchors = [akTop, akRight]
      Caption = 'N2'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      ExplicitLeft = 42
    end
    object lb1: TLabel
      Left = 56
      Top = 4
      Width = 14
      Height = 11
      Anchors = [akTop, akRight]
      Caption = 'N1'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -9
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbN3: TLabel
      Left = 118
      Top = 15
      Width = 24
      Height = 13
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '6666'
      ExplicitLeft = 70
    end
    object lbN4: TLabel
      Left = 150
      Top = 15
      Width = 24
      Height = 13
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '6666'
      ExplicitLeft = 102
    end
    object lbN2: TLabel
      Left = 85
      Top = 15
      Width = 24
      Height = 13
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '6666'
      ExplicitLeft = 37
    end
    object lbN1: TLabel
      Left = 48
      Top = 15
      Width = 31
      Height = 13
      Alignment = taCenter
      Anchors = [akTop, akRight]
      AutoSize = False
      Caption = '6666'
    end
    object Bevel6: TBevel
      Left = 177
      Top = 19
      Width = 2
      Height = 6
      Anchors = [akTop, akRight]
      ExplicitLeft = 129
    end
    object Bevel7: TBevel
      Left = 145
      Top = 19
      Width = 2
      Height = 6
      Anchors = [akTop, akRight]
      ExplicitLeft = 97
    end
    object Bevel8: TBevel
      Left = 80
      Top = 19
      Width = 2
      Height = 6
      Anchors = [akTop, akRight]
      ExplicitLeft = 32
    end
    object Bevel9: TBevel
      Left = 112
      Top = 19
      Width = 2
      Height = 6
      Anchors = [akTop, akRight]
      ExplicitLeft = 64
    end
    object Label28: TLabel
      Left = 8
      Top = 0
      Width = 35
      Height = 13
      Caption = 'Words:'
    end
    object cbSum: TCheckBox
      Left = 5
      Top = 13
      Width = 41
      Height = 17
      Anchors = [akLeft, akBottom]
      Caption = 'SUM'
      TabOrder = 0
      OnClick = cbSumClick
    end
  end
  object pAuto: TPanel
    Left = 0
    Top = 29
    Width = 65535
    Height = 51
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 6
    object btnAuto: TSpeedButton
      Left = 24
      Top = 21
      Width = 41
      Height = 22
      Caption = 'Start!'
      OnClick = btnAutoClick
    end
    object Label10: TLabel
      Left = 8
      Top = 5
      Width = 80
      Height = 13
      Caption = 'Autocompute N3'
    end
    object Label11: TLabel
      Left = 104
      Top = 16
      Width = 27
      Height = 13
      Caption = 'Kanji:'
    end
    object Label12: TLabel
      Left = 138
      Top = 6
      Width = 55
      Height = 13
      Caption = 'N wrd freq:'
    end
    object Label13: TLabel
      Left = 225
      Top = 6
      Width = 39
      Height = 13
      Caption = 'ex freq:'
    end
    object Label14: TLabel
      Left = 298
      Top = 6
      Width = 60
      Height = 13
      Caption = 'kanji w freq:'
    end
    object Label15: TLabel
      Left = 137
      Top = 30
      Width = 49
      Height = 13
      Caption = 'kanji freq:'
    end
    object Label16: TLabel
      Left = 221
      Top = 30
      Width = 46
      Height = 13
      Caption = 'word cnt:'
    end
    object Label17: TLabel
      Left = 400
      Top = 16
      Width = 30
      Height = 13
      Caption = 'Word:'
    end
    object Label18: TLabel
      Left = 442
      Top = 6
      Width = 51
      Height = 13
      Caption = 'kanji JLPT:'
    end
    object Label19: TLabel
      Left = 529
      Top = 6
      Width = 49
      Height = 13
      Caption = 'kanji freq:'
    end
    object Label20: TLabel
      Left = 440
      Top = 30
      Width = 51
      Height = 13
      Caption = 'word freq:'
    end
    object Label21: TLabel
      Left = 530
      Top = 30
      Width = 39
      Height = 13
      Caption = 'ex freq:'
    end
    object Label22: TLabel
      Left = 303
      Top = 30
      Width = 38
      Height = 13
      Caption = 'N3 kanji'
    end
    object Bevel11: TBevel
      Left = 0
      Top = 50
      Width = 999999
      Height = 1
      Shape = bsTopLine
    end
    object Label5: TLabel
      Left = 621
      Top = 29
      Width = 70
      Height = 13
      Caption = 'min word freq:'
    end
    object edKwrd: TEdit2
      Left = 193
      Top = 3
      Width = 25
      Height = 21
      TabOrder = 0
      Text = '1'
    end
    object edKex: TEdit2
      Left = 269
      Top = 3
      Width = 25
      Height = 21
      TabOrder = 1
      Text = '10'
    end
    object edKKwf: TEdit2
      Left = 361
      Top = 3
      Width = 25
      Height = 21
      TabOrder = 2
      Text = '200'
    end
    object edKk: TEdit2
      Left = 193
      Top = 27
      Width = 25
      Height = 21
      TabOrder = 3
      Text = '20'
    end
    object edKwcnt: TEdit2
      Left = 269
      Top = 27
      Width = 25
      Height = 21
      TabOrder = 4
      Text = '5'
    end
    object edNkj: TEdit2
      Left = 495
      Top = 3
      Width = 25
      Height = 21
      TabOrder = 5
      Text = '1'
    end
    object edNkf: TEdit2
      Left = 583
      Top = 3
      Width = 25
      Height = 21
      TabOrder = 6
      Text = '30'
    end
    object edNwf: TEdit2
      Left = 495
      Top = 27
      Width = 25
      Height = 21
      TabOrder = 7
      Text = '80'
    end
    object edNef: TEdit2
      Left = 583
      Top = 27
      Width = 25
      Height = 21
      TabOrder = 8
      Text = '1'
    end
    object edKN3: TEdit2
      Left = 361
      Top = 27
      Width = 25
      Height = 21
      TabOrder = 9
      Text = '365'
    end
    object cbExclude: TCheckBox
      Left = 624
      Top = 8
      Width = 113
      Height = 17
      Caption = 'Exclude kanaonly'
      TabOrder = 10
    end
    object edNminf: TEdit2
      Left = 695
      Top = 27
      Width = 42
      Height = 21
      TabOrder = 11
      Text = '1000'
    end
  end
  object Panel3: TPanel
    Left = 526
    Top = 442
    Width = 217
    Height = 30
    Anchors = [akRight, akBottom]
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 7
    DesignSize = (
      217
      30)
    object lbK1: TLabel
      Left = 40
      Top = 0
      Width = 5
      Height = 17
      Anchors = [akTop, akRight]
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Meiryo'
      Font.Style = []
      ParentFont = False
    end
    object Label29: TLabel
      Left = 7
      Top = 1
      Width = 27
      Height = 13
      Caption = 'Kanji:'
    end
    object lbK2: TLabel
      Left = 72
      Top = 0
      Width = 5
      Height = 17
      Anchors = [akTop, akRight]
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Meiryo'
      Font.Style = []
      ParentFont = False
    end
    object lbK3: TLabel
      Left = 104
      Top = 0
      Width = 5
      Height = 17
      Anchors = [akTop, akRight]
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Meiryo'
      Font.Style = []
      ParentFont = False
    end
    object lbK4: TLabel
      Left = 136
      Top = 0
      Width = 5
      Height = 17
      Anchors = [akTop, akRight]
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Meiryo'
      Font.Style = []
      ParentFont = False
    end
    object lbK5: TLabel
      Left = 168
      Top = 0
      Width = 5
      Height = 17
      Anchors = [akTop, akRight]
      Caption = '-'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Meiryo'
      Font.Style = []
      ParentFont = False
    end
  end
  object Panel4: TPanel
    Left = 0
    Top = 392
    Width = 65535
    Height = 19
    Anchors = [akLeft, akBottom]
    BevelOuter = bvNone
    TabOrder = 8
    object lbDefinition: TLabel
      Left = 80
      Top = 3
      Width = 999999
      Height = 13
      AutoSize = False
      Caption = '-'
    end
    object btnDefinition: TSpeedButton
      Left = 2
      Top = 0
      Width = 67
      Height = 18
      Caption = 'English'
      OnClick = btnDefinitionClick
    end
  end
  object main: TMainMenu
    Left = 16
    Top = 80
    object File1: TMenuItem
      Caption = '&File'
      object miOpen: TMenuItem
        Caption = '&Open'
        ShortCut = 16463
        OnClick = miOpenClick
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object miSave: TMenuItem
        Caption = '&Save'
        ShortCut = 16467
        OnClick = miSaveClick
      end
      object miSaveAs: TMenuItem
        Caption = 'Save &As...'
        ShortCut = 16449
        OnClick = miSaveAsClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object Export1: TMenuItem
        Caption = 'Export'
        OnClick = Export1Click
      end
    end
    object ools2: TMenuItem
      Caption = '&Tools'
      object AutoFix1: TMenuItem
        Caption = 'Auto Fix'
        OnClick = AutoFix1Click
      end
      object AllN3toN21: TMenuItem
        Caption = 'Convert N3 to N2'
        OnClick = AllN3toN21Click
      end
      object Katakanaautodef1: TMenuItem
        Caption = 'Auto def. katakana'
        OnClick = Katakanaautodef1Click
      end
      object Autodefshortword1: TMenuItem
        Caption = 'Auto def. short words'
        OnClick = Autodefshortword1Click
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object JumptoSame1: TMenuItem
        Caption = 'Jump to Same'
        ShortCut = 16458
        OnClick = JumptoSame1Click
      end
    end
    object Edit1: TMenuItem
      Caption = 'Edit'
      object miL: TMenuItem
        Caption = 'Level change'
        ShortCut = 16433
        OnClick = miLClick
      end
      object miK: TMenuItem
        Caption = 'Kanji change'
        ShortCut = 16434
        OnClick = miKClick
      end
      object miF: TMenuItem
        Caption = 'Fix in place'
        ShortCut = 16435
        OnClick = miFClick
      end
    end
    object miGroups: TMenuItem
      Caption = 'Groups'
    end
    object ools1: TMenuItem
      Caption = '&View'
      object miAutoN3: TMenuItem
        AutoCheck = True
        Caption = 'Autocompute bar'
        Checked = True
        ShortCut = 114
        OnClick = miAutoN3Click
      end
    end
  end
  object dlgOpen: TOpenDialog
    DefaultExt = 'zjv'
    Filter = 'zkanji JLPT vocabular file (*.zjv)|*.zjv'
    Options = [ofHideReadOnly, ofFileMustExist, ofEnableSizing]
    Title = 'Open JLPT vocabulary file'
    Left = 16
    Top = 128
  end
  object dlgSave: TSaveDialog
    DefaultExt = 'zjv'
    Filter = 'zkanji JLPT vocabular file (*.zjv)|*.zjv'
    Options = [ofOverwritePrompt, ofHideReadOnly, ofPathMustExist, ofEnableSizing]
    Title = 'Save JLPT vocabulary file'
    Left = 16
    Top = 176
  end
  object pmAlt: TPopupMenu
    AutoHotkeys = maManual
    Images = data.imgSmall
    Left = 16
    Top = 272
  end
end
