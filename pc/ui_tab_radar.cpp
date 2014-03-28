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


	void f1::UpdateRadar(float a, int b)
	{
			Brush^ b1 =  gcnew SolidBrush(Color::White);
			Brush^ b2 =  gcnew SolidBrush(Color::Black);
			Brush^ b3 =  gcnew SolidBrush(Color::Red);
			Pen^ p = gcnew Pen(Color::Gray, 1.0);
			static int i=0;
			float theta;
			float x,y;
			//DrawGrid(e->Graphics);
			if(history_index - i > 2000) i=history_index-2000;

			//for(i=history_index-360; i<=history_index;i++)
			while(i<history_index)
			{
				if(i>=0)
				{
					theta = inputs_history[i].theta;
					//printf("UpdateRadar(): theta = %04.1f\n",theta);
					y=310.0f + 300.0f * sinf( theta * 3.14f / 180.0f);
					x=310.0f + 300.0f * cosf( theta * 3.14f / 180.0f);
					//g->FillEllipse(b2,(int) x, (int) y, 4, 4);
					g->DrawEllipse(p,(int) x, (int) y, 4, 4);
					//g->FillEllipse(b3,(int) x, (int) y, 6, 6);
				}
				i++;
			}
	}

		System::Void f1::radar_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
		{
			if(cb_radar_enable_updates->Checked) UpdateRadar(0,0);
		}




		System::Void f1::panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
		{
			Brush^ b3 =  gcnew SolidBrush(Color::Red);
			Brush^ b1 =  gcnew SolidBrush(Color::White);
			Brush^ b2 =  gcnew SolidBrush(Color::Black);
			Pen^ p = gcnew Pen(Color::Gray, 1.0);
			static int i=0;
			float theta;
			float x,y;

			log("Paint\n");

#if 0

			if(history_index - i > 2000) i=history_index-2000;

			for(i=history_index-360; i<=history_index;i++)
			{
				if(i>=0)
				{
					theta = inputs_history[i].theta;
					printf("panel1_Paint(): theta = %04.1f\n",theta);
					y=310.0f + 300.0f * sinf( theta * 3.14f / 180.0f);
					x=310.0f + 300.0f * cosf( theta * 3.14f / 180.0f);
					//g->FillEllipse(b2,(int) x, (int) y, 4, 4);
					g->DrawEllipse(p,(int) x, (int) y, 4, 4);
				}
				//i++;
			}
			//g->FillEllipse(b3,(int) x, (int) y, 3, 3);
#endif
		}
}