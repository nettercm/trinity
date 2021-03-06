
#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>



namespace robot_ui 
{
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
		System::Void f1::graphs_checkBox_enable_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(graphs_checkBox_enable->Checked) graphs_timer->Enabled = TRUE;
			 else graphs_timer->Enabled = FALSE;
		 }




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
		System::Void f1::graphs_chart_AxisViewChanged(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^  e) 
		 {
			 if(e->ChartArea->Name == "ChartArea1")
			 {
				 //graphs_chart->ChartAreas["ChartArea2"]->AxisX->ScaleView = graphs_chart->ChartAreas["ChartArea1"]->AxisX->ScaleView;
			 }
		 }




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::graphs_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
	{
		static int x=0;
		static int y=0;

		if(history_index - x > 2000) x=history_index-2000;
		if(x<0) x=0;
		if(x>history_index) x=history_index; //wrap-around

		while(x<history_index)
		{
			if(		graphs_comboBox_series_1a->Text == "analog[0]") y = inputs_history[x].analog[0];
			else if(graphs_comboBox_series_1a->Text == "analog[1]") y = inputs_history[x].analog[1];
			else if(graphs_comboBox_series_1a->Text == "analog[2]") y = inputs_history[x].analog[2];
			else if(graphs_comboBox_series_1a->Text == "analog[3]") y = inputs_history[x].analog[3];
			else if(graphs_comboBox_series_1a->Text == "analog[4]") y = inputs_history[x].analog[4];
			else if(graphs_comboBox_series_1a->Text == "analog[5]") y = inputs_history[x].analog[5];
			else if(graphs_comboBox_series_1a->Text == "analog[6]") y = inputs_history[x].analog[6];
			else if(graphs_comboBox_series_1a->Text == "analog[7]") y = inputs_history[x].analog[7];
			else if(graphs_comboBox_series_1a->Text == "analog[10]") y = inputs_history[x].analog[10];
			else if(graphs_comboBox_series_1a->Text == "analog[11]") y = inputs_history[x].analog[11];
			else if(graphs_comboBox_series_1a->Text == "analog[12]") y = inputs_history[x].analog[12];
			else if(graphs_comboBox_series_1a->Text == "analog[13]") y = inputs_history[x].analog[13];
			else if(graphs_comboBox_series_1a->Text == "analog[14]") y = inputs_history[x].analog[14];
			else if(graphs_comboBox_series_1a->Text == "ir[0]") y = inputs_history[x].ir[0];
			else if(graphs_comboBox_series_1a->Text == "ir[1]") y = inputs_history[x].ir[1];
			else if(graphs_comboBox_series_1a->Text == "ir[2]") y = inputs_history[x].ir[2];
			else if(graphs_comboBox_series_1a->Text == "ir[3]") y = inputs_history[x].ir[3];
			else if(graphs_comboBox_series_1a->Text == "sonar[0]") y = inputs_history[x].sonar[0];
			else if(graphs_comboBox_series_1a->Text == "sonar[1]") y = inputs_history[x].sonar[1];
			else if(graphs_comboBox_series_1a->Text == "sonar[2]") y = inputs_history[x].sonar[2];
			else if(graphs_comboBox_series_1a->Text == "sonar[3]") y = inputs_history[x].sonar[3];
			else if(graphs_comboBox_series_1a->Text == "sonar[4]") y = inputs_history[x].sonar[4];
			else if(graphs_comboBox_series_1a->Text == "theta") y = inputs_history[x].theta;
			else y = x % 500;
			graphs_chart->Series[0]->Points->AddXY(x, y);

			if(		graphs_comboBox_series_1b->Text == "analog[0]") y = inputs_history[x].analog[0];
			else if(graphs_comboBox_series_1b->Text == "analog[1]") y = inputs_history[x].analog[1];
			else if(graphs_comboBox_series_1b->Text == "analog[2]") y = inputs_history[x].analog[2];
			else if(graphs_comboBox_series_1b->Text == "analog[3]") y = inputs_history[x].analog[3];
			else if(graphs_comboBox_series_1b->Text == "analog[4]") y = inputs_history[x].analog[4];
			else if(graphs_comboBox_series_1b->Text == "analog[5]") y = inputs_history[x].analog[5];
			else if(graphs_comboBox_series_1b->Text == "analog[6]") y = inputs_history[x].analog[6];
			else if(graphs_comboBox_series_1b->Text == "analog[7]") y = inputs_history[x].analog[7];
			else if(graphs_comboBox_series_1b->Text == "analog[10]") y = inputs_history[x].analog[10];
			else if(graphs_comboBox_series_1b->Text == "analog[11]") y = inputs_history[x].analog[11];
			else if(graphs_comboBox_series_1b->Text == "analog[12]") y = inputs_history[x].analog[12];
			else if(graphs_comboBox_series_1b->Text == "analog[13]") y = inputs_history[x].analog[13];
			else if(graphs_comboBox_series_1b->Text == "analog[14]") y = inputs_history[x].analog[14];
			else if(graphs_comboBox_series_1b->Text == "ir[0]") y = inputs_history[x].ir[0];
			else if(graphs_comboBox_series_1b->Text == "ir[1]") y = inputs_history[x].ir[1];
			else if(graphs_comboBox_series_1b->Text == "ir[2]") y = inputs_history[x].ir[2];
			else if(graphs_comboBox_series_1b->Text == "ir[3]") y = inputs_history[x].ir[3];
			else if(graphs_comboBox_series_1b->Text == "sonar[0]") y = inputs_history[x].sonar[0];
			else if(graphs_comboBox_series_1b->Text == "sonar[1]") y = inputs_history[x].sonar[1];
			else if(graphs_comboBox_series_1b->Text == "sonar[2]") y = inputs_history[x].sonar[2];
			else if(graphs_comboBox_series_1b->Text == "sonar[3]") y = inputs_history[x].sonar[3];
			else if(graphs_comboBox_series_1b->Text == "sonar[4]") y = inputs_history[x].sonar[4];
			else if(graphs_comboBox_series_1b->Text == "theta") y = inputs_history[x].theta;
			else y = x % 200;
			graphs_chart->Series[2]->Points->AddXY(x, y);

			if(		graphs_comboBox_series_2a->Text == "analog[0]") y = inputs_history[x].analog[0];
			else if(graphs_comboBox_series_2a->Text == "analog[1]") y = inputs_history[x].analog[1];
			else if(graphs_comboBox_series_2a->Text == "analog[2]") y = inputs_history[x].analog[2];
			else if(graphs_comboBox_series_2a->Text == "analog[3]") y = inputs_history[x].analog[3];
			else if(graphs_comboBox_series_2a->Text == "analog[4]") y = inputs_history[x].analog[4];
			else if(graphs_comboBox_series_2a->Text == "analog[5]") y = inputs_history[x].analog[5];
			else if(graphs_comboBox_series_2a->Text == "analog[6]") y = inputs_history[x].analog[6];
			else if(graphs_comboBox_series_2a->Text == "analog[7]") y = inputs_history[x].analog[7];
			else if(graphs_comboBox_series_2a->Text == "analog[10]") y = inputs_history[x].analog[10];
			else if(graphs_comboBox_series_2a->Text == "analog[11]") y = inputs_history[x].analog[11];
			else if(graphs_comboBox_series_2a->Text == "analog[12]") y = inputs_history[x].analog[12];
			else if(graphs_comboBox_series_2a->Text == "analog[13]") y = inputs_history[x].analog[13];
			else if(graphs_comboBox_series_2a->Text == "analog[14]") y = inputs_history[x].analog[14];
			else if(graphs_comboBox_series_2a->Text == "ir[0]") y = inputs_history[x].ir[0];
			else if(graphs_comboBox_series_2a->Text == "ir[1]") y = inputs_history[x].ir[1];
			else if(graphs_comboBox_series_2a->Text == "ir[2]") y = inputs_history[x].ir[2];
			else if(graphs_comboBox_series_2a->Text == "ir[3]") y = inputs_history[x].ir[3];
			else if(graphs_comboBox_series_2a->Text == "sonar[0]") y = inputs_history[x].sonar[0];
			else if(graphs_comboBox_series_2a->Text == "sonar[1]") y = inputs_history[x].sonar[1];
			else if(graphs_comboBox_series_2a->Text == "sonar[2]") y = inputs_history[x].sonar[2];
			else if(graphs_comboBox_series_2a->Text == "sonar[3]") y = inputs_history[x].sonar[3];
			else if(graphs_comboBox_series_2a->Text == "sonar[4]") y = inputs_history[x].sonar[4];
			else if(graphs_comboBox_series_2a->Text == "theta") y = inputs_history[x].theta;
			else y = x % 500;
			graphs_chart->Series[1]->Points->AddXY(x, y);
			x++;
		}
		// Keep a constant number of points by removing them from the left
		while(graphs_chart->Series[0]->Points->Count > 2000) graphs_chart->Series[0]->Points->RemoveAt(0);
		while(graphs_chart->Series[1]->Points->Count > 2000) graphs_chart->Series[1]->Points->RemoveAt(0);
		while(graphs_chart->Series[2]->Points->Count > 2000) graphs_chart->Series[2]->Points->RemoveAt(0);

		//graphs_chart->Invalidate();
	}

		 
		 
		 
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::InitializeGraphsTab(void)
	{
		// Zoom into the X axis
		//graphs_chart->ChartAreas["ChartArea1"]->AxisX->ScaleView->Zoom(2, 3);

		// Enable range selection and zooming end user interface
		graphs_chart->ChartAreas["ChartArea1"]->CursorX->IsUserEnabled = true;
		graphs_chart->ChartAreas["ChartArea1"]->CursorX->IsUserSelectionEnabled = true;
		graphs_chart->ChartAreas["ChartArea1"]->AxisX->ScaleView->Zoomable = true;
		graphs_chart->ChartAreas["ChartArea1"]->AxisX->ScrollBar->IsPositionedInside = true;
		graphs_chart->ChartAreas["ChartArea1"]->CursorY->IsUserEnabled = true;
		graphs_chart->ChartAreas["ChartArea1"]->CursorY->IsUserSelectionEnabled = true;
		graphs_chart->ChartAreas["ChartArea1"]->AxisY->ScaleView->Zoomable = true;
		graphs_chart->ChartAreas["ChartArea1"]->AxisY->ScrollBar->IsPositionedInside = true;

		// Enable range selection and zooming end user interface
		graphs_chart->ChartAreas["ChartArea2"]->CursorX->IsUserEnabled = true;
		graphs_chart->ChartAreas["ChartArea2"]->CursorX->IsUserSelectionEnabled = true;
		graphs_chart->ChartAreas["ChartArea2"]->AxisX->ScaleView->Zoomable = true;
		graphs_chart->ChartAreas["ChartArea2"]->AxisX->ScrollBar->IsPositionedInside = true;
		graphs_chart->ChartAreas["ChartArea2"]->CursorY->IsUserEnabled = true;
		graphs_chart->ChartAreas["ChartArea2"]->CursorY->IsUserSelectionEnabled = true;
		graphs_chart->ChartAreas["ChartArea2"]->AxisY->ScaleView->Zoomable = true;
		graphs_chart->ChartAreas["ChartArea2"]->AxisY->ScrollBar->IsPositionedInside = true;

		graphs_chart->ChartAreas["ChartArea2"]->AxisX->ScaleView = graphs_chart->ChartAreas["ChartArea1"]->AxisX->ScaleView;

		graphs_chart->Series["Series 1a"]->Points->Clear();
		graphs_chart->Series["Series 1a"]->IsXValueIndexed=TRUE;
		graphs_chart->Series["Series 1a"]->Enabled = TRUE; //FALSE;

		graphs_chart->Series["Series 2a"]->Points->Clear();
		graphs_chart->Series["Series 2a"]->IsXValueIndexed=TRUE;
		graphs_chart->Series["Series 2a"]->Enabled = TRUE; //FALSE;

		graphs_chart->Series["Series 1b"]->Points->Clear();
		graphs_chart->Series["Series 1b"]->IsXValueIndexed=TRUE;
		graphs_chart->Series["Series 1b"]->Enabled = TRUE; //FALSE;

		graphs_comboBox_series_1a->Items->Clear();
		graphs_comboBox_series_1b->Items->Clear();
		graphs_comboBox_series_2a->Items->Clear();
		graphs_comboBox_series_2b->Items->Clear();

		graphs_comboBox_series_1a->Items->AddRange(gcnew cli::array< System::Object^  >(34) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"analog[10]", L"analog[11]", L"analog[12]", L"analog[13]", L"analog[14]",
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", 
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]",
			L"sonar[0]", L"sonar[1]", L"sonar[2]", L"sonar[3]", L"sonar[4]",
		});

		graphs_comboBox_series_1b->Items->AddRange(gcnew cli::array< System::Object^  >(34) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"analog[10]", L"analog[11]", L"analog[12]", L"analog[13]", L"analog[14]",
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", 
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]",
			L"sonar[0]", L"sonar[1]", L"sonar[2]", L"sonar[3]", L"sonar[4]",
		});

		graphs_comboBox_series_2a->Items->AddRange(gcnew cli::array< System::Object^  >(34) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"analog[10]", L"analog[11]", L"analog[12]", L"analog[13]", L"analog[14]",
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]",
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]",
			L"sonar[0]", L"sonar[1]", L"sonar[2]", L"sonar[3]", L"sonar[4]",
		});

		graphs_comboBox_series_2b->Items->AddRange(gcnew cli::array< System::Object^  >(34) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"analog[10]", L"analog[11]", L"analog[12]", L"analog[13]", L"analog[14]",
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]",
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]",
			L"sonar[0]", L"sonar[1]", L"sonar[2]", L"sonar[3]", L"sonar[4]",
		});

		graphs_comboBox_series_1a->SelectedIndex = 0;
		graphs_comboBox_series_1b->SelectedIndex = 0;
		graphs_comboBox_series_2a->SelectedIndex = 0;
		graphs_comboBox_series_2b->SelectedIndex = 0;
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::UpdateChart_method(String^ series, double x, double y)
	{
#if 0 //this way of updating the chart is not actually used right now
		unsigned int t;
		double y1,y2;

		if(x>=500) 
		{
			graphs_chart->Series["Series 1a"]->Points->RemoveAt(0);
			graphs_chart->Series["Series 2a"]->Points->RemoveAt(0);
		}

		y1 = s.inputs->watch[0]; //actual_speed[0]; //ir[0];
		y2 = s.inputs->watch[1]; //ir[2]; //ir[1];
		t = timeGetTime();

		graphs_chart->Series["Series 1a"]->Points->AddXY(x,y1);
		graphs_chart->Series["Series 2a"]->Points->AddXY(x,y2);
		//graphs_chart->Invalidate();
#endif
	}





}
