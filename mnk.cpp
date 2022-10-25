#include <iostream>
#include <string.h>
#include <stdio.h>
#include <math.h>
using namespace std;
void sort (float  **a, int n,int l)
{
	float max=0,d;
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
void Gaysa (float **M,int n)
{
	int i,j,l;
	float *x,d;
	x=new  float[n];	
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

	cout<<"\nИскомые Х\n";
	for(i=n-1;i>=0;i--)
	{
		cout<<"x"<<i+1<<"="<<x[i];
		cout<<"\n";
	}

	
	system("pause");
	



}
float step (float a, int n)
{
	float b=1;
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
float symma (float *T, int i, int j,int m)
{
	float s=0;
	int i1;
	for(i1=0;i1<m;i1++)
		s=s+step(T[i1],i+j);
	return s;
		
}
void main()
{
	setlocale(LC_CTYPE,"Russian");
	FILE *in;
	int i,j,n,m;
	float *X,*Y,**M, Sy=0;
	in=fopen("input.txt","r");
	cout<<"Степень функции\n";
	cin>>n;
	n=n+1;
	cout<<"Количество точек\n ";
	cin>>m;
	X=new float [m];
	Y=new float [m];
	M=new float *[n];
	for(i=0;i<n;i++)
		M[i]=new float [n+1];
	cout<<endl;
	//cout<<"vvedite X Y \n";
	for(i=0;i<m;i++)
	{
		//cin>>X[i];
		//cin>>Y[i];
		fscanf(in,"%f ",&X[i]);
		fscanf(in,"%f ",&Y[i]);
		cout<<X[i]<<"  "<<Y[i]<<endl;
	}


	for(i=0;i<m;i++)
	{
		Sy=Sy+Y[i];
	}


	cout<<endl;
	

	for(i=0;i<n;i++)
	{
		for(j=0;j<n;j++)
		{
			M[i][j]=symma(X,i,j,m);
		}

	}

	
	M[0][n]=Sy;
	for(i=1;i<n;i++)
	{
		M[i][n]=0;
		for(j=0;j<m;j++)
			M[i][n]=M[i][n]+Y[j]*step(X[j],i);
	}

cout<<"\n";
cout<<"Система из коэффициентов\n";
	for(i=0;i<n;i++)
	{
		for(j=0;j<n+1;j++)
			cout<<M[i][j]<<" ";
		cout<<endl;
	}


Gaysa(M,n);
	system("pause");






}