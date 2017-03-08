object fMsg: TfMsg
  Left = 1224
  Top = 556
  BorderIcons = []
  BorderStyle = bsNone
  ClientHeight = 34
  ClientWidth = 403
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnActivate = FormActivate
  OnPaint = FormPaint
  DesignSize = (
    403
    34)
  PixelsPerInch = 96
  TextHeight = 13
  object lbMsg: TLabel
    AlignWithMargins = True
    Left = 3
    Top = 0
    Width = 317
    Height = 34
    Margins.Top = 0
    Margins.Right = 83
    Margins.Bottom = 0
    Align = alClient
    Alignment = taCenter
    AutoSize = False
    Caption = 'Importing example sentences. This can take several minutes...'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    Transparent = True
    Layout = tlCenter
    ExplicitWidth = 487
    ExplicitHeight = 33
  end
  object btnAbort: TButton
    Left = 324
    Top = 4
    Width = 75
    Height = 25
    Anchors = [akTop, akRight]
    Caption = 'Abort'
    TabOrder = 0
    OnClick = btnAbortClick
    ExplicitLeft = 344
  end
end
