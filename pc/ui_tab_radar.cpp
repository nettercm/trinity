#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>

#define grid_size 200
#define cell_size (25.4/1)
#define DRAW_ROBOT 1
#define IMMEDIATE_UPDATE 1
#define DRAW_GRID 0

#if IMMEDIATE_UPDATE
#define GRID_VALUE_INC 5
#define GRID_VALUE_MAX 100
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
		/*
			d:			distance in mm;  i.e. the sensor reading
			x0,y0,t0:	robot position & heading (x, y, theta)
			x1,y1,t1:	sensor position & direction, relative to the robot's center 
						(x1 and y1 are 0 for Lidar, but non-zero for IR sensors positioned around the robot base perimeter)

		*/
		float x2,y2,x3,y3,x4,y4;
		int ix3,iy3, ix4, iy4;
		volatile static int lx3=0,ly3=0;
		int i = history_index;
		int inc, dec, max;
		
		inc = Convert::ToInt32(radar_textBox_inc->Text);
		dec = Convert::ToInt32(radar_textBox_dec->Text);
		max = Convert::ToInt32(radar_textBox_max->Text);

		//x2,y2 are the coordinates of the obstacle, relaive to the sensor's origin
		x2=x1+d*cos(t1);
		y2=y1+d*sin(t1);

		//x3,y3 are the absolute coordinates of the obstacle
		x3=x2*cos(t0)-y2*sin(t0);
		y3=x2*sin(t0)+y2*cos(t0);
		x3+=x0; 
		y3+=y0; 
		ix3 = (int)(x3/cell_size);
		iy3 = (int)(y3/cell_size);


		//x4,y4 are the absolute coordinates of the sensor;  only needed so that grid cells that are known to be empty can be marked empty
		x4=x1*cos(t0)-y1*sin(t0);
		y4=x1*sin(t0)+y1*cos(t0);
		x4+=x0; 
		y4+=y0; 
		ix4 = (int)(x4/cell_size);
		iy4 = (int)(y4/cell_size);


		//only updated the grid if the sensor reading is valid
		if((d>150) && (d<6000))
		{
			plotLine(ix4,iy4, ix3,iy3, dec);

			if( (ix3>0) && (iy3>0) && (ix3<grid_size) && (iy3<grid_size) )
			{
				grid[ix3][iy3].value += inc;
				if(grid[ix3][iy3].value > max) grid[ix3][iy3].value = max;
				grid[ix3][iy3].changed = 1;

				if ((abs(ix3 - lx3) < 20) && (abs(iy3 - ly3) < 20))
				{
					if (radar_checkBox_use_lines->Checked)
					{
						plotLine(lx3, ly3, ix3, iy3, inc);
					}
				}
			}
			lx3=ix3;
			ly3=iy3;
		}
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

	}

	void f1::ShowRadar(int ex, int ey)
	{
		int x,y,i,j;
		float increment;
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
#if 0
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
				if(grid[x][y].value>0) grid[x][y].value--;
			}
		}

		System::IntPtr ptr(bitmap);
		System::Drawing::Bitmap^ bm  = gcnew System::Drawing::Bitmap(grid_size,grid_size,grid_size*4,System::Drawing::Imaging::PixelFormat::Format32bppRgb,ptr);
		bm->RotateFlip(System::Drawing::RotateFlipType::Rotate270FlipNone);
		System::Drawing::RectangleF rect(0,0,pictureBox1->Width,pictureBox1->Height);
		g->DrawImage(bm,rect);	

#endif
#if DRAW_ROBOT
		g->DrawEllipse(p2, ex, ey,(int)((20.0f/cell_size)*increment), (int)((20.0f/cell_size)*increment));
#endif
	}



	void f1::UpdateRadar(float a, int b)
	{
		static int i=1;
		static float x=0,y=0,theta=0;
		float x1,y1,x2,y2,rx,ry;
		int scan_completed=0;
		static DWORD t1,t2,t3,count=0;
		float angle = Convert::ToSingle(radar_textBox_angle->Text);
		int j;
		t_inputs h,h2;

#ifdef USE_LIDAR

#if DRAW_GRID
		DrawGrid(g);
#endif

		if(history_index - i > 200) i=history_index-200;

		t1 = timeGetTime();

		scan_completed=0;
		while( (i<(history_index-1)) )
		{
			{
				h = inputs_history[i+0];
				h2= inputs_history[i+1]; //need +1 here for motion comp
				h.lidar.angle*=4;
				h2.lidar.angle*=4;
				//log_printf("numsamples=%d  angle=%d,  s1=%d\n",h2.lidar.num_samples,  h2.lidar.angle, h2.lidar.samples[0]);

				if(radar_CheckBox_immediate_update->Checked)
				{
					h.x = (grid_size*cell_size)/2; //  1000;
					h.y = (grid_size*cell_size)/2; //  1000;
					h.theta = 0;
				}

				for(j=0;j<h2.lidar.num_samples;j++)
				{
#if IMMEDIATE_UPDATE
					update_grid(h2.lidar.samples[j]/* *1000 */, h.x, h.y, h.theta, 0,0,(h2.lidar.angle+j+angle) * (PI/180.0f));
#else
					update_grid(h2.lidar.samples[j]/* *1000 */, (grid_size*cell_size)/2 + h.x-x, (grid_size*cell_size)/2 + h.y-y, h.theta - theta, 0,0,(h2.lidar.angle+j+angle) * (PI/180.0f));
					if(((int)(h2.lidar.angle+j))==180)
					{
						int increment = (g->VisibleClipBounds.Height / grid_size);
						int ex = (int)((((grid_size*cell_size) / 2) /*- 100*/ ) / cell_size)*increment;
						int ey = (int)g->VisibleClipBounds.Height - (int)((((grid_size*cell_size) / 2) /*+ 100*/) / cell_size)*(increment);
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

#if IMMEDIATE_UPDATE
		{
			h = inputs_history[i + 0];
			if(radar_CheckBox_immediate_update->Checked)
			{
				h.x = (grid_size*cell_size)/2; //  1000;
				h.y = (grid_size*cell_size)/2; //  1000;
				h.theta = 0;
			}
			float increment = (g->VisibleClipBounds.Height / grid_size);
			int ex = (int)(((h.x - 0) / cell_size)*increment);
			int ey = (int)(g->VisibleClipBounds.Height - ((int)((h.y + 0) / cell_size))*(increment));
			ShowRadar(ex, ey);
		}
#endif

	if(radar_checkBox_grid->Checked) DrawGrid(g);

	t2 = timeGetTime();

	count++;
	if(timeGetTime() - t3 > 1000)
	{
		t3 = timeGetTime();
		log_printf("%d grid updates per second\n",count);
		count=0;
	}

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