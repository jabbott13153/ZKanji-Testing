object fImportGroups: TfImportGroups
  Left = 1373
  Top = 75
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'User data import options'
  ClientHeight = 463
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
    463)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 40
    Top = 148
    Width = 408
    Height = 26
    Caption = 
      'Show dialog if the imported items'#39' definitions differ from those' +
      ' in the dictionary. Don'#39't check this if you want to prevent modi' +
      'fications to the dictionary.'
    WordWrap = True
  end
  object Label3: TLabel
    Left = 40
    Top = 220
    Width = 389
    Height = 26
    Caption = 
      'Which action to take when the imported group has the same name a' +
      's an already existing one. "Choose for each" allows renaming or ' +
      'skipping every group.'
    WordWrap = True
  end
  object Label4: TLabel
    Left = 16
    Top = 76
    Width = 78
    Height = 13
    Caption = 'Word options:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label5: TLabel
    Left = 240
    Top = 76
    Width = 76
    Height = 13
    Caption = 'Kanji options:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label6: TLabel
    Left = 40
    Top = 308
    Width = 409
    Height = 39
    Caption = 
      'Show a dialog if imported items are not in the dictionary or con' +
      'flict with dictionary entries. This step won'#39't modify the dictio' +
      'nary but allows selection of different entries to be used.'
    WordWrap = True
  end
  object Bevel1: TBevel
    Left = 16
    Top = 188
    Width = 445
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Bevel2: TBevel
    Left = 232
    Top = 68
    Width = 2
    Height = 75
    Shape = bsLeftLine
  end
  object Bevel4: TBevel
    Left = 16
    Top = 361
    Width = 445
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Bevel5: TBevel
    Left = 232
    Top = 189
    Width = 2
    Height = 26
    Shape = bsLeftLine
  end
  object Label7: TLabel
    Left = 16
    Top = 40
    Width = 66
    Height = 13
    Caption = 'File to import:'
  end
  object Label2: TLabel
    Left = 16
    Top = 198
    Width = 65
    Height = 13
    Caption = 'Group action:'
  end
  object Label8: TLabel
    Left = 240
    Top = 198
    Width = 65
    Height = 13
    Caption = 'Group action:'
  end
  object Bevel7: TBevel
    Left = 232
    Top = 362
    Width = 2
    Height = 21
    Shape = bsLeftLine
  end
  object bBottom: TBevel
    Left = 0
    Top = 428
    Width = 476
    Height = 2
    Anchors = [akLeft, akRight, akBottom]
    Shape = bsTopLine
    ExplicitTop = 400
  end
  object Bevel3: TBevel
    Left = 16
    Top = 260
    Width = 445
    Height = 2
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Bevel6: TBevel
    Left = 232
    Top = 261
    Width = 2
    Height = 43
    Shape = bsLeftLine
  end
  object Label9: TLabel
    Left = 40
    Top = 388
    Width = 381
    Height = 26
    Caption = 
      'Import meanings added by the user for example words selected for' +
      ' kanji. (This feature might become unsupported in future version' +
      's.)'
    WordWrap = True
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
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object cbWModify: TCheckBox
    Left = 16
    Top = 124
    Width = 201
    Height = 17
    Caption = 'Modify dictionary if needed'
    TabOrder = 3
  end
  object cbWord: TCheckBox
    Left = 16
    Top = 92
    Width = 177
    Height = 17
    Hint = 'Import exported groups of words.'
    Caption = 'Import word groups'
    Checked = True
    State = cbChecked
    TabOrder = 2
    OnClick = cbWordClick
  end
  object cbKanji: TCheckBox
    Left = 240
    Top = 92
    Width = 145
    Height = 17
    Hint = 'Import exported groups of kanji.'
    Caption = 'Import kanji groups'
    Checked = True
    State = cbChecked
    TabOrder = 6
    OnClick = cbWordClick
  end
  object cbWMissing: TCheckBox
    Left = 16
    Top = 285
    Width = 209
    Height = 17
    Caption = 'Select substitutes for word entries'
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object cbKMissing: TCheckBox
    Left = 256
    Top = 285
    Width = 216
    Height = 17
    Caption = 'Select substitutes for missing words'
    Checked = True
    State = cbChecked
    TabOrder = 9
  end
  object edFile: TEdit2
    Left = 88
    Top = 38
    Width = 336
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
  object btnImport: TButton
    Left = 155
    Top = 435
    Width = 97
    Height = 25
    Anchors = [akLeft, akRight, akBottom]
    Caption = 'Import'
    Default = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
    OnClick = btnImportClick
  end
  object btnCancel: TButton
    Left = 253
    Top = 435
    Width = 67
    Height = 25
    Anchors = [akLeft, akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    TabOrder = 12
    OnClick = btnCancelClick
  end
  object cbWReplace: TComboBox
    Left = 112
    Top = 194
    Width = 113
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 4
    Text = 'Add to existing'
    Items.Strings = (
      'Add to existing'
      'Rename imported'
      'Replace old'
      'Skip group'
      'Choose for each')
  end
  object cbKReplace: TComboBox
    Left = 336
    Top = 194
    Width = 113
    Height = 21
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 7
    Text = 'Add to existing'
    Items.Strings = (
      'Add to existing'
      'Rename imported'
      'Replace old'
      'Skip group'
      'Choose for each')
  end
  object cbKExDefinition: TCheckBox
    Left = 240
    Top = 365
    Width = 225
    Height = 17
    Caption = 'Import kanji example word meanings'
    Checked = True
    State = cbChecked
    TabOrder = 10
  end
  object cbKExamples: TCheckBox
    Left = 240
    Top = 268
    Width = 177
    Height = 17
    Hint = 'Import words selected for kanji as examples.'
    Caption = 'Import kanji example words'
    Checked = True
    State = cbChecked
    TabOrder = 8
    OnClick = cbWordClick
  end
  object cbDict: TComboBox
    Left = 116
    Top = 12
    Width = 145
    Height = 21
    Style = csDropDownList
    TabOrder = 13
  end
  object cbKDefinition: TCheckBox
    Left = 240
    Top = 109
    Width = 193
    Height = 17
    Hint = 
      'Import custom definitions added to kanji by the user. (This feat' +
      'ure might become unsupported in future versions.)'
    Caption = 'Import user kanji definitions'
    Checked = True
    State = cbChecked
    TabOrder = 14
    OnClick = cbWordClick
  end
  object dlgOpen: TOpenDialog
    Filter = 'zkanji export file (*.zkanji.export)|*.zkanji.export'
    Options = [ofPathMustExist, ofFileMustExist, ofEnableSizing]
    Title = 'Select a file containing exported entries'
    Left = 432
    Top = 76
  end
end
