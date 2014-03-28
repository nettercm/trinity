
#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>



namespace robot_ui 
{
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
		System::Void f1::checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(checkBox2->Checked) t1->Enabled = TRUE;
			 else t1->Enabled = FALSE;
		 }




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
		System::Void f1::chart1_AxisViewChanged(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^  e) 
		 {
			 if(e->ChartArea->Name == "ChartArea1")
			 {
				 //chart1->ChartAreas["ChartArea2"]->AxisX->ScaleView = chart1->ChartAreas["ChartArea1"]->AxisX->ScaleView;
			 }
		 }




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::t1_Tick(System::Object^  sender, System::EventArgs^  e) 
	{
		static int x=0;
		static int y=0;

		if(history_index - x > 2000) x=history_index-2000;

		while(x<history_index)
		{
			if(		comboBox2->Text == "analog[0]") y = inputs_history[x].analog[0];
			else if(comboBox2->Text == "analog[1]") y = inputs_history[x].analog[1];
			else if(comboBox2->Text == "analog[2]") y = inputs_history[x].analog[2];
			else if(comboBox2->Text == "analog[3]") y = inputs_history[x].analog[3];
			else if(comboBox2->Text == "analog[4]") y = inputs_history[x].analog[4];
			else if(comboBox2->Text == "analog[5]") y = inputs_history[x].analog[5];
			else if(comboBox2->Text == "analog[6]") y = inputs_history[x].analog[6];
			else if(comboBox2->Text == "analog[7]") y = inputs_history[x].analog[7];
			else if(comboBox2->Text == "ir[0]") y = inputs_history[x].ir[0];
			else if(comboBox2->Text == "ir[1]") y = inputs_history[x].ir[1];
			else if(comboBox2->Text == "ir[2]") y = inputs_history[x].ir[2];
			else if(comboBox2->Text == "ir[3]") y = inputs_history[x].ir[3];
			else if(comboBox2->Text == "theta") y = inputs_history[x].theta;
			else y = x % 500;
			chart1->Series[0]->Points->AddXY(x, y);

			if(		comboBox4->Text == "analog[0]") y = inputs_history[x].analog[0];
			else if(comboBox4->Text == "analog[1]") y = inputs_history[x].analog[1];
			else if(comboBox4->Text == "analog[2]") y = inputs_history[x].analog[2];
			else if(comboBox4->Text == "analog[3]") y = inputs_history[x].analog[3];
			else if(comboBox4->Text == "analog[4]") y = inputs_history[x].analog[4];
			else if(comboBox4->Text == "analog[5]") y = inputs_history[x].analog[5];
			else if(comboBox4->Text == "analog[6]") y = inputs_history[x].analog[6];
			else if(comboBox4->Text == "analog[7]") y = inputs_history[x].analog[7];
			else if(comboBox4->Text == "ir[0]") y = inputs_history[x].ir[0];
			else if(comboBox4->Text == "ir[1]") y = inputs_history[x].ir[1];
			else if(comboBox4->Text == "ir[2]") y = inputs_history[x].ir[2];
			else if(comboBox4->Text == "ir[3]") y = inputs_history[x].ir[3];
			else if(comboBox4->Text == "theta") y = inputs_history[x].theta;
			else y = x % 500;
			chart1->Series[1]->Points->AddXY(x, y);
			x++;
		}
		// Keep a constant number of points by removing them from the left
		while(chart1->Series[0]->Points->Count > 2000) chart1->Series[0]->Points->RemoveAt(0);
		while(chart1->Series[1]->Points->Count > 2000) chart1->Series[1]->Points->RemoveAt(0);

		//chart1->Invalidate();
	}

		 
		 
		 
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::InitializeGraphsTab(void)
	{
		// Zoom into the X axis
		//chart1->ChartAreas["ChartArea1"]->AxisX->ScaleView->Zoom(2, 3);

		// Enable range selection and zooming end user interface
		chart1->ChartAreas["ChartArea1"]->CursorX->IsUserEnabled = true;
		chart1->ChartAreas["ChartArea1"]->CursorX->IsUserSelectionEnabled = true;
		chart1->ChartAreas["ChartArea1"]->AxisX->ScaleView->Zoomable = true;
		chart1->ChartAreas["ChartArea1"]->AxisX->ScrollBar->IsPositionedInside = true;
		chart1->ChartAreas["ChartArea1"]->CursorY->IsUserEnabled = true;
		chart1->ChartAreas["ChartArea1"]->CursorY->IsUserSelectionEnabled = true;
		chart1->ChartAreas["ChartArea1"]->AxisY->ScaleView->Zoomable = true;
		chart1->ChartAreas["ChartArea1"]->AxisY->ScrollBar->IsPositionedInside = true;

		// Enable range selection and zooming end user interface
		chart1->ChartAreas["ChartArea2"]->CursorX->IsUserEnabled = true;
		chart1->ChartAreas["ChartArea2"]->CursorX->IsUserSelectionEnabled = true;
		chart1->ChartAreas["ChartArea2"]->AxisX->ScaleView->Zoomable = true;
		chart1->ChartAreas["ChartArea2"]->AxisX->ScrollBar->IsPositionedInside = true;
		chart1->ChartAreas["ChartArea2"]->CursorY->IsUserEnabled = true;
		chart1->ChartAreas["ChartArea2"]->CursorY->IsUserSelectionEnabled = true;
		chart1->ChartAreas["ChartArea2"]->AxisY->ScaleView->Zoomable = true;
		chart1->ChartAreas["ChartArea2"]->AxisY->ScrollBar->IsPositionedInside = true;

		chart1->ChartAreas["ChartArea2"]->AxisX->ScaleView = chart1->ChartAreas["ChartArea1"]->AxisX->ScaleView;

		chart1->Series["Series 1"]->Points->Clear();
		chart1->Series["Series 1"]->IsXValueIndexed=TRUE;
		chart1->Series["Series 1"]->Enabled = TRUE; //FALSE;

		chart1->Series["Series 2"]->Points->Clear();
		chart1->Series["Series 2"]->IsXValueIndexed=TRUE;
		chart1->Series["Series 2"]->Enabled = TRUE; //FALSE;

		comboBox2->Items->Clear();
		comboBox3->Items->Clear();
		comboBox4->Items->Clear();
		comboBox5->Items->Clear();

		comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(24) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", 
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", 
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"
		});

		comboBox3->Items->AddRange(gcnew cli::array< System::Object^  >(24) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", 
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", 
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"
		});

		comboBox4->Items->AddRange(gcnew cli::array< System::Object^  >(24) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", 
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", 
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"
		});

		comboBox5->Items->AddRange(gcnew cli::array< System::Object^  >(24) 
		{
			L"--NONE--", 
			L"analog[0]", L"analog[1]", L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", 
			L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", 
			L"actual_speed[0]", L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", 
			L"x", L"y", L"theta", 
			L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"
		});

		comboBox2->SelectedIndex = 0;
		comboBox3->SelectedIndex = 0;
		comboBox4->SelectedIndex = 0;
		comboBox5->SelectedIndex = 0;
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
			chart1->Series["Series 1"]->Points->RemoveAt(0);
			chart1->Series["Series 2"]->Points->RemoveAt(0);
		}

		y1 = s.inputs->watch[0]; //actual_speed[0]; //ir[0];
		y2 = s.inputs->watch[1]; //ir[2]; //ir[1];
		t = GetTickCount();

		chart1->Series["Series 1"]->Points->AddXY(x,y1);
		chart1->Series["Series 2"]->Points->AddXY(x,y2);
		//chart1->Invalidate();
#endif
	}





}
