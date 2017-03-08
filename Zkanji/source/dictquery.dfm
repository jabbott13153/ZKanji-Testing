object fDictQuery: TfDictQuery
  Left = 1304
  Top = 393
  Caption = 'fDictQuery'
  ClientHeight = 230
  ClientWidth = 465
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
    465
    230)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 96
    Height = 13
    Caption = 'Dictionary &name:'
    FocusControl = edName
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 8
    Top = 60
    Width = 115
    Height = 13
    Caption = 'Custom &information:'
    FocusControl = mInfo
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object b4: TBevel
    Left = 0
    Top = 199
    Width = 999999
    Height = 1
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 231
  end
  object b3: TBevel
    Left = 0
    Top = 197
    Width = 999999
    Height = 2
    Anchors = [akLeft, akBottom]
    Shape = bsTopLine
    ExplicitTop = 229
  end
  object Label3: TLabel
    Left = 144
    Top = 60
    Width = 159
    Height = 13
    Caption = '(Enter author name, license etc.)'
  end
  object edName: TEdit2
    Left = 8
    Top = 27
    Width = 449
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    TabOrder = 0
    OnChange = edNameChange
    OnExit = edNameExit
  end
  object mInfo: TMemo
    Left = 8
    Top = 80
    Width = 449
    Height = 105
    Anchors = [akLeft, akTop, akRight, akBottom]
    ScrollBars = ssVertical
    TabOrder = 1
    OnEnter = mInfoEnter
    OnExit = mInfoExit
    OnKeyPress = mInfoKeyPress
  end
  object btnCancel: TButton
    Left = 384
    Top = 203
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Cancel = True
    Caption = 'Cancel'
    TabOrder = 3
    OnClick = btnCancelClick
  end
  object btnOk: TButton
    Left = 195
    Top = 203
    Width = 75
    Height = 25
    Anchors = [akBottom]
    Caption = 'Ok'
    Default = True
    TabOrder = 2
    OnClick = btnOkClick
  end
end
