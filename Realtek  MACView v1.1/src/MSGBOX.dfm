object frmMsg: TfrmMsg
  Left = 0
  Top = 0
  BorderIcons = []
  BorderStyle = bsToolWindow
  ClientHeight = 240
  ClientWidth = 551
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object plWOInfo: TPanel
    Left = 0
    Top = 0
    Width = 551
    Height = 233
    Align = alTop
    Color = clWhite
    ParentBackground = False
    TabOrder = 0
    object Label2: TLabel
      Left = 1
      Top = 1
      Width = 549
      Height = 48
      Align = alTop
      Alignment = taCenter
      AutoSize = False
      Caption = #36664#20837#24037#21934#36039#35338
      Color = clTeal
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -43
      Font.Name = #27161#26999#39636
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      Transparent = False
      ExplicitLeft = 2
      ExplicitTop = -7
      ExplicitWidth = 439
    end
    object Panel39: TPanel
      Left = 1
      Top = 44
      Width = 549
      Height = 47
      Align = alBottom
      Color = clCream
      ParentBackground = False
      TabOrder = 0
      ExplicitTop = 51
      object Panel40: TPanel
        Left = 1
        Top = 1
        Width = 176
        Height = 45
        Align = alLeft
        Caption = #24037#21934#34399
        Color = clGradientActiveCaption
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -24
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentBackground = False
        ParentFont = False
        TabOrder = 0
      end
      object edtSetWorkOrderNumber: TEdit
        Left = 177
        Top = 1
        Width = 371
        Height = 45
        Align = alClient
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlue
        Font.Height = -37
        Font.Name = 'Times New Roman'
        Font.Style = []
        ImeMode = imSAlpha
        ParentFont = False
        TabOrder = 1
        Text = '000-00000000000'
        OnKeyDown = edtSetWorkOrderNumberKeyDown
        ExplicitHeight = 50
      end
    end
    object Panel5: TPanel
      Left = 1
      Top = 185
      Width = 549
      Height = 47
      Align = alBottom
      Color = clCream
      ParentBackground = False
      TabOrder = 1
      ExplicitTop = 145
      object edtSetEmployeeID: TEdit
        Left = 177
        Top = 1
        Width = 249
        Height = 45
        Align = alClient
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlue
        Font.Height = -37
        Font.Name = 'Times New Roman'
        Font.Style = []
        ImeMode = imSAlpha
        ParentFont = False
        TabOrder = 0
        Text = 'T00000'
        OnKeyDown = edtSetEmployeeIDKeyDown
        ExplicitHeight = 50
      end
      object Panel41: TPanel
        Left = 1
        Top = 1
        Width = 176
        Height = 45
        Align = alLeft
        Caption = #24037#34399
        Color = clSkyBlue
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -24
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentBackground = False
        ParentFont = False
        TabOrder = 1
      end
      object btnID_ok: TBitBtn
        Left = 426
        Top = 1
        Width = 122
        Height = 45
        Align = alRight
        Caption = #30906#23450
        DoubleBuffered = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -33
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentDoubleBuffered = False
        ParentFont = False
        TabOrder = 2
        OnClick = btnID_okClick
      end
    end
    object Panel2: TPanel
      Left = 1
      Top = 138
      Width = 549
      Height = 47
      Align = alBottom
      Color = clCream
      ParentBackground = False
      TabOrder = 2
      ExplicitTop = 98
      object Panel3: TPanel
        Left = 1
        Top = 1
        Width = 176
        Height = 45
        Align = alLeft
        Caption = #29986#21697#22411#34399
        Color = clGradientActiveCaption
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -24
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentBackground = False
        ParentFont = False
        TabOrder = 0
      end
      object edtSetModel: TEdit
        Left = 177
        Top = 1
        Width = 371
        Height = 45
        Align = alClient
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlue
        Font.Height = -37
        Font.Name = 'Times New Roman'
        Font.Style = []
        ImeMode = imSAlpha
        ParentFont = False
        TabOrder = 1
        OnKeyDown = edtSetModelKeyDown
        ExplicitWidth = 320
      end
    end
    object Panel1: TPanel
      Left = 1
      Top = 91
      Width = 549
      Height = 47
      Align = alBottom
      Color = clCream
      ParentBackground = False
      TabOrder = 3
      ExplicitTop = 98
      object Panel6: TPanel
        Left = 1
        Top = 1
        Width = 176
        Height = 45
        Align = alLeft
        Caption = #24037#31449
        Color = clGradientActiveCaption
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -24
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentBackground = False
        ParentFont = False
        TabOrder = 0
      end
      object edtWorkStation: TEdit
        Left = 177
        Top = 1
        Width = 371
        Height = 45
        Align = alClient
        Alignment = taCenter
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clBlue
        Font.Height = -37
        Font.Name = 'Times New Roman'
        Font.Style = []
        ImeMode = imSAlpha
        ParentFont = False
        TabOrder = 1
        OnKeyDown = edtWorkStationKeyDown
        ExplicitHeight = 50
      end
    end
  end
  object TouchKeyboard: TTouchKeyboard
    Left = 0
    Top = 241
    Width = 551
    Height = 180
    Align = alTop
    GradientEnd = clSilver
    GradientStart = clGray
    Layout = 'Standard'
  end
  object plSwitch: TPanel
    Left = 0
    Top = 233
    Width = 551
    Height = 8
    Align = alTop
    Caption = 'V'
    Color = clHighlight
    ParentBackground = False
    TabOrder = 2
    OnClick = plSwitchClick
    ExplicitTop = 193
  end
  object plBarcode: TPanel
    Left = 0
    Top = 421
    Width = 551
    Height = 132
    Align = alTop
    Color = clWhite
    ParentBackground = False
    TabOrder = 3
    ExplicitTop = 381
    object lbBarcode: TLabel
      Left = 1
      Top = 1
      Width = 549
      Height = 48
      Align = alTop
      Alignment = taCenter
      AutoSize = False
      Caption = #35531#25475#25551'MAC'#26781#30908
      Color = clTeal
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -43
      Font.Name = #27161#26999#39636
      Font.Style = [fsBold]
      ParentColor = False
      ParentFont = False
      Transparent = False
      ExplicitLeft = 2
      ExplicitTop = -7
      ExplicitWidth = 439
    end
    object Panel4: TPanel
      Left = 1
      Top = 90
      Width = 549
      Height = 41
      Align = alBottom
      Caption = 'Panel4'
      TabOrder = 0
      object BitBtn1: TBitBtn
        Left = 272
        Top = 1
        Width = 276
        Height = 39
        Align = alRight
        Caption = #21462#28040
        DoubleBuffered = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -33
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentDoubleBuffered = False
        ParentFont = False
        TabOrder = 0
        OnClick = BitBtn1Click
      end
      object BitBtn2: TBitBtn
        Left = -4
        Top = 1
        Width = 276
        Height = 39
        Align = alRight
        Caption = #28165#38500
        DoubleBuffered = True
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clWindowText
        Font.Height = -33
        Font.Name = #27161#26999#39636
        Font.Style = []
        ParentDoubleBuffered = False
        ParentFont = False
        TabOrder = 1
        OnClick = BitBtn2Click
      end
    end
    object edtBarcodeMAC: TEdit
      Left = 1
      Top = 49
      Width = 549
      Height = 41
      Align = alClient
      Alignment = taCenter
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clBlue
      Font.Height = -37
      Font.Name = 'Times New Roman'
      Font.Style = []
      ImeMode = imSAlpha
      ParentFont = False
      TabOrder = 1
      OnChange = edtBarcodeMACChange
      OnKeyDown = edtBarcodeMACKeyDown
      ExplicitHeight = 50
    end
  end
end
