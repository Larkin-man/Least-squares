//------------файл реализации---------------------------------------------------------------

#include <vcl.h>
#include "About.h"
#include "Table.h"
#include <math.h>
#include <stdio.h>
#include <vector>
#pragma hdrstop
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
	OpenDialog1->InitialDir = GetCurrentDir();
	SaveDialog1->InitialDir = GetCurrentDir();
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
	K = Degree->Position;
	if (N < K)
	{
		Out->Lines->Add("Недостаточно входных данных для расчета.");
		return;
	}
	Form2->StringGrid1->ColCount = K+2;
	Form2->StringGrid1->RowCount = K+1;

	a.resize(K+1);
   b.resize(K+1);
   x.resize(N);
   y.resize(N);
   sums.resize(K+1);

	for(int i=0; i<K+1; i++)
   {
      sums[i].resize(K+1);
		a[i]=0;
		b[i]=0;
		for(int j=0; j<K+1; j++)
			sums[i][j] = 0;
	}
	for(int j=0; j<N; j++)
	{
		x[j] = StrToFloat(Grid->Cells[1][j+1]);
		y[j] = StrToFloat(Grid->Cells[2][j+1]);
	}

	for(int i=0; i<K+1; i++)
	{
		for(int j=0; j<K+1; j++)
		{
			int ij = i+j;
			sums[i][j] = 0;
			for(int k=0; k<N; k++)
			{
				sums[i][j] += pow(x[k], ij);
				Form2->StringGrid1->Cells[i][j]=FloatToStr(sums[i][j]);
			}
		}
	}
	for(int i=0; i<K+1; i++)
		for(int k=0; k<N; k++)
		{
			b[i] += pow(x[k], i) * y[k];
			Form2->StringGrid1->Cells[K+1][i]=FloatToStr(b[i]);
		}
	//check if there are 0 on main diagonal and exchange rows in that case
	double temp = 0;
	for(int i=0; i<K+1; i++)
	{
		if(sums[i][i]==0)
		{
			for(int j=0; j<K+1; j++)
			{
				if(j==i)
					continue;
				if((sums[j][i] !=0) && (sums[i][j]!=0))
				{
					for(int k=0; k<K+1; k++)
					{
						temp = sums[j][k];
						sums[j][k] = sums[i][k];
						sums[i][k] = temp;
					}
					temp = b[j];
					b[j] = b[i];
					b[i] = temp;
					break;
				}
			}
		}
	}
	for(int k=0; k<K+1; k++)
	{
		for(int i=k+1; i<K+1; i++)
		{
			if(sums[k][k]==0)
			{
				Out->Lines->Add("Solution is not exist.");
				return;
			}
			double M = sums[i][k] / sums[k][k];
			for(int j=k; j<K+1; j++)
				sums[i][j] -= M * sums[k][j];
			b[i] -= M*b[k];
		}
	}
   if (Out->Lines->Count > 0)
      Out->Lines->Append("===== "+IntToStr(K)+ " =====");
	for(int i=(K+1)-1; i>=0; i--)
	{
		double s = 0;
		for(int j = i; j<K+1; j++)
			s = s + sums[i][j]*a[j];
		a[i] = (b[i] - s) / sums[i][i];
	}

	for(int i=0; i<K+1; i++)
		Out->Lines->Add("C["+IntToStr(i)+"] = "+FloatToStr(a[i]));
	double s2 = 0;
	for(int j=0; j<N; j++)
	{
		double cu = 0;
		for(int i=0; i<K+1; i++)
			cu += a[i] * pow(x[j], i);
		s2 += (y[j] - cu) * (y[j] - cu);
		if (NPrints2->Checked)
			Out->Lines->Append(FloatToStr(fabs(s2-cu)));
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
	{
		//Grid->Font=FontDialog1->Font;
		Form1->Font=FontDialog1->Font;
	}
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
//О программе
void __fastcall TForm1::NAboutClick(TObject *Sender)
{
	Application->CreateForm( __classid(TAboutBox1),&AboutBox1);
	AboutBox1->ShowModal();
	AboutBox1->Free();
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
//ПОСТРОИТЬ
void __fastcall TForm1::RunGraphClick(TObject *Sender)
{
	Series2->Clear();
	double y;
	for (double xc=AppXn->Position; xc<=AppXk->Position; xc+=AppStep->Position)
	{
		y = 0;
		for (int j=0; j<Degree->Position+1; j++)
			y += a[j] * pow(xc, j);
		Series2->AddXY(xc, y,"",clRed);
	}
	for (unsigned int i = 0; i < x.size(); i++)
	{
		if (x[i] >= AppXn->Position && x[i] <= AppXk->Position)
		{
			y = 0;
			for (int j=0; j<Degree->Position+1; j++)
				y += a[j] * pow(x[i], j);
			Series2->AddXY(x[i], y,"",clRed);
		}
	}
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

