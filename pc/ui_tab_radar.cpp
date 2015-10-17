#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>

#define grid_size 200
#define cell_size (25.4/2)
#define DRAW_ROBOT 1
#define IMMEDIATE_UPDATE 1

#if IMMEDIATE_UPDATE
#define GRID_VALUE_INC 20
#define GRID_VALUE_MAX 20
#define GRID_VALUE_DEC -1
#else
#define GRID_VALUE_INC 2
#define GRID_VALUE_MAX 2
#define GRID_VALUE_DEC -1
#endif

typedef struct
{
	int value;
	int changed;
} t_grid;

unsigned int bitmap[grid_size][grid_size];

t_grid grid[grid_size][grid_size];

//index into s.ir and s.inputs.ir - does not need to match actual input channel numbers
#define IR_N			0
#define IR_NE			4
#define IR_E			6
#define IR_W			5
#define IR_NW			3

#define IR_SE			1
#define IR_S			2
#define IR_SW			7

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
		if ((x0>0) && (y0>0) && (x0 < grid_size) && (y0 < grid_size))
		{
			grid[x0][y0].value += value;
			grid[x0][y0].changed = 1;
			if (grid[x0][y0].value < 0) grid[x0][y0].value = 0;
			if (grid[x0][y0].value > GRID_VALUE_MAX) grid[x0][y0].value = GRID_VALUE_MAX;
		}
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
	void f1::update_grid(float d, float x0, float y0, float t0,float x1, float y1, float t1)
	{
		float t2,x2,y2,x3,y3,x4,y4;
		static float lx3=0,ly3=0;
		int i = history_index;
		int offset = radar_trackBar1->Value;

		//d=((float)inputs_history[i].ir[ir_sensor_index]/10.0)*cell_size;
		//d=inputs_history[i].lidar.samples[ir_sensor_index]*1000;

		i=i+offset;

		t2= t0; //inputs_history[i].theta; 
		x2=x1+d*cos(t1);
		y2=y1+d*sin(t1);
		x3=x2*cos(t2)-y2*sin(t2);
		y3=x2*sin(t2)+y2*cos(t2);
		x3+=x0; //inputs_history[i].x;
		y3+=y0; //inputs_history[i].y;
		x4=x1*cos(t2)-y1*sin(t2);
		y4=x1*sin(t2)+y1*cos(t2);
		x4+=x0; //inputs_history[i].x;
		y4+=y0; //inputs_history[i].y;

		//if(d<750) 
		if((d>0) && (d<5000))
		{
			plotLine((int)(x4/cell_size),(int)(y4/cell_size), (int)(x3/cell_size),(int)(y3/cell_size) , GRID_VALUE_DEC);

			if ((x3 / cell_size>0) && (y3 / cell_size > 0) && (x3 / cell_size < grid_size) && (y3 / cell_size < grid_size))
			{

				grid[(int)(x3 / cell_size)][(int)(y3 / cell_size)].value += GRID_VALUE_INC;
				if (grid[(int)(x3 / cell_size)][(int)(y3 / cell_size)].value > GRID_VALUE_MAX) grid[(int)(x3 / cell_size)][(int)(y3 / cell_size)].value = GRID_VALUE_MAX;
				grid[(int)(x3 / cell_size)][(int)(y3 / cell_size)].changed = 1;

				if ((abs(x3 - lx3) < 150) && (abs(y3 - ly3) < 150))
				{
					if (radar_checkBox_use_lines->Checked)
					{
						plotLine((int)(lx3 / cell_size), (int)(ly3 / cell_size), (int)(x3 / cell_size), (int)(y3 / cell_size), GRID_VALUE_INC);
					}
				}
			}
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
		increment = g->VisibleClipBounds.Height / grid_size;

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

	void f1::ShowRadar(int ex, int ey)
	{
		int x,y,i,j;
		int increment;
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

		increment = (g->VisibleClipBounds.Height / grid_size);
#if 1
		for(x=0;x<grid_size;x++)
		{
			for(y=0;y<grid_size;y++)
			{
				//grid[x][y].changed=1;
				if(grid[x][y].value)
				{
					grid[x][y].value--;
					if(grid[x][y].value==0) grid[x][y].changed=1;
				}

				if(grid[x][y].changed)
				{
					grid[x][y].changed = 0;
					if(grid[x][y].value) 
					{
						g->FillRectangle(b1,x*increment,(int)g->VisibleClipBounds.Height - (y+1)*increment,increment,increment);
					}
					else g->FillRectangle(b0,x*increment,(int)g->VisibleClipBounds.Height - (y+1)*increment,increment,increment);
					//else g->DrawRectangle(p0,x*increment,(int)g->VisibleClipBounds.Height - (y+1)*increment,increment,increment);
				}
				//grid[x][y].value=0;  
			}
		}
#else

		for(x=0;x<grid_size;x++)
		{
			for(y=0;y<grid_size;y++)
			{
				if(grid[x][y].value>0) bitmap[x][y]=0; else bitmap[x][y]=0xffffffffUL;
			}
		}

		System::IntPtr ptr(bitmap);
		System::Drawing::Bitmap^ bm  = gcnew System::Drawing::Bitmap(grid_size,grid_size,400,System::Drawing::Imaging::PixelFormat::Format32bppRgb,ptr);
		System::Drawing::RectangleF rect(0,0,pictureBox1->Width,pictureBox1->Height);
		g->DrawImage(bm,rect);	

#endif
#if DRAW_ROBOT
		//g->FillEllipse(b2, (int)((inputs_history[i].x-100)/cell_size)*increment, (int)g->VisibleClipBounds.Height - (int)((inputs_history[i].y+100)/cell_size)*(increment) ,(int)(200/cell_size)*increment, (int)(200/cell_size)*increment);
		//if(ex>2)
		{
			g->DrawEllipse(p2, ex, ey,(int)(50/cell_size)*increment, (int)(50/cell_size)*increment);
		}
#endif
	}

	void f1::UpdateRadar(float a, int b)
	{
		static int i=1;
		static float x=0,y=0,theta=0;
		float x1,y1,x2,y2,rx,ry;
		int scan_completed=0;
		DWORD t1,t2,t3,count=0;

#ifdef USE_LIDAR

		//DrawGrid(g);
		if(history_index - i > 2000) i=history_index-2000;

		t1 = timeGetTime();

		//for(i=history_index-360; i<=history_index;i++)
		scan_completed=0;
		while( (i<(history_index-1)) )
		{
			/*
			if(radar_checkBox_show_ir_north->Checked)		update_grid(i,IR_N,Convert::ToSingle(radar_txt_calib_n_x->Text), Convert::ToSingle(radar_txt_calib_n_y->Text), Convert::ToSingle(radar_txt_calib_n_theta->Text) * (PI/180.0f));
			//if(radar_checkBox_show_ir_far_north->Checked)
			if(radar_checkBox_show_ir_nw->Checked)			update_grid(i,IR_NW,Convert::ToSingle(radar_txt_calib_nw_x->Text),Convert::ToSingle(radar_txt_calib_nw_y->Text),Convert::ToSingle(radar_txt_calib_nw_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_ne->Checked)			update_grid(i,IR_NE,Convert::ToSingle(radar_txt_calib_ne_x->Text),Convert::ToSingle(radar_txt_calib_ne_y->Text),Convert::ToSingle(radar_txt_calib_ne_theta->Text) * (PI/180.0f));

			if(radar_checkBox_show_ir_4->Checked)			update_grid(i,IR_E,Convert::ToSingle(radar_txt_calib_4_x->Text),Convert::ToSingle(radar_txt_calib_4_y->Text),Convert::ToSingle(radar_txt_calib_4_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_5->Checked)			update_grid(i,IR_SE,Convert::ToSingle(radar_txt_calib_5_x->Text),Convert::ToSingle(radar_txt_calib_5_y->Text),Convert::ToSingle(radar_txt_calib_5_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_6->Checked)			update_grid(i,IR_SW,Convert::ToSingle(radar_txt_calib_6_x->Text),Convert::ToSingle(radar_txt_calib_6_y->Text),Convert::ToSingle(radar_txt_calib_6_theta->Text) * (PI/180.0f));
			if(radar_checkBox_show_ir_7->Checked)			update_grid(i,IR_W,Convert::ToSingle(radar_txt_calib_7_x->Text),Convert::ToSingle(radar_txt_calib_7_y->Text),Convert::ToSingle(radar_txt_calib_7_theta->Text) * (PI/180.0f));
			//g->FillEllipse(b2, (int)rx, (int)ry ,pensize, pensize);
			*/
			{
				int j;
				t_inputs h,h2;
				h = inputs_history[i+0];
				h2= inputs_history[i+1]; //need +1 here for motion comp

				for(j=0;j<h2.lidar.num_samples;j++)
				{
					//log_printf("inputs_history[i].lidar.angle=%f\n",inputs_history[i].lidar.angle+j);
#if IMMEDIATE_UPDATE
					update_grid(h2.lidar.samples[j]*1000, h.x, h.y, h.theta, 0,0,(h2.lidar.angle+j+90) * (PI/180.0f));
#else
					update_grid(h2.lidar.samples[j]*1000, (grid_size*cell_size)/2 + h.x-x, (grid_size*cell_size)/2 + h.y-y, h.theta - theta, 0,0,(h2.lidar.angle+j+90) * (PI/180.0f));
					if(((int)(h2.lidar.angle+j))==180)
					{
						int increment = (g->VisibleClipBounds.Height / grid_size);
						int ex = (int)((((grid_size*cell_size) / 2) - 100) / cell_size)*increment;
						int ey = (int)g->VisibleClipBounds.Height - (int)((((grid_size*cell_size) / 2) + 100) / cell_size)*(increment);
						t2 = timeGetTime();
						ShowRadar(ex, ey);
						t3 = timeGetTime(); //t3-t2 is 160ms
						log_printf("dT=%d\n", t3-t2);
						x = h2.x;
						y = h2.y;
						theta = h2.theta;
					}
#endif
				}
			}
			i++;
		}

		t2 = timeGetTime();
#if IMMEDIATE_UPDATE
		{
			t_inputs h = inputs_history[i + 0];
			int increment = (g->VisibleClipBounds.Height / grid_size);
			int ex = (int)((h.x - 100) / cell_size)*increment;
			int ey = (int)g->VisibleClipBounds.Height - (int)((h.y + 100) / cell_size)*(increment);
			ShowRadar(ex, ey);
		}
#endif
#endif //#ifdef USE_LIDAR
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
		for(i=0;i<grid_size;i++)
		{
			for(j=0;j<grid_size;j++)
			{
				grid[i][j].value=0;
				grid[i][j].changed=1;
			}
		}
		radar_tabPage->Invalidate();
		//g = radar_tabPage->CreateGraphics();
		pictureBox1->Invalidate();
		g = pictureBox1->CreateGraphics();
		log_printf("g-> X,Y,Width,Height = %f,%f,%f,%f\n",g->VisibleClipBounds.X,g->VisibleClipBounds.Y,g->VisibleClipBounds.Width,g->VisibleClipBounds.Height);

	}
}