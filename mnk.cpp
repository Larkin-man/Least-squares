//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
USEFORM("Table.cpp", Form2);
USEFORM("MainSource.cpp", Form1);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
     try
     {
           Application->Initialize();
           Application->Title = "(@)^(@)";
           Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TForm2), &Form2);
		Application->Run();
     }
     catch (Exception &exception)
     {
           Application->ShowException(&exception);
     }
     catch (...)
     {
           try
           {
                throw Exception("");
           }
           catch (Exception &exception)
           {
                Application->ShowException(&exception);
           }
     }
     return 0;
}
//---------------------------------------------------------------------------
