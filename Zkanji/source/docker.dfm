object fDock: TfDock
  Left = 1049
  Top = 462
  Cursor = crHSplit
  Margins.Left = 0
  Margins.Top = 0
  Margins.Right = 0
  Margins.Bottom = 0
  Caption = 'zkanji v'
  ClientHeight = 205
  ClientWidth = 596
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Menu = mMenu
  OldCreateOrder = False
  OnClose = FormClose
  OnMouseDown = FormMouseDown
  OnMouseMove = FormMouseMove
  OnMouseUp = FormMouseUp
  OnPaint = FormPaint
  OnResize = FormResize
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object mMenu: TMainMenu
    Images = data.imgMenu
    Left = 40
    Top = 80
    object File1: TMenuItem
      Caption = 'File'
      object Export1: TMenuItem
        Caption = 'Export...'
        OnClick = Export1Click
      end
      object Import1: TMenuItem
        Caption = 'Import'
        object Groupimport1: TMenuItem
          Caption = 'Import User Data...'
          OnClick = Groupimport1Click
        end
        object Dictionaryimport1: TMenuItem
          Caption = 'Import Dictionary Entries...'
          OnClick = Dictionaryimport1Click
        end
        object Fulldictionaryimport1: TMenuItem
          Caption = 'Import Full Dictionary...'
          OnClick = Fulldictionaryimport1Click
        end
      end
      object N28: TMenuItem
        Caption = '-'
      end
      object miOpenFolder: TMenuItem
        Caption = 'Open User Data Folder'
        OnClick = miOpenFolderClick
      end
      object N31: TMenuItem
        Caption = '-'
      end
      object miExamples: TMenuItem
        Caption = 'Import Examples...'
        OnClick = miExamplesClick
      end
      object N3: TMenuItem
        Caption = '-'
      end
      object miExit: TMenuItem
        Action = data.aExit
      end
    end
    object miDictionary: TMenuItem
      Caption = 'Dictionary'
      object JapaneseEnglish1: TMenuItem
        Action = data.aJtoE
      end
      object EnglishJapanese1: TMenuItem
        Action = data.aEtoJ
      end
      object BrowseJapanese1: TMenuItem
        Action = data.aJ
      end
      object N7: TMenuItem
        Caption = '-'
      end
      object Submenu1: TMenuItem
        Caption = 'Options'
        ImageIndex = 34
        object miShowHideEx: TMenuItem
          Action = data.aShowHideEx
        end
        object N6: TMenuItem
          Caption = '-'
        end
        object Anystartofword1: TMenuItem
          Action = data.aWordStart
        end
        object Anyendofword1: TMenuItem
          Action = data.aWordEnd
        end
        object miToggleFilter: TMenuItem
          Caption = '-'
        end
        object oggleFilter1: TMenuItem
          Action = data.aFilterWords
          AutoCheck = True
        end
        object oggleinfconj1: TMenuItem
          Action = data.aInfConj
        end
      end
      object N1: TMenuItem
        Caption = '-'
      end
      object miDictSelect: TMenuItem
        Caption = 'Dictionaries'
        ImageIndex = 33
      end
    end
    object miTools: TMenuItem
      Caption = 'Tools'
      object Exit2: TMenuItem
        Action = data.aStats
      end
      object N5: TMenuItem
        Caption = '-'
      end
      object Clipboard1: TMenuItem
        Action = data.aClipboard
      end
      object N2: TMenuItem
        Caption = '-'
      end
      object Kanapractice1: TMenuItem
        Action = data.aKanaPractice
      end
      object Longtermstudylist1: TMenuItem
        Action = data.aStudyList
      end
      object N8: TMenuItem
        Caption = '-'
      end
      object Strokorderdiagrameditor1: TMenuItem
        Action = data.aStrokeEdit
      end
      object JLPTvocabularylisteditor1: TMenuItem
        Action = data.aJLPTEdit
      end
      object BETATestBETA1: TMenuItem
        Action = data.aBeta
      end
      object N10: TMenuItem
        Caption = '-'
      end
      object Editfilters1: TMenuItem
        Action = data.aEditFilters
      end
      object N4: TMenuItem
        Caption = '-'
      end
      object Settings1: TMenuItem
        Action = data.aSettings
      end
    end
    object miKanjiList: TMenuItem
      Caption = 'Kanji List'
      object miKInfo: TMenuItem
        Caption = 'Kanji Info'
        Enabled = False
        ImageIndex = 10
        OnClick = miKInfoClick
      end
      object N11: TMenuItem
        Caption = '-'
      end
      object miKTranslate: TMenuItem
        Caption = 'Translate Kanji'
        Enabled = False
        ImageIndex = 22
        OnClick = miKTranslateClick
      end
      object N12: TMenuItem
        Caption = '-'
      end
      object miKKanjiDef: TMenuItem
        Caption = 'Edit Kanji Definition...'
        Enabled = False
        ImageIndex = 18
        OnClick = miKKanjiDefClick
      end
      object N13: TMenuItem
        Caption = '-'
      end
      object miKGenerate: TMenuItem
        Caption = 'Generate Word Group...'
        ImageIndex = 25
        ShortCut = 16455
        OnClick = miKGenerateClick
      end
      object N14: TMenuItem
        Caption = '-'
      end
      object miKCopy: TMenuItem
        Caption = 'Copy Kanji'
        Enabled = False
        ImageIndex = 5
        OnClick = miKCopyClick
      end
      object miKAppend: TMenuItem
        Caption = 'Append Kanji'
        Enabled = False
        ImageIndex = 6
        OnClick = miKAppendClick
      end
      object N15: TMenuItem
        Caption = '-'
      end
      object miKCopyAll: TMenuItem
        Caption = 'Copy All'
        Enabled = False
        ImageIndex = 9
        OnClick = miKCopyAllClick
      end
      object miKAppendAll: TMenuItem
        Caption = 'Append All'
        Enabled = False
        ImageIndex = 8
        OnClick = miKAppendAllClick
      end
    end
    object miWordList: TMenuItem
      Caption = 'Word Results'
      object miWToVocab: TMenuItem
        Caption = 'Add Word to Vocabulary...'
        Enabled = False
        ImageIndex = 15
        OnClick = miWToVocabClick
      end
      object N16: TMenuItem
        Caption = '-'
      end
      object miWEditWord: TMenuItem
        Caption = 'Edit Word Definitions...'
        ImageIndex = 18
        OnClick = miWEditWordClick
      end
      object miWNewWord: TMenuItem
        Caption = 'New Word...'
        ImageIndex = 17
        OnClick = miWNewWordClick
      end
      object miWDeleteWord: TMenuItem
        Caption = 'Delete Word Entry'
        Enabled = False
        ImageIndex = 19
        OnClick = miWDeleteWordClick
      end
      object miWRevertWord: TMenuItem
        Caption = 'Revert to original...'
        Enabled = False
        ImageIndex = 18
        OnClick = miWRevertWordClick
      end
      object N17: TMenuItem
        Caption = '-'
      end
      object miWCopyKanji: TMenuItem
        Caption = 'Copy Word'
        Enabled = False
        ImageIndex = 5
        OnClick = miWCopyKanjiClick
      end
      object miWAppendKanji: TMenuItem
        Caption = 'Append Word'
        Enabled = False
        ImageIndex = 6
        OnClick = miWAppendKanjiClick
      end
      object N18: TMenuItem
        Caption = '-'
      end
      object miWCopyKana: TMenuItem
        Caption = 'Copy Word Kana'
        Enabled = False
        ImageIndex = 4
        OnClick = miWCopyKanaClick
      end
      object miWAppendKana: TMenuItem
        Caption = 'Append Word Kana'
        Enabled = False
        ImageIndex = 7
        OnClick = miWAppendKanaClick
      end
    end
    object miKanjiGroups: TMenuItem
      Caption = 'Kanji Groups'
      object miGKKanjiInfo: TMenuItem
        Caption = 'Kanji Info'
        Enabled = False
        ImageIndex = 10
        OnClick = miGKKanjiInfoClick
      end
      object N19: TMenuItem
        Caption = '-'
      end
      object miGKTranslateKanji: TMenuItem
        Caption = 'Translate Kanji'
        Enabled = False
        ImageIndex = 22
        OnClick = miGKTranslateKanjiClick
      end
      object N20: TMenuItem
        Caption = '-'
      end
      object miGKKanjiDef: TMenuItem
        Caption = 'Edit Kanji Definition...'
        Enabled = False
        ImageIndex = 18
        OnClick = miGKKanjiDefClick
      end
      object N21: TMenuItem
        Caption = '-'
      end
      object miGKGenerate: TMenuItem
        Caption = 'Generate Word Group...'
        ImageIndex = 25
        ShortCut = 16455
        OnClick = miGKGenerateClick
      end
      object N22: TMenuItem
        Caption = '-'
      end
      object miGKCopyKanji: TMenuItem
        Caption = 'Copy Kanji'
        Enabled = False
        ImageIndex = 5
        OnClick = miGKCopyKanjiClick
      end
      object miGKAppendKanji: TMenuItem
        Caption = 'Append Kanji'
        Enabled = False
        ImageIndex = 6
        OnClick = miGKAppendKanjiClick
      end
      object N23: TMenuItem
        Caption = '-'
      end
      object miGKCopyAll: TMenuItem
        Caption = 'Copy All Kanji'
        Enabled = False
        ImageIndex = 9
        OnClick = miGKCopyAllClick
      end
      object miGKAppendAll: TMenuItem
        Caption = 'Append All Kanji'
        Enabled = False
        ImageIndex = 8
        OnClick = miGKAppendAllClick
      end
    end
    object miWordGroups: TMenuItem
      Caption = 'Word groups'
      object miGWToVocab: TMenuItem
        Caption = 'Add Word to Vocabulary...'
        Enabled = False
        ImageIndex = 15
        OnClick = miGWToVocabClick
      end
      object N24: TMenuItem
        Caption = '-'
      end
      object miGWTranslateWritten: TMenuItem
        Caption = 'Translate Written'
        ImageIndex = 22
        ShortCut = 16468
        OnClick = miGWTranslateWrittenClick
      end
      object miGWTranslateKana: TMenuItem
        Caption = 'Translate Kana'
        ImageIndex = 22
        ShortCut = 24660
        OnClick = miGWTranslateKanaClick
      end
      object N25: TMenuItem
        Caption = '-'
      end
      object miGWEditWord: TMenuItem
        Caption = '&Edit Word Definitions...'
        ImageIndex = 18
        OnClick = miGWEditWordClick
      end
      object N26: TMenuItem
        Caption = '-'
      end
      object miGWPrintVocab: TMenuItem
        Caption = 'Print Word List...'
        ImageIndex = 26
        OnClick = miGWPrintVocabClick
      end
      object N29: TMenuItem
        Caption = '-'
      end
      object miGWCopyWord: TMenuItem
        Caption = 'Copy Word'
        Enabled = False
        ImageIndex = 5
        OnClick = miGWCopyWordClick
      end
      object miGWAppendWord: TMenuItem
        Caption = 'Append Word'
        Enabled = False
        ImageIndex = 6
        OnClick = miGWAppendWordClick
      end
      object N30: TMenuItem
        Caption = '-'
      end
      object miGWCopyKana: TMenuItem
        Caption = 'Copy Word Kana'
        Enabled = False
        ImageIndex = 4
        OnClick = miGWCopyKanaClick
      end
      object miGWAppendKana: TMenuItem
        Caption = 'Append Word Kana'
        Enabled = False
        ImageIndex = 7
        OnClick = miGWAppendKanaClick
      end
    end
    object Window1: TMenuItem
      Caption = 'Window'
      object miShowKanjiList: TMenuItem
        Caption = 'Kanji List'
        Checked = True
        ShortCut = 16496
        OnClick = miShowKanjiListClick
      end
      object miShowGroupList: TMenuItem
        Tag = 1
        Caption = 'Group List'
        Checked = True
        ShortCut = 16497
        OnClick = miShowKanjiListClick
      end
      object miShowDictionary: TMenuItem
        Tag = 2
        Caption = 'Dictionary'
        Checked = True
        ShortCut = 16498
        OnClick = miShowKanjiListClick
      end
      object N27: TMenuItem
        Caption = '-'
      end
      object miExpandKanji: TMenuItem
        Caption = 'Expand Kanji List'
        ImageIndex = 20
        OnClick = miExpandKanjiClick
      end
      object miExpandDict: TMenuItem
        Tag = 1
        Caption = 'Expand Dictionary'
        ImageIndex = 21
        OnClick = miExpandKanjiClick
      end
    end
    object Help1: TMenuItem
      Caption = 'Help'
      object About1: TMenuItem
        Action = data.aAbout
      end
      object N9: TMenuItem
        Caption = '-'
      end
      object CheckforUpdates1: TMenuItem
        Action = data.aUpdate
      end
    end
  end
end
