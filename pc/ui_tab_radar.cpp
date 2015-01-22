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
		for(x=g->VisibleClipBounds.X; x<g->VisibleClipBounds.X + g->VisibleClipBounds.Height; x+=10)
		{
			g->DrawLine(p2,x,y,x,(int)(y+g->VisibleClipBounds.Height));
			x_count++;
			//printf(".");
		}

		x=g->VisibleClipBounds.X;
		for(y=g->VisibleClipBounds.Y; y<g->VisibleClipBounds.Y + g->VisibleClipBounds.Height; y+=10)
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
		Brush^ b1 =  gcnew SolidBrush(Color::Black);
		Brush^ b2 =  gcnew SolidBrush(Color::Red);
		Brush^ b3 =  gcnew SolidBrush(Color::Yellow);
		Brush^ b4 =  gcnew SolidBrush(Color::Blue);
		Pen^ p1 = gcnew Pen(Color::Black, pensize);
		Pen^ p2 = gcnew Pen(Color::Red, pensize);
		Pen^ p3 = gcnew Pen(Color::Yellow, pensize);
		Pen^ p4 = gcnew Pen(Color::Blue, pensize);
		static int i=1;
		float theta;
		float x1,y1,x2,y2,rx,ry;
		//DrawGrid(g);
		if(history_index - i > 2000) i=history_index-2000;

		//for(i=history_index-360; i<=history_index;i++)
		while(i<history_index)
		{
			rx = inputs_history[i].x/4;
			ry = g->VisibleClipBounds.Height-((int)inputs_history[i].y/4);

			if(radar_checkBox_show_ir_north->Checked)
			{
				float d,x1,y1,t1,t2,x2,y2,x3,y3;
				static float lx3=0,ly3=0;
				d=((float)inputs_history[i].ir[1]/10.0)*25.4;
				x1=Convert::ToSingle(radar_txt_calib_n_x->Text);
				y1=Convert::ToSingle(radar_txt_calib_n_y->Text);
				t1=Convert::ToSingle(radar_txt_calib_n_theta->Text) * (PI/180.0f);
				t2= inputs_history[i].theta; 
				x2=x1+d*cos(t1);
				y2=y1+d*sin(t1);
				x3=x2*cos(t2)-y2*sin(t2);
				y3=x2*sin(t2)+y2*cos(t2);
				x3+=inputs_history[i].x;
				y3+=inputs_history[i].y;
				//log_printf("d,x2,y2,x3,y3=%f   %f,%f   %f,%f\n",d,x2,y2,x3,y3);
				if(d<750) 
				{
					//if( (abs(x3-lx3)<90) && (abs(y3-ly3)<90) ) 	g->DrawLine(p1,(int)lx3/4, (int)g->VisibleClipBounds.Height-(int)ly3/4, (int) x3/4, (int)g->VisibleClipBounds.Height-(int) y3/4);
					//else 
					g->FillEllipse(b1,(int) x3/4, (int)g->VisibleClipBounds.Height  - (int)y3/4, pensize, pensize);
				}
				lx3=x3;
				ly3=y3;
			}
			if(radar_checkBox_show_ir_far_north->Checked)
			{
				float d,x1,y1,t1,t2,x2,y2,x3,y3;
				static float lx3=0,ly3=0;
				/*
				d=((float)inputs_history[i].sonar[0]/10.0)*25.4;
				x1=Convert::ToSingle(radar_txt_calib_fn_x->Text);
				y1=Convert::ToSingle(radar_txt_calib_fn_y->Text);
				t1=Convert::ToSingle(radar_txt_calib_fn_theta->Text) * (PI/180.0f);
				t2= inputs_history[i].theta; 
				x2=x1+d*cos(t1);
				y2=y1+d*sin(t1);
				x3=x2*cos(t2)-y2*sin(t2);
				y3=x2*sin(t2)+y2*cos(t2);
				x3+=inputs_history[i].x;
				y3+=inputs_history[i].y;
				//log_printf("d,x2,y2,x3,y3=%f   %f,%f   %f,%f\n",d,x2,y2,x3,y3);
				if(d<5000) 
				{
					//if( (abs(x3-lx3)<90) && (abs(y3-ly3)<90) ) g->DrawLine(p1,(int)lx3/4, (int)g->VisibleClipBounds.Height-(int)ly3/4, (int) x3/4, (int)g->VisibleClipBounds.Height-(int) y3/4);
					//else 
					g->FillEllipse(b2,(int) x3/4, (int)g->VisibleClipBounds.Height  - (int)y3/4, 4, 4);
				}
				lx3=x3;
				ly3=y3;
				*/
				d=((float)inputs_history[i].sonar[3]/10.0)*25.4;
				x1=-50; //Convert::ToSingle(radar_txt_calib_fn_x->Text);
				y1=-50; //Convert::ToSingle(radar_txt_calib_fn_y->Text);
				t1=-90 *(PI/180); //Convert::ToSingle(radar_txt_calib_fn_theta->Text) * (PI/180.0f);
				t2= inputs_history[i].theta; 
				x2=x1+d*cos(t1);
				y2=y1+d*sin(t1);
				x3=x2*cos(t2)-y2*sin(t2);
				y3=x2*sin(t2)+y2*cos(t2);
				x3+=inputs_history[i].x;
				y3+=inputs_history[i].y;
				//log_printf("d,x2,y2,x3,y3=%f   %f,%f   %f,%f\n",d,x2,y2,x3,y3);
				if(d<750) 
				{
					//if( (abs(x3-lx3)<90) && (abs(y3-ly3)<90) ) g->DrawLine(p1,(int)lx3/4, (int)g->VisibleClipBounds.Height-(int)ly3/4, (int) x3/4, (int)g->VisibleClipBounds.Height-(int) y3/4);
					//else 
					g->FillEllipse(b2,(int) x3/4, (int)g->VisibleClipBounds.Height  - (int)y3/4, pensize, pensize);
				}
				lx3=x3;
				ly3=y3;
			}
			if(radar_checkBox_show_ir_nw->Checked)
			{
				float d,x1,y1,t1,t2,x2,y2,x3,y3;
				static float lx3=0,ly3=0;
				d=((float)inputs_history[i].ir[0]/10.0)*25.4;
				x1=Convert::ToSingle(radar_txt_calib_nw_x->Text);
				y1=Convert::ToSingle(radar_txt_calib_nw_y->Text);
				t1=Convert::ToSingle(radar_txt_calib_nw_theta->Text) * (PI/180.0f);
				t2= inputs_history[i].theta; 
				x2=x1+d*cos(t1);
				y2=y1+d*sin(t1);
				x3=x2*cos(t2)-y2*sin(t2);
				y3=x2*sin(t2)+y2*cos(t2);
				x3+=inputs_history[i].x;
				y3+=inputs_history[i].y;
				//log_printf("d,x2,y2,x3,y3=%f   %f,%f   %f,%f\n",d,x2,y2,x3,y3);
				if(d<750) 
				{
					//if( (abs(x3-lx3)<90) && (abs(y3-ly3)<90) ) g->DrawLine(p3,(int)lx3/4, (int)g->VisibleClipBounds.Height-(int)ly3/4, (int) x3/4, (int)g->VisibleClipBounds.Height-(int) y3/4);
					//else 
					g->FillEllipse(b3,(int) x3/4, (int)g->VisibleClipBounds.Height  - (int)y3/4, pensize,pensize);
				}
				lx3=x3;
				ly3=y3;
			}
			if(radar_checkBox_show_ir_ne->Checked)
			{
				float d,x1,y1,t1,t2,x2,y2,x3,y3;
				static float lx3=0,ly3=0;
				d=((float)inputs_history[i].ir[2]/10.0)*25.4;
				x1=Convert::ToSingle(radar_txt_calib_ne_x->Text);
				y1=Convert::ToSingle(radar_txt_calib_ne_y->Text);
				t1=Convert::ToSingle(radar_txt_calib_ne_theta->Text) * (PI/180.0f);
				t2= inputs_history[i].theta; 
				x2=x1+d*cos(t1);
				y2=y1+d*sin(t1);
				x3=x2*cos(t2)-y2*sin(t2);
				y3=x2*sin(t2)+y2*cos(t2);
				x3+=inputs_history[i].x;
				y3+=inputs_history[i].y;
				//log_printf("d,x2,y2,x3,y3=%f   %f,%f   %f,%f\n",d,x2,y2,x3,y3);
				if(d<750) 
				{
					//if( (abs(x3-lx3)<90) && (abs(y3-ly3)<90) ) g->DrawLine(p4,(int)lx3/4, (int)g->VisibleClipBounds.Height-(int)ly3/4, (int) x3/4, (int)g->VisibleClipBounds.Height-(int) y3/4);
					//else 
					g->FillEllipse(b4,(int) x3/4, (int)g->VisibleClipBounds.Height  - (int)y3/4, pensize, pensize);
				}
				lx3=x3;
				ly3=y3;
			}
			g->FillEllipse(b2, (int)rx, (int)ry ,pensize, pensize);
			i++;
		}
		//g->FillEllipse(b2,10, 10, 6, 6);
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

	System::Void f1::radar_btn_start_scan_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		radar_tabPage->Invalidate();
		g = radar_tabPage->CreateGraphics();
		log_printf("g-> X,Y,Width,Height = %f,%f,%f,%f\n",g->VisibleClipBounds.X,g->VisibleClipBounds.Y,g->VisibleClipBounds.Width,g->VisibleClipBounds.Height);

	}
}