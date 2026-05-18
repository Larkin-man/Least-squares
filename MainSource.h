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
#include <VclTee.TeeGDIPlus.hpp>
#include <VCLTee.Chart.hpp>
#include <VCLTee.Series.hpp>
#include <VCLTee.TeEngine.hpp>
#include <VCLTee.TeeProcs.hpp>
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
	TMenuItem *NNew;
	TFontDialog *FontDialog1;
	TMenuItem *N3D;
	TMenuItem *NAdd;
	TPopupMenu *PopupMenu2;
	TMenuItem *NFont2;
	TMenuItem *Nformat;
	TMenuItem *NGridClear;
	TColorDialog *ColorDialog1;
	TBitBtn *RunBtn;
	TChart *Chart1;
	TLineSeries *Series1;
	TMenuItem *NGColor;
	TGroupBox *PanelApprox;
	TLineSeries *Series2;
	TBitBtn *RunGraph;
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
	TMenuItem *NPrints2;
	TCheckBox *OnlyX;
	TSplitter *Splitter1;
	TPanel *Panel1;
   TLabel *Label1;
   TNotebook *Notebook1;
   TStringGrid *Grid;
   TGroupBox *GroupBox1;
   TRadioButton *RadioNormal;
   TRadioButton *RadioChebyshev;
   TPanel *PanelNewPoint;
   TLabel *Label4;
   TLabel *Label7;
   TBitBtn *Add;
   TLabeledEdit *NewXE;
   TUpDown *NewX;
   TUpDown *NewY;
   TLabeledEdit *NewYE;
   TMenuItem *NXS;
   TMenuItem *N2;
   TMemo *Ins;
   TBitBtn *BitBtn1;
   TMemo *Memo1;
	void __fastcall NExitClick(TObject *Sender);
	void __fastcall RunClick(TObject *Sender);
	void __fastcall NNewClick(TObject *Sender);
	void __fastcall NOpenClick(TObject *Sender);
	void __fastcall NSaveClick(TObject *Sender);
	void __fastcall NFontClick(TObject *Sender);
	void __fastcall NDeleteClick(TObject *Sender);
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
	void __fastcall DegreeChanging(TObject *Sender, bool &AllowChange);
	void __fastcall OnlyXClick(TObject *Sender);
	void __fastcall GridRowMoved(TObject *Sender, int FromIndex, int ToIndex);
   void __fastcall NXSClick(TObject *Sender);
   void __fastcall DegreeEChange(TObject *Sender);
   void __fastcall BitBtn1Click(TObject *Sender);
private:

public:
	int N, S;
	typedef double T;
	std::vector<double> x, y;
	unsigned int point;
	__fastcall TForm1(TComponent* Owner); //Объявление конструктора
	void AddRedDot(double x);
	inline void Exchange(T &a, T &b);
	std::vector<double> PolyFit(const std::vector<double>& x, const std::vector<double>& y, int degree);
	double GetPredict(const LSResult& res, double x);
	void CalculateError(const std::vector<double>& x, const std::vector<double>& y, LSResult& res);
	void tolog(String str)
	{
		Out->Lines->Append(str);
	}
	void logv(const std::vector<T> &c, String nam="")
	{
		String str;
		for (std::vector<T>::const_iterator el = c.begin(); el != c.end(); ++el)
			str += FloatToStr(*el)+" ";
		if (nam.IsEmpty())
			Out->Lines->Append(str);
		else
			Out->Lines->Append(nam+" "+str);
	}
	TLeastSquares ti;
   TChebResult ch;

   enum ResultType {RT_NONE, RT_LS, RT_CHEB};
   struct LMSOutResult
   {
      LSResult ls;
      TChebResult cheb;
      ResultType type;
      // Конструктор по умолчанию
      LMSOutResult()
      {
         type = RT_NONE;
      }

      // Метод сброса флага и очистки векторов для освобождения памяти
      void Clear()
      {
         type = RT_NONE;
         cheb.coeffs.clear();
         ls.coeffs.clear();
      }

      // Оператор присваивания для TChebResult
      LMSOutResult& operator=(const TChebResult& src)
      {
         this->cheb = src;
         this->type = RT_CHEB;
         return *this;
      }

      // Оператор присваивания для LSResult
      LMSOutResult& operator=(const LSResult& src)
      {
         this->ls = src;
         this->type = RT_LS;
         return *this;
      }

      // Доступ к общему вектору coeffs по ссылке (для чтения и изменения)
      std::vector<double>& coeffs()
      {
         return (type == RT_CHEB) ? cheb.coeffs : ls.coeffs;
      }
      // Константная версия coeffs для безопасного чтения
      const std::vector<double>& coeffs() const
      {
         return (type == RT_CHEB) ? cheb.coeffs : ls.coeffs;
      }
      // Доступ к остальным общим полям
      double rmse() const
      {
         return (type == RT_CHEB) ? cheb.rmse : ls.rmse;
      }
      double r2() const
      {
         return (type == RT_CHEB) ? cheb.r2 : ls.r2;
      }

      bool success() const
      {
         return (type == RT_CHEB) ? cheb.success : ls.success;
      }

   } res;
   void BringColor(int NewDegree);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif