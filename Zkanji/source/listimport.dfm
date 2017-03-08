object fListImport: TfListImport
  Left = 555
  Top = 313
  ActiveControl = pb
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Import words to long-term study list'
  ClientHeight = 515
  ClientWidth = 629
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  OnResize = FormResize
  DesignSize = (
    629
    515)
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 5
    Top = 463
    Width = 80
    Height = 13
    Anchors = [akLeft, akBottom]
    BiDiMode = bdLeftToRight
    Caption = 'Tested meaning:'
    ParentBiDiMode = False
    ExplicitTop = 405
  end
  object b2: TBevel
    Left = 0
    Top = 456
    Width = 9999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 398
  end
  object b1: TBevel
    Left = 0
    Top = 25
    Width = 9999
    Height = 1
    Shape = bsTopLine
  end
  object b3: TBevel
    Left = 0
    Top = 483
    Width = 9999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 425
  end
  object b4: TBevel
    Left = 0
    Top = 485
    Width = 9999
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 427
  end
  object Label1: TLabel
    Left = 102
    Top = 6
    Width = 167
    Height = 13
    Caption = 'for words normally written in kana.'
  end
  object Label3: TLabel
    Left = 284
    Top = 6
    Width = 33
    Height = 13
    Caption = 'Check:'
  end
  object btnSave: TButton
    Left = 540
    Top = 459
    Width = 44
    Height = 21
    Anchors = [akRight, akBottom]
    Caption = 'Save'
    TabOrder = 6
    OnClick = btnSaveClick
  end
  object btnReset: TButton
    Left = 584
    Top = 459
    Width = 44
    Height = 21
    Anchors = [akRight, akBottom]
    Caption = '&Reset'
    TabOrder = 7
    OnClick = btnResetClick
  end
  object edMeaning: TEdit2
    Left = 90
    Top = 460
    Width = 446
    Height = 21
    Anchors = [akLeft, akRight, akBottom]
    AutoSelect = False
    TabOrder = 5
    OnChange = edMeaningChange
    OnEnter = edMeaningEnter
    OnExit = edMeaningExit
    OnKeyDown = edMeaningKeyDown
  end
  object Panel3: TPanel
    Left = 0
    Top = 26
    Width = 629
    Height = 430
    Anchors = [akLeft, akTop, akRight, akBottom]
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 4
    object sbInfo: TStatusBar
      Left = 0
      Top = 411
      Width = 629
      Height = 19
      Panels = <
        item
          Alignment = taRightJustify
          Bevel = pbNone
          Text = '0 :'
          Width = 55
        end
        item
          Bevel = pbNone
          Text = '0'
          Width = 50
        end
        item
          Bevel = pbNone
          Text = 'Written:'
          Width = 50
        end
        item
          Bevel = pbNone
          Text = '0/0/0'
          Width = 80
        end
        item
          Bevel = pbNone
          Text = 'Kana:'
          Width = 35
        end
        item
          Bevel = pbNone
          Text = '0/0/0'
          Width = 80
        end
        item
          Bevel = pbNone
          Text = 'Meaning:'
          Width = 57
        end
        item
          Bevel = pbNone
          Text = '0/0/0'
          Width = 80
        end>
      SizeGrip = False
    end
    object pb: TZPaintBox
      Width = 629
      Height = 411
      Align = alClient
      BorderStyle = bsNone
      TabOrder = 0
      OnKeyDown = pbKeyDown
      OnKeyPress = pbKeyPress
    end
  end
  object btnOk: TBitBtn
    Left = 229
    Top = 488
    Width = 75
    Height = 25
    Anchors = [akBottom]
    Caption = 'Ok'
    Default = True
    DoubleBuffered = True
    ModalResult = 1
    ParentDoubleBuffered = False
    TabOrder = 8
  end
  object btnCancel: TBitBtn
    Left = 325
    Top = 488
    Width = 75
    Height = 25
    Anchors = [akBottom]
    Cancel = True
    Caption = 'Cancel'
    DoubleBuffered = True
    ModalResult = 2
    ParentDoubleBuffered = False
    TabOrder = 9
  end
  object btnUncheckKanaonly: TButton
    Left = 3
    Top = 3
    Width = 93
    Height = 20
    Caption = '&Uncheck written'
    TabOrder = 0
    OnClick = btnUncheckKanaonlyClick
  end
  object sbTestKanji: TButton
    Left = 322
    Top = 3
    Width = 62
    Height = 20
    Caption = '&Written'
    TabOrder = 1
    OnClick = sbTestKanjiClick
  end
  object sbTestKana: TButton
    Tag = 1
    Left = 384
    Top = 3
    Width = 62
    Height = 20
    Caption = '&Kana'
    TabOrder = 2
    OnClick = sbTestKanjiClick
  end
  object sbTestMeaning: TButton
    Tag = 2
    Left = 446
    Top = 3
    Width = 62
    Height = 20
    Caption = '&Meaning'
    TabOrder = 3
    OnClick = sbTestKanjiClick
  end
end
