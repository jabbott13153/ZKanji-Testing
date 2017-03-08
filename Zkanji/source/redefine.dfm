object fRedefine: TfRedefine
  Left = 1032
  Top = 371
  ActiveControl = edMeaning
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSingle
  Caption = 'Word editor'
  ClientHeight = 475
  ClientWidth = 587
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001001800680300001600000028000000100000002000
    0000010018000000000040030000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000A4A0A0A4A0A0000000A4A0A0A4A0A0A4A0A0
    A4A0A0A4A0A0000000A4A0A0A4A0A0A4A0A0A4A0A0A4A0A0000000A4A0A00000
    00000000A4A0A0000000000000000000000000000000000000000000000000A4
    A0A0000000000000A4A0A0000000000000000000A4A0A0000000A4A0A0000000
    808080808080000000000000A4A0A0000000A4A0A0000000A4A0A00000000000
    00000000A4A0A0000000808080C0C0C0F2F2F2C0C0C07A7A7AA4A0A000000000
    0000A4A0A0000000A4A0A0000000000000000000A4A0A0808080F2F2F2FFFFFF
    FFFFFFF2F2F2C0C0C07A7A7A000000A4A0A0A4A0A0000000A4A0A00000000000
    00000000A4A0A0C0C0C0FFFFFFFFFFFFFFFFFFFFFFFFC0C0C0C0C0C07A7A7A00
    0000000000000000A4A0A0000000000000000000000000C0C0C0FFFFFFFFFFFF
    FFFFFFFFFFFFDADADA4848FF7A7A7A0000000000000000000000000000000000
    00000000000000000000C0C0C0FFFFFFFFFFFFDADADAFF8F6BFF8F6B4848FF7A
    7A7A000000000000000000000000000000000000000000000000C0C0C0DADADA
    FF8F6BFF8F6BFFC7B1FF8F6B4848FF7A7A7A0000000000000000000000000000
    00000000000000000000000000FF8F6BFFC7B1FFC7B1FFC7B1FFC7B1FF8F6B48
    48FF7A7A7A0000000000000000000000000000000000000000000000007A7A7A
    FFC7B1FFC7B1FFC7B1FFC7B1FFC7B14848FF7A7A7A0000000000000000000000
    00000000000000000000000000000000FF8F6BFFC7B1FFC7B1FFC7B1FF8F6BFF
    8F6BA4A0A0000000000000000000000000000000000000000000000000000000
    7A7A7AFFC7B1FF8F6BFF8F6BFFFFFFFFFFFFA4A0A00000000000000000000000
    00000000000000000000000000000000000000FF8F6BFFFFFFFFFFFFA4A0A0A4
    A0A0000000000000000000000000000000000000000000000000000000000000
    000000000000A4A0A0A4A0A0000000000000000000000000000000000000FFFF
    0000208200000380000084900000903000008020000000000000E01E0000F00E
    0000F00E0000F8070000F8070000FC070000FC070000FE0F0000FF3F0000}
  KeyPreview = True
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnCloseQuery = FormCloseQuery
  OnKeyPress = FormKeyPress
  OnPaint = FormPaint
  DesignSize = (
    587
    475)
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 5
    Top = 4
    Width = 268
    Height = 14
    Caption = 'Select the definition you would like to &edit:'
    FocusControl = pbWords
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object bv1: TBevel
    Left = 0
    Top = 185
    Width = 9999
    Height = 6
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object Label2: TLabel
    Left = 8
    Top = 193
    Width = 44
    Height = 13
    Caption = '&Meaning:'
    FocusControl = edMeaning
  end
  object bv2: TBevel
    Left = 0
    Top = 214
    Width = 9999
    Height = 4
    Shape = bsTopLine
  end
  object btnNew: TSpeedButton
    Left = 208
    Top = 219
    Width = 125
    Height = 23
    Hint = 'Add a new definition with the specified meaning.'
    Caption = 'Create &new definition'
    OnClick = btnNewClick
  end
  object btnSave: TSpeedButton
    Left = 335
    Top = 219
    Width = 82
    Height = 23
    Hint = 'Save changes so they don'#39't get lost! (Ctrl+Enter)'
    Caption = '&Save changes'
    Enabled = False
    Flat = True
    OnClick = btnSaveClick
  end
  object btnDelete: TSpeedButton
    Left = 496
    Top = 189
    Width = 89
    Height = 23
    Caption = '&Delete definition'
    OnClick = btnDeleteClick
  end
  object Label6: TLabel
    Left = 9
    Top = 223
    Width = 82
    Height = 13
    Anchors = [akTop, akRight]
    Caption = 'Word &frequency:'
    FocusControl = edFreq
  end
  object lbCount: TLabel
    Left = 4
    Top = 170
    Width = 37
    Height = 13
    Alignment = taRightJustify
    AutoSize = False
    Caption = '0 :'
  end
  object lbPos: TLabel
    Left = 45
    Top = 170
    Width = 30
    Height = 13
    AutoSize = False
    Caption = '0'
  end
  object bv3: TBevel
    Left = 492
    Top = 186
    Width = 2
    Height = 28
    Shape = bsLeftLine
  end
  object Bevel1: TBevel
    Left = 0
    Top = 167
    Width = 9999
    Height = 1
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object bv0: TBevel
    Left = 0
    Top = 23
    Width = 9999
    Height = 1
    Anchors = [akLeft, akTop, akRight]
    Shape = bsTopLine
  end
  object btnUp: TSpeedButton
    Tag = -1
    Left = 538
    Top = 0
    Width = 23
    Height = 23
    Hint = 'Move meaning up'
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
    OnMouseDown = btnUpMouseDown
  end
  object btnDown: TSpeedButton
    Tag = 1
    Left = 561
    Top = 0
    Width = 23
    Height = 23
    Hint = 'Move meaning down'
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
    OnMouseDown = btnUpMouseDown
  end
  object bv4: TBevel
    Left = 533
    Top = 0
    Width = 2
    Height = 23
    Shape = bsLeftLine
  end
  object pbWords: TZPaintBox
    Top = 24
    Width = 586
    Height = 143
    Anchors = [akLeft, akTop, akRight]
    BorderStyle = bsNone
    InvalidateOnScroll = True
    TabOrder = 0
  end
  object edMeaning: TEdit2
    Left = 56
    Top = 190
    Width = 431
    Height = 21
    Anchors = [akLeft, akTop, akRight]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnChange = edMeaningChange
  end
  object pBoxes: TPanel
    Left = 8
    Top = 246
    Width = 571
    Height = 223
    Anchors = [akLeft, akTop, akRight]
    BevelOuter = bvLowered
    FullRepaint = False
    TabOrder = 2
    object bvH: TBevel
      Left = 1
      Top = 112
      Width = 457
      Height = 2
      Shape = bsTopLine
    end
    object bvV: TBevel
      Left = 457
      Top = 1
      Width = 2
      Height = 221
      Shape = bsLeftLine
    end
    object Label3: TLabel
      Left = 470
      Top = 6
      Width = 29
      Height = 13
      Caption = 'F&ield:'
      FocusControl = cb45
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label4: TLabel
      Left = 6
      Top = 6
      Width = 62
      Height = 13
      Caption = 'Word &type:'
      FocusControl = cb1
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object Label5: TLabel
      Left = 6
      Top = 118
      Width = 38
      Height = 13
      Caption = '&Usage:'
      FocusControl = cb24
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Tahoma'
      Font.Style = [fsBold]
      ParentFont = False
    end
    object cb1: TCheckBox
      Left = 120
      Top = 8
      Width = 49
      Height = 17
      Caption = 'noun'
      TabOrder = 0
      OnClick = cb1Click
    end
    object cb2: TCheckBox
      Left = 232
      Top = 8
      Width = 73
      Height = 17
      Caption = 'takes -suru'
      TabOrder = 1
      OnClick = cb1Click
    end
    object cb3: TCheckBox
      Left = 344
      Top = 8
      Width = 79
      Height = 17
      Caption = 'Godan verb'
      TabOrder = 2
      OnClick = cb1Click
    end
    object cb4: TCheckBox
      Left = 8
      Top = 24
      Width = 74
      Height = 17
      Caption = 'expression'
      TabOrder = 3
      OnClick = cb1Click
    end
    object cb5: TCheckBox
      Left = 120
      Top = 24
      Width = 84
      Height = 17
      Caption = '-na adjective'
      TabOrder = 4
      OnClick = cb1Click
    end
    object cb6: TCheckBox
      Left = 232
      Top = 24
      Width = 89
      Height = 17
      Caption = 'transitive verb'
      TabOrder = 5
      OnClick = cb1Click
    end
    object cb7: TCheckBox
      Left = 344
      Top = 24
      Width = 83
      Height = 17
      Caption = 'Ichidan verb'
      TabOrder = 6
      OnClick = cb1Click
    end
    object cb8: TCheckBox
      Left = 8
      Top = 40
      Width = 96
      Height = 17
      Caption = 'intransitive verb'
      TabOrder = 7
      OnClick = cb1Click
    end
    object cb9: TCheckBox
      Left = 120
      Top = 40
      Width = 56
      Height = 17
      Caption = 'adverb'
      TabOrder = 8
      OnClick = cb1Click
    end
    object cb10: TCheckBox
      Left = 232
      Top = 40
      Width = 65
      Height = 17
      Caption = 'adjective'
      TabOrder = 9
      OnClick = cb1Click
    end
    object cb11: TCheckBox
      Left = 344
      Top = 40
      Width = 84
      Height = 17
      Caption = 'may take -no'
      TabOrder = 10
      OnClick = cb1Click
    end
    object cb12: TCheckBox
      Left = 8
      Top = 56
      Width = 48
      Height = 17
      Caption = 'suffix'
      TabOrder = 11
      OnClick = cb1Click
    end
    object cb13: TCheckBox
      Left = 120
      Top = 56
      Width = 91
      Height = 17
      Caption = 'Taru adjective'
      TabOrder = 12
      OnClick = cb1Click
    end
    object cb14: TCheckBox
      Left = 232
      Top = 56
      Width = 49
      Height = 17
      Caption = 'prefix'
      TabOrder = 13
      OnClick = cb1Click
    end
    object cb15: TCheckBox
      Left = 344
      Top = 56
      Width = 69
      Height = 17
      Caption = 'suru verb'
      TabOrder = 14
      OnClick = cb1Click
    end
    object cb16: TCheckBox
      Left = 8
      Top = 72
      Width = 88
      Height = 17
      Caption = 'pre -noun adj.'
      TabOrder = 15
      OnClick = cb1Click
    end
    object cb17: TCheckBox
      Left = 120
      Top = 72
      Width = 80
      Height = 17
      Caption = 'conjunction'
      TabOrder = 16
      OnClick = cb1Click
    end
    object cb18: TCheckBox
      Left = 232
      Top = 72
      Width = 73
      Height = 17
      Caption = 'interjection'
      TabOrder = 17
      OnClick = cb1Click
    end
    object cb19: TCheckBox
      Left = 344
      Top = 72
      Width = 67
      Height = 17
      Caption = '-iku verb'
      TabOrder = 18
      OnClick = cb1Click
    end
    object cb20: TCheckBox
      Left = 8
      Top = 88
      Width = 84
      Height = 17
      Caption = 'auxiliary verb'
      TabOrder = 19
      OnClick = cb1Click
    end
    object cb21: TCheckBox
      Left = 120
      Top = 88
      Width = 72
      Height = 17
      Caption = '-kuru verb'
      TabOrder = 20
      OnClick = cb1Click
    end
    object cb22: TCheckBox
      Left = 232
      Top = 88
      Width = 103
      Height = 17
      Caption = 'auxiliary adjective'
      TabOrder = 21
      OnClick = cb1Click
    end
    object cb23: TCheckBox
      Left = 344
      Top = 88
      Width = 69
      Height = 17
      Caption = '-aru verb'
      TabOrder = 22
      OnClick = cb1Click
    end
    object cb24: TCheckBox
      Left = 120
      Top = 120
      Width = 68
      Height = 17
      Caption = 'sensitivity'
      TabOrder = 23
      OnClick = cb1Click
    end
    object cb25: TCheckBox
      Left = 232
      Top = 120
      Width = 81
      Height = 17
      Caption = 'abbreviation'
      TabOrder = 24
      OnClick = cb1Click
    end
    object cb26: TCheckBox
      Left = 344
      Top = 120
      Width = 62
      Height = 17
      Caption = 'honorifix'
      TabOrder = 25
      OnClick = cb1Click
    end
    object cb27: TCheckBox
      Left = 8
      Top = 136
      Width = 105
      Height = 17
      Caption = 'humble language'
      TabOrder = 26
      OnClick = cb1Click
    end
    object cb28: TCheckBox
      Left = 120
      Top = 136
      Width = 83
      Height = 17
      Caption = 'colloquialism'
      TabOrder = 27
      OnClick = cb1Click
    end
    object cb29: TCheckBox
      Left = 232
      Top = 136
      Width = 48
      Height = 17
      Caption = 'polite'
      TabOrder = 28
      OnClick = cb1Click
    end
    object cb30: TCheckBox
      Left = 344
      Top = 136
      Width = 49
      Height = 17
      Caption = 'slang'
      TabOrder = 29
      OnClick = cb1Click
    end
    object cb31: TCheckBox
      Left = 8
      Top = 152
      Width = 102
      Height = 17
      Caption = 'familiar language'
      TabOrder = 30
      OnClick = cb1Click
    end
    object cb32: TCheckBox
      Left = 120
      Top = 152
      Width = 45
      Height = 17
      Caption = 'male'
      TabOrder = 31
      OnClick = cb1Click
    end
    object cb33: TCheckBox
      Left = 232
      Top = 152
      Width = 54
      Height = 17
      Caption = 'female'
      TabOrder = 32
      OnClick = cb1Click
    end
    object cb34: TCheckBox
      Left = 344
      Top = 152
      Width = 79
      Height = 17
      Caption = 'kana usage'
      TabOrder = 33
      OnClick = cb1Click
    end
    object cb35: TCheckBox
      Left = 8
      Top = 168
      Width = 93
      Height = 17
      Caption = 'only kanji used'
      TabOrder = 34
      OnClick = cb1Click
    end
    object cb36: TCheckBox
      Left = 120
      Top = 168
      Width = 65
      Height = 17
      Caption = 'archaism'
      TabOrder = 35
      OnClick = cb1Click
    end
    object cb37: TCheckBox
      Left = 232
      Top = 168
      Width = 42
      Height = 17
      Caption = 'rare'
      TabOrder = 36
      OnClick = cb1Click
    end
    object cb38: TCheckBox
      Left = 344
      Top = 168
      Width = 86
      Height = 17
      Caption = 'obsolete term'
      TabOrder = 37
      OnClick = cb1Click
    end
    object cb39: TCheckBox
      Left = 8
      Top = 184
      Width = 53
      Height = 17
      Caption = 'vulgar'
      TabOrder = 38
      OnClick = cb1Click
    end
    object cb40: TCheckBox
      Left = 120
      Top = 184
      Width = 57
      Height = 17
      Caption = 'X rated'
      TabOrder = 39
      OnClick = cb1Click
    end
    object cb41: TCheckBox
      Left = 232
      Top = 184
      Width = 75
      Height = 17
      Caption = 'derogatory'
      TabOrder = 40
      OnClick = cb1Click
    end
    object cb42: TCheckBox
      Left = 344
      Top = 184
      Width = 61
      Height = 17
      Caption = 'obscure'
      TabOrder = 41
      OnClick = cb1Click
    end
    object cb43: TCheckBox
      Left = 8
      Top = 200
      Width = 65
      Height = 17
      Caption = 'idiomatic'
      TabOrder = 42
      OnClick = cb1Click
    end
    object cb44: TCheckBox
      Left = 120
      Top = 200
      Width = 82
      Height = 17
      Caption = 'manga slang'
      TabOrder = 43
      OnClick = cb1Click
    end
    object cb45: TCheckBox
      Left = 472
      Top = 29
      Width = 46
      Height = 17
      Caption = 'math'
      TabOrder = 44
      OnClick = cb1Click
    end
    object cb46: TCheckBox
      Left = 472
      Top = 53
      Width = 68
      Height = 17
      Caption = 'linguistics'
      TabOrder = 45
      OnClick = cb1Click
    end
    object cb47: TCheckBox
      Left = 472
      Top = 77
      Width = 73
      Height = 17
      Caption = 'computers'
      TabOrder = 46
      OnClick = cb1Click
    end
    object cb48: TCheckBox
      Left = 472
      Top = 101
      Width = 44
      Height = 17
      Caption = 'food'
      TabOrder = 47
      OnClick = cb1Click
    end
    object cb49: TCheckBox
      Left = 472
      Top = 125
      Width = 73
      Height = 17
      Caption = 'martial arts'
      TabOrder = 48
      OnClick = cb1Click
    end
    object cb50: TCheckBox
      Left = 472
      Top = 149
      Width = 66
      Height = 17
      Caption = 'geometry'
      TabOrder = 49
      OnClick = cb1Click
    end
    object cb51: TCheckBox
      Left = 472
      Top = 173
      Width = 57
      Height = 17
      Caption = 'physics'
      TabOrder = 50
      OnClick = cb1Click
    end
    object cb52: TCheckBox
      Left = 472
      Top = 197
      Width = 89
      Height = 17
      Caption = 'Buddhist term'
      TabOrder = 51
      OnClick = cb1Click
    end
  end
  object BitBtn1: TBitBtn
    Left = 520
    Top = 219
    Width = 61
    Height = 23
    Cancel = True
    Caption = 'Close'
    DoubleBuffered = True
    Glyph.Data = {
      CA010000424DCA0100000000000042000000280000000D0000000E0000000100
      10000300000088010000120B0000120B00000000000000000000007C0000E003
      00001F000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F
      00000000FF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7FFF7F00000000FF7F00000000
      FF7FFF7FFF7FFF7F00000000FF7FFF7FFF7FFF7F0000FF7F00000000FF7F0000
      0000FF7FFF7F0000FF7FFF7FFF7FFF7F0000FF7F00000000FF7FFF7FFF7F0000
      FF7F0000FF7FFF7FFF7FFF7F0000FF7F00000000FF7FFF7FFF7FFF7F00000000
      FF7FFF7FFF7FFF7F0000FF7F00000000FF7F0000000000000000000000000000
      0000FF7F0000FF7F00000000FF7FFF7FFF7FFF7FFF7F0000FF7FFF7FFF7FFF7F
      0000FF7F00000000FF7FFF7FFF7FFF7FFF7F0000FF7FFF7FFF7FFF7F0000FF7F
      000000000000000000000000FF7FFF7F00000000000000000000FF7F00000000
      FF7FFF7FFF7F0000FF7FFF7F0000FF7FFF7FFF7F0000FF7F0000000000000000
      00000000FF7FFF7F00000000000000000000FF7F00000000FF7FFF7FFF7F0000
      FF7FFF7F0000FF7FFF7FFF7F0000FF7F000000000000000000000000FF7FFF7F
      00000000000000000000FF7F0000}
    ModalResult = 1
    ParentDoubleBuffered = False
    TabOrder = 3
  end
  object edFreq: TEdit2
    Left = 92
    Top = 219
    Width = 87
    Height = 21
    Hint = 
      'This number is set to all meanings. Above 3500 is popular, below' +
      ' 1500 is unpopular.'
    Anchors = [akTop, akRight]
    TabOrder = 4
    Text = '0'
  end
end
