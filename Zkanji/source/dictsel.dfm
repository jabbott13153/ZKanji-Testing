object fDictSel: TfDictSel
  Left = 949
  Top = 380
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Dictionary selector'
  ClientHeight = 122
  ClientWidth = 367
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnPaint = FormPaint
  PixelsPerInch = 96
  TextHeight = 13
  object lbMsg: TLabel
    Left = 6
    Top = 6
    Width = 330
    Height = 30
    Caption = 
      'Two lines of message in case someone needs to specify something ' +
      'specific because  this is how dialogs work, right?'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
    WordWrap = True
  end
  object pControls: TPanel
    Left = 0
    Top = 40
    Width = 367
    Height = 82
    BevelOuter = bvNone
    FullRepaint = False
    TabOrder = 0
    DesignSize = (
      367
      82)
    object Label1: TLabel
      Left = 50
      Top = 15
      Width = 61
      Height = 13
      Alignment = taRightJustify
      Caption = 'Dictionary:'
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object bvBottom: TBevel
      Left = 0
      Top = 43
      Width = 9999999
      Height = 2
      Shape = bsTopLine
    end
    object cbDict: TComboBox
      Left = 123
      Top = 12
      Width = 185
      Height = 21
      Style = csDropDownList
      TabOrder = 0
      OnChange = cbDictChange
      OnClick = cbDictClick
    end
    object btnOk: TButton
      Left = 106
      Top = 51
      Width = 75
      Height = 25
      Anchors = [akLeft, akBottom]
      Caption = 'Select'
      Default = True
      TabOrder = 1
      OnClick = btnOkClick
    end
    object btnCancel: TButton
      Left = 186
      Top = 51
      Width = 75
      Height = 25
      Anchors = [akLeft, akBottom]
      Cancel = True
      Caption = 'Cancel'
      TabOrder = 2
      OnClick = btnCancelClick
    end
  end
end
