object fImportDict: TfImportDict
  Left = 1409
  Top = 73
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Dictionary entry import options'
  ClientHeight = 462
  ClientWidth = 475
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
  DesignSize = (
    475
    462)
  PixelsPerInch = 96
  TextHeight = 13
  object Label7: TLabel
    Left = 16
    Top = 40
    Width = 66
    Height = 13
    Caption = 'File to import:'
  end
  object Label4: TLabel
    Left = 16
    Top = 76
    Width = 88
    Height = 13
    Caption = 'Import options:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label1: TLabel
    Left = 40
    Top = 234
    Width = 416
    Height = 26
    Caption = 
      'Select what to do with words not in the dictionary. Note, that c' +
      'hecking "Only meanings in group" when creating an export file, w' +
      'ords might miss some meanings.'
    WordWrap = True
  end
  object Label2: TLabel
    Left = 40
    Top = 306
    Width = 419
    Height = 26
    Caption = 
      'Select which action to take for imported words, if the export fi' +
      'le only contains the same definitions found in the dictionary an' +
      'd additional meanings.'
    WordWrap = True
  end
  object Bevel1: TBevel
    Left = 16
    Top = 274
    Width = 445
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label3: TLabel
    Left = 16
    Top = 210
    Width = 172
    Height = 13
    Caption = 'Words not in the current dictionary:'
  end
  object Label5: TLabel
    Left = 16
    Top = 282
    Width = 155
    Height = 13
    Caption = 'Words with additional meanings:'
  end
  object Bevel3: TBevel
    Left = 16
    Top = 202
    Width = 445
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object bBottom: TBevel
    Left = -1
    Top = 427
    Width = 476
    Height = 2
    Anchors = [akLeft, akRight, akBottom]
    Shape = bsTopLine
    ExplicitTop = 368
  end
  object Label6: TLabel
    Left = 16
    Top = 92
    Width = 391
    Height = 13
    Caption = 
      'Words with meanings in the current dictionary not matching the e' +
      'xported entries:'
  end
  object Label8: TLabel
    Left = 40
    Top = 378
    Width = 416
    Height = 26
    Caption = 
      'Select which action to take if the imported word contains fewer ' +
      'meanings compared to the word in the dictionary, but every impor' +
      'ted meaning matches one in the dictionary. '
    WordWrap = True
  end
  object Bevel2: TBevel
    Left = 16
    Top = 346
    Width = 445
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label9: TLabel
    Left = 16
    Top = 354
    Width = 137
    Height = 13
    Caption = 'Words with fewer meanings:'
  end
  object Label10: TLabel
    Left = 16
    Top = 16
    Width = 86
    Height = 13
    Caption = 'Target dictionary:'
  end
  object bTop: TBevel
    Left = -1
    Top = 68
    Width = 476
    Height = 2
    ParentCustomHint = False
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object edFile: TEdit2
    Left = 96
    Top = 38
    Width = 328
    Height = 21
    TabOrder = 0
  end
  object btnSelectFile: TButton
    Left = 427
    Top = 35
    Width = 41
    Height = 23
    Caption = '...'
    TabOrder = 1
    OnClick = btnSelectFileClick
  end
  object cbSkipConflict: TCheckBox
    Left = 16
    Top = 108
    Width = 321
    Height = 17
    Hint = 
      'Only import new words or words which have extra meanings beside ' +
      'the ones already in the dictionary.'
    Caption = 'Skip words having conflicting meaning'
    TabOrder = 2
    OnClick = cbSkipConflictClick
  end
  object cbTypeDiff: TCheckBox
    Left = 32
    Top = 125
    Width = 337
    Height = 17
    Hint = 'Check both the text and the word types, when comparing meanings.'
    Caption = 'Word type difference counts as meaning difference'
    Enabled = False
    TabOrder = 3
    OnClick = cbTypeDiffClick
  end
  object cbUpdateType: TCheckBox
    Left = 16
    Top = 142
    Width = 361
    Height = 17
    Hint = 
      'Update dictionary with word types from the export file, if the t' +
      'ext of the meanings are the same for matching words. If not chec' +
      'ked, types in the dictionary will remain unchanged for meanings ' +
      'having the same text.'
    Caption = 'Update word types for meanings with matching text'
    Checked = True
    State = cbChecked
    TabOrder = 4
  end
  object btnImport: TButton
    Left = 155
    Top = 434
    Width = 97
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Import'
    Default = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 10
    OnClick = btnImportClick
    ExplicitTop = 422
  end
  object btnCancel: TButton
    Left = 253
    Top = 434
    Width = 67
    Height = 25
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Cancel'
    TabOrder = 11
    OnClick = btnCancelClick
    ExplicitTop = 422
  end
  object cbNewWords: TComboBox
    Left = 208
    Top = 206
    Width = 254
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 7
    Text = 'Import new words'
    Items.Strings = (
      'Import new words'
      'Choose which new to add'
      'Don'#39't add any')
  end
  object cbNewMeanings: TComboBox
    Left = 208
    Top = 278
    Width = 254
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 8
    Text = 'Import new meanings'
    Items.Strings = (
      'Import new meanings'
      'Choose which new to add'
      'Don'#39't add any')
  end
  object cbMissingMeanings: TComboBox
    Left = 208
    Top = 350
    Width = 254
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 9
    Text = 'Keep missing meanings'
    Items.Strings = (
      'Keep missing meanings'
      'Choose what to do'
      'Remove missing meanings')
  end
  object cbDict: TComboBox
    Left = 116
    Top = 12
    Width = 145
    Height = 21
    Style = csDropDownList
    TabOrder = 12
    OnChange = cbDictChange
  end
  object cbKDefinition: TCheckBox
    Left = 16
    Top = 164
    Width = 209
    Height = 17
    Hint = 'Import definitions added for kanji in user dictionaries.'
    Caption = 'Import kanji dictionary definitions'
    Checked = True
    Enabled = False
    State = cbChecked
    TabOrder = 5
    OnClick = cbKDefinitionClick
  end
  object cbKDefSelect: TCheckBox
    Left = 32
    Top = 181
    Width = 265
    Height = 17
    Hint = 
      'Show a dialog window to select which kanji should keep its origi' +
      'nal definition instead of being overwritten with an imported one' +
      '.'
    Caption = 'Ask before overwriting existing definitions'
    Enabled = False
    TabOrder = 6
  end
  object dlgOpen: TOpenDialog
    Filter = 'zkanji export file (*.zkanji.export)|*.zkanji.export'
    Options = [ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Select a file containing exported entries'
    Left = 432
    Top = 72
  end
end
