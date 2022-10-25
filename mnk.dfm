object Form1: TForm1
  Left = 40
  Top = 225
  Width = 696
  Height = 375
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object StringGrid1: TStringGrid
    Left = 200
    Top = 32
    Width = 320
    Height = 120
    TabOrder = 0
  end
  object BitBtn1: TBitBtn
    Left = 24
    Top = 24
    Width = 75
    Height = 25
    Caption = 'BitBtn1'
    TabOrder = 1
    OnClick = BitBtn1Click
  end
  object BitBtn2: TBitBtn
    Left = 112
    Top = 64
    Width = 75
    Height = 25
    Caption = 'BitBtn2'
    TabOrder = 2
    OnClick = BitBtn2Click
  end
end
