object fStats: TfStats
  Left = 661
  Top = 287
  BorderIcons = [biSystemMenu]
  Caption = 'Dictionary information: English'
  ClientHeight = 346
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
    0004040400000000000000000000000000000000000086858587858588868689
    87870000000000000000000000000707070B0B0B0F0F0F1313131817171C1B1B
    201F1F252424C85C28C95F2CCB63318A88883635353A39393E3D3D4342420000
    001212121818E0340BA7868585EAEAEAEBEBEBA2A2A2FFBC3AECECECCE69398C
    8A8A8D8A8A8E8B8B8F8C8C908D8D000000191919350CA82525E22929E3898787
    ECECEC343232FFBF43FFC048D16F41403E3ED5764AD7794FD97D54918F8F8685
    858685858786868887873131E53716B5363535ADADAD8C8B8B391BBD8E8C8CB6
    B6B6FFC963E4E4E4DC845C939090C85B27C95E2BCB62308A89893939E63819BA
    8D8B8B8E8C8C4848E93A1EC28F8D8DF0F0F0FFCC6CFFCE71DF8A64615E5EFFBC
    38E4E4E4CE68388C8A8A3C3B3B4646E93A1EC25050EB7150D85454EB918F8FF1
    F1F1F2F2F2C9C9C9F3F3F3F4F4F4FFC042FFC147D26F40B1B1B1444343B8B8B8
    5353EC7758DCBFBFBF5C5CED939191C8C8C8CCCCCCCFCFCFD2D2D2D6D6D60000
    003736363B3A3AEFEFEF444343F0F0F07353DABFBFBFF2F2F25C5CED939191F3
    F3F3F4F4F4CFCFCFF5F5F5F6F6F60000003E3D3DEFEFEF4745454B4A4AF1F1F1
    F2F2F2C4C4C4F3F3F36464EE3E2AD3959393F5F5F5D4D4D49895959996960000
    00454444B8B8B8918E8E928F8F939090949191CACACACACACACDCDCD7171F197
    9494D7D7D79996964135E39B98980000004D4B4BF1F1F1DD855DDF8861E18C66
    959393D0D0D0F5F5F5F5F5F57979F34032E09A96964136E54238E89D99990000
    00545252F2F2F2FFD178E4E4E4E4926E979494D5D5D5F6F6F6F6F6F6DBDBDB1B
    1BF54238E77272FF433CED9E9B9B0000005C5959C9C9C9FFD482FFD687E79876
    D7D7D7DBDBDBDBDBDBDEDEDEE1E1E11C1CF77575FF7979FF443FF2A09C9C0000
    00636060F4F4F4F5F5F5D6D6D6F6F6F6F7F7F7E1E1E1F8F8F8F8F8F81D1DF877
    77FF7B7BFF7F7FFF4542F7A29E9E0000006B6868F6F6F6F7F7F7DCDCDCF8F8F8
    F9F9F9E7E7E7FAFAFA1E1EF91F1FFA2020FB2121FC2222FD4746FC808080BF0F
    0000000000008000000080000000000000000000000000000000000000008000
    000080000000800000008000000080000000800000008000000080000000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    503
    346)
  PixelsPerInch = 96
  TextHeight = 13
  object lbBuild: TLabel
    Left = 6
    Top = 37
    Width = 408
    Height = 13
    Caption = 
      'The current dictionary was built with * of zkanji at @ *********' +
      '********* aaaaaaa'
  end
  object lbBuildEx: TLabel
    Left = 6
    Top = 57
    Width = 225
    Height = 13
    Caption = 'Example sentence data build date: **.**.****'
  end
  object b1: TBevel
    Left = 0
    Top = 78
    Width = 999999
    Height = 2
    Shape = bsTopLine
  end
  object Label5: TLabel
    Left = 12
    Top = 8
    Width = 112
    Height = 13
    Caption = 'Selected dictionary:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object b0: TBevel
    Left = 0
    Top = 30
    Width = 999999
    Height = 2
    Shape = bsTopLine
  end
  object b4: TBevel
    Left = 0
    Top = 317
    Width = 999999
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object b3: TBevel
    Left = 0
    Top = 315
    Width = 999999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object b2: TBevel
    Left = 0
    Top = 193
    Width = 999999
    Height = 2
    Shape = bsTopLine
  end
  object lbInfo: TLabel
    Left = 12
    Top = 200
    Width = 119
    Height = 13
    Caption = 'About the dictionary:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object BitBtn1: TBitBtn
    Left = 221
    Top = 321
    Width = 62
    Height = 23
    Anchors = [akBottom]
    Cancel = True
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
    ModalResult = 2
    ParentDoubleBuffered = False
    TabOrder = 1
  end
  object cbDict: TComboBox
    Left = 142
    Top = 4
    Width = 149
    Height = 21
    Style = csDropDownList
    TabOrder = 0
    OnClick = cbDictClick
  end
  object pStats: TPanel
    Left = 4
    Top = 82
    Width = 495
    Height = 110
    Anchors = [akTop]
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 2
    DesignSize = (
      495
      110)
    object Label1: TLabel
      Left = 274
      Top = 5
      Width = 112
      Height = 13
      Caption = 'Words in dictionary:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbWCnt: TLabel
      Left = 418
      Top = 5
      Width = 35
      Height = 13
      Caption = 'lbWCnt'
    end
    object Label2: TLabel
      Left = 16
      Top = 59
      Width = 82
      Height = 13
      Caption = 'Kanjis defined:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbKWCnt: TLabel
      Left = 176
      Top = 59
      Width = 41
      Height = 13
      Caption = 'lbKWCnt'
    end
    object Label3: TLabel
      Left = 16
      Top = 5
      Width = 86
      Height = 13
      Caption = 'Available kanji:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbKCnt: TLabel
      Left = 176
      Top = 5
      Width = 31
      Height = 13
      Caption = 'lbKCnt'
    end
    object Label4: TLabel
      Left = 16
      Top = 27
      Width = 141
      Height = 13
      Caption = 'Number of stroke orders:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbKECnt: TLabel
      Left = 176
      Top = 27
      Width = 37
      Height = 13
      Caption = 'lbKECnt'
    end
    object Label6: TLabel
      Left = 274
      Top = 59
      Width = 83
      Height = 13
      Caption = 'Popular words:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbPop: TLabel
      Left = 418
      Top = 59
      Width = 60
      Height = 13
      Caption = 'COMPUTING'
    end
    object Label8: TLabel
      Left = 274
      Top = 75
      Width = 108
      Height = 13
      Caption = 'Medium frequency:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbMed: TLabel
      Left = 418
      Top = 75
      Width = 60
      Height = 13
      Caption = 'COMPUTING'
    end
    object Label10: TLabel
      Left = 274
      Top = 91
      Width = 115
      Height = 13
      Caption = 'Non-frequent words:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbNon: TLabel
      Left = 418
      Top = 91
      Width = 60
      Height = 13
      Caption = 'COMPUTING'
    end
    object Label12: TLabel
      Left = 274
      Top = 27
      Width = 79
      Height = 13
      Caption = 'Unique words:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object lbCnt: TLabel
      Left = 418
      Top = 27
      Width = 60
      Height = 13
      Caption = 'COMPUTING'
    end
    object Bevel3: TBevel
      Left = 9
      Top = 21
      Width = 474
      Height = 2
      Anchors = [akLeft, akTop, akRight]
      Shape = bsTopLine
      ExplicitWidth = 476
    end
    object Label7: TLabel
      Left = 274
      Top = 43
      Width = 100
      Height = 13
      Caption = 'Unique meanings:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
      Visible = False
    end
    object lbMCnt: TLabel
      Left = 418
      Top = 43
      Width = 60
      Height = 13
      Caption = 'COMPUTING'
      Visible = False
    end
    object Bevel2: TBevel
      Left = 247
      Top = 4
      Width = 2
      Height = 100
      Shape = bsLeftLine
      Visible = False
    end
  end
  object mInfo: TMemo
    Left = 10
    Top = 218
    Width = 483
    Height = 90
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentColor = True
    ParentFont = False
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 3
    WantReturns = False
  end
end
