object fElemSettings: TfElemSettings
  Left = 668
  Top = 431
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'fElemSettings'
  ClientHeight = 85
  ClientWidth = 337
  Color = clWhite
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  OnClose = FormClose
  OnDeactivate = FormDeactivate
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 5
    Top = 5
    Width = 101
    Height = 13
    Caption = 'Background kanji:'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Label2: TLabel
    Left = 232
    Top = 28
    Width = 23
    Height = 13
    Caption = '&Size:'
    FocusControl = cbSizes
  end
  object Label3: TLabel
    Left = 13
    Top = 28
    Width = 31
    Height = 13
    Caption = '&Name:'
    FocusControl = cbFonts
  end
  object cbFonts: TComboBox
    Left = 48
    Top = 24
    Width = 177
    Height = 21
    TabOrder = 0
    OnChange = cbFontsChange
  end
  object btnColor: TButton
    Left = 8
    Top = 51
    Width = 89
    Height = 25
    Caption = 'Select &color...'
    TabOrder = 1
    OnClick = btnColorClick
  end
  object pbColor: TZPaintBox
    Left = 104
    Top = 51
    Width = 65
    Height = 25
    HorzScrollBar.Visible = False
    TabOrder = 2
    VertScrollBar.Visible = False
    OnPaint = pbColorPaint
  end
  object cbSizes: TComboBox
    Left = 259
    Top = 24
    Width = 69
    Height = 21
    Style = csDropDownList
    ItemIndex = 2
    TabOrder = 3
    Text = 'Normal'
    OnChange = cbSizesChange
    Items.Strings = (
      'Tiny'
      'Small'
      'Normal'
      'Large'
      'Huge')
  end
  object btnOk: TBitBtn
    Left = 178
    Top = 51
    Width = 72
    Height = 25
    Caption = '&Ok'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 4
    OnClick = btnOkClick
  end
  object btnCancel: TBitBtn
    Left = 257
    Top = 51
    Width = 72
    Height = 25
    Caption = 'Cancel'
    DoubleBuffered = True
    ParentDoubleBuffered = False
    TabOrder = 5
    OnClick = btnCancelClick
  end
  object cbForceCentre: TCheckBox
    Left = 238
    Top = 3
    Width = 94
    Height = 17
    Hint = 
      'Because of imperfections in the system many fonts cannot be cent' +
      'ered automatically. Check this box if you want each kanji measur' +
      'ed before they are drawn to the background for perfect vertical ' +
      'centering.'
    Caption = 'Force centered'
    TabOrder = 6
    OnClick = cbForceCentreClick
  end
  object dColor: TColorDialog
    Options = [cdAnyColor]
    Left = 8
    Top = 19
  end
end
