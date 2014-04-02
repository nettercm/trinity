#include "stdafx.h"
#include "f1.h"
#include <stdio.h>
#include <windows.h>



namespace robot_ui 
{
	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::parameters_dataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) 
	{
		int r,c;
		float f;
		c = e->ColumnIndex;
		r = e->RowIndex;

		//log("parameters_dataGridView_CellContentClick\n");

		//don't allow a change if this is a group name definition only
		if(Convert::ToInt16(parameters_dataGridView->Rows[r]->Cells[1]->Value)==0) return;

		if(r>=0 && c==5)
		{
			f = (float)(Convert::ToSingle(parameters_dataGridView->Rows[r]->Cells[4]->Value));
			f+=1;
			parameters_dataGridView->Rows[r]->Cells[4]->Value = Convert::ToString(f);
		}
		if(r>=0 && c==6)
		{
			f = (float)(Convert::ToSingle(parameters_dataGridView->Rows[r]->Cells[4]->Value));
			f-=1;
			parameters_dataGridView->Rows[r]->Cells[4]->Value = Convert::ToString(f);
		}
	}


	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	System::Void f1::parameters_dataGridView_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) 
	{
		t_config_value v;
		int r,c,grp,id,i;

		if(!ignore_parameter_changes)
		{
			c = e->ColumnIndex;
			r = e->RowIndex;
			grp = Convert::ToInt32(parameters_dataGridView->Rows[r]->Cells[0]->Value);
			id	= Convert::ToInt32(parameters_dataGridView->Rows[r]->Cells[1]->Value);
			i = cfg_get_index_by_grp_and_id(grp,id);
			log("parameters_dataGridView_CellValueChanged(): ");
			log(Convert::ToString(e->RowIndex) + "," + Convert::ToString(e->ColumnIndex) + " = ");
			switch(config[i].type)
			{
			case U08_VALUE:
				v.u08 = (uint8) Convert::ToUInt16(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString(v.u08) + "\n");
				break;
			case S08_VALUE:
				v.s08 = (sint8) Convert::ToInt16(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString(v.s08) + "\n");
				break;
			case U16_VALUE:
				v.u16 = Convert::ToUInt16(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString(v.u16) + "\n");
				break;
			case S16_VALUE:
				v.s16 = Convert::ToInt16(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString(v.s16) + "\n");
				break;
			case U32_VALUE:
				v.u32 = Convert::ToUInt32(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString((unsigned int)v.u32) + "\n");
				break; 
			case S32_VALUE:
				v.s32 = Convert::ToInt32(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString(v.s32) + "\n");
				break;
			case FLT_VALUE:
				v.f = Convert::ToSingle(parameters_dataGridView->Rows[r]->Cells[4]->Value);
				log(Convert::ToString(v.f) + "\n");
				break;

			}
			log("\n");
			//for some reasone,  issuing commands from here results in some seq # skipping 
			CMD_set_config_value(grp,id, (uint8*)&v);
			CMD_send();
		}
	}




	/******************************************************************************************************************************************
	*
	******************************************************************************************************************************************/
	void f1::InitializeParametersTab(void)
	{
		int i;

		for(i=0; i<=MAX_CFG_ITEMS;i++)
		{
			if(config[i].grp == 255) break;
			parameters_dataGridView->Rows->Add();
			parameters_dataGridView->Rows[i]->Cells[0]->Value = config[i].grp;
			parameters_dataGridView->Rows[i]->Cells[1]->Value = config[i].id;
			if(config[i].id == 0)
			{
				parameters_dataGridView->Rows[i]->Cells[0]->Value = Convert::ToString(config[i].grp) + " - " + gcnew String(config[i].name);
				parameters_dataGridView->Rows[i]->Cells[2]->Value = Convert::ToString(config[i].grp) + " - " + gcnew String(config[i].name);
				parameters_dataGridView->Rows[i]->Cells[0]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[1]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[2]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[3]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[4]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[5]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[6]->Style->BackColor = System::Drawing::Color::Yellow;
				parameters_dataGridView->Rows[i]->Cells[0]->Style->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				parameters_dataGridView->Rows[i]->Cells[1]->Style->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				parameters_dataGridView->Rows[i]->Cells[2]->Style->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				parameters_dataGridView->Rows[i]->Cells[3]->Style->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
				parameters_dataGridView->Rows[i]->Cells[4]->Style->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			}
			else
			{
				parameters_dataGridView->Rows[i]->Cells[2]->Value = gcnew String(config[i].name);
			}
			parameters_dataGridView->Rows[i]->Cells[5]->Value = "+";
			parameters_dataGridView->Rows[i]->Cells[6]->Value = "-";
			switch(config[i].type)
			{
			case U08_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "u08";
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.u08);
				break;
			case S08_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "s08";
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.s08);
				break;
			case U16_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "u16";
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.u16);
				break;
			case S16_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "s16";
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.s16);
				break;
			case U32_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "u32";
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString((unsigned int)config[i].v.u32);
				break; 
			case S32_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "s32";
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.s32);
				break;
			case FLT_VALUE:
				parameters_dataGridView->Rows[i]->Cells[3]->Value = "flt";
				//parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.f);
				parameters_dataGridView->Rows[i]->Cells[4]->Value = Convert::ToString(config_float_only[i].f);
				break;
			} //switch
		} //for
	} //function


	System::Void f1::parameters_btn_write_all_Click(System::Object^  sender, System::EventArgs^  e) 
	{
		int i;

		i=0;
		Application::UseWaitCursor = 1;
		while(config[i].grp != 255)
		{
			if(config[i].id != 0)
			{
				CMD_set_config_value(config[i].grp, config[i].id, (uint8*)&(config[i].v));
				CMD_send();
				Sleep(50);
				Application::DoEvents();
			}
			i++;
		}
		Application::UseWaitCursor = 0;
	}
}