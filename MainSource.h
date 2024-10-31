//---------------------------------------------------------------------------

#ifndef MainSourceH
#define MainSourceH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <stdio.h>
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
//#include <VclTee.TeeGDIPlus.hpp>
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
     TPanel *Panel1;
     TLabel *Label4;
   TMenuItem *NNew;
   TListBox *Out;
     TLabel *Label7;
     TOpenDialog *OpenDialog1;
     TSaveDialog *SaveDialog1;
     TFontDialog *FontDialog1;
   TMenuItem *N3D;
   TMenuItem *NCopy;
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
   TPanel *PanelGraph;
   TChart *Chart1;
   TLineSeries *Series1;
   TMenuItem *NGColor;
   TGroupBox *GroupBox1;
   TLineSeries *Series2;
   TBitBtn *BuiltBtn;
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
     void __fastcall NExitClick(TObject *Sender);
     void __fastcall RunClick(TObject *Sender);
     void __fastcall NNewClick(TObject *Sender);
     void __fastcall NOpenClick(TObject *Sender);
     void __fastcall NSaveClick(TObject *Sender);
     void __fastcall NFontClick(TObject *Sender);
     void __fastcall NCopyClick(TObject *Sender);
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
   void __fastcall BuiltBtnClick(TObject *Sender);
private:	// User declarations
//Только в пределах данного модуля
     void __fastcall F1(char c);
     int ij;

public:		// User declarations
   int minX,maxX;

double *a, *b, *x, *y, **sums;
int N, K;
     int i,j,n,m;
     unsigned int point;
	double *X,*Y,**M, Sy;
     double minmax[4],Kx,Ky;
     bool fool;
     char *txt;
          char g[7];
          float fl;
     struct Node //двусвязный список
     {
     double A;
     double B;
     Node *pNext;
     Node *pPrev;
     } *Record;

     __fastcall TForm1(TComponent* Owner);  //Объявление конструктора
     int MNK();

void sort (double  **a, int n,int l)
{
	double max=0,d;
	int maxi=0,i;

	for(i=l;i<n;i++)
	{
		if(fabs(a[i][l])>max)
		{

			max=fabs(a[i][l]);
			maxi=i;
		}
	}

	for(i=0;i<n+1;i++)
	{
		d=a[l][i];
		a[l][i]=a[maxi][i];
		a[maxi][i]=d;
	}
		
}
void Gaysa (double **M,int n)
{
	int i,j,l;
	double *x,d;
	x=new  double[n];

	for(l=0;l<n;l++)
	{
		sort(M,n,l);
		for(i=l;i<n-1;i++)
		{
			d=M[i+1][l]/M[l][l];
			for(j=l;j<n+1;j++)
				M[i+1][j]=M[i+1][j]-d*M[l][j];
		}
	}

	x[n-1]=M[n-1][n]/M[n-1][n-1];

	for(i=n-1-1,l=1;i>=0;i--,l++)
	{
		d=0;
		for(j=n-l;j<n;j++)
		{
			d=d+x[j]*M[i][j];
		}
		x[i]=(M[i][n]-d)/M[i][i];
	}

     char *exit;
     for(i=n-1;i>=0;i--)
	{

          Out->Items->Add(x[i]);
          //Memo1->Text=Memo1->Text+"   X["+i+"]=";
          //sprintf(exit,"x[%d]=%u",i+1,x[i]);
          //Memo1->Lines->Add(x[i]);
          //exit=(ListBox1->Items[0];
          //Memo1->Lines[i].SetText("FUSRO");
          //Memo1->Text=Memo1->Text+x[i];
     }

       }

     double step (double a, int n)
{
	double b=1;
	if(n!=0)
	{
		
		for (int i=1;i<=n;i++)
		{
			b=b*a;
		}
	}
	else 
		return 1;
	return b;
}
double symma (double *T, int i, int j,int m)
{
	float s=0;
	int i1;
	for(i1=0;i1<m;i1++)
		s=s+step(T[i1],i+j);
	return s;

}


Node *CreateItem (double A, double B)
{                //Функция создает новый элемент
   Node *pItem = new Node;
   pItem->A=A;
   pItem->B=B;
   pItem->pNext=NULL;
   pItem->pPrev=NULL;
   return pItem;
}

Node *InsertAfter (Node *parent, Node *Item)
{                 //Функция связывает два элемента
   if (parent != NULL)
   {
      //Item->pNext=parent->pNext;
      parent->pNext=Item;
      Item->pPrev=parent;
   }
   return Item;
}

Node *getTail (Node *head)
{             //Функция поиска последного элемента ы списке
   if(head == NULL)
      return NULL;
   Node *Item=head;
   while(Item->pNext != NULL)
      Item = Item->pNext;
   return Item;
}

Node *PushBack (Node *head, Node *Item)
{              //Функция вставляет элемент в конец списка
   Node *tail = getTail(head);
   return InsertAfter (tail, Item);
}


};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
//Сюда могут помещатся объявления типов, переменных, функций
//которые не включаются в класс формы

#endif
