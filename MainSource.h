//---------------------------------------------------------------------------

#ifndef MainSourceH
#define MainSourceH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
#include <Dialogs.hpp>
#include <ComCtrls.hpp>
#include <Buttons.hpp>
#include <Mask.hpp>
#include <Grids.hpp>
#include <Chart.hpp>
#include <Series.hpp>
#include <TeEngine.hpp>
#include <TeeProcs.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TMainMenu *MainMenu1;
	TPopupMenu *PopupMenu1;
	TMenuItem *N1;
	TMenuItem *NOpen;
	TMenuItem *NSave;
	TMenuItem *NExit;
	TMenuItem *N5;
	TMenuItem *NFont;
	TMenuItem *N9;
	TMenuItem *NKoeff;
	TMenuItem *NPointClear;
	TPanel *PanelNewPoint;
	TLabel *Label4;
	TMenuItem *NNew;
	TLabel *Label7;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TFontDialog *FontDialog1;
	TMenuItem *N3D;
	TMenuItem *NAdd;
	TMenuItem *NDelete;
	TMenuItem *N18;
	TMenuItem *NAbout;
	TPopupMenu *PopupMenu2;
	TMenuItem *NFont2;
	TMenuItem *Nformat;
	TMenuItem *NGridClear;
	TColorDialog *ColorDialog1;
	TBitBtn *Add;
	TBitBtn *RunBtn;
	TStringGrid *Grid;
	TChart *Chart1;
	TLineSeries *Series1;
	TMenuItem *NGColor;
	TGroupBox *PanelApprox;
	TLineSeries *Series2;
	TBitBtn *RunGraph;
	TLabeledEdit *NewXE;
	TUpDown *NewX;
	TLabeledEdit *NewYE;
	TUpDown *NewY;
	TUpDown *AppXk;
	TLabeledEdit *AppXnE;
	TUpDown *AppXn;
	TUpDown *AppStep;
	TUpDown *Degree;
	TLabeledEdit *DegreeE;
	TLabeledEdit *AppXkE;
	TLabeledEdit *AppStepE;
	TPanel *PanelLeft;
	TMemo *Out;
	void __fastcall NExitClick(TObject *Sender);
	void __fastcall RunClick(TObject *Sender);
	void __fastcall NNewClick(TObject *Sender);
	void __fastcall NOpenClick(TObject *Sender);
	void __fastcall NSaveClick(TObject *Sender);
	void __fastcall NFontClick(TObject *Sender);
	void __fastcall NDeleteClick(TObject *Sender);
	void __fastcall NAboutClick(TObject *Sender);
	void __fastcall Edit3KeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift);
	void __fastcall Edit4KeyDown(TObject *Sender, WORD &Key,
		TShiftState Shift);
	void __fastcall NKoeffClick(TObject *Sender);
	void __fastcall NFont2Click(TObject *Sender);
	void __fastcall NGridClearClick(TObject *Sender);
	void __fastcall NformatClick(TObject *Sender);
	void __fastcall NPointClearClick(TObject *Sender);
	void __fastcall AddClick(TObject *Sender);
	void __fastcall N3DClick(TObject *Sender);
	void __fastcall NGColorClick(TObject *Sender);
	void __fastcall RunGraphClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
//Только в пределах данного модуля
	//void __fastcall F1(char c);
	//int ij;

public:		// User declarations
	int minX, maxX;
	double *a, *b, *x, *y, **sums;
	int N, K;
   int CapacityK;
   int CapacityN;
	unsigned int point;
	__fastcall TForm1(TComponent* Owner); //Объявление конструктора
//   double pow(double a, double b)
//   {
//   	int res = 1;
//   	for (int i = 0; i < b; i++)
//      	res *= a;
//      return res;
//   }
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif