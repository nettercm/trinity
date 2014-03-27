#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>



namespace robot_ui 
{
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::DrawGrid(Graphics ^g)
	{
		int x,y,width,height, x_count=0,y_count=0,count=0;
		// Create pen.
		Brush^ b =  gcnew SolidBrush(Color::LightBlue);
		Pen^ p1 = gcnew Pen( b );
		Pen^ p2 = gcnew Pen(Color::Gray, 0.5);

		y=g->VisibleClipBounds.X;
		for(x=g->VisibleClipBounds.X; x<g->VisibleClipBounds.X + g->VisibleClipBounds.Width; x+=10)
		{
			g->DrawLine(p2,x,y,x,(int)(y+g->VisibleClipBounds.Height));
			x_count++;
			//printf(".");
		}

		x=g->VisibleClipBounds.X;
		for(y=g->VisibleClipBounds.Y; y<g->VisibleClipBounds.Y + g->VisibleClipBounds.Height; y+=10)
		{
			g->DrawLine(p2,x,y,(int)(x+g->VisibleClipBounds.Width),y);
			y_count++;
			//printf("-");
		}

		for(x=g->VisibleClipBounds.X; x<g->VisibleClipBounds.X + g->VisibleClipBounds.Width; x+=10)
		{
			for(y=g->VisibleClipBounds.Y; y<g->VisibleClipBounds.Y + g->VisibleClipBounds.Height; y+=10)
			{
				//g->DrawRectangle(p2,x,y,10,10);
				g->FillRectangle(b,x+2,y+2,7,7);
				count++;
			}
		}
		printf("grid x,y,total,w,h=%d,%d,%d,%f,%f\n",x_count,y_count,count,g->VisibleClipBounds.Width,g->VisibleClipBounds.Height);
	}



		System::Void f1::radar_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
		{
			static int x=0;
			static int y=0;

			if(history_index - x > 2000) x=history_index-2000;

			while(x<history_index)
			{
				UpdateRadar( inputs_history[x].theta, inputs_history[x].ir[1]);
				x++;
			}
		}




		System::Void f1::panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		 {
			DrawGrid(e->Graphics);
		 }

}