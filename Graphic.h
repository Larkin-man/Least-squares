#ifndef GraphicH
#define GraphicH

class Graphic
{
   public:
      int minX, maxX, minY, maxY;
      Graphic(TCanvas *Canvas)
      {
         Out = Canvas;
         Rec = (Out->ClipRect);
         Out->Pen->Width=1;
         Out->Pen->Color=clBlack;
         AxisX = Rec.Height()/2;
         AxisY = Rec.Width()/2;
      }
      TCanvas *Out;

      void Axis()
      {           
         Out->MoveTo(0, AxisX);
         Out->LineTo(Rec.Width(), AxisX);
         Arrow(false, Rec.Width(), AxisX);
         Out->TextOutA(Rec.Width()-15, AxisX, "x");
         Out->MoveTo(AxisY, 0);
         Out->LineTo(AxisY, Rec.Height());
         Arrow(true, AxisY, 0);         
         Out->TextOutA(AxisY-15, 0, "y");
      }

      void GridLines(unsigned int interval)
      {
         Out->Pen->Color=clSilver;
         Out->Pen->Style=psDot;
         //horizontal
         for (int i=interval; i<AxisY; i+=interval)
         {
            Out->MoveTo(0, AxisY + i);
            Out->LineTo(Rec.Width(), AxisY + i);
            Out->MoveTo(0, AxisY - i);
            Out->LineTo(Rec.Width(), AxisY - i);
         }

         for (int i=interval; i<AxisX; i+=interval)
         {
            Out->MoveTo(AxisX + i, 0);
            Out->LineTo(AxisX + i, Rec.Height());
            Out->MoveTo(AxisX - i, 0);
            Out->LineTo(AxisX - i, Rec.Height());
         }
         Out->Pen->Color=clBlack;
         Out->Pen->Style=psSolid;

      }

      void test()
      {
         Out->TextOutA(0, 0, Out->ClipRect.Width());
         Out->TextOutA(0, 20, Out->ClipRect.Height());
         Out->TextOutA(0, 40, Out->ClipRect.Right);
         Out->TextOutA(0, 60, Out->ClipRect.right);
      }
      void Arrow(bool up, int x, int y)
      {
         int size = 10;
         Out->MoveTo(x, y);
         if (up)
         {
            Out->LineTo(x-size/2, y+size);
            Out->MoveTo(x, y);
            Out->LineTo(x+size/2, y+size);
         } else
         {
            Out->LineTo(x-size, y-size/2);
            Out->MoveTo(x, y);
            Out->LineTo(x-size, y+size/2);
         }
      }
   protected:
      int gsd;
      TRect Rec;
      int AxisX, AxisY;


};

#endif
