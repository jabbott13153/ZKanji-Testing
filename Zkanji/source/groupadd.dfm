object fWordToGroup: TfWordToGroup
  Left = 775
  Top = 541
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Add word to group'
  ClientHeight = 91
  ClientWidth = 566
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  DesignSize = (
    566
    91)
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 10
    Top = 8
    Width = 62
    Height = 13
    Caption = 'Group name:'
  end
  object Label3: TLabel
    Left = 10
    Top = 31
    Width = 73
    Height = 13
    Caption = 'Word meaning:'
  end
  object b2: TBevel
    Left = 0
    Top = 58
    Width = 999999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 79
  end
  object Label1: TLabel
    Left = 234
    Top = 8
    Width = 202
    Height = 13
    Caption = '* group will be created if it does not exist.'
  end
  object cbGroup: TComboBox
    Left = 88
    Top = 5
    Width = 139
    Height = 21
    TabOrder = 0
    Text = 'New Group'
    OnChange = cbGroupChange
  end
  object btnAdd: TButton
    Left = 205
    Top = 64
    Width = 75
    Height = 23
    Anchors = [akLeft, akBottom]
    Caption = 'Add'
    Default = True
    ModalResult = 1
    TabOrder = 2
  end
  object Button2: TButton
    Left = 286
    Top = 64
    Width = 75
    Height = 23
    Anchors = [akLeft, akBottom]
    Cancel = True
    Caption = 'Cancel'
    ModalResult = 2
    TabOrder = 3
  end
  object cbMeaning: TComboBox
    Left = 88
    Top = 29
    Width = 471
    Height = 21
    Style = csDropDownList
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 1
  end
end
