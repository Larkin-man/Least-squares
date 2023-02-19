//------------файл реализации---------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "MainSource.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner): TForm(Owner)
{
     Grid->Cells[0][0]="№";
     Grid->Cells[1][0]="X";
     Grid->Cells[2][0]="Y";
     Grid->ColWidths[0]=24;
     point =1;
     NNewClick(NULL);
     minX =  32000;
     maxX = -32000;
     OpenDialog1->InitialDir = GetCurrentDir();
     SaveDialog1->InitialDir = GetCurrentDir();
     a = NULL; 
}
//---------------------------------------------------------------------------
//сюда могут помещатся объявления типов и переменных
//Тут должны быть реализации всех функций,
//объявленных в заголовочном файле
//и других функций не объявленных ранее
void __fastcall TForm1::NNewClick(TObject *Sender)
{
     Out->Clear();
     //Memo1->Clear();
     fool = false;
     Sy=0;

     for (unsigned int i=1;i<point+1;i++)
     {
         Grid->Cells[0][i]=""; Grid->Cells[1][i]="";  Grid->Cells[2][i]="";
     }
     point =1;
     Label7->Caption=1;
     Series1->Clear();
     Series2->Clear();
     BuiltBtn->Enabled = false;
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
   AppXn->FloatValue = minX-1;
   AppXk->FloatValue = maxX+1;
   //Сам расчет
   int i=0,j=0, k=0;
   N = point-1;
   K = Degree->Value;
   if (N < K)
   {
      Out->Items->Add("Недостаточно входных данных для расчета");
      return;
   }
   Form2->StringGrid1->ColCount = K+2;
   Form2->StringGrid1->RowCount = K+1;
   delete a;
   a = new double[K+1];
   b = new double[K+1];
   x = new double[N];
   y = new double[N];
   sums = new double *[K+1];
   if(x==NULL || y==NULL || a==NULL || sums==NULL)
   {
       Out->Items->Add("Not enough memory to allocate.");
       return;
   }
   for(i=0; i<K+1; i++)
   {
      sums[i] = new double[K+1];
      if(sums[i]==NULL)
      {
         Out->Items->Add("Not enough memory to allocate equations");
         return;
      }
   }
   for(i=0; i<K+1; i++)
   {
      a[i]=0;
      b[i]=0;
      for(j=0; j<K+1; j++)
         sums[i][j] = 0;
   }
   for(k=0; k<N; k++)
   {
      x[k]=0;
      y[k]=0;
   }
   i=0,j=0, k=0;
   for(k=0; k<N; k++)
   {
      x[k] = StrToFloat(Grid->Cells[1][k+1]);
      y[k] = StrToFloat(Grid->Cells[2][k+1]);
      //x[i]=atof(ListBox1->Items->Strings[i].c_str());
      //y[i]=atof(ListBox2->Items->Strings[i].c_str());
   }
   for(i=0; i<K+1; i++)
   {
      for(j=0; j<K+1; j++)
      {
         double ij = i+j;
	      sums[i][j] = 0;
	      for(k=0; k<N; k++)
         {
	         sums[i][j] += pow(x[k], ij);
            Form2->StringGrid1->Cells[i][j]=FloatToStr(sums[i][j]);
         }
      }
   }
   for(i=0; i<K+1; i++)
      for(k=0; k<N; k++)
      {
         b[i] += pow(x[k], i) * y[k];
         Form2->StringGrid1->Cells[K+1][i]=FloatToStr(b[i]);
      }
   //check if there are 0 on main diagonal and exchange rows in that case
   double temp=0;
   for(i=0; i<K+1; i++)
   {
      if(sums[i][i]==0)
      {
	      for(j=0; j<K+1; j++)
         {
	         if(j==i)
               continue;
	         if((sums[j][i] !=0) && (sums[i][j]!=0))
            {
		         for(k=0; k<K+1; k++)
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

   for(k=0; k<K+1; k++)
   {
      for(i=k+1; i<K+1; i++)
      {
	      if(sums[k][k]==0)
         {
            Out->Items->Add("Solution is not exist.");
	         //printf("\nSolution is not exist.\n");
	         return;
	      }
	      double M = sums[i][k] / sums[k][k];
	      for(j=k; j<K+1; j++)
	         sums[i][j] -= M * sums[k][j];
	      b[i] -= M*b[k];
      }
   }
   for(i=(K+1)-1; i>=0; i--)
   {
      double s = 0;
         for(j = i; j<K+1; j++)
	         s = s + sums[i][j]*a[j];
         a[i] = (b[i] - s) / sums[i][i];
   }

   for(i=0; i<K+1; i++)
      Out->Items->Add("C["+IntToStr(i)+"] = "+FloatToStr(a[i]));

   for(i=0; i<K+1; i++){
       delete [] sums[i];
   }
   //delete [] a;
   delete [] b;
   delete [] x;
   delete [] y;
   delete [] sums;
   BuiltBtn->Enabled = true;
}
//---------------------------------------------------------------------------
//Функция загружает список из текстового файла
void __fastcall TForm1::NOpenClick(TObject *Sender)
{
   if(OpenDialog1->Execute()==IDOK)
   {
      FILE *file = fopen (OpenDialog1->FileName.c_str(), "r");
	   if (!file)
	   {
		   ShowMessage ( "Cannot open text file");
		   return;
	   }
      int n;
      int fx,fy;
	   fscanf(file, "%d", &n);
      Degree->Value = n;
      fscanf(file, "%d", &point);
      point++;
      Label7->Caption = point;
      if (Grid->RowCount < (int)point)
         Grid->RowCount = point+1;
	   for (unsigned int i = 1; i < point; i++)
	   {
         fscanf(file, "%d", &fx);
         fscanf(file, "%d", &fy);
         Grid->Cells[0][i] = i;
         Grid->Cells[1][i] = fx;
         Grid->Cells[2][i] = fy;
         if (fx < minX)
            minX = fx;
         if (fx > maxX)
            maxX = fx;
         Series1->AddXY(fx, fy,"",clBlue);

	   }
	   fclose(file);
      //StatusBar1->Panels->Items[0]->Text=OpenDialog1->FileName;
   }
   BuiltBtn->Enabled = true;
}
//---------------------------------------------------------------------------
//Функция сохраняет список в текстовый файл
void __fastcall TForm1::NSaveClick(TObject *Sender)
{
   if(SaveDialog1->Execute()==IDOK)
   {
      FILE *file = fopen (SaveDialog1->FileName.c_str(), "w");
	   if (!file)
	   {
	      ShowMessage ( "Cannot create text file");
		   return;
	   }
	   fprintf(file, "%d ", Degree->Value);
      fprintf(file, "%d ", point-1);
	   for (unsigned int i = 1; i < point; i++)
	   {
         fprintf(file, "%d ", StrToInt(Grid->Cells[1][i]));
         fprintf(file, "%d ", StrToInt(Grid->Cells[2][i]));
	   }
	   fclose(file);
      ShowMessage ( "Данные сохранены!");
   }
  //Out->Items->SaveToFile(SaveDialog1->FileName);
  //Memo1->Lines->SaveToFile(SaveDialog1->FileName);
  //StatusBar1->Panels->Items[0]->Text=SaveDialog1->FileName;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::NFontClick(TObject *Sender)
{
     if (FontDialog1->Execute()==ID_OK)
     {
         Grid->Font=FontDialog1->Font;
          Out->Font=FontDialog1->Font;
     }
          //Memo1->Font=FontDialog1->Font;
          //((TComponent*)Sender)->
          //Sender->ClassName()
          //((TEdit *)Sender)->Font=FontDialog1->Font;
}
//---------------------------------------------------------------------------
//Копировать
void __fastcall TForm1::NCopyClick(TObject *Sender)
{
     //Memo1->CopyToClipboard();
     //Memo1->SelText
     ShowMessage("Хочешь копировать жми CTRL+C!");
}
//---------------------------------------------------------------------------
//Удалить
void __fastcall TForm1::NDeleteClick(TObject *Sender)
{
     /*if ((ListBox1->ItemIndex == -1)&&(ListBox2->ItemIndex == -1))
          ShowMessage("Чтобы удалить элементы их сперва надо выделить.");
     if (!(ListBox1->ItemIndex == -1))
          ListBox1->Items->Delete(ListBox1->ItemIndex);
     if (!(ListBox2->ItemIndex == -1))
          ListBox2->Items->Delete(ListBox2->ItemIndex);        */
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
//ДОБАВИТЬ
void __fastcall TForm1::AddClick(TObject *Sender)
{
      double X = NewX->FloatValue;
      double Y = NewY->FloatValue;
     Grid->Cells[0][point]=point;
     Grid->Cells[1][point]=X;
     Grid->Cells[2][point]=Y;
      if (X < minX)
         minX = X;
      if (X > maxX)
         maxX = X;
     point++;
     Label7->Caption=point;
     if (Grid->RowCount < (int)point)
      Grid->RowCount++;
      //Series1->Add(NewX->FloatValue,"Цех1",clNavy);
      Series1->AddXY(X, Y,"",clNavy);
     //StatusBar1->SimpleText=("");
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
void __fastcall TForm1::BuiltBtnClick(TObject *Sender)
{
 Series2->Clear();
   if (!a)
      return;
   double y;
   for (double x=AppXn->FloatValue; x<AppXk->FloatValue+AppStep->FloatValue; x+=AppStep->FloatValue)
   {
      y = 0;
      for (int j=0; j<Degree->Value+1; j++)
         if ((x == 0) && (j == 0))
             y += a[j];
         else
            y += a[j]*pow(x,j);

      Series2->AddXY(x, y,"",clRed);
   }
}
//---------------------------------------------------------------------------



