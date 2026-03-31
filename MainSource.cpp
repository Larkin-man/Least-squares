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
// Основная функция МНК
// degree - степень полинома (1 - прямая, 2 - парабола и т.д.)
//МНК подготавливает систему уравнений (матрицу сумм)
std::vector<double> TForm1::PolyFit(const std::vector<double>& x, const std::vector<double>& y, int degree)
{
	int n = (int)x.size();
	int m = degree + 1; // Размерность матрицы (m x m)
	int num_sums = 2 * degree + 1; // Столько уникальных сумм x^k нам нужно

	if (degree >= (int)x.size())
		return std::vector<double>();
	logv(x,"x"); logv(y, "y");
	// 1. Вычисляем все необходимые суммы степеней x: sum_x[k] = \sum x_i^k
	// С оптимизацией std::pow(x[k], i + j);
	std::vector<double> sum_x(num_sums, 0.0);
	sum_x[0] = (double)n; // x^0 это 1, сумма единиц это n

	for (int i = 0; i < n; i++) {
		double x_pow = 1.0;
		for (int k = 1; k < num_sums; k++) {
			// Проверка перед умножением, чтобы не вылететь за пределы double
			if (std::abs(x_pow) > DBL_MAX / std::abs(x[i])) {
				// Ошибка: слишком большие значения для такой степени
				return std::vector<double>();
			}
			x_pow *= x[i];

			// Проверка суммы
			if (DBL_MAX - std::abs(x_pow) < std::abs(sum_x[k])) {
				return std::vector<double>(); // Переполнение суммы
			}
			sum_x[k] += x_pow;
		}
	}
	logv(sum_x,"sum_x");

	// 2. Вычисляем суммы произведений: sum_xy[k] = \sum (y_i * x_i^k)
	std::vector<double> sum_xy(m, 0.0);
	for (int i = 0; i < n; i++) {
		double x_pow = 1.0;
		for (int k = 0; k < m; k++) {
			sum_xy[k] += y[i] * x_pow;
			x_pow *= x[i];
		}
	}
	logv(sum_xy,"sum_xy");

	// 3. Заполняем матрицу sum, просто копируя уже вычисленные значения
	std::vector< std::vector<double> > sum(m, std::vector<double>(m + 1, 0.0));
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < m; j++) {
			sum[i][j] = sum_x[i + j]; // Берем готовую сумму из массива
		}
		sum[i][m] = sum_xy[i]; // Свободный член
	}

	// Далее вызываем SolveGauss(sum, coeffs)...
	std::vector<double> coeffs(m, 0.0);
	if (ti.SolveGauss(sum, coeffs)) return coeffs;
	return std::vector<double>();
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
	//std::vector<double> x, y;
	//std::vector<T> a, b;
	//std::vector<std::vector<T> > sums;
	//std::vector<std::vector<double> > pows;
	//a.resize(S+1);
	//b.resize(S+1);
	x.resize(N);
	y.resize(N);

	for(int p=0; p<N; p++)
	{
		x[p] = StrToFloat(Grid->Cells[1][p+1]);
		y[p] = StrToFloat(Grid->Cells[2][p+1]);
	}

	//int powcount = S+S+1;
	//pows.resize(powcount);

	std::vector<double> result(S, 0.0);
	//result = PolyFit(x, y, S);

	//lsr = TLeastSquares::Calculate(x, y, S);
	res = ti.Calculate(x, y, S);

	//a.clear();
	//a.resize(S + 1, 0);

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
	tolog("shiftX=\n"+FloatToStr(res.shiftX));
	tolog("rmse=\n"+FloatToStr(res.rmse));
	tolog("r2=\n"+FloatToStr(res.r2));

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
	if (Grid->Row < 1)
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
void __fastcall TForm1::DegreeChangingEx(TObject *Sender, bool &AllowChange, int NewValue,
          TUpDownDirection Direction)
{
	if (NewValue >= (int)point-1)
		DegreeE->Font->Color = clMaroon;
	else
		DegreeE->Font->Color = clWindowText;
   static const String DegreeNames[] = {
      L"Константа",
      L"Прямая",
      L"Парабола",
      L"Кубическая функция",
      L"Квартическая функция",
      L"Квинтическая функция" // Для степени 5
   };
   if (NewValue < 0)
      NewValue = 0;
   if (NewValue <= 5)
      Label1->Caption = DegreeNames[NewValue];
   else
      Label1->Caption ="++";
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OnlyXClick(TObject *Sender)
{
	AppStepE->Enabled = !OnlyX->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::GridRowMoved(TObject *Sender, int FromIndex, int ToIndex)
{
	//String t(Grid->Cells[1][FromIndex]);
	//Grid->Cells[1][FromIndex] = Grid->Cells[1][ToIndex];
	//Grid->Cells[1][ToIndex] = t;
	for (int i = 1; i < Grid->RowCount; i++)
		Grid->Cells[0][i] = i;
}
//---------------------------------------------------------------------------


