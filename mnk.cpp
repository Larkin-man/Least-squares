//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "mnk.h"
#include "math.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
   : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BitBtn1Click(TObject *Sender)
{
   int i=0,j=0, k=0;
   int N, K;
   double *x, *y, **sums;
   StringGrid1->Cells[1][1]=3;
   StringGrid1->Cells[2][1]=4;
   StringGrid1->Cells[1][2]=4;
   StringGrid1->Cells[2][2]=6;
   StringGrid1->Cells[1][3]=6;
   StringGrid1->Cells[2][3]=7;
   K=2;
   N=1;

   x = new double[N];
   y = new double[N];
   sums = new double *[K+1];
   for(i=0; i<K+1; i++)
      sums[i] = new double[K+1];

   for(i=0; i<K+1; i++)
      for(j=0; j<K+1; j++)
         sums[i][j] = 0;

   i=0,j=0, k=0;
   for(k=0; k<N; k++)
   {
      x[k] = StrToFloatDef(StringGrid1->Cells[1][k+1],0);
      y[k] = StrToFloatDef(StringGrid1->Cells[2][k+1],0);  //*/
   }
   for(i=0; i<K+1; i++)
   {
      for(j=0; j<K+1; j++)
      {
         double ij = i+j;
	      sums[i][j] = 0;   //Вылет
	      for(k=0; k<N; k++)
	         sums[i][j] += pow(x[k], ij);
      }
   }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::BitBtn2Click(TObject *Sender)
{
   StringGrid1->Cells[1][1]=3;
   StringGrid1->Cells[2][1]=4;
   StringGrid1->Cells[1][2]=4;
   StringGrid1->Cells[2][2]=6;
   StringGrid1->Cells[1][3]=6;
   StringGrid1->Cells[2][3]=7;
}
//---------------------------------------------------------------------------
