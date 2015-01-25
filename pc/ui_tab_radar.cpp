#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>

int grid[100][100];

#if 0
void plotLine(int x0, int y0, int x1, int y1)
{
	int dx,dy,D,x,y;
	dx=x1-x0;
	dy=y1-y0;
	D = 2*dy - dx;
	grid[x0][y0]=1;
	y=y0;

	for(x=x0+1;x<=x1;x++)
	{
		if( D > 0)
		{
			y = y+1;
			grid[x][y]=1;
			D = D + (2*dy-2*dx);
		}
		else
		{
			grid[x][y]=1;
			D = D + (2*dy);
		}
	}
}
#endif

void plotLine(int x0, int y0, int x1, int y1, int value)
{
	int dx =  abs(x1-x0), sx = x0<x1 ? 1 : -1;
	int dy = -abs(y1-y0), sy = y0<y1 ? 1 : -1;
	int err = dx+dy, e2;                                   /* error value e_xy */

	for (;;)
	{                                                          /* loop */
		grid[x0][y0]+=value;                              
		if(grid[x0][y0]<0) grid[x0][y0]=0;
		e2 = 2*err;                                   
		if (e2 >= dy) 
		{                                         /* e_xy+e_x > 0 */
			if (x0 == x1) break;                       
			err += dy; x0 += sx;                       
		}                                             
		if (e2 <= dx) 
		{                                         /* e_xy+e_y < 0 */
			if (y0 == y1) break;
			err += dx; y0 += sy;
		}
	}
}






namespace robot_ui 
{
	void f1::update_grid(int history_index, int ir_sensor_index,float x1, float y1, float t1)
	{
		float d,t2,x2,y2,x3,y3,x4,y4;
		static float lx3=0,ly3=0;
		int i = history_index;

		d=((float)inputs_history[i].ir[ir_sensor_index]/10.0)*25.4;
		t2= inputs_history[i].theta; 
		x2=x1+d*cos(t1);
		y2=y1+d*sin(t1);
		x3=x2*cos(t2)-y2*sin(t2);
		y3=x2*sin(t2)+y2*cos(t2);
		x3+=inputs_history[i].x;
		y3+=inputs_history[i].y;
		x4=x1*cos(t2)-y1*sin(t2);
		y4=x1*sin(t2)+y1*cos(t2);
		x4+=inputs_history[i].x;
		y4+=inputs_history[i].y;

		if(d<750) 
		{
			plotLine((int)(x4/25.4),(int)(y4/25.4), (int)(x3/25.4),(int)(y3/25.4) , -1);
			grid[(int)(x3/25.4)][(int)(y3/25.4)]+=2;
		}
		lx3=x3;
		ly3=y3;
	}
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

		int increment;
		increment = g->VisibleClipBounds.Height / 100;

		y=g->VisibleClipBounds.X;
		for(x=g->VisibleClipBounds.X; x<g->VisibleClipBounds.X + g->VisibleClipBounds.Height; x+=increment)
		{
			g->DrawLine(p2,x,y,x,(int)(y+g->VisibleClipBounds.Height));
			x_count++;
			//printf(".");
		}

		x=g->VisibleClipBounds.X;
		for(y=g->VisibleClipBounds.Y; y<g->VisibleClipBounds.Y + g->VisibleClipBounds.Height; y+=increment)
		{
			g->DrawLine(p2,x,y,(int)(x+g->VisibleClipBounds.Height),y);
			y_count++;
			//printf("-");
		}

		/*
		for(x=g->VisibleClipBounds.X; x<g->VisibleClipBounds.X + g->VisibleClipBounds.Width; x+=10)
		{
		for(y=g->VisibleClipBounds.Y; y<g->VisibleClipBounds.Y + g->VisibleClipBounds.Height; y+=10)
		{
		//g->DrawRectangle(p2,x,y,10,10);
		g->FillRectangle(b,x+2,y+2,7,7);
		count++;
		}
		}
		*/
		//log_printf("grid x,y,total,w,h=%d,%d,%d,%f,%f\n",x_count,y_count,count,g->VisibleClipBounds.Width,g->VisibleClipBounds.Height);
	}


	void f1::UpdateRadar(float a, int b)
	{
		const int pensize=10;
		Brush^ b0 =  gcnew SolidBrush(Color::White);
		Brush^ b1 =  gcnew SolidBrush(Color::Black);
		Brush^ b2 =  gcnew SolidBrush(Color::Red);
		Brush^ b3 =  gcnew SolidBrush(Color::Yellow);
		Brush^ b4 =  gcnew SolidBrush(Color::Blue);
		Pen^ p0 = gcnew Pen(Color::Black, 1);
		Pen^ p1 = gcnew Pen(Color::Black, pensize);
		Pen^ p2 = gcnew Pen(Color::Red, pensize);
		Pen^ p3 = gcnew Pen(Color::Yellow, pensize);
		Pen^ p4 = gcnew Pen(Color::Blue, pensize);
		static int i=1;
		int x,y;
		float theta;
		float x1,y1,x2,y2,rx,ry;
		int increment;

		increment = (g->VisibleClipBounds.Height / 100);
		//DrawGrid(g);
		if(history_index - i > 2000) i=history_index-2000;

		//for(i=history_index-360; i<=history_index;i++)
		while(i<history_index)
		{

			if(radar_checkBox_show_ir_north->Checked)		update_grid(i,1,Convert::ToSingle(radar_txt_calib_n_x->Text), Convert::ToSingle(radar_txt_calib_n_y->Text), Convert::ToSingle(radar_txt_calib_n_theta->Text) * (PI/180.0f));
			//if(radar_checkBox_show_ir_far_north->Checked)
			if(radar_checkBox_show_ir_nw->Checked)			update_grid(i,0,Convert::ToSingle(radar_txt_calib_nw_x->Text),Convert::ToSingle(radar_txt_calib_nw_y->Text),Convert::ToSingle(radar_txt_calib_nw_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_ne->Checked)			update_grid(i,2,Convert::ToSingle(radar_txt_calib_ne_x->Text),Convert::ToSingle(radar_txt_calib_ne_y->Text),Convert::ToSingle(radar_txt_calib_ne_theta->Text) * (PI/180.0f));

			if(radar_checkBox_show_ir_4->Checked)			update_grid(i,4,Convert::ToSingle(radar_txt_calib_4_x->Text),Convert::ToSingle(radar_txt_calib_4_y->Text),Convert::ToSingle(radar_txt_calib_4_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_5->Checked)			update_grid(i,5,Convert::ToSingle(radar_txt_calib_5_x->Text),Convert::ToSingle(radar_txt_calib_5_y->Text),Convert::ToSingle(radar_txt_calib_5_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_6->Checked)			update_grid(i,6,Convert::ToSingle(radar_txt_calib_6_x->Text),Convert::ToSingle(radar_txt_calib_6_y->Text),Convert::ToSingle(radar_txt_calib_6_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_7->Checked)			update_grid(i,7,Convert::ToSingle(radar_txt_calib_7_x->Text),Convert::ToSingle(radar_txt_calib_7_y->Text),Convert::ToSingle(radar_txt_calib_7_theta->Text) * (PI/180.0f));
			//g->FillEllipse(b2, (int)rx, (int)ry ,pensize, pensize);
			i++;
		}

		for(x=0;x<100;x++)
		{
			for(y=0;y<100;y++)
			{
				if(grid[x][y]) 
				{
					g->FillRectangle(b1,x*increment,(int)g->VisibleClipBounds.Height - (y+1)*increment,increment,increment);
					//grid[x][y]--;
				}
				else g->FillRectangle(b0,x*increment,(int)g->VisibleClipBounds.Height - (y+1)*increment,increment,increment);
				//else g->DrawRectangle(p0,x*increment,(int)g->VisibleClipBounds.Height - (y+1)*increment,increment,increment);
			}
		}
		g->FillEllipse(b2, (int)((inputs_history[i].x-100)/25.4)*increment, (int)g->VisibleClipBounds.Height - (int)((inputs_history[i].y+100)/25.4)*(increment) ,(int)(200/25.4)*increment, (int)(200/25.4)*increment);
	}

	System::Void f1::radar_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
	{
		if(radar_checkBox_enable_updates->Checked) UpdateRadar(0,0);
	}




	System::Void f1::radar_tabPage_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
	{
		Brush^ b3 =  gcnew SolidBrush(Color::Red);
		Brush^ b1 =  gcnew SolidBrush(Color::White);
		Brush^ b2 =  gcnew SolidBrush(Color::Black);
		Pen^ p = gcnew Pen(Color::Gray, 1.0);
		static int i=0;
		float theta;
		float x,y;

		//log("radar_tabPage_Paint\n");

#if 0

		if(history_index - i > 2000) i=history_index-2000;

		for(i=history_index-360; i<=history_index;i++)
		{
			if(i>=0)
			{
				theta = inputs_history[i].theta;
				log_printf("radar_tabPage_Paint(): theta = %04.1f\n",theta);
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

	System::Void f1::radar_btn_Clear(System::Object^  sender, System::EventArgs^  e) 
	{
		int i,j;
		for(i=0;i<100;i++)
		{
			for(j=0;j<100;j++)
			{
				grid[i][j]=0;
			}
		}
		radar_tabPage->Invalidate();
		g = radar_tabPage->CreateGraphics();
		log_printf("g-> X,Y,Width,Height = %f,%f,%f,%f\n",g->VisibleClipBounds.X,g->VisibleClipBounds.Y,g->VisibleClipBounds.Width,g->VisibleClipBounds.Height);

	}
}