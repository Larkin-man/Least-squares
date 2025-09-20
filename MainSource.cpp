//------------файл реализации---------------------------------------------------------------

#include <vcl.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <float.h>
#include <Rational.cpp>
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

	a.resize(S+1);
	b.resize(S+1);
	x.resize(N);
	y.resize(N);
	sums.resize(S+1);
	for(int p=0; p<N; p++)
	{
		x[p] = StrToFloat(Grid->Cells[1][p+1]);
		y[p] = StrToFloat(Grid->Cells[2][p+1]);
	}

	int powcount = S+S+1;
	pows.resize(powcount);
	for (int i = 0; i < powcount; i++)
	{
		pows[i].resize(N);
		for(int p=0; p<N; p++)
			pows[i][p] = pow(x[p], i);
	}

	for(int i=0; i<S+1; i++)
	{
		sums[i].resize(S+1);
		a[i]=0;
		b[i]=0;
		for(int j=0; j<S+1; j++)
		{
			int ij = i+j;
			sums[i][j] = 0;
			for(int p=0; p<N; p++)
				sums[i][j] += pows[ij][p];//pow(x[p], ij);
		}
		for(int p=0; p<N; p++)
			b[i] += y[p] * pows[i][p];//pow(x[p], i);
	}
	//check if there are 0 on main diagonal and exchange rows in that case
	for(int i=0; i<S+1; i++)
		if(sums[i][i] == 0)
			for(int j=0; j<S+1; j++)
				if((j != i) && (sums[j][i] != 0) && (sums[i][j] != 0))
				{
					for(int k=0; k<S+1; k++)
						Exchange(sums[j][k], sums[j][k]);
					Exchange(b[j], b[i]);
					break;
				}
	for(int k=0; k<S+1; k++)
		for(int i=k+1; i<S+1; i++)
		{
			if(sums[k][k] == 0)
			{
				Out->Lines->Add("Solution is not exist.");
				return;
			}
			double M = sums[i][k] / sums[k][k];
			//Out->Lines->Append(IntToStr((int)sums[i][k])+"\t"+IntToStr((int)sums[k][k]));
			for(int j=k; j<S+1; j++)
				sums[i][j] -= M * sums[k][j];
			b[i] -= M * b[k];
		}
	for(int i=(S+1)-1; i>=0; i--)
	{
		double s = 0;
		for(int j = i; j<S+1; j++)
			s = s + sums[i][j] * a[j];
		a[i] = (b[i] - s) / sums[i][i];
	}
	if (Out->Lines->Count > 0)
		Out->Lines->Append("===== "+IntToStr(S)+ " =====");
	a[i].
	for(int i=0; i<S+1; i++)
		Out->Lines->Add("C["+IntToStr(i)+"]="+FloatToStr(a[i])+"\t"+a[i]);
	double s2 = 0;
	double mino = DBL_MAX;
	double maxo = DBL_MIN;
	for(int j=0; j<N; j++)
	{
		double cu = 0;
		for(int i=0; i<S+1; i++)
			cu += a[i] * pows[i][j];//pow(x[j], i);
		s2 += (y[j] - cu) * (y[j] - cu);
		if (NPrints2->Checked)
		{
			Out->Lines->Append(FloatToStr(cu)+" "+FloatToStr(y[j]-cu));
			if ((y[j] - cu) < mino)
				mino = y[j] - cu;
			if ((y[j] - cu) > maxo)
				maxo = y[j] - cu;
		}
	}
	if (NPrints2->Checked)
	{
		Out->Lines->Add("Минимальное отклонение");
		Out->Lines->Add(FloatToStr(mino));
		Out->Lines->Add("Максимальное отклонение");
		Out->Lines->Add(FloatToStr(maxo));
	}
	Out->Lines->Add("Сумма квадратов отклонений");
	Out->Lines->Add(FloatToStr(s2));
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
	Degree->Position = point-1;
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
	double y = 0;
	for (int j=0; j<Degree->Position+1; j++)
		y += a[j] * pow(X, j);
	Series2->AddXY(X, y,"",clRed);
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
void __fastcall TForm1::DegreeChangingEx(TObject *Sender, bool &AllowChange, short NewValue,
			 TUpDownDirection Direction)
{
	if ((unsigned)NewValue >= point)
		DegreeE->Font->Color = clMaroon;
	else
		DegreeE->Font->Color = clWindowText;
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

void __fastcall TForm1::N2Click(TObject *Sender)
{
	//

}
//---------------------------------------------------------------------------

