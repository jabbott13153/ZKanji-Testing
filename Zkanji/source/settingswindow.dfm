object fSettings: TfSettings
  Left = 878
  Top = 700
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Settings'
  ClientHeight = 369
  ClientWidth = 780
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    000001001800000000004003000000000000000000000000000000000000A4A0
    A0000000A4A0A080808080808080808080808080808080808080808080808080
    8080808080A4A0A0000000000000000000A4A0A0000000A4A0A0A4A0A0A4A0A0
    A4A0A00092DC0062960062960062960000007A7A7A5656560000000000000000
    000000000000000000000000000000000000000092DC8ED4FF8ED4FF0062967A
    7A7AC0C0C0C0C0C09E9E9E000000000000000000000000000000000000000000
    0000000092DC48B8FF0062960092DC7A7A7A9E9E9E9E9E9E8686860000000000
    000000000000000000000000000000000000000092DC00AAFF00AAFF565656E6
    E6E6868686868686000000000000000000000000000000000000000000000000
    0000000092DC00AAFF565656E6E6E68686860000000000000000000000000000
    0000000000000000000000000000000000000000AAFF565656E6E6E6C0C0C000
    0000000000000000000000000000000000000000000000000000000000000000
    000000565656E6E6E6C0C0C00062960000000000000000000000000000000000
    00000000000000000000000000004A73004A73C0C0C0868686B1E2FF00629600
    0000000000000000000000000000000000000000000000000000004A733D00B9
    B1B1FF004A7348B8FF0062960092DC0000000000000000000000000000000000
    00000000000000004A738F6BFFB1B1FF8EABFF004A73D4F0FFB1E2FF00629600
    0000000000000000000000000000000000000000004A73B1B1FF0049DC8EABFF
    004A7300AAFFD4F0FFD4F0FF0062960000000000000000000000000000000000
    00004A733D00B90000FF8EABFF004A730000008ED4FF48B8FF0062960092DC00
    0000000000000000000000000000000000004A730000FF8EABFF004A73000000
    00000000AAFFD4F0FFD4F0FF0062960000000000000000000000000000000000
    00000000004A73004A7300000000000000000000AAFFD4F0FFD4F0FF00629600
    0000000000000000000000000000000000000000000000000000000000000000
    00000048B8FF8ED4FF8ED4FF0062960000000000000000000000000000004003
    0000A0030000FE010000FE010000FE030000FE0F0000FE1F0000FE1F0000F81F
    0000F01F0000E01F0000C01F0000821F0000861F0000CE1F0000FE1F0000}
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnClose = FormClose
  OnCloseQuery = FormCloseQuery
  OnPaint = FormPaint
  DesignSize = (
    780
    369)
  PixelsPerInch = 96
  TextHeight = 13
  object bvBottom: TBevel
    Left = 0
    Top = 335
    Width = 99999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 293
  end
  object pages: TPageControl
    AlignWithMargins = True
    Left = 0
    Top = 0
    Width = 780
    Height = 335
    HelpContext = -1
    Margins.Left = 0
    Margins.Top = 0
    Margins.Right = 0
    Margins.Bottom = 34
    ActivePage = tabDictionary
    Align = alClient
    MultiLine = True
    TabOrder = 0
    ExplicitHeight = 293
    object tabGlobal: TTabSheet
      Caption = 'Global'
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        772
        307)
      object Shape2: TShape
        Left = 440
        Top = 214
        Width = 305
        Height = 65
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape1: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
        ExplicitWidth = 761
      end
      object Label13: TLabel
        Left = 8
        Top = 6
        Width = 120
        Height = 19
        Caption = 'Global settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label14: TLabel
        Left = 510
        Top = 109
        Width = 118
        Height = 13
        HelpContext = 13
        Caption = 'Main reference for kanji:'
      end
      object Label2: TLabel
        Left = 9
        Top = 36
        Width = 94
        Height = 16
        Caption = 'Startup / Exit:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label59: TLabel
        Left = 510
        Top = 36
        Width = 36
        Height = 16
        Caption = 'Kanji:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label66: TLabel
        Left = 265
        Top = 36
        Width = 54
        Height = 16
        Caption = 'General:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Shape16: TShape
        Left = 23
        Top = 221
        Width = 316
        Height = 50
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Label42: TLabel
        Left = 265
        Top = 58
        Width = 62
        Height = 13
        Caption = 'Date format:'
      end
      object cbKanjiHint: TCheckBox
        Left = 512
        Top = 56
        Width = 202
        Height = 17
        Hint = 
          'Pop up a character information hint window when the cursor is ov' +
          'er a kanji in a kanji list or in the dictionary.'
        HelpContext = 16
        Caption = 'Show hint when mouse is over a kanji'
        TabOrder = 11
        OnClick = cbKanjiHintClick
      end
      object cbPositionRestore: TCheckBox
        Left = 11
        Top = 56
        Width = 165
        Height = 17
        Hint = 
          'Save main window positions and restore them when the program sta' +
          'rts the next time.'
        HelpContext = 1
        Caption = 'Restore last window positions'
        TabOrder = 0
      end
      object cbSearchSave: TCheckBox
        Left = 11
        Top = 122
        Width = 168
        Height = 17
        Hint = 
          'Save search queries, so you can see a list of the last items you' +
          ' were looking for.'
        HelpContext = 2
        Caption = 'Save dictionary lookup history'
        TabOrder = 3
      end
      object cbKanjiRestore: TCheckBox
        Left = 11
        Top = 78
        Width = 169
        Height = 17
        Hint = 
          'Restore search filters of the kanji window on startup, so you wo' +
          'n'#39't have to look for the last kanji again.'
        HelpContext = 3
        Caption = 'Restore last kanji filter options'
        TabOrder = 1
      end
      object cbStartInTray: TCheckBox
        Left = 11
        Top = 144
        Width = 143
        Height = 17
        Hint = 
          'Always minimize the program on startup, indipendent of how it wa' +
          's closed.'
        HelpContext = 4
        Caption = 'Always start minimized'
        TabOrder = 4
        OnClick = cbStartInTrayClick
      end
      object cbColumnRestore: TCheckBox
        Left = 11
        Top = 100
        Width = 214
        Height = 17
        Hint = 
          'Restore dictionary columns to their original width on startup. (' +
          'Only when the auto resize columns option is turned off.)'
        HelpContext = 5
        Caption = 'Reset dictionary column sizes to default'
        TabOrder = 2
      end
      object cbRememberTray: TCheckBox
        Left = 11
        Top = 166
        Width = 206
        Height = 17
        Hint = 'Minimize zkanji window on startup if it was closed minimized.'
        HelpContext = 6
        Caption = 'Remember minimized state on startup'
        TabOrder = 5
      end
      object cbInfo: TComboBox
        Left = 633
        Top = 106
        Width = 104
        Height = 21
        Hint = 
          'Select which information is to be shown and/or highlighted in th' +
          'e kanji information window and in the kanji hint window.'
        HelpContext = 13
        Style = csDropDownList
        DropDownCount = 13
        TabOrder = 14
      end
      object cbKanjiHintTime: TCheckBox
        Left = 512
        Top = 78
        Width = 177
        Height = 17
        Hint = 
          'Hide the kanji hint window after a few seconds, if the displayed' +
          ' kanji does not change.'
        HelpContext = 17
        Caption = 'Hide hint. Delay in seconds:'
        TabOrder = 12
        OnClick = cbKanjiHintTimeClick
      end
      object edKanjiHintTime: TEdit2
        Left = 689
        Top = 77
        Width = 48
        Height = 21
        Hint = 
          'Set number of seconds to wait before hiding the kanji hint toolt' +
          'ip.'
        HelpContext = 18
        TabOrder = 13
        Text = '5'
      end
      object Panel1: TPanel
        Left = 452
        Top = 226
        Width = 281
        Height = 41
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 16
        object lbAutoStart: TLabel
          Left = 175
          Top = 22
          Width = 90
          Height = 13
          Caption = '(current user only)'
        end
        object cbAutoStart: TCheckBox
          Left = 8
          Top = 5
          Width = 271
          Height = 17
          Caption = 'Automatically run zkanji on system startup'
          DragCursor = crDefault
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
        end
      end
      object Panel4: TPanel
        Left = 35
        Top = 230
        Width = 292
        Height = 31
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 15
        object cbCheckUpdate: TCheckBox
          Left = 5
          Top = 7
          Width = 175
          Height = 17
          Hint = 
            'Allow the program to connect to the internet, and show a notific' +
            'ation window when a new version was found.'
          Caption = 'Automatically check for updates'
          TabOrder = 0
        end
        object cbCheckInterval: TComboBox
          Left = 180
          Top = 5
          Width = 106
          Height = 21
          Hint = 'The interval the program should look for a new version.'
          Style = csDropDownList
          ItemIndex = 1
          TabOrder = 1
          Text = 'Weekly'
          Items.Strings = (
            'On startup'
            'Weekly'
            'Monthly'
            'Every 3 months')
        end
      end
      object cbWinDecor: TCheckBox
        Left = 267
        Top = 100
        Width = 126
        Height = 17
        Hint = 'Shade windows to give the program a nicer look and feel.'
        Caption = 'Window decoration'
        TabOrder = 8
      end
      object cbStatDate: TComboBox
        Left = 336
        Top = 54
        Width = 112
        Height = 21
        Hint = 'Format of dates shown on statistics.'
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 6
        Text = 'Day.Month.Year'
        Items.Strings = (
          'Day.Month.Year'
          'Month.Day.Year'
          'Year.Month.Day')
      end
      object cbOldMenu: TCheckBox
        Left = 267
        Top = 122
        Width = 142
        Height = 17
        Hint = 'Use old version of the context menu in kanji and word lists.'
        Caption = 'Old style context menu'
        TabOrder = 9
      end
      object cbShadow: TCheckBox
        Left = 267
        Top = 144
        Width = 166
        Height = 17
        Hint = 
          'Mimic the shadow effect of Aero. This might be too slow for olde' +
          'r systems or old graphics cards.'
        Caption = 'Shadow under tool windows'
        TabOrder = 10
      end
      object cbMinTray: TCheckBox
        Left = 267
        Top = 78
        Width = 118
        Height = 17
        Hint = 
          'Minimize zkanji to the tray notification area instead of the def' +
          'ault window behavior.'
        Caption = 'Minimize to tray'
        TabOrder = 7
      end
    end
    object tabDictionary: TTabSheet
      HelpContext = -1
      Caption = 'Dictionary'
      ImageIndex = 1
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        772
        307)
      object Shape3: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
        ExplicitWidth = 761
      end
      object Label17: TLabel
        Left = 9
        Top = 126
        Width = 76
        Height = 13
        HelpContext = 15
        Caption = 'Browsing order:'
      end
      object Label64: TLabel
        Left = 9
        Top = 150
        Width = 123
        Height = 13
        Caption = 'Word frequency symbols:'
      end
      object Label58: TLabel
        Left = 8
        Top = 6
        Width = 149
        Height = 19
        Caption = 'Dictionary settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label68: TLabel
        Left = 9
        Top = 36
        Width = 54
        Height = 16
        Caption = 'General:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Shape4: TShape
        Left = 504
        Top = 132
        Width = 255
        Height = 37
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape5: TShape
        Left = 504
        Top = 172
        Width = 255
        Height = 37
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape6: TShape
        Left = 504
        Top = 212
        Width = 255
        Height = 37
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Label69: TLabel
        Left = 503
        Top = 57
        Width = 262
        Height = 43
        AutoSize = False
        Caption = 
          'You can select key combinations to activate the popup dictionari' +
          'es when they are minimized. Press the Esc key, and the dictionar' +
          'y will disappear again.'
        WordWrap = True
      end
      object Label78: TLabel
        Left = 502
        Top = 37
        Width = 232
        Height = 16
        Caption = 'Popup dictionary system shortcuts:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label1: TLabel
        Left = 265
        Top = 37
        Width = 114
        Height = 16
        Caption = 'Popup dictionary:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label82: TLabel
        Left = 9
        Top = 174
        Width = 120
        Height = 13
        Caption = 'Indicate when word is in:'
      end
      object Label100: TLabel
        Left = 266
        Top = 170
        Width = 70
        Height = 13
        Caption = 'Transparency:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label38: TLabel
        Left = 225
        Top = 213
        Width = 100
        Height = 16
        Caption = 'Search history:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label84: TLabel
        Left = 225
        Top = 234
        Width = 132
        Height = 13
        Caption = 'Number of saved searches:'
      end
      object cbShowInflectionText: TCheckBox
        Left = 11
        Top = 56
        Width = 185
        Height = 17
        Hint = 
          'Show a short description on how the searched word was inflected,' +
          ' when it is the selected line in the dictionary.'
        HelpContext = 7
        Caption = 'Show inflection / conjugation type'
        TabOrder = 0
      end
      object cbShowInflectionAlways: TCheckBox
        Left = 35
        Top = 78
        Width = 94
        Height = 17
        Hint = 'Always show description text for inflected words in dictionary.'
        HelpContext = 14
        Caption = 'Always show.'
        TabOrder = 1
      end
      object cbAutosize: TCheckBox
        Left = 11
        Top = 100
        Width = 245
        Height = 17
        Hint = 
          'Automatically grow or shrink the size of dictionary columns to m' +
          'atch the longest visible word.'
        HelpContext = 12
        Caption = 'Auto-size dictionary column for kanji and kana'
        TabOrder = 2
      end
      object cbABC: TComboBox
        Left = 93
        Top = 122
        Width = 83
        Height = 21
        Hint = 
          'Select which alphabetical order to use when the dictionary is in' +
          ' "Browse Japanese" mode.'
        HelpContext = 15
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 3
        Text = 'a-b-c-d-e...'
        Items.Strings = (
          'a-b-c-d-e...'
          'a-i-u-e-o...')
      end
      object cbFreq: TComboBox
        Left = 157
        Top = 147
        Width = 83
        Height = 22
        Hint = 'Select which symbols represent the 3 word popularity levels.'
        AutoComplete = False
        Style = csOwnerDrawFixed
        ItemIndex = 0
        TabOrder = 4
        Text = 'xyz'
        OnDrawItem = cbFreqDrawItem
        Items.Strings = (
          'xyz'
          'xyz'
          'xyz')
      end
      object cbDictKey: TCheckBox
        Left = 568
        Top = 103
        Width = 138
        Height = 17
        Caption = 'Enable system shortcuts'
        TabOrder = 13
        OnClick = cbDictKeyClick
      end
      object pJShortcut: TPanel
        Left = 508
        Top = 135
        Width = 247
        Height = 30
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 14
        object Label10: TLabel
          Left = 141
          Top = 7
          Width = 8
          Height = 16
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object Label9: TLabel
          Left = 195
          Top = 7
          Width = 8
          Height = 16
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object Label70: TLabel
          Left = 4
          Top = 8
          Width = 54
          Height = 13
          Caption = 'Japanese'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          WordWrap = True
        end
        object cbJKeys: TComboBox
          Left = 64
          Top = 4
          Width = 72
          Height = 21
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 0
          Text = 'Alt+Ctrl'
          Items.Strings = (
            'Alt+Ctrl'
            'Alt'
            'Ctrl')
        end
        object cbJShift: TCheckBox
          Left = 154
          Top = 6
          Width = 42
          Height = 17
          Caption = 'Shift'
          Checked = True
          Enabled = False
          State = cbChecked
          TabOrder = 1
        end
        object cbJChar: TComboBox
          Left = 208
          Top = 4
          Width = 32
          Height = 21
          Style = csDropDownList
          DropDownCount = 12
          Enabled = False
          ItemIndex = 9
          TabOrder = 2
          Text = 'J'
          Items.Strings = (
            'A'
            'B'
            'C'
            'D'
            'E'
            'F'
            'G'
            'H'
            'I'
            'J'
            'K'
            'L'
            'M'
            'N'
            'O'
            'P'
            'Q'
            'R'
            'S'
            'T'
            'U'
            'V'
            'W'
            'X'
            'Y'
            'Z')
        end
      end
      object pEShortcut: TPanel
        Left = 508
        Top = 175
        Width = 247
        Height = 30
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 15
        object Label74: TLabel
          Left = 4
          Top = 1
          Width = 45
          Height = 26
          Caption = 'English/ other'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          WordWrap = True
        end
        object Label12: TLabel
          Left = 141
          Top = 6
          Width = 8
          Height = 16
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object Label11: TLabel
          Left = 195
          Top = 6
          Width = 8
          Height = 16
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object cbEShift: TCheckBox
          Left = 154
          Top = 6
          Width = 42
          Height = 17
          Caption = 'Shift'
          Checked = True
          Enabled = False
          State = cbChecked
          TabOrder = 1
        end
        object cbEKeys: TComboBox
          Left = 64
          Top = 4
          Width = 72
          Height = 21
          Style = csDropDownList
          ItemIndex = 2
          TabOrder = 0
          Text = 'Ctrl'
          Items.Strings = (
            'Alt+Ctrl'
            'Alt'
            'Ctrl')
        end
        object cbEChar: TComboBox
          Left = 208
          Top = 4
          Width = 32
          Height = 21
          Style = csDropDownList
          DropDownCount = 12
          Enabled = False
          ItemIndex = 9
          TabOrder = 2
          Text = 'J'
          Items.Strings = (
            'A'
            'B'
            'C'
            'D'
            'E'
            'F'
            'G'
            'H'
            'I'
            'J'
            'K'
            'L'
            'M'
            'N'
            'O'
            'P'
            'Q'
            'R'
            'S'
            'T'
            'U'
            'V'
            'W'
            'X'
            'Y'
            'Z')
        end
      end
      object pKShortcut: TPanel
        Left = 508
        Top = 215
        Width = 247
        Height = 30
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 16
        object Label72: TLabel
          Left = 4
          Top = 8
          Width = 48
          Height = 13
          Caption = 'Kanji list'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          WordWrap = True
        end
        object Label73: TLabel
          Left = 141
          Top = 6
          Width = 8
          Height = 16
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object Label75: TLabel
          Left = 195
          Top = 6
          Width = 8
          Height = 16
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -13
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object cbKShift: TCheckBox
          Left = 154
          Top = 6
          Width = 42
          Height = 17
          Caption = 'Shift'
          Checked = True
          Enabled = False
          State = cbChecked
          TabOrder = 1
        end
        object cbKKeys: TComboBox
          Left = 64
          Top = 4
          Width = 72
          Height = 21
          Style = csDropDownList
          ItemIndex = 2
          TabOrder = 0
          Text = 'Ctrl'
          Items.Strings = (
            'Alt+Ctrl'
            'Alt'
            'Ctrl')
        end
        object cbKChar: TComboBox
          Left = 208
          Top = 4
          Width = 32
          Height = 21
          Style = csDropDownList
          DropDownCount = 12
          Enabled = False
          ItemIndex = 9
          TabOrder = 2
          Text = 'J'
          Items.Strings = (
            'A'
            'B'
            'C'
            'D'
            'E'
            'F'
            'G'
            'H'
            'I'
            'J'
            'K'
            'L'
            'M'
            'N'
            'O'
            'P'
            'Q'
            'R'
            'S'
            'T'
            'U'
            'V'
            'W'
            'X'
            'Y'
            'Z')
        end
      end
      object cbPopupPositionRestore: TCheckBox
        Left = 267
        Top = 57
        Width = 219
        Height = 17
        Hint = 
          'Check this option, if you'#39'd like the dictionary to keep it'#39's siz' +
          'e after resizing.'
        Caption = 'Remember dictionary window dimensions'
        TabOrder = 7
      end
      object cbPopupDisappear: TCheckBox
        Left = 267
        Top = 79
        Width = 225
        Height = 17
        Hint = 
          'Check this option to let the popup dictionary disappear when you' +
          ' activate another program, or when you click outside the diction' +
          'ary.'
        Caption = 'Hide popup dictionary when it loses focus'
        TabOrder = 8
      end
      object cbPopupColumnRestore: TCheckBox
        Left = 267
        Top = 101
        Width = 209
        Height = 17
        Hint = 
          'Make columns return to their default size each time the popup di' +
          'ctionary closes. (Only when the auto-size dictionary column opti' +
          'on is turned off.)'
        Caption = 'Reset column sizes when popup closes'
        TabOrder = 9
      end
      object cbPopupUseClipboard: TCheckBox
        Left = 267
        Top = 123
        Width = 201
        Height = 17
        Hint = 
          'Check this option to use the clipboard'#39's contents as search word' +
          's, when the popup dictionary gains focus.'
        Caption = 'Paste from clipboard when activated'
        TabOrder = 10
      end
      object cbKanjiReset: TCheckBox
        Left = 267
        Top = 145
        Width = 177
        Height = 17
        Hint = 'Reset search filters before the popup kanji list appears.'
        Caption = 'Reset kanji list filters on popup'
        TabOrder = 11
      end
      object cbInGroup: TCheckBox
        Left = 96
        Top = 189
        Width = 89
        Height = 17
        Hint = 
          'Show a red + in front of word meanings that are added to a group' +
          '.'
        Caption = 'word groups'
        TabOrder = 5
      end
      object tbTransparency: TTrackBar
        Left = 344
        Top = 168
        Width = 137
        Height = 25
        Max = 100
        PageSize = 3
        ShowSelRange = False
        TabOrder = 12
        TickStyle = tsNone
      end
      object cbInJLPT: TCheckBox
        Left = 96
        Top = 212
        Width = 89
        Height = 17
        Hint = 
          'Show the Japanese Language Profeciency Test N level in front of ' +
          'words.'
        Caption = 'the (old) JLPT'
        TabOrder = 6
      end
      object edHistCnt: TEdit2
        Left = 381
        Top = 231
        Width = 44
        Height = 21
        Hint = 
          'Number of past searches to be kept in the list of dictionary sea' +
          'rch boxes. '
        TabOrder = 17
        Text = '100'
      end
      object cbTimedHistSave: TCheckBox
        Left = 227
        Top = 253
        Width = 228
        Height = 17
        Hint = 
          'Save the last entered dictionary search in the search history if' +
          ' it is not changed for a given time.'
        Caption = 'Save unchanged search after seconds:'
        TabOrder = 18
      end
      object edHistDelay: TEdit2
        Left = 455
        Top = 251
        Width = 25
        Height = 21
        Hint = 
          'Number of seconds to wait after the last change of a dictionary ' +
          'search to put it in the search history list.'
        TabOrder = 19
        Text = '4'
      end
    end
    object tabData: TTabSheet
      Caption = 'User data'
      ImageIndex = 7
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 265
      DesignSize = (
        772
        307)
      object Shape18: TShape
        Left = 255
        Top = 201
        Width = 501
        Height = 53
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape17: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
        ExplicitWidth = 761
      end
      object Label31: TLabel
        Left = 8
        Top = 6
        Width = 79
        Height = 19
        Caption = 'User data:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label16: TLabel
        Left = 48
        Top = 80
        Width = 93
        Height = 13
        Caption = 'Interval in minutes:'
      end
      object Label28: TLabel
        Left = 9
        Top = 36
        Width = 68
        Height = 16
        Caption = 'User data:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label32: TLabel
        Left = 266
        Top = 36
        Width = 51
        Height = 16
        Caption = 'Backup:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label71: TLabel
        Left = 10
        Top = 104
        Width = 199
        Height = 26
        Caption = 
          '* If not checked, user data is only saved when the program close' +
          's.'
        WordWrap = True
      end
      object Label109: TLabel
        Left = 264
        Top = 6
        Width = 124
        Height = 18
        Caption = 'Advanced settings:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBtnText
        Font.Height = -15
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object Label111: TLabel
        Left = 266
        Top = 83
        Width = 96
        Height = 13
        Caption = 'Number of backups:'
      end
      object Label112: TLabel
        Left = 266
        Top = 107
        Width = 115
        Height = 13
        Caption = 'Days between backups:'
      end
      object Label114: TLabel
        Left = 418
        Top = 185
        Width = 323
        Height = 13
        Alignment = taRightJustify
        Caption = 
          'A subfolder called "backup" will be created at the specified loc' +
          'ation.'
      end
      object Label34: TLabel
        Left = 266
        Top = 184
        Width = 88
        Height = 14
        Caption = 'Backup folder:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label33: TLabel
        Left = 512
        Top = 59
        Width = 207
        Height = 52
        Caption = 
          'A single backup takes up around 30 megabytes or more. Make sure ' +
          'you have enough free space. Completely turning off backup creati' +
          'on is not recommended.'
        WordWrap = True
      end
      object lbBck: TLabel
        Left = 264
        Top = 136
        Width = 231
        Height = 13
        Caption = 'Excess backups will be deleted from disk.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
        Visible = False
      end
      object edAutoSave: TEdit2
        Left = 166
        Top = 77
        Width = 48
        Height = 21
        Hint = 'Specify the number of minutes between automatic saves.'
        HelpContext = 10
        TabOrder = 1
        Text = '2'
      end
      object cbAutoSave: TCheckBox
        Left = 11
        Top = 56
        Width = 198
        Height = 17
        Hint = 'Save user data if it has changed while the program is running.'
        HelpContext = 9
        Caption = 'Save user data periodically*'
        TabOrder = 0
        OnClick = cbAutoSaveClick
      end
      object cbBckCreate: TCheckBox
        Left = 267
        Top = 57
        Width = 142
        Height = 17
        Hint = 
          'Create a safe copy of your old data before saving changes. This ' +
          'copy is kept until the set limit is reached.'
        Caption = 'Create backups'
        TabOrder = 2
        OnClick = cbBckCreateClick
      end
      object edBckCount: TEdit2
        Left = 404
        Top = 80
        Width = 48
        Height = 21
        Hint = 'Keep this many versions of past user data.'
        HelpContext = 10
        TabOrder = 3
        Text = '2'
        OnChange = cbBckCreateClick
      end
      object edBckSpacing: TEdit2
        Left = 404
        Top = 104
        Width = 48
        Height = 21
        Hint = 
          'Number of days that must pass before creating another set of saf' +
          'e copies of the current user data.'
        HelpContext = 10
        TabOrder = 4
        Text = '7'
        OnChange = cbBckCreateClick
      end
      object Panel12: TPanel
        Left = 260
        Top = 206
        Width = 491
        Height = 41
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 5
        DesignSize = (
          491
          41)
        object Label110: TLabel
          Left = 8
          Top = 0
          Width = 204
          Height = 13
          Caption = '<> = zkanji application data relative path.'
        end
        object edBckPath: TEdit2
          Left = 8
          Top = 16
          Width = 387
          Height = 21
          Anchors = [akLeft, akTop, akRight]
          TabOrder = 0
          Text = '<>\data'
          OnExit = edBckPathExit
        end
        object btnBckBrowse: TButton
          Left = 399
          Top = 14
          Width = 35
          Height = 23
          Anchors = [akTop, akRight]
          Caption = '...'
          TabOrder = 1
          OnClick = btnBckBrowseClick
        end
        object btnBckReset: TButton
          Left = 434
          Top = 14
          Width = 51
          Height = 23
          Anchors = [akTop, akRight]
          Caption = 'Reset'
          TabOrder = 2
          OnClick = btnBckResetClick
        end
      end
    end
    object tabTools: TTabSheet
      Caption = 'Tools'
      ImageIndex = 6
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 265
      DesignSize = (
        772
        307)
      object Shape15: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
        ExplicitWidth = 761
      end
      object Label79: TLabel
        Left = 8
        Top = 6
        Width = 104
        Height = 19
        Caption = 'Tool settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label44: TLabel
        Left = 9
        Top = 36
        Width = 213
        Height = 16
        Caption = 'Handwriting recognition window:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label80: TLabel
        Left = 545
        Top = 36
        Width = 106
        Height = 16
        Caption = 'Dictionary sites:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label81: TLabel
        Left = 545
        Top = 56
        Width = 175
        Height = 41
        AutoSize = False
        Caption = 
          'Manage a list of web sites to be listed in the right-click menu ' +
          'of the dictionary to look up words online.'
        WordWrap = True
      end
      object lbSite1: TLabel
        Left = 545
        Top = 136
        Width = 50
        Height = 13
        Caption = 'There are '
      end
      object lbSiteCnt: TLabel
        Left = 596
        Top = 136
        Width = 22
        Height = 13
        Alignment = taCenter
        AutoSize = False
        Caption = 'no'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object lbSite2: TLabel
        Left = 622
        Top = 136
        Width = 59
        Height = 13
        Caption = 'sites added.'
      end
      object Label39: TLabel
        Left = 297
        Top = 36
        Width = 114
        Height = 16
        Caption = 'Kanji information:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label89: TLabel
        Left = 297
        Top = 82
        Width = 174
        Height = 30
        AutoSize = False
        Caption = 'Select data to be displayed in the kanji information window.'
        WordWrap = True
      end
      object sbKRefUp: TSpeedButton
        Tag = -1
        Left = 297
        Top = 119
        Width = 23
        Height = 22
        Enabled = False
        Flat = True
        Glyph.Data = {
          3A080000424D3A080000000000004200000028000000440000000F0000000100
          100003000000F8070000C30E0000C30E00000000000000000000007C0000E003
          00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7F945294529452945294529452945294529452945294529452
          9452945294529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F9452945294529452945294529452945294529452
          945294529452945294529452FF7F945294529452945294529452945294529452
          945294529452945294529452945249360832062E062AE529E525E425C421C421
          C319C319C319C219A215A215A215A215D65AD65AB556B556B556B55694529452
          945294529452734E734E734E734E734E734E4315851D851D851D851D851D8521
          852185218521A521A625A625A625A625A625A6258319C525C525C625C625C625
          C625C625C625C625C629C729E729E729E729E729E829FF7F4A366E534D4F4C4B
          2B472A43293F083B083BE636E532E52EC52EC42AA2159452FF7FD65AD65AD65A
          B556B556B556B556945294529452945294529452734E734EFF7FFF7F2315E525
          E525052A062A062A062A262E282E282E2832493249364936A6259452FF7FA421
          A836A836C93AC93ACA3ACA3EEA3EEB42EB46EC460C4B0D4B0D4FE7299452FF7F
          FF7F4A366F576E534D534C4F4B472A472943093F083BE636E632C3199452FF7F
          FF7FFF7FF75ED65AD65AD65AD65AB556B556B556B5569452945294529452FF7F
          FF7FFF7FFF7F2311E425E525E525E529052A062A062A062E282E282E2832A521
          9452FF7FFF7FFF7FA419A632A836A836A836C93AC93ACA3ECA3EEA42EB42EC46
          C6259452FF7FFF7FFF7FFF7F6B3A915F705B6E576D534C4F4B4B2A472A43093F
          E4259452FF7FFF7FFF7FFF7FFF7FF75EF75ED65AD65AD65AD65AB556B556B556
          B5569452FF7FFF7FFF7FFF7FFF7FFF7F2211C421C421E425E525E525E529052A
          062A062A851D9452FF7FFF7FFF7FFF7FFF7F831986328632A632A836A836A93A
          C93AC93ACA3EC6259452FF7FFF7FFF7FFF7FFF7FFF7F6C3E9363915F905B6F57
          6E534D4F4C4B062E9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EF75EF75EF75E
          D65AD65AD65AB556B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F2211C421C421
          C421E425E525E525E525851D9452FF7FFF7FFF7FFF7FFF7FFF7FFF7F8319852E
          862E8632A632A832A836A836C5259452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          6D3EB4679363925F905B6F5B09369452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F18631863F75EF75EF75ED65AD65AFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7F2211C419C421C421C421C421841D9452FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7F8315852A852E852E862E8632A5219452FF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7F6D42B56BB46793632B3A9452FF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F186318631863F75EF75EFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7F0211C319C319C421641D9452FF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F8315642A652A852AA51D9452FF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6D42B76F4D429452FF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F396718631863FF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F020DA31963199452
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6215642AA41D
          9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F6E42
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          3967FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F020DFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7F6215FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F}
        NumGlyphs = 4
        OnMouseDown = sbKRefUpMouseDown
      end
      object sbKRefDown: TSpeedButton
        Tag = 1
        Left = 297
        Top = 141
        Width = 23
        Height = 22
        Flat = True
        Glyph.Data = {
          3A080000424D3A080000000000004200000028000000440000000F0000000100
          100003000000F8070000C30E0000C30E00000000000000000000007C0000E003
          00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7F94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7F94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7F9452A31994529452FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F524AFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452C82D94529452FF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F94522B3A94529452FF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452A41DC632A31994529452FF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F945294529452FF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F94524419072EA8299452
          9452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452A625CB422A36
          94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452C525083BE736
          E736A41D94529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556B556
          945294529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F94522419
          E625062A072AA82994529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452
          A521A93AC93ACA3E2A3694529452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F9452
          E72D2B4B2A430943083FE83AC42194529452FF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FD65AD65AB556B556B556B5569452FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7F94522315C525E525E625E625E629A82994529452FF7FFF7FFF7FFF7FFF7F
          FF7FFF7F9452841D8732A836A836A93AA93A293694529452FF7FFF7FFF7FFF7F
          FF7FFF7F945209364F574D534C4F2B4B2A470943093FC52194529452FF7FFF7F
          FF7FFF7FFF7FFF7FF75ED65AD65AD65AD65AB556B556B556B556FF7FFF7FFF7F
          FF7FFF7FFF7FFF7F94520211A41DC421C521C521C525E525E625A72594529452
          FF7FFF7FFF7FFF7FFF7F94528419662E862E873287328836A836A83609329452
          9452FF7FFF7FFF7FFF7F94522C3E725F725F705B6F574E534D4F2C4B2B470A43
          C52594529452FF7FFF7FFF7FFF7F1863F75EF75EF75EF75ED65AD65AD65AB556
          B556B556FF7FFF7FFF7FFF7FFF7F94520211A41DA41DA41DA41DC421C521C521
          C521E525A72594529452FF7FFF7FFF7F945263196526652A662A662E862E872E
          87328732A836082E94529452FF7FFF7F94522E42B66B956794639363725F715B
          6F574E574D4F2C4B2B4BE62994529452FF7FFF7F39671863186318631863F75E
          F75EF75ED65AD65AD65AD65AB556FF7FFF7FFF7F9452E20C8319A319A319A419
          A41DA41DA41DA41DC421C521C521872594529452FF7F94526215442644264526
          6526652A652A662A662E862E87328732082E94529452FF7F4F46B973B86FB76F
          B66B956B94679363725F715B705B4F574D534C4FE72D9452FF7F396739673967
          39671863186318631863F75EF75EF75ED65AD65AD65AD65AFF7FFF7FE10C8315
          8315831983198319A319A319A41DA41DA41DA41DA421C52187219452FF7F4211
          24222422242244264426442645266526652A662A662E662E872E082E9452D456
          D456D356D356B352B252B252B14EB14EB04E8F4A8F4A8E468D466D426C426B3E
          5A6B5A6B396739673967396739673967186318631863F75EF75EF75EF75ED65A
          D65AE10CE10CE10CE10CE10CE20CE20CE20CE210E21002110211021102110211
          0311031142114211421142114211421542154215631563156315631963196319
          63198419841DFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F}
        NumGlyphs = 4
        OnMouseDown = sbKRefUpMouseDown
      end
      object sbKRefAll: TSpeedButton
        Left = 297
        Top = 163
        Width = 23
        Height = 22
        Hint = 'Select all'
        Flat = True
        Glyph.Data = {
          4A090000424D4A09000000000000420000002800000044000000110000000100
          10000300000008090000C30E0000C30E00000000000000000000007C0000E003
          00001F000000FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53B556B556B556
          B556B556B556B556B556B556B556B556FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53F75E
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EFB53FB53FB53FB53FB53FB53
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EFB53FB53FB53FB53FB53
          FB53B556FF7FFF7FFF7F5A6BFF7FFF7FFF7FFF7FFF7FB556FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53F75EFF7FFF7FFF7F5A6BFF7FFF7FFF7FFF7FFF7FF75EFB53FB53
          FB53FB53FB53FB53F75EFF7FFF7FFF7F5A6BFF7FFF7FFF7FFF7FFF7FF75EFB53
          FB53FB53FB53FB53FB53B556FF7FFF7F5A6B00305A6BFF7FFF7FFF7FFF7FB556
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53734EFB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53F75EFF7FFF7F5A6B6C515A6BFF7FFF7FFF7F
          FF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7F5A6B6C515A6BFF7FFF7F
          FF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556FF7F5A6B2034417020305A6B
          FF7FFF7FFF7FB556FB53FB53FB53FB53FB53FB53FB53FB53FB5394529452734E
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53F75EFF7F5A6BC63C6C65
          4B515A6BFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7F5A6BC63C
          6C654B515A6BFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53B5565A6B2138
          63746274417020305A6BFF7FFF7FB556FB53FB53FB53FB53FB53FB53FB53FB53
          B556B556945294529452FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53F75E
          5A6BA53429612A654A654A515A6BFF7FFF7FF75EFB53FB53FB53FB53FB53FB53
          F75E5A6BA53429612A654A654A515A6BFF7FFF7FF75EFB53FB53FB53FB53FB53
          FB53B556643CC878A67885746374427421345A6BFF7FB556FB53FB53FB53FB53
          FB53FB53FB53D65AB556B556B556B55694529452FB53FB53FB53FB53FB53FB53
          FB53FB53FB53F75EA43407610861086129612A612A515A6BFF7FF75EFB53FB53
          FB53FB53FB53FB53F75EA43407610861086129612A612A515A6BFF7FF75EFB53
          FB53FB53FB53FB539452A6408E7D4D7D0A79433CA6788578637421345A6BB556
          FB53FB53FB53FB53FB53FB53F75ED65AD65AD65AD65AB556B556B556B556FB53
          FB53FB53FB53FB53FB53FB5394528430E658E65CE75C0849076108610961294D
          5A6BF75EFB53FB53FB53FB53FB5394528430E658E65CE75C0849076108610961
          294D5A6BF75EFB53FB53FB53FB5394520945757E347EF27DA640FF7FE850E978
          C778857821389452FB53FB53FB53FB53FB531863F75EF75EF75ED65AFB53D65A
          D65AB556B556B556FB53FB53FB53FB53FB539452842CC558C558C6588430FF7F
          0749E75CE75C0861084DF75EFB53FB53FB53FB539452842CC558C558C6588430
          FF7F0749E75CE75C0861084DF75EFB53FB53FB53FB531156197FD87EB77EE944
          FF7FFF7FFF7F09512C790A79C87842389452FB53FB53FB533967186318631863
          F75EFB53FB53FB53D65AD65AD65AB556B556FB53FB53FB53FB53832CA454C554
          C554842CFF7FFF7FFF7FE748E65CE75CE75C08499452FB53FB53FB53832CA454
          C554C554842CFF7FFF7FFF7FE748E65CE75CE75C08499452FB53FB53FB53FB53
          325A3B7F4C45FF7FFF7FFF7FFF7FFF7F2B558F7D4C7D0A79433C9452FB53FB53
          FB53396739671863FB53FB53FB53FB53FB53F75ED65AD65AD65AD65AFB53FB53
          FB53FB538328A454832CFF7FFF7FFF7FFF7FFF7FE744E658E658E65C08499452
          FB53FB53FB538328A454832CFF7FFF7FFF7FFF7FFF7FE744E658E658E65C0849
          9452FB53FB53FB53FB53535AFF7FFF7FFF7FFF7FFF7FFF7FFF7F6C55F27DAF7D
          6D7D643C9452FB53FB53FB533967FB53FB53FB53FB53FB53FB53FB53F75EF75E
          D65AD65AD65AFB53FB53FB53FB538328FF7FFF7FFF7FFF7FFF7FFF7FFF7FE744
          C658C658C658E7489452FB53FB53FB538328FF7FFF7FFF7FFF7FFF7FFF7FFF7F
          E744C658C658C658E7489452FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7F8D55547E127EB07D853C9452FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53F75EF75EF75EF75ED65AFB53FB53FB53F75EFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FE644C558C558C658E7449452FB53FB53F75EFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FE644C558C558C658E7449452FB53FB53B556B556B556
          B556B556B556B556B556B556B556AF55967E547E127EA640FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB5318631863F75EF75EF75EFB53FB53F75E
          F75EF75EF75EF75EF75EF75EF75EF75EF75EE640C554C554C558E744FB53FB53
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EE640C554C554C558E744FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53F055D87E09459452
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53186318631863
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53E640A454
          842C9452FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53E640
          A454842C9452FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB5311569452FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB533967FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB5383289452FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB5383289452FB53}
        NumGlyphs = 4
        OnClick = sbKRefAllClick
      end
      object sbKRefNone: TSpeedButton
        Tag = 1
        Left = 297
        Top = 185
        Width = 23
        Height = 22
        Hint = 'Select none'
        Flat = True
        Glyph.Data = {
          4A090000424D4A09000000000000420000002800000044000000110000000100
          10000300000008090000C30E0000C30E00000000000000000000007C0000E003
          00001F000000FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53B556B556B556
          B556B556B556B556B556B556B556B556FB53FB53FB53FB53FB53FB53B556B556
          B556B556B556B556B556B556B556B556B556FB53FB53FB53FB53FB53FB53F75E
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EFB53FB53FB53FB53FB53FB53
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EFB53FB53FB53FB53FB53
          FB53B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556FB53FB53FB53FB53
          FB53FB53B556FB53FB53FB53FB53FB53FB53FB53FB53FB53B556FB53FB53FB53
          FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53
          FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53
          FB53FB53FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556
          FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53FB53FB53FB53
          B556FB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FB556FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53
          FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556FF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FB556FB53FB53FB53FB53FB53FB53B556FB53
          FB53FB53FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53F75E
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53
          F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53
          FB53B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556FB53FB53FB53FB53
          FB53FB53B556FB53FB53FB53FB53FB53FB53FB53FB53FB53B556FB53FB53FB53
          FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53
          FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53
          FB53FB53FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556
          FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53FB53FB53FB53
          B556FB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FB556FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53
          FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556FF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FB556FB53FB53FB53FB53FB53FB53B556FB53
          FB53FB53FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53F75E
          FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53
          F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53
          FB53B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556FB53FB53FB53FB53
          FB53FB53B556FB53FB53FB53FB53FB53FB53FB53FB53FB53B556FB53FB53FB53
          FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53
          FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FF75EFB53
          FB53FB53FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FB556
          FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53FB53FB53FB53
          B556FB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556FF7FFF7FFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FB556FB53FB53FB53FB53FB53FB53B556FB53FB53FB53FB53FB53
          FB53FB53FB53FB53B556FB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53F75EFF7FFF7FFF7F
          FF7FFF7FFF7FFF7FFF7FFF7FF75EFB53FB53FB53FB53FB53FB53B556B556B556
          B556B556B556B556B556B556B556B556FB53FB53FB53FB53FB53FB53B556B556
          B556B556B556B556B556B556B556B556B556FB53FB53FB53FB53FB53FB53F75E
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EFB53FB53FB53FB53FB53FB53
          F75EF75EF75EF75EF75EF75EF75EF75EF75EF75EF75EFB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53FB53
          FB53FB53FB53FB53FB53FB53FB53}
        NumGlyphs = 4
        OnClick = sbKRefAllClick
      end
      object Label90: TLabel
        Left = 9
        Top = 114
        Width = 113
        Height = 16
        Caption = 'Clipboard viewer:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label15: TLabel
        Left = 9
        Top = 136
        Width = 103
        Height = 13
        Caption = 'Open clipboard after:'
      end
      object cbHWRememberSize: TCheckBox
        Left = 11
        Top = 56
        Width = 137
        Height = 17
        Hint = 
          'The handwriting recognition window will remember its width and h' +
          'eight when it closes.'
        HelpContext = 12
        Caption = 'Remember window size'
        TabOrder = 0
      end
      object cbHWRememberPosition: TCheckBox
        Left = 11
        Top = 78
        Width = 153
        Height = 17
        Hint = 
          'The handwriting recognition window will appear at the same posit' +
          'ion where you left it.'
        HelpContext = 12
        Caption = 'Remember window position'
        TabOrder = 1
      end
      object btnManageSites: TButton
        Left = 547
        Top = 105
        Width = 72
        Height = 23
        Caption = 'Manage...'
        TabOrder = 7
        OnClick = btnManageSitesClick
      end
      object cbKRef: TCheckListBox
        Left = 321
        Top = 119
        Width = 144
        Height = 130
        Flat = False
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -12
        Font.Name = 'Tahoma'
        Font.Style = []
        IntegralHeight = True
        ItemHeight = 14
        ParentFont = False
        TabOrder = 6
        OnClick = cbKRefClick
      end
      object cbShowClipboardCopy: TCheckBox
        Left = 34
        Top = 152
        Width = 43
        Height = 17
        Hint = 
          'Show the clipboard window every time something is copied to the ' +
          'clipboard from zkanji. (Only works when zkanji is not in minimiz' +
          'ed to the tray.)'
        HelpContext = 8
        Caption = 'copy'
        TabOrder = 2
      end
      object cbShowClipboardAppend: TCheckBox
        Left = 34
        Top = 174
        Width = 56
        Height = 17
        Hint = 
          'Show the clipboard window every time something is appended to th' +
          'e clipboard from zkanji. (Only works when zkanji is not in minim' +
          'ized to the tray.)'
        HelpContext = 11
        Caption = 'append'
        TabOrder = 3
      end
      object btnKRef: TButton
        Left = 425
        Top = 96
        Width = 41
        Height = 19
        Hint = 
          'Reset the order and visibility of reference numbers to the defau' +
          'lt state.'
        Caption = 'Reset'
        TabOrder = 5
        OnClick = btnKRefClick
      end
      object cbShowNonKanji: TCheckBox
        Left = 300
        Top = 56
        Width = 189
        Height = 17
        Hint = 
          'Show and make selectable parts of kanji in the parts and parents' +
          ' list that are not kanji themselves.'
        Caption = 'Show non-kanji parts and parents'
        TabOrder = 4
      end
    end
    object tabFonts: TTabSheet
      Caption = 'Fonts'
      ImageIndex = 3
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        772
        307)
      object Shape7: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Label5: TLabel
        Left = 14
        Top = 61
        Width = 55
        Height = 13
        Caption = 'Kanji/Kana:'
      end
      object Label6: TLabel
        Left = 26
        Top = 83
        Width = 44
        Height = 13
        Caption = 'Meaning:'
      end
      object Label7: TLabel
        Left = 23
        Top = 107
        Width = 49
        Height = 13
        Caption = 'Comment:'
      end
      object Bevel5: TBevel
        Left = 608
        Top = 37
        Width = 159
        Height = 159
        Shape = bsFrame
      end
      object lbDSize: TLabel
        Left = 17
        Top = 143
        Width = 96
        Height = 13
        Alignment = taRightJustify
        Caption = 'Dictionary font size:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
      end
      object lbPopSize: TLabel
        Left = 17
        Top = 217
        Width = 78
        Height = 13
        Alignment = taRightJustify
        Caption = 'Popup font size:'
      end
      object Label8: TLabel
        Left = 8
        Top = 6
        Width = 106
        Height = 19
        Caption = 'Font settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label3: TLabel
        Left = 352
        Top = 37
        Width = 67
        Height = 16
        Caption = 'Kanji font:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label4: TLabel
        Left = 9
        Top = 36
        Width = 109
        Height = 16
        Caption = 'Dictionary fonts:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object cbPopFontSize: TComboBox
        Left = 116
        Top = 213
        Width = 69
        Height = 21
        Style = csDropDownList
        ItemIndex = 1
        TabOrder = 6
        Text = 'medium'
        OnChange = cbKanaChange
        Items.Strings = (
          'big'
          'medium'
          'small')
      end
      object cbKanji: TComboBox
        Left = 352
        Top = 61
        Width = 249
        Height = 21
        DropDownCount = 12
        TabOrder = 3
        OnChange = cbKanjiChange
      end
      object cbKana: TComboBox
        Left = 80
        Top = 57
        Width = 249
        Height = 21
        DropDownCount = 12
        TabOrder = 0
        OnChange = cbKanaChange
      end
      object cbRomaji: TComboBox
        Left = 80
        Top = 81
        Width = 249
        Height = 21
        DropDownCount = 12
        TabOrder = 1
        OnChange = cbKanaChange
      end
      object pbKanji: TZPaintBox
        Left = 610
        Top = 39
        Width = 155
        Height = 155
        BorderStyle = bsNone
        Enabled = False
        TabOrder = 9
        TabStop = False
        OnPaint = pbKanjiPaint
      end
      object cbSmallRomaji: TComboBox
        Left = 80
        Top = 105
        Width = 249
        Height = 21
        DropDownCount = 12
        TabOrder = 2
        OnChange = cbKanaChange
      end
      object cbFontSize: TComboBox
        Left = 116
        Top = 139
        Width = 69
        Height = 21
        Style = csDropDownList
        ItemIndex = 1
        TabOrder = 5
        Text = 'medium'
        OnChange = cbKanaChange
        Items.Strings = (
          'big'
          'medium'
          'small')
      end
      object Panel2: TPanel
        Left = 198
        Top = 139
        Width = 395
        Height = 64
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 7
        object pbText1: TZPaintBox
          Left = 2
          Top = 2
          Width = 391
          Height = 60
          Align = alClient
          BorderStyle = bsNone
          Enabled = False
          TabOrder = 0
          TabStop = False
          OnPaint = pbText1Paint
        end
      end
      object cbKanjiAntialias: TCheckBox
        Left = 352
        Top = 88
        Width = 250
        Height = 17
        Caption = 'Disable cleartype for kanji font (where available)'
        TabOrder = 4
        OnClick = cbKanjiAntialiasClick
      end
      object Panel3: TPanel
        Left = 198
        Top = 213
        Width = 395
        Height = 64
        BevelInner = bvRaised
        BevelOuter = bvLowered
        TabOrder = 8
        object pbText2: TZPaintBox
          Tag = 1
          Left = 2
          Top = 2
          Width = 391
          Height = 60
          Align = alClient
          BorderStyle = bsNone
          Enabled = False
          TabOrder = 0
          TabStop = False
          OnPaint = pbText1Paint
          ExplicitTop = -6
        end
      end
    end
    object tabColors: TTabSheet
      Caption = 'Colors'
      ImageIndex = 4
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        772
        307)
      object Shape14: TShape
        Left = 474
        Top = 219
        Width = 111
        Height = 37
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape13: TShape
        Left = 354
        Top = 219
        Width = 111
        Height = 37
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape12: TShape
        Left = 234
        Top = 219
        Width = 111
        Height = 37
        Brush.Color = 15395562
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Shape8: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Label45: TLabel
        Left = 9
        Top = 83
        Width = 60
        Height = 13
        Caption = 'Background:'
      end
      object Label46: TLabel
        Left = 9
        Top = 107
        Width = 57
        Height = 13
        Caption = 'Highlighted:'
      end
      object Label48: TLabel
        Left = 9
        Top = 131
        Width = 47
        Height = 13
        Caption = 'Grid lines:'
      end
      object Label49: TLabel
        Left = 9
        Top = 59
        Width = 52
        Height = 13
        Caption = 'Text color:'
      end
      object Label51: TLabel
        Left = 10
        Top = 176
        Width = 97
        Height = 13
        Caption = 'Kanji with no words:'
      end
      object Label52: TLabel
        Left = 11
        Top = 216
        Width = 148
        Height = 13
        Caption = 'Background for unsorted kanji:'
      end
      object Label54: TLabel
        Left = 288
        Top = 84
        Width = 58
        Height = 13
        Caption = 'Type labels:'
      end
      object Label55: TLabel
        Left = 288
        Top = 108
        Width = 64
        Height = 13
        Caption = 'Usage/name:'
      end
      object Label56: TLabel
        Left = 288
        Top = 132
        Width = 56
        Height = 13
        Caption = 'Field labels:'
      end
      object Label57: TLabel
        Left = 288
        Top = 60
        Width = 70
        Height = 13
        Caption = 'Inf. and conj.:'
      end
      object Label60: TLabel
        Left = 238
        Top = 196
        Width = 202
        Height = 16
        Caption = 'Word frequency symbol colors:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label26: TLabel
        Left = 8
        Top = 6
        Width = 113
        Height = 19
        Caption = 'Color settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label27: TLabel
        Left = 9
        Top = 36
        Width = 54
        Height = 16
        Caption = 'General:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label47: TLabel
        Left = 287
        Top = 36
        Width = 106
        Height = 16
        Caption = 'Word list colors:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label53: TLabel
        Left = 9
        Top = 156
        Width = 102
        Height = 16
        Caption = 'Kanji list colors:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label63: TLabel
        Left = 288
        Top = 156
        Width = 78
        Height = 13
        Caption = 'Kana only word:'
      end
      object Label67: TLabel
        Left = 578
        Top = 60
        Width = 17
        Height = 13
        Caption = 'N5:'
      end
      object Label102: TLabel
        Left = 578
        Top = 84
        Width = 17
        Height = 13
        Caption = 'N4:'
      end
      object Label103: TLabel
        Left = 578
        Top = 108
        Width = 17
        Height = 13
        Caption = 'N3:'
      end
      object Label104: TLabel
        Left = 578
        Top = 132
        Width = 17
        Height = 13
        Caption = 'N2:'
      end
      object Label105: TLabel
        Left = 578
        Top = 156
        Width = 17
        Height = 13
        Caption = 'N1:'
      end
      object Label106: TLabel
        Left = 577
        Top = 36
        Width = 102
        Height = 16
        Caption = 'JLPT Level text:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Shape9: TShape
        Left = 628
        Top = 256
        Width = 127
        Height = 36
        Brush.Color = 11162931
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object cbBg: TColorBox
        Left = 89
        Top = 79
        Width = 145
        Height = 22
        Hint = 'Main background color'
        DefaultColorColor = clWhite
        Selected = clWhite
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 1
        OnChange = cbBgChange
      end
      object cbHl: TColorBox
        Left = 89
        Top = 103
        Width = 145
        Height = 22
        Hint = 'Highlighted item'#39's background color'
        DefaultColorColor = 14737632
        Selected = 14737632
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 2
      end
      object cbGrid: TColorBox
        Left = 89
        Top = 127
        Width = 145
        Height = 22
        Hint = 'Grid line color in kanji and word lists'
        DefaultColorColor = 13290186
        Selected = 13290186
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 3
      end
      object cbText: TColorBox
        Left = 89
        Top = 55
        Width = 145
        Height = 22
        Hint = 'Main text color'
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 0
      end
      object cbNoWd: TColorBox
        Left = 8
        Top = 194
        Width = 145
        Height = 22
        Hint = 
          'Text color of kanji that has no words associated with it in the ' +
          'dictionary'
        DefaultColorColor = clSilver
        Selected = clSilver
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 14
      end
      object cbUns: TColorBox
        Left = 8
        Top = 234
        Width = 145
        Height = 22
        Hint = 
          'Background color of kanji that cannot be sorted by the currently' +
          ' selected sorting method'
        DefaultColorColor = 16645621
        Selected = 16645621
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 15
      end
      object cbTp: TColorBox
        Left = 384
        Top = 80
        Width = 145
        Height = 22
        Hint = 'Color of small text showing a word'#39's grammatical information'
        DefaultColorColor = 3178560
        Selected = 3178560
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 5
      end
      object cbUsg: TColorBox
        Left = 384
        Top = 104
        Width = 145
        Height = 22
        Hint = 'Color of small text showing a word'#39's usage information'
        DefaultColorColor = 2113680
        Selected = 2113680
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 6
      end
      object cbFld: TColorBox
        Left = 384
        Top = 128
        Width = 145
        Height = 22
        Hint = 'Color of small text showing a word'#39's field of use'
        DefaultColorColor = 8405040
        Selected = 8405040
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 7
      end
      object btnRestore: TBitBtn
        Left = 635
        Top = 261
        Width = 113
        Height = 25
        Caption = 'Restore defaults'
        DoubleBuffered = True
        ParentDoubleBuffered = False
        TabOrder = 19
        OnClick = btnRestoreClick
      end
      object cbInfCon: TColorBox
        Left = 384
        Top = 56
        Width = 145
        Height = 22
        Hint = 
          'Background color of resulting word when searching for its inflec' +
          'ted form'
        DefaultColorColor = 15400959
        Selected = 15400959
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 4
      end
      object Panel13: TPanel
        Left = 239
        Top = 224
        Width = 100
        Height = 26
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 16
        object Label61: TLabel
          Left = 3
          Top = 6
          Width = 40
          Height = 13
          Caption = 'Popular:'
        end
        object pbFreq1: TZPaintBox
          Tag = 1
          Left = 52
          Top = 1
          Width = 24
          Height = 24
          HorzScrollBar.Visible = False
          TabOrder = 0
          TabStop = False
          VertScrollBar.Visible = False
          OnPaint = pbFreq1Paint
        end
        object btnFreq1: TBitBtn
          Tag = 1
          Left = 77
          Top = 1
          Width = 21
          Height = 24
          Caption = '...'
          DoubleBuffered = True
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -9
          Font.Name = 'Verdana'
          Font.Style = []
          ParentDoubleBuffered = False
          ParentFont = False
          TabOrder = 1
          OnClick = btnFreq1Click
        end
      end
      object Panel14: TPanel
        Left = 359
        Top = 224
        Width = 100
        Height = 26
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 17
        object Label101: TLabel
          Left = 3
          Top = 6
          Width = 45
          Height = 13
          Caption = 'Average:'
        end
        object pbFreq2: TZPaintBox
          Tag = 2
          Left = 52
          Top = 1
          Width = 24
          Height = 24
          HorzScrollBar.Visible = False
          TabOrder = 0
          TabStop = False
          VertScrollBar.Visible = False
          OnPaint = pbFreq1Paint
        end
        object btnFreq2: TBitBtn
          Tag = 2
          Left = 77
          Top = 1
          Width = 21
          Height = 24
          Caption = '...'
          DoubleBuffered = True
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -9
          Font.Name = 'Verdana'
          Font.Style = []
          ParentDoubleBuffered = False
          ParentFont = False
          TabOrder = 1
          OnClick = btnFreq1Click
        end
      end
      object Panel15: TPanel
        Left = 479
        Top = 224
        Width = 100
        Height = 26
        BevelOuter = bvNone
        Color = 15395562
        ParentBackground = False
        TabOrder = 18
        object Label62: TLabel
          Left = 3
          Top = 6
          Width = 40
          Height = 13
          Caption = 'Unused:'
        end
        object pbFreq3: TZPaintBox
          Tag = 3
          Left = 52
          Top = 1
          Width = 24
          Height = 24
          HorzScrollBar.Visible = False
          TabOrder = 0
          TabStop = False
          VertScrollBar.Visible = False
          OnPaint = pbFreq1Paint
        end
        object btnFreq3: TBitBtn
          Tag = 3
          Left = 77
          Top = 1
          Width = 21
          Height = 24
          Caption = '...'
          DoubleBuffered = True
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -9
          Font.Name = 'Verdana'
          Font.Style = []
          ParentDoubleBuffered = False
          ParentFont = False
          TabOrder = 1
          OnClick = btnFreq1Click
        end
      end
      object cbKnO: TColorBox
        Left = 384
        Top = 152
        Width = 145
        Height = 22
        Hint = 'Kanji text color when the word is usually written with kana only'
        DefaultColorColor = 9482400
        Selected = 9482400
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 8
      end
      object cbN1: TColorBox
        Left = 610
        Top = 152
        Width = 145
        Height = 22
        Hint = 'Kanji text color when the word is usually written with kana only'
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 13
      end
      object cbN2: TColorBox
        Left = 610
        Top = 128
        Width = 145
        Height = 22
        Hint = 'Color of small text showing a word'#39's field of use'
        DefaultColorColor = 8405040
        Selected = 8405040
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 12
      end
      object cbN3: TColorBox
        Left = 610
        Top = 104
        Width = 145
        Height = 22
        Hint = 'Color of small text showing a word'#39's usage information'
        DefaultColorColor = 2113680
        Selected = 2113680
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 11
      end
      object cbN4: TColorBox
        Left = 610
        Top = 80
        Width = 145
        Height = 22
        Hint = 'Color of small text showing a word'#39's grammatical information'
        DefaultColorColor = 3178560
        Selected = 3178560
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 10
      end
      object cbN5: TColorBox
        Left = 610
        Top = 56
        Width = 145
        Height = 22
        Hint = 
          'Background color of resulting word when searching for its inflec' +
          'ted form'
        DefaultColorColor = 15400959
        Selected = 15400959
        Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbCustomColor, cbPrettyNames]
        TabOrder = 9
      end
    end
    object tabStudy: TTabSheet
      Caption = 'Long-term study'
      ImageIndex = 5
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        772
        307)
      object Label93: TLabel
        Left = 541
        Top = 202
        Width = 212
        Height = 52
        Caption = 
          '*Part of the word that is shown, when the question is presented ' +
          'to you. After pressing the "Show hint" button, the secondary hin' +
          't will be revealed as well.'
        WordWrap = True
      end
      object Shape10: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Label50: TLabel
        Left = 8
        Top = 6
        Width = 197
        Height = 19
        Caption = 'Long-term study settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label29: TLabel
        Left = 9
        Top = 36
        Width = 54
        Height = 16
        Caption = 'General:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label30: TLabel
        Left = 8
        Top = 151
        Width = 86
        Height = 13
        Caption = 'Test day starts at'
      end
      object Label41: TLabel
        Left = 129
        Top = 151
        Width = 112
        Height = 13
        Caption = 'o'#39'clock [1AM - 12 noon]'
      end
      object Label36: TLabel
        Left = 9
        Top = 104
        Width = 90
        Height = 13
        Caption = 'Include new items:'
      end
      object Label40: TLabel
        Left = 9
        Top = 234
        Width = 129
        Height = 13
        Caption = 'Warn about piled up items:'
      end
      object lbKWarnThres: TLabel
        Left = 9
        Top = 259
        Width = 123
        Height = 13
        Caption = 'Warn threshold [5 - 999]:'
      end
      object Label65: TLabel
        Left = 272
        Top = 58
        Width = 169
        Height = 15
        AutoSize = False
        Caption = 'Repetition interval of missed items:'
        WordWrap = True
      end
      object Label35: TLabel
        Left = 273
        Top = 36
        Width = 90
        Height = 16
        Caption = 'Test settings:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label37: TLabel
        Left = 9
        Top = 126
        Width = 178
        Height = 13
        Caption = 'Items to include: [0=ask before test]'
      end
      object Label91: TLabel
        Left = 8
        Top = 213
        Width = 169
        Height = 13
        Caption = 'Warning bubble for due items:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label83: TLabel
        Left = 545
        Top = 80
        Width = 66
        Height = 13
        Caption = 'Type answer:'
      end
      object Label94: TLabel
        Left = 544
        Top = 38
        Width = 192
        Height = 13
        Caption = 'Suspend test if left alone for some time.'
      end
      object Label43: TLabel
        Left = 272
        Top = 229
        Width = 166
        Height = 13
        Caption = 'Round 2 - kanji readings test:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label95: TLabel
        Left = 273
        Top = 251
        Width = 81
        Height = 13
        Caption = 'Tested readings:'
      end
      object Label96: TLabel
        Left = 273
        Top = 275
        Width = 59
        Height = 13
        Caption = 'Readings in:'
      end
      object Label97: TLabel
        Left = 273
        Top = 176
        Width = 65
        Height = 13
        Caption = 'Deck limits:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -11
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label98: TLabel
        Left = 272
        Top = 197
        Width = 49
        Height = 13
        Caption = 'Try again:'
      end
      object Label99: TLabel
        Left = 358
        Top = 197
        Width = 71
        Height = 13
        Caption = 'Couldn'#39't recall:'
      end
      object edDayStart: TEdit2
        Left = 100
        Top = 148
        Width = 23
        Height = 21
        Hint = 'Time in the morning when previous test day ends.'
        TabOrder = 4
        Text = '4'
      end
      object cbIncludeInterval: TComboBox
        Left = 111
        Top = 100
        Width = 129
        Height = 21
        Hint = 'How often include new words from kanji on tests.'
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 2
        Text = 'Daily (Recommended)'
        Items.Strings = (
          'Daily (Recommended)'
          'Every second day'
          'Every third day')
      end
      object cbWarnTime: TComboBox
        Left = 143
        Top = 230
        Width = 97
        Height = 21
        Hint = 
          'Set how often zkanji should check for items that are due for rev' +
          'iew.'
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 6
        Text = 'Never'
        OnChange = cbWarnTimeChange
        Items.Strings = (
          'Never'
          'On startup'
          'Every hour'
          'Every 2 hours'
          'Every 6 hours'
          'Every 24 hours')
      end
      object edKWarnThres: TEdit2
        Left = 138
        Top = 256
        Width = 41
        Height = 21
        Hint = 'Number of items that must be due before warning.'
        TabOrder = 7
        Text = '10'
      end
      object cbRepetitionInterval: TComboBox
        Left = 446
        Top = 54
        Width = 61
        Height = 21
        Hint = 
          'Length of interval for items that were incorrectly answered to r' +
          'e-appear in the same test.'
        Style = csDropDownList
        ItemIndex = 1
        TabOrder = 8
        Text = 'Medium'
        Items.Strings = (
          'Short'
          'Medium'
          'Long')
      end
      object edKWarnItems: TEdit2
        Left = 438
        Top = 101
        Width = 41
        Height = 21
        Hint = 
          'Number of items to be tested before showing question to stop tes' +
          'ting.'
        TabOrder = 11
        Text = '60'
      end
      object cbKWarnItems: TCheckBox
        Left = 273
        Top = 102
        Width = 165
        Height = 17
        Hint = 
          'Offer the possibility to quit testing after a given number of it' +
          'ems.'
        Caption = 'Warn after tested items:'
        TabOrder = 10
        OnClick = cbKWarnItemsClick
      end
      object cbLTOpenTab: TCheckBox
        Left = 11
        Top = 56
        Width = 142
        Height = 17
        Hint = 
          'Reopen the long-term study window with the last selected tab and' +
          ' statistics.'
        Caption = 'Remember window state'
        TabOrder = 0
      end
      object cbLTRestoreSizes: TCheckBox
        Left = 11
        Top = 78
        Width = 182
        Height = 17
        Hint = 
          'Restore columns to their original width when the long-term study' +
          ' window opens'
        Caption = 'Restore column sizes to default'
        TabOrder = 1
      end
      object edKItemInclude: TEdit2
        Left = 193
        Top = 124
        Width = 41
        Height = 21
        Hint = 'Number of new items to include when the given time has passed.'
        TabOrder = 3
        Text = '0'
      end
      object cbKWarnAfter: TCheckBox
        Left = 273
        Top = 126
        Width = 144
        Height = 17
        Hint = 'Offer the possibility to quit testing after a given time.'
        Caption = 'Warn after minutes:'
        TabOrder = 12
        OnClick = cbKWarnAfterClick
      end
      object edKWarnAfter: TEdit2
        Left = 417
        Top = 125
        Width = 41
        Height = 21
        Hint = 'Number of minutes before showing question to stop testing.'
        TabOrder = 13
        Text = '10'
      end
      object cbKTypeKanji: TCheckBox
        Left = 565
        Top = 96
        Width = 57
        Height = 17
        Hint = 
          'Type written form of word in test, using the keyboard and the ka' +
          'nji recognizer'
        Caption = 'Written'
        TabOrder = 20
      end
      object cbKTypeKana: TCheckBox
        Left = 629
        Top = 96
        Width = 45
        Height = 17
        Hint = 'Type kana reading of word in test'
        Caption = 'Kana'
        TabOrder = 21
      end
      object cbKTypeMeaning: TCheckBox
        Left = 681
        Top = 96
        Width = 61
        Height = 17
        Hint = 'Type word meaning in test'
        Caption = 'Meaning'
        TabOrder = 22
      end
      object cbLabelInterval: TCheckBox
        Left = 273
        Top = 78
        Width = 184
        Height = 17
        Hint = 
          'Show label indicating approximate interval when word gets repeat' +
          'ed after pressing a given answer button.'
        Caption = 'Show next interval above buttons'
        TabOrder = 9
      end
      object Panel11: TPanel
        Left = 540
        Top = 121
        Width = 211
        Height = 78
        BevelOuter = bvNone
        Color = 15658734
        ParentBackground = False
        TabOrder = 23
        object Label92: TLabel
          Left = 80
          Top = 5
          Width = 77
          Height = 13
          Caption = 'Primary hint*'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label85: TLabel
          Left = 14
          Top = 5
          Width = 39
          Height = 13
          Caption = 'Tested'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -11
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          WordWrap = True
        end
        object Bevel1: TBevel
          Left = 68
          Top = 6
          Width = 1
          Height = 66
          Shape = bsLeftLine
        end
        object Bevel2: TBevel
          Left = 4
          Top = 20
          Width = 203
          Height = 1
          Shape = bsTopLine
        end
        object Panel5: TPanel
          Left = 9
          Top = 21
          Width = 52
          Height = 17
          BevelEdges = []
          BevelOuter = bvNone
          Color = 15658734
          FullRepaint = False
          ParentBackground = False
          TabOrder = 0
          object Label86: TLabel
            Left = 6
            Top = 2
            Width = 40
            Height = 13
            Alignment = taRightJustify
            Caption = 'Written:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
        end
        object Panel6: TPanel
          Left = 9
          Top = 37
          Width = 52
          Height = 17
          BevelEdges = []
          BevelOuter = bvNone
          Color = 15658734
          FullRepaint = False
          ParentBackground = False
          TabOrder = 1
          object Label87: TLabel
            Left = 6
            Top = 2
            Width = 28
            Height = 13
            Alignment = taRightJustify
            Caption = 'Kana:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
        end
        object Panel7: TPanel
          Left = 9
          Top = 53
          Width = 52
          Height = 17
          BevelEdges = []
          BevelOuter = bvNone
          Color = 15658734
          FullRepaint = False
          ParentBackground = False
          TabOrder = 2
          object Label88: TLabel
            Left = 6
            Top = 2
            Width = 44
            Height = 13
            Alignment = taRightJustify
            Caption = 'Meaning:'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -11
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
          end
        end
        object Panel8: TPanel
          Left = 72
          Top = 21
          Width = 132
          Height = 17
          BevelEdges = []
          BevelOuter = bvNone
          Color = 15658734
          FullRepaint = False
          ParentBackground = False
          TabOrder = 3
          object rbKanjiKana: TRadioButton
            Left = 7
            Top = 0
            Width = 46
            Height = 17
            Hint = 'Show kana reading when testing written form'
            Caption = 'Kana'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object rbKanjiMeaning: TRadioButton
            Left = 66
            Top = 0
            Width = 62
            Height = 17
            Hint = 'Show meaning when testing written form'
            Caption = 'Meaning'
            TabOrder = 1
          end
        end
        object Panel9: TPanel
          Left = 72
          Top = 37
          Width = 132
          Height = 17
          BevelEdges = []
          BevelOuter = bvNone
          Color = 15658734
          FullRepaint = False
          ParentBackground = False
          TabOrder = 4
          object rbKanaKanji: TRadioButton
            Left = 7
            Top = 0
            Width = 56
            Height = 17
            Hint = 'Show written form when testing kana reading'
            Caption = 'Written'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object rbKanaMeaning: TRadioButton
            Left = 66
            Top = 0
            Width = 62
            Height = 17
            Hint = 'Show meaning when testing kana reading'
            Caption = 'Meaning'
            TabOrder = 1
          end
        end
        object Panel10: TPanel
          Left = 72
          Top = 53
          Width = 132
          Height = 17
          BevelEdges = []
          BevelOuter = bvNone
          Color = 15658734
          FullRepaint = False
          ParentBackground = False
          TabOrder = 5
          object rbMeaningKanji: TRadioButton
            Left = 7
            Top = 0
            Width = 56
            Height = 17
            Hint = 'Show written form when testing meaning'
            Caption = 'Written'
            Checked = True
            TabOrder = 0
            TabStop = True
          end
          object rbMeaningKana: TRadioButton
            Left = 66
            Top = 0
            Width = 62
            Height = 17
            Hint = 'Show kana reading when testing meaning'
            Caption = 'Kana'
            TabOrder = 1
          end
        end
      end
      object cbSuspendAfter: TComboBox
        Left = 544
        Top = 54
        Width = 201
        Height = 21
        Hint = 
          'Select the time that must pass while the window is not touched, ' +
          'before the test is automatically paused.'
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 19
        Text = 'Don'#39't suspend'
        Items.Strings = (
          'Don'#39't suspend'
          'After a minute'
          'After 1 and a half minutes'
          'After 2 minutes'
          'After 3 minutes'
          'After 5 minutes')
      end
      object cbKNewFirst: TCheckBox
        Left = 273
        Top = 150
        Width = 144
        Height = 17
        Hint = 
          'Show items that are tested for the first time before any other i' +
          'tem.'
        Caption = 'Newly added items first'
        TabOrder = 14
        OnClick = cbKWarnAfterClick
      end
      object cbLTShowLast: TCheckBox
        Left = 11
        Top = 174
        Width = 197
        Height = 17
        Hint = 
          'Switch to last tested items view in the long-term study window a' +
          'fter studying.'
        Caption = 'Show last tested items after study'
        TabOrder = 5
      end
      object cbKReadings: TComboBox
        Left = 367
        Top = 247
        Width = 130
        Height = 21
        Hint = 
          'Select kanji reading to study after main test, when it appeared ' +
          'in a tested word.'
        Style = csDropDownList
        ItemIndex = 1
        TabOrder = 17
        Text = 'ON readings'
        OnChange = cbKReadingsChange
        Items.Strings = (
          'None'
          'ON readings'
          'Kun readings'
          'All readings')
      end
      object cbKReadingWords: TComboBox
        Left = 344
        Top = 271
        Width = 153
        Height = 21
        Hint = 'Study readings in the specified words.'
        Style = csDropDownList
        ItemIndex = 2
        TabOrder = 18
        Text = 'New and mistaken words'
        Items.Strings = (
          'New words'
          'Mistakes'
          'New and mistaken words'
          'Every tested word')
      end
      object edKRetryLimit: TEdit2
        Left = 325
        Top = 195
        Width = 26
        Height = 21
        Hint = 
          'Limit item deck to set number when selecting the "Try again" but' +
          'ton.'
        TabOrder = 15
        Text = '4'
      end
      object edKHardLimit: TEdit2
        Left = 434
        Top = 195
        Width = 26
        Height = 21
        Hint = 
          'Limit item deck to set number when selecting the "Couldn'#39't recal' +
          'l" button.'
        TabOrder = 16
        Text = '3'
      end
    end
    object tabPrint: TTabSheet
      Caption = 'Word list printing'
      ImageIndex = 6
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      DesignSize = (
        772
        307)
      object Shape11: TShape
        Left = 3
        Top = 3
        Width = 764
        Height = 26
        Anchors = [akLeft, akTop, akRight]
        Brush.Color = 14211288
        Pen.Style = psClear
        Shape = stRoundRect
      end
      object Label19: TLabel
        Left = 13
        Top = 83
        Width = 55
        Height = 13
        Caption = 'Kanji/Kana:'
      end
      object Label20: TLabel
        Left = 25
        Top = 105
        Width = 44
        Height = 13
        Caption = 'Meaning:'
      end
      object Bevel8: TBevel
        Left = 7
        Top = 157
        Width = 529
        Height = 108
        Shape = bsFrame
      end
      object Label21: TLabel
        Left = 554
        Top = 181
        Width = 64
        Height = 13
        Caption = 'Row height:*'
      end
      object Label22: TLabel
        Left = 555
        Top = 64
        Width = 45
        Height = 13
        Caption = 'Left side:'
      end
      object Label24: TLabel
        Left = 554
        Top = 88
        Width = 51
        Height = 13
        Caption = 'Right side:'
      end
      object Label25: TLabel
        Left = 560
        Top = 205
        Width = 199
        Height = 26
        Caption = 
          '*Includes text and space between lines. Does not include furigan' +
          'a, when present.'
        WordWrap = True
      end
      object Label76: TLabel
        Left = 8
        Top = 6
        Width = 201
        Height = 19
        Caption = 'Word list printing settings:'
        Color = 14211288
        Font.Charset = EASTEUROPE_CHARSET
        Font.Color = clBlack
        Font.Height = -16
        Font.Name = 'Arial'
        Font.Style = [fsBold]
        ParentColor = False
        ParentFont = False
        Transparent = False
      end
      object Label77: TLabel
        Left = 9
        Top = 36
        Width = 90
        Height = 16
        Caption = 'Font settings:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label18: TLabel
        Left = 553
        Top = 36
        Width = 107
        Height = 16
        Caption = 'Layout settings:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object Label23: TLabel
        Left = 360
        Top = 36
        Width = 76
        Height = 16
        Caption = 'Decoration:'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlack
        Font.Height = -13
        Font.Name = 'Tahoma'
        Font.Style = [fsBold]
        ParentFont = False
      end
      object cbVKana: TComboBox
        Left = 72
        Top = 79
        Width = 249
        Height = 21
        Hint = 'Font used to print kanji and kana characters.'
        DropDownCount = 12
        TabOrder = 1
        OnChange = cbVKanaChange
      end
      object cbVRomaji: TComboBox
        Left = 72
        Top = 103
        Width = 249
        Height = 21
        Hint = 'Font used to print word definition.'
        DropDownCount = 12
        TabOrder = 2
        OnChange = cbVKanaChange
      end
      object cbVDictFonts: TCheckBox
        Left = 11
        Top = 56
        Width = 113
        Height = 17
        Hint = 
          'Use fonts selected on the '#39'Fonts'#39' tab, ignore those specified he' +
          're.'
        HelpContext = 1
        Caption = 'Use dictionary fonts'
        TabOrder = 0
        OnClick = cbVDictFontsClick
      end
      object pbVText: TZPaintBox
        Left = 9
        Top = 159
        Width = 524
        Height = 103
        BorderStyle = bsNone
        Enabled = False
        TabOrder = 12
        TabStop = False
        OnPaint = pbVTextPaint
      end
      object cbVHeight: TComboBox
        Left = 631
        Top = 177
        Width = 120
        Height = 21
        Hint = 
          'Height of each printed row, excluding furigana but including emp' +
          'ty space.'
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 11
        Text = '4 mm / 0.16 inches'
        Items.Strings = (
          '4 mm / 0.16 inches'
          '5 mm / 0.2 inches'
          '6 mm / 0.24 inches'
          '7 mm / 0.28 inches'
          '8 mm / 0.32 inches')
      end
      object cbVLeft: TComboBox
        Left = 631
        Top = 60
        Width = 120
        Height = 21
        Style = csDropDownList
        ItemIndex = 0
        TabOrder = 6
        Text = 'Meaning'
        Items.Strings = (
          'Meaning'
          'Kana reading'
          'Kanji writing')
      end
      object cbVDouble: TCheckBox
        Left = 554
        Top = 154
        Width = 153
        Height = 17
        Hint = 'Print two columns of word definitions on each printed page.'
        Caption = 'Two columns on each page'
        TabOrder = 10
      end
      object cbVRight: TComboBox
        Left = 631
        Top = 84
        Width = 120
        Height = 21
        Style = csDropDownList
        ItemIndex = 1
        TabOrder = 7
        Text = 'Kana reading'
        OnChange = cbVRightChange
        Items.Strings = (
          'Meaning'
          'Kana reading'
          'Kanji writing'
          '- Nothing')
      end
      object cbVSeparate: TCheckBox
        Left = 554
        Top = 110
        Width = 173
        Height = 17
        Hint = 'Check this if you want to use printed list to test yourself.'
        Caption = 'Left and right on separate pages'
        TabOrder = 8
      end
      object cbVFurigana: TCheckBox
        Left = 554
        Top = 132
        Width = 121
        Height = 17
        Hint = 'Show kanji readings in words by small kana above them.'
        Caption = 'Furigana above kanji.'
        TabOrder = 9
      end
      object cbVShade: TCheckBox
        Left = 360
        Top = 56
        Width = 176
        Height = 17
        Hint = 'Use a light gray shade as background for every even row.'
        Caption = 'Background for every even row'
        TabOrder = 3
      end
      object cbVSep: TCheckBox
        Left = 360
        Top = 78
        Width = 161
        Height = 17
        Hint = 'Print lines between rows and columns for easier reading.'
        Caption = 'Separator lines between rows'
        TabOrder = 4
      end
      object cbVNumber: TCheckBox
        Left = 360
        Top = 100
        Width = 105
        Height = 17
        Hint = 'Print page number on page.'
        Caption = 'Print page number'
        TabOrder = 5
      end
    end
  end
  object btnOk: TButton
    Left = 313
    Top = 340
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 1
  end
  object btnCancel: TButton
    Left = 393
    Top = 340
    Width = 75
    Height = 25
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 2
  end
  object dColor: TColorDialog
    OnClose = dColorClose
    OnShow = dColorShow
    Options = [cdFullOpen, cdAnyColor]
    Left = 740
  end
end
