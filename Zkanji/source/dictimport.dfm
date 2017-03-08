object fDictImport: TfDictImport
  Left = 1152
  Top = 113
  Caption = 'Dictionary import conflict manager'
  ClientHeight = 515
  ClientWidth = 734
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    734
    515)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 5
    Top = 3
    Width = 241
    Height = 13
    Caption = 'Imported words requiring user interaction:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object b1: TBevel
    Left = 0
    Top = 19
    Width = 99999
    Height = 1
    Shape = bsTopLine
  end
  object bvBottom: TBevel
    Left = 0
    Top = 484
    Width = 999999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object Label2: TLabel
    Left = 282
    Top = 3
    Width = 224
    Height = 13
    Caption = 'Uncheck words you don'#39't want to be imported.'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object b3: TBevel
    Left = 0
    Top = 169
    Width = 99999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
  end
  object wbox: TZPaintBox
    Top = 20
    Width = 734
    Height = 149
    Anchors = [akLeft, akTop, akRight, akBottom]
    BorderStyle = bsNone
    TabOrder = 0
  end
  object btnNext: TBitBtn
    Left = 314
    Top = 488
    Width = 105
    Height = 25
    Anchors = [akBottom]
    Caption = '&Import'
    Default = True
    DoubleBuffered = True
    NumGlyphs = 4
    ParentDoubleBuffered = False
    TabOrder = 1
    OnClick = btnNextClick
  end
  object btnAbort: TButton
    Left = 654
    Top = 488
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Abort'
    TabOrder = 2
    OnClick = btnAbortClick
  end
  object pcSelector: TPageControl
    Left = 0
    Top = 171
    Width = 735
    Height = 313
    ActivePage = tabMeanings
    Anchors = [akLeft, akRight, akBottom]
    Style = tsButtons
    TabOrder = 3
    object tabWords: TTabSheet
      Caption = 'Replacement word'
      TabVisible = False
      DesignSize = (
        727
        303)
      object lbOrig1: TLabel
        Left = 33
        Top = 6
        Width = 661
        Height = 48
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Anchors = [akTop]
        AutoSize = False
        Caption = 
          'This word was not found in the current dictionary and will be ad' +
          'ded as a new entry. If it is a replacement for an existing word,' +
          ' select the existing word which will be replaced. Uncheck the wo' +
          'rd above if you don'#39't want to import the entry.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Layout = tlCenter
        WordWrap = True
      end
      object lbOrig2: TLabel
        Left = 33
        Top = 128
        Width = 661
        Height = 48
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Anchors = [akTop]
        AutoSize = False
        Caption = 
          'This word was not found in the current dictionary and will be ad' +
          'ded as a new entry. Uncheck the word above, if you don'#39't want to' +
          ' import the entry.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Layout = tlCenter
        Visible = False
        WordWrap = True
      end
      object lbOrig3: TLabel
        Left = 40
        Top = 128
        Width = 647
        Height = 48
        Margins.Left = 0
        Margins.Top = 0
        Margins.Right = 0
        Margins.Bottom = 0
        Anchors = [akTop]
        AutoSize = False
        Caption = 
          'A replacement word was selected as this kanji and kana pair was ' +
          'not found in the dictionary. All of the selected word'#39's meanings' +
          ' match a meaning in the dictionary. If you want to select a diff' +
          'erent replacement word, press '#39'Undo word replace'#39'.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Layout = tlCenter
        Visible = False
        WordWrap = True
      end
      object pOrig: TPanel
        AlignWithMargins = True
        Left = 0
        Top = 65
        Width = 727
        Height = 238
        Margins.Left = 0
        Margins.Top = 65
        Margins.Right = 0
        Margins.Bottom = 0
        Align = alClient
        BevelOuter = bvNone
        FullRepaint = False
        TabOrder = 0
        DesignSize = (
          727
          238)
        object Bevel3: TBevel
          Left = 56
          Top = 25
          Width = 2
          Height = 18
          Shape = bsLeftLine
        end
        object btnRomaji: TSpeedButton
          Left = 29
          Top = 21
          Width = 25
          Height = 24
          Hint = 'English to Japanese [F3]'
          GroupIndex = 1
          Flat = True
          Glyph.Data = {
            36030000424D3603000000000000360000002800000010000000100000000100
            18000000000000030000120B0000120B0000000000000000000085FAF585FAF5
            85FAF585FAF585FAF584848420202085FAF585FAF585FAF585FAF585FAF58585
            859A9A9AADADAD93939385FAF585FAF585FAF585FAF585FAF583838318181885
            FAF5878787919191A6A6A6CCCCCCF5F5F5FFFFFFFFFFFF9D9D9D85FAF585FAF5
            85FAF585FAF585FAF5797979272727B1B1B1D9D9D9F4F4F4FFFFFFFFFFFFFFFF
            FFFFFFFFFFFFFFABABAB85FAF585FAF585FAF585FAF585FAF55C5C5CB6B6B6FF
            FFFFFFFFFFE4E4FF5A5AFFA6A6FFFFFFFFFFFFFFFCFCFCADADAD85FAF585FAF5
            85FAF585FAF585FAF52A2A2AE5E5E5FFFFFFFFFFFF5454FF1A1AFF6969FFFFFF
            FFFFFFFFF7F7F7ACACAC85FAF585FAF585FAF585FAF5848484040404F8F8F8FF
            FFFFFFFFFF4C4CFF2828FF9191FFFFFFFFFFFFFFFFFFFFC6C6C685FAF585FAF5
            85FAF585FAF5848484666666FFFFFFFFFFFFFFFFFFFFFFFFCFCFFFF7F7FFFEFE
            FEFCFCFCC1C1C185FAF585FAF584848458585885FAF5848484A5A5A5FDFDFDFF
            FFFFFFFFFFFAFAFAF2F2F2C4C4C485858685FAF585FAF585FAF585FAF5838383
            27272785FAF5848484929191D3D2D2C5B8B6A87A6FC16F65CD98976E51C87B73
            9185FAF585FAF585FAF585FAF57B7B7B343434A090818F857AD45C50B776A227
            07CC832320CBADC16A59E38E59B29E9A9885FAF585FAF585FAF585FAF5716C6A
            BF7A76B96782C36D67C77656AE9DE2411EC4EAAB94664DD9794EBBCE95AF7875
            B885FAF585FAF585FAF585FAF55A5A5AEEB18EA786C48773DDEEBEA99484E423
            1BF0A27AB85744DE5344E64729D57D78AB85FAF585FAF585FAF585FAF5171717
            F27629F2A87AE9C4BC8474E13534FD0706FE2720F3835FC0DA7A74C328238E80
            7585FAF585FAF585FAF58383833E3E409B6AA86952D6382AE5493AE4322BF111
            08EFB265849A7BCBB091D0CA89A69E857685FAF585FAF585FAF57F7F7F484857
            90508BC3838BD2A2A7987ED1A78FD73417D2CA765EE55E35C98C9BB195D285FA
            F585FAF585FAF585FAF57D7D7D9D727AD88890E2ABB37457D8A165A1DBAAA885
            FAF585FAF585FAF585FAF585FAF585FAF585FAF585FAF585FAF5}
          Layout = blGlyphRight
          OnClick = btnKanjiClick
        end
        object btnKanji: TSpeedButton
          Left = 4
          Top = 21
          Width = 25
          Height = 24
          Hint = 'Japanese to English [F2]'
          GroupIndex = 1
          Down = True
          Flat = True
          Glyph.Data = {
            36030000424D3603000000000000360000002800000010000000100000000100
            18000000000000030000120B0000120B0000000000000000000085FAF585FAF5
            85FAF585FAF585FAF584848420202085FAF585FAF585FAF585FAF585FAF58B72
            7399716A9D695E83828385FAF585FAF585FAF585FAF585FAF58383830F0F0F85
            FAF59A685CBD583ADA95889A426DD2837B9276CF5223A984788285FAF585FAF5
            85FAF585FAF585FAF57979792E2C33C56D60D96441ED9E74F1C1A76C53D28C6B
            C4CD8881E0865E8E858B85FAF585FAF585FAF585FAF585FAF55C5C5C39323775
            4FBA6B48C19575C6613FC22519E85B3CC83821D55627AC857A8585FAF585FAF5
            85FAF585FAF585FAF52C2C2C906E39EA9362DC91766850D42413E01A11ED9F5F
            8DE68C5CDB5314877F7D85FAF585FAF585FAF585FAF584848411111A7850A781
            6AD76149D35347E9D48F827A48A5724BB97D5FCAD4A6A88D848685FAF585FAF5
            85FAF585FAF584848458301DE4967ECFBADA4C2FC9C28897E9BDB38F66AECC5D
            18D7641790517D85FAF585FAF584848458585885FAF5838384C26652A26EA671
            2B88BC4026D9865FB77F64867B9C87838485FAF585FAF585FAF585FAF5838383
            27272785FAF58484849B9797AAA8A8CACACAD1CECED4D4D4C8C2C2BEB8B88484
            8485FAF585FAF585FAF585FAF57A7A7A333333B8B1B1E8E8E8F3F3F3FBFBFBFF
            FFFFFFFFFFFCFCFCFFFFFFE4E1E184848485FAF585FAF585FAF585FAF5696969
            656565FFFFFFFEFEFFC8C8FF5D5DFF9898FFF4F4FFFFFFFFFFFFFFE7E5E58484
            8485FAF585FAF585FAF585FAF5494949AEAEAEFFFFFFD1D1FF1414FF3333FF49
            49FFC5C5FFFFFFFFFFFFFFF1F1F184848485FAF585FAF585FAF585FAF5171717
            ECECECFFFFFFE8E8FF1F1FFF2020FF4747FFBFBFFFFFFFFFFFFFFFEFEFEF8484
            8485FAF585FAF585FAF5828282222222FFFFFFFFFFFFFFFFFFE5E5FF8A8AFFBF
            BFFFFFFFFFFFFFFFFFFFFFBDB6B684848485FAF585FAF585FAF57F7F7F4B4B4B
            FFFFFFFFFFFFFEFEFEFFFFFFFAFAFAEAEAEAD8D5D5CECECEC4C4C4BFBFBF85FA
            F585FAF585FAF585FAF57D7D7D6F6F6FF1F1F1DEDEDECDCDCDC6C6C6BEBEBE85
            FAF585FAF585FAF585FAF585FAF585FAF585FAF585FAF585FAF5}
          Layout = blGlyphRight
          OnClick = btnKanjiClick
        end
        object Label6: TLabel
          Left = 3
          Top = 4
          Width = 262
          Height = 13
          Caption = 'Select a word to be replaced by the new entry:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object pRomaji: TPanel
          Left = 58
          Top = 20
          Width = 610
          Height = 27
          Anchors = [akLeft, akTop, akRight]
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 2
          Visible = False
          DesignSize = (
            610
            27)
          object edRomaji: TEdit2
            Left = 3
            Top = 4
            Width = 604
            Height = 21
            Anchors = [akLeft, akTop, akRight]
            TabOrder = 0
            OnChange = edRomajiChange
            OnEnter = edRomajiEnter
            OnExit = edRomajiEnter
            OnKeyPress = pbKanjiKeyPress
          end
        end
        object pbDict: TZPaintBox
          Left = 1
          Top = 48
          Width = 724
          Height = 188
          Anchors = [akLeft, akTop, akRight, akBottom]
          TabOrder = 0
        end
        object pKanji: TPanel
          Left = 58
          Top = 20
          Width = 610
          Height = 27
          Anchors = [akLeft, akTop, akRight]
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 1
          DesignSize = (
            610
            27)
          object sbRecognize: TSpeedButton
            Left = 2
            Top = 1
            Width = 25
            Height = 24
            AllowAllUp = True
            GroupIndex = 2
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
          end
          object pbKanji: TZPaintBox
            Left = 29
            Top = 2
            Width = 578
            Height = 23
            Anchors = [akLeft, akTop, akRight]
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -16
            Font.Name = 'MS Gothic'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
            OnKeyPress = pbKanjiKeyPress
            OnEnter = edRomajiEnter
            OnExit = edRomajiEnter
          end
        end
        object btnSelect: TButton
          Left = 669
          Top = 22
          Width = 57
          Height = 23
          Anchors = [akTop, akRight]
          Caption = 'Select'
          Enabled = False
          TabOrder = 3
          OnClick = btnSelectClick
        end
      end
    end
    object tabMeanings: TTabSheet
      Caption = 'Meanings selector '
      ImageIndex = 1
      TabVisible = False
      DesignSize = (
        727
        303)
      object Label7: TLabel
        Left = 40
        Top = 42
        Width = 647
        Height = 55
        Margins.Left = 25
        Margins.Top = 0
        Margins.Right = 25
        Margins.Bottom = 0
        Anchors = [akTop]
        AutoSize = False
        Caption = 
          'All imported meanings of the selected word match a meaning of th' +
          'e word in the dictionary. Check the box below if you want to del' +
          'ete the extra meanings from the dictionary.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Layout = tlCenter
        WordWrap = True
      end
      object Label9: TLabel
        AlignWithMargins = True
        Left = 36
        Top = 196
        Width = 655
        Height = 55
        Margins.Left = 25
        Margins.Top = 0
        Margins.Right = 25
        Margins.Bottom = 0
        Anchors = [akTop]
        AutoSize = False
        Caption = 
          'All meanings in the dictionary match a meaning of the imported w' +
          'ord. Uncheck any extra meanings you don'#39't want to be added to th' +
          'e word in the dictionary.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        Layout = tlCenter
        WordWrap = True
      end
      object pIMeanings: TPanel
        Left = 0
        Top = 0
        Width = 727
        Height = 149
        Anchors = [akLeft, akTop, akRight]
        BevelOuter = bvNone
        FullRepaint = False
        ParentBackground = False
        TabOrder = 0
        DesignSize = (
          727
          149)
        object Label8: TLabel
          Left = 3
          Top = -1
          Width = 175
          Height = 13
          Caption = 'Meanings of the selected word:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label4: TLabel
          Left = 200
          Top = -1
          Width = 259
          Height = 13
          Caption = 'Uncheck the meanings you don'#39't want to be imported.'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object ibox: TZPaintBox
          Left = 1
          Top = 17
          Width = 724
          Height = 131
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
        end
      end
      object pDMeanings: TPanel
        Left = 0
        Top = 152
        Width = 727
        Height = 397
        Anchors = [akLeft, akTop, akRight]
        BevelOuter = bvNone
        FullRepaint = False
        ParentBackground = False
        TabOrder = 1
        DesignSize = (
          727
          397)
        object Label3: TLabel
          Left = 4
          Top = 0
          Width = 283
          Height = 13
          Caption = 'Meanings to be replaced by the selected meaning:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label5: TLabel
          Left = 306
          Top = 0
          Width = 126
          Height = 13
          Caption = '(Multiple can be selected.)'
        end
        object cbDelUnmatched: TCheckBox
          Left = 456
          Top = -2
          Width = 265
          Height = 17
          Hint = 
            'Remove meanings from the current dictionary, which are not match' +
            'ed with an imported definition.'
          Caption = 'Delete meanings which are not replaced'
          TabOrder = 0
          OnClick = cbDelUnmatchedClick
        end
        object dbox: TZPaintBox
          Left = 1
          Top = 18
          Width = 724
          Height = 131
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 1
          TabStop = False
        end
      end
    end
  end
  object btnWordSel: TButton
    Left = 576
    Top = 178
    Width = 145
    Height = 25
    Hint = 
      'Cancel replacement of a word, which was selected to be replaced ' +
      'by the newly imported one.'
    Anchors = [akRight, akBottom]
    Caption = '&Undo word replace'
    TabOrder = 4
    OnClick = btnWordSelClick
  end
end
