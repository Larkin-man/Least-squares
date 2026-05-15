//------------файл реализации---------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <float.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "LMSUnit.h"
#include "ChebyshevUnit.h"
#include "NamesLMSUnit.h"
#pragma hdrstop

#include "Table.h"
#include "MainSource.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
	Grid->Cells[0][0] = "№";
	Grid->Cells[1][0] = "X";
	Grid->Cells[2][0] = "Y";
	Grid->ColWidths[0] = 24;
	point = 1;
	NNewClick(NULL);
	//OpenDialog1->InitialDir = GetCurrentDir();
	//SaveDialog1->InitialDir = GetCurrentDir();
   Notebook1->PageIndex = 0;
}
//---------------------------------------------------------------------------
//Начать заново
void __fastcall TForm1::NNewClick(TObject *Sender)
{
	Out->Clear();
	for (unsigned int i=1; i<point+1; i++)
	{
		Grid->Cells[0][i]="";
		Grid->Cells[1][i]="";
		Grid->Cells[2][i]="";
	}
	point = 1;
	Label7->Caption = 1;
	Series1->Clear();
	Series2->Clear();
	RunGraph->Enabled = false;
	RunBtn->Enabled = false;
}
//---------------------------------------------------------------------------
void CheckDot(TLabeledEdit* &Sender)
{
	int p;
	if ((p = Sender->Text.Pos('.')) <= 0)
		return;
	String s(Sender->Text);
	s[p] = ',';
	Sender->Text = s;
}
//---------------------------------------------------------------------------
//ДОБАВИТЬ
void __fastcall TForm1::AddClick(TObject *Sender)
{
	CheckDot(NewXE);
	CheckDot(NewYE);
	double X = NewXE->Text.ToDouble();
	double Y = NewYE->Text.ToDouble();
	Grid->Cells[0][point] = point;
	Grid->Cells[1][point] = X;
	Grid->Cells[2][point] = Y;
	if (point <= 1)
	{
		AppXn->Position = X;
		AppXk->Position = X;
	}
	else
	{
		if (X < AppXn->Position)
			AppXn->Position = X-1;
		if (X > AppXk->Position)
			AppXk->Position = X+1;
	}
	point++;
	Label7->Caption = point;
	if (Grid->RowCount < (int)point)
		Grid->RowCount++;
	//Series1->Add(NewX->FloatValue,"Цех1",clNavy);
	Series1->AddXY(X, Y, "", clNavy);
	//StatusBar1->SimpleText=("");
	RunBtn->Enabled = true;
   BringColor(Degree->Position);
}
//---------------------------------------------------------------------------
void __fastcall TForm1::NExitClick(TObject *Sender)
{
	Close();
}
//---------------------------------------------------------------------------
inline void TForm1::Exchange(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}
//---------------------------------------------------------------------------
// Функция для расчета ошибки после нахождения коэффициентов
void TForm1::CalculateError(const std::vector<double>& x, const std::vector<double>& y, LSResult& res)
{
	if (!res.success) {
		res.rmse = -1.0;
		return;
	}

	double sse = 0.0; // Сумма квадратов отклонений
	int n = (int)x.size();

	for (int i = 0; i < n; i++) {
		// Считаем значение полинома в данной точке
		double y_pred = 0.0;
		double x_shifted = x[i] - res.shiftX;
		double x_pow = 1.0;
		for (size_t j = 0; j < res.coeffs.size(); j++) {
			y_pred += res.coeffs[j] * x_pow;
			x_pow *= x_shifted;
		}

		// Квадрат разности между реальным y и предсказанным
		double diff = y[i] - y_pred;
		sse += diff * diff;
	}

	// RMSE = корень из среднего квадрата ошибки
	res.rmse = std::sqrt(sse / n);
}
//---------------------------------------------------------------------------
//Вычислить
void __fastcall TForm1::RunClick(TObject *Sender)
{
	N = point - 1;
	S = Degree->Position;
	if (N < S)
	{
		Out->Lines->Add("Недостаточно входных данных для расчета.");
		return;
	}
	Form2->StringGrid1->ColCount = S+2;
	Form2->StringGrid1->RowCount = S+1;
	x.resize(N);
	y.resize(N);
	for(int p=0; p<N; p++)
	{
		x[p] = StrToFloat(Grid->Cells[1][p+1]);
		y[p] = StrToFloat(Grid->Cells[2][p+1]);
	}
   res.Clear();
   if (RadioNormal->Checked)
	   res = TLeastSquares::Calculate(x, y, S);
   else if (RadioChebyshev->Checked)
   {
      TChebyshevLMS::Calculate(x, y, S, ch); // Степень 5
      res = ch;
   }
	if (res.success == false)
		return;

	if (Out->Lines->Count > 0)
		Out->Lines->Append("===== "+IntToStr(S)+ " =====");

	for(int i=0; i<S+1; i++)
   {
	   // ffGeneral — умный формат: уберет лишние нули и округлит "мусор" в конце
      // 12 — общая точность (хватит для double)
      // 4 — если число станет очень маленьким, перейдет в экспоненциальный вид (1E-15)
      //String value = FloatToStrF(res.coeffs[i], ffGeneral, 12, 4);
      double val = res.coeffs[i];
      // Если число по модулю меньше одной триллионной — это шум
      if (std::abs(val) < 1e-12) val = 0.0;
      // Теперь выводим с хорошей точностью (14 знаков)
      // Но шум уже превращен в 0.0 и выведется как "0"
      String value = FloatToStrF(val, ffGeneral, 14, 0);

      Out->Lines->Add("C[" + IntToStr(i) + "] = " + value);
   }
   if (res.Method == LMSOutResult::Norm)
   {
      tolog("shiftX = \n"+FloatToStr(res.shiftX));
      tolog(TLeastSquares::GetFormulaString(res));
   }
   else
   {
      tolog(TChebyshevLMS::GetFormulaString(ch));
      String status;
      switch(res.quality)
      {
          case fqExact:      status = "Perfect (Machine Zero)"; break;
          case fqNoise:      status = "High Precision (Double Noise)"; break;
          case fqApprox:     status = "Approximation"; break;
          case fqInaccurate: status = "Low Accuracy / Check Data"; break;
      }

      tolog("Quality: " + status);
   }
	tolog("RMSE = " + FloatToStrF(res.rmse, ffExponent, 4, 2));
	tolog("R2 = \n"+FloatToStr(res.r2));

	if (NPrints2->Checked)
	{
		double s2 = 0;
		double mino = DBL_MAX;
		double maxo = DBL_MIN;
		for(int j=0; j<N; j++)
		{
			double cu = 0;
			for(int i=0; i<S+1; i++)
				cu += res.coeffs[i] * pow(x[j], i);
			s2 += (y[j] - cu) * (y[j] - cu);

			{
				Out->Lines->Append(FloatToStr(cu)+" "+FloatToStr(y[j]-cu));
				if ((y[j] - cu) < mino)
					mino = y[j] - cu;
				if ((y[j] - cu) > maxo)
					maxo = y[j] - cu;
			}
		}
		Out->Lines->Add("Минимальное отклонение");
		Out->Lines->Add(FloatToStr(mino));
		Out->Lines->Add("Максимальное отклонение");
		Out->Lines->Add(FloatToStr(maxo));
	}
	//Out->Lines->Add("Сумма квадратов отклонений");
	//Out->Lines->Add(FloatToStr(s2));
	RunGraph->Enabled = true;
}
//---------------------------------------------------------------------------
//Функция загружает список из текстового файла
void __fastcall TForm1::NOpenClick(TObject *Sender)
{
	//if(OpenDialog1->Execute()==IDOK)
	{
		FILE *file = fopen("data.txt", "rt");
		if (!file)
		{
			ShowMessage ( "Cannot open text file");
			return;
		}
		int n;
		float fx, fy;
		int EoF;
		fseek(file, 0, SEEK_END);
		EoF = ftell(file);
		fseek(file, 0, SEEK_SET);
		if ((unsigned int)Grid->RowCount < point)
			Grid->RowCount = point+1;
		point = 1;
		while (ftell(file) < EoF)
		{
			fscanf(file, "%g\t", &fx);
			fscanf(file, "%g\n", &fy);
			if ((unsigned int)Grid->RowCount <= point)
				Grid->RowCount++;
			Grid->Cells[0][point] = point;
			Grid->Cells[1][point] = fx;
			Grid->Cells[2][point] = fy;
			if (fx < AppXn->Position)
				AppXnE->Text = fx-1;
			if (fx > AppXk->Position)
				AppXkE->Text = fx+1;
			Series1->AddXY(fx, fy,"",clBlue);
			point++;
		}
		Label7->Caption = point;
		fclose(file);
	}
	RunGraph->Enabled = true;
	RunBtn->Enabled = true;
	Degree->Position = point-2;
}
//---------------------------------------------------------------------------
//Функция сохраняет список в текстовый файл
void __fastcall TForm1::NSaveClick(TObject *Sender)
{
	//if(SaveDialog1->Execute()==IDOK)
	{
		//FILE *file = _wfopen (SaveDialog1->FileName.w_str(), L"wt");
		FILE *file = fopen("data.txt", "wt");
		if (!file)
		{
			ShowMessage ( "Cannot create text file");
			return;
		}
		for (unsigned int i = 1; i < point; i++)
		{
			float f1 = StrToFloat(Grid->Cells[1][i]);
			float f2 = StrToFloat(Grid->Cells[2][i]);
			fprintf(file, "%g\t", f1);
			fprintf(file, "%g\n", f2);
		}
		fclose(file);
		ShowMessage ( "Данные сохранены!");
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NFontClick(TObject *Sender)
{
	if (FontDialog1->Execute()==ID_OK)
		Form1->Font=FontDialog1->Font;
}
//---------------------------------------------------------------------------
//Удалить точку
void __fastcall TForm1::NDeleteClick(TObject *Sender)
{
	if (Grid->RowCount < 1)
		return;
	point--;
	for (int i = Grid->Row; i < Grid->RowCount-1; i++)
	{
		Grid->Cells[0][i] = i;
		Grid->Cells[1][i] = Grid->Cells[1][i+1];
		Grid->Cells[2][i] = Grid->Cells[2][i+1];
	}
	Grid->RowCount--;
	Label7->Caption = point;
	Series1->Delete(Grid->Row-1);
   BringColor(Degree->Position);
}
//---------------------------------------------------------------------------
//Таб
void __fastcall TForm1::Edit3KeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	if(Key == VK_RETURN)
		NewY->SetFocus();
}
//---------------------------------------------------------------------------
//Таб 2
void __fastcall TForm1::Edit4KeyDown(TObject *Sender, WORD &Key,
	TShiftState Shift)
{
	if(Key == VK_RETURN)
		RunBtn->SetFocus();
}
//---------------------------------------------------------------------------
//Показать коэффициенты
void __fastcall TForm1::NKoeffClick(TObject *Sender)
{
	Form2->Show();
}
//---------------------------------------------------------------------------
//Шрифт
void __fastcall TForm1::NFont2Click(TObject *Sender)
{
	if (FontDialog1->Execute())
	{
		Grid->Font=FontDialog1->Font;
		Out->Font=FontDialog1->Font;
	}
}
//---------------------------------------------------------------------------
//Очистить список точек
void __fastcall TForm1::NGridClearClick(TObject *Sender)
{
	for (unsigned int i=1;i<point;i++)
	{
		Grid->Cells[0][point]="";
		Grid->Cells[1][point]="";
		Grid->Cells[2][point]="";
	}
	point = 1;
	Label7->Caption = point;
}
//---------------------------------------------------------------------------
//формат разделителя
void __fastcall TForm1::NformatClick(TObject *Sender)
{
	ShowMessage("Разделителем дробной части может быть точка(.) или запятая(,). При открытии файла разделителем может быть только точка(.)!");
}
//---------------------------------------------------------------------------
//Очистка результатов
void __fastcall TForm1::NPointClearClick(TObject *Sender)
{
	Out->Clear();
}
//---------------------------------------------------------------------------
//График - 3D
void __fastcall TForm1::N3DClick(TObject *Sender)
{
	N3D->Checked=!N3D->Checked;
	Chart1->View3D = N3D->Checked;
}
//---------------------------------------------------------------------------
//График - Цвет
void __fastcall TForm1::NGColorClick(TObject *Sender)
{
   if (ColorDialog1->Execute()==ID_OK)
	{
		// Series1->ValueColor=ColorDialog1->Color;
		Series1->AreaColor = ColorDialog1->Color;
	}
}
//---------------------------------------------------------------------------
void TForm1::AddRedDot(double X)
{
   if (res.shiftX == 0)
   {
	   double y = res.coeffs[0];
      double xPower = 1.0;
      for (int j = 1; j <= Degree->Position; j++)
      {
          xPower *= X; // На каждой итерации просто домножаем на X
          y += res.coeffs[j] * xPower;
      }
      Series2->AddXY(X, y, "", clRed);
   }
   else
   {
      X -= res.shiftX;
      double y = res.coeffs[0];
      double xPower = 1.0;
      for (int j = 1; j <= Degree->Position; j++)
      {
          xPower *= X; // На каждой итерации просто домножаем на X
          y += res.coeffs[j] * xPower;
      }
      Series2->AddXY(X+res.shiftX, y, "", clRed);
   }
}
//---------------------------------------------------------------------------
//ПОСТРОИТЬ
void __fastcall TForm1::RunGraphClick(TObject *Sender)
{
	Series2->BeginUpdate();
	Series2->Clear();
   //Series1->Pointer->Visible = true;
   //Series1->Pointer->Style = psRectangle; // Квадратики
   //Series1->SeriesColor = clRed;
   if (res.Method == LMSOutResult::Chebyshev)
   {
      if (OnlyX->Checked)
      {
         for (unsigned int i = 0; i < x.size(); i++)
		      if (x[i] >= AppXn->Position && x[i] <= AppXk->Position)
            {
               double curY = TChebyshevLMS::Predict(ch, x[i]);
               Series2->AddXY(x[i], curY);
            }
      }
      else
      {
         // 3. Рисуем аппроксимирующую линию // Рисуем плавную кривую (берем 50 шагов между min и max)
         // Важно: берем чуть больше точек, чтобы линия была плавной
         double step = (res.maxX - res.minX) / 50.0;
         for(double curX = res.minX; curX <= res.maxX; curX += step)
         {
             double curY = TChebyshevLMS::Predict(ch, curX);
             Series2->AddXY(curX, curY);
         }
      }
      return;
   }
	if (!OnlyX->Checked)
		for (double xc=AppXn->Position; xc<=AppXk->Position; xc+=AppStep->Position)
			AddRedDot(xc);
	for (unsigned int i = 0; i < x.size(); i++)
		if (x[i] >= AppXn->Position && x[i] <= AppXk->Position)
			AddRedDot(x[i]);
	Series2->EndUpdate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DegreeChanging(TObject *Sender, bool &AllowChange)
{
	RunGraph->Enabled = false;
}
//---------------------------------------------------------------------------
void TForm1::BringColor(int NewDegree)
{
   if (NewDegree >= (int)point-1)
		DegreeE->Font->Color = clMaroon;
	else
		DegreeE->Font->Color = clWindowText;
   static const String DegreeNames[] =
   {
      L"Константа",
      L"Прямая",
      L"Парабола",
      L"Кубическая функция",
      L"Квартическая функция",
      L"Квинтическая функция" // Для степени 5
   };
   if (NewDegree < 0)
      NewDegree = 0;
   if (NewDegree <= 5)
      Label1->Caption = DegreeNames[NewDegree];
   else
      Label1->Caption ="++";
}
//---------------------------------------------------------------------------
void __fastcall TForm1::DegreeChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction)
{
   BringColor(NewValue);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OnlyXClick(TObject *Sender)
{
	AppStepE->Enabled = !OnlyX->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::GridRowMoved(TObject *Sender, int FromIndex, int ToIndex)
{
	for (int i = 1; i < Grid->RowCount; i++)
		Grid->Cells[0][i] = i;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NXSClick(TObject *Sender)
{
   if (NXS->Checked)
      Notebook1->PageIndex = 0;
   else
      Notebook1->PageIndex = 1;
}
//---------------------------------------------------------------------------

