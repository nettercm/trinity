#pragma once

#include "serial.h"
#include "logic.h"
#include "data.h"
#include "kalman.h"
#include "commands.h"
#include "..\robot\config.h"

#include <io.h>
#include <fcntl.h>     /* for _O_TEXT and _O_BINARY */


extern "C" 
{ 
extern volatile unsigned char key; 
extern int loop(void);
extern void show_last_error(char *s);
extern t_inputs inputs_history[200000];
extern int history_index;
}


//#include <atlstr.h>
//#include <msclr/marshal.h>

namespace robot_ui {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;

	/// <summary>
	/// Summary for f1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class f1 : public System::Windows::Forms::Form
	{

	public: delegate void UpdateUI(String ^str);
	public: delegate void UpdateChart(String^ series, double x, double y);
	public: UpdateUI^ UpdateUI_delegate;
	public: UpdateChart^ UpdateChart_delegate; 
	public: UpdateUI^ Update_textBox1_delegate;

	public: 





	private: System::Windows::Forms::DataGridView^  dataGridView1;







	private: System::Windows::Forms::DataVisualization::Charting::Chart^  chart1;

	private: System::Windows::Forms::CheckBox^  checkBox2;
	private: System::Windows::Forms::ComboBox^  comboBox2;
	private: System::Windows::Forms::ComboBox^  comboBox5;
	private: System::Windows::Forms::ComboBox^  comboBox4;
	private: System::Windows::Forms::ComboBox^  comboBox3;







	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Group;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  id;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  name;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Type;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  val;
	private: System::Windows::Forms::DataGridViewButtonColumn^  inc;
	private: System::Windows::Forms::DataGridViewButtonColumn^  dec;
	private: System::Windows::Forms::Button^  btn_write_all;
	private: System::Windows::Forms::Button^  btn_read_all;

































	public: UpdateUI^ Update_textBoxLog_delegate;

	public: void Update_textBox1_method(String ^str)
	{
		//if(textBox1->TextLength > 64000) 	textBox1->Text = textBox1->Text->Substring(32000,textBox1->TextLength - 32000);
		textBox1->AppendText(str);
	}

	public: void Update_textBoxLog_method(String ^str)
	{
		//if(textBoxLog->TextLength > 64000) 	textBoxLog->Text = textBoxLog->Text->Substring(32000,textBoxLog->TextLength - 32000);
		textBoxLog->AppendText(str);
	}

	public: void log(String ^str) { this->Invoke( this->Update_textBoxLog_delegate, str); }
	public: void log(char *str)  { this->Invoke( this->Update_textBoxLog_delegate, gcnew String(str)); }
	public: void term(char *str)  { this->Invoke( this->Update_textBox1_delegate, gcnew String(str)); }
	public: void update_ui(void) { this->Invoke( this->UpdateUI_delegate, gcnew String("")); }

	private: System::Random^	random;

	private: System::Windows::Forms::ComboBox::ObjectCollection^ chartitems;


	public: void UpdateChart_method(String^ series, double x, double y)
	{
		unsigned int t;
		double y1,y2;

		if(x>=500) 
		{
			//chart1->Series[series]->Points->RemoveAt(0);
			chart1->Series["analog[6]"]->Points->RemoveAt(0);
			chart1->Series["analog[7]"]->Points->RemoveAt(0);
		}
		//chart1->Series[series]->Points->AddXY(x,y);
		//ir[0] == NW
		//ir[2] == W (rear)
		y1 = s.inputs->watch[0]; //actual_speed[0]; //ir[0];
		y2 = s.inputs->watch[1]; //ir[2]; //ir[1];
		t = GetTickCount();
		//log(Convert::ToString(t) + ": y = " + Convert::ToString(y2) + "\n");
		chart1->Series["analog[6]"]->Points->AddXY(x,y1);
		chart1->Series["analog[7]"]->Points->AddXY(x,y2);
		//chart1->Invalidate();
	}


	public: void UpdateUI_method(String ^str)
	{
		/*
		if(textBox1->TextLength > 64000)
		{
			textBox1->Text = textBox1->Text->Substring(32000,textBox1->TextLength - 32000);
		}
		//textBox1->Text += str;
		textBox1->AppendText(str);
		*/
		tb_Vbatt->Text = Convert::ToString(((float)s.inputs->vbatt)/1000.0f)+"V";
	}

	private: int ignore_parameter_changes;

	public:
		f1(void)
		{
			int i;
			float f;
			String ^s;

			timeBeginPeriod(1);

			DWORD t1 = timeGetTime();
			Sleep(1);
			DWORD t2 = timeGetTime();

			ignore_parameter_changes = 1;

			random = gcnew Random();

			UpdateUI_delegate = gcnew UpdateUI( this, &f1::UpdateUI_method );
			Update_textBox1_delegate =   gcnew UpdateUI( this, &f1::Update_textBox1_method );
			Update_textBoxLog_delegate = gcnew UpdateUI( this, &f1::Update_textBoxLog_method );
			UpdateChart_delegate = gcnew UpdateChart( this, &f1::UpdateChart_method );

			InitializeComponent();

			
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


			chart1->Series["analog[7]"]->Points->Clear();
			chart1->Series["analog[7]"]->IsXValueIndexed=TRUE;
			chart1->Series["analog[6]"]->Points->Clear();
			chart1->Series["analog[6]"]->IsXValueIndexed=TRUE;

			chart1->Series["analog[6]"]->Enabled = TRUE; //FALSE;
			chart1->Series["analog[7]"]->Enabled = TRUE; //FALSE;

			/*
			chart1->Series->Add("2");
			chart1->Series[2]->ChartArea = L"ChartArea1";
			chart1->Series[2]->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			chart1->Series[2]->IsXValueIndexed = true;
			chart1->Series[2]->Legend = L"Legend1";
			chart1->Series[2]->MarkerSize = 1;
			chart1->Series[2]->Name = L"2d";
			chart1->Series[2]->YValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Int32;

			chart1->Series->Add("3");
			*/

			comboBox3->Items->Clear();
			comboBox3->Items->Add("--- NONE ---");
			comboBox3->Items->Add("ir[0]");
			comboBox3->Items->Add("ir[1]");
			comboBox3->Items->Add("ir[2]");
			comboBox3->Items->Add("ir[3]");
			comboBox3->Items->Add("sonar[0]");
			comboBox3->Items->Add("sonar[1]");
			comboBox3->Items->Add("sonar[2]");
			comboBox3->Items->Add("sonar[3]");

			for(i=0; i<=MAX_CFG_ITEMS;i++)
			{
				if(config[i].grp == 255) break;
				dataGridView1->Rows->Add();
				dataGridView1->Rows[i]->Cells[0]->Value = config[i].grp;
				dataGridView1->Rows[i]->Cells[1]->Value = config[i].id;
				if(config[i].id == 0)
				{
					dataGridView1->Rows[i]->Cells[0]->Value = Convert::ToString(config[i].grp) + " - " + gcnew String(config[i].name);
					dataGridView1->Rows[i]->Cells[2]->Value = Convert::ToString(config[i].grp) + " - " + gcnew String(config[i].name);
					//dataGridView1->Rows[i]->Cells[2]->Style = 
				}
				else
				{
					dataGridView1->Rows[i]->Cells[2]->Value = gcnew String(config[i].name);
				}
				dataGridView1->Rows[i]->Cells[5]->Value = "+";
				dataGridView1->Rows[i]->Cells[6]->Value = "-";
				switch(config[i].type)
				{
				case U08_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "u08";
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.u08);
					break;
				case S08_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "s08";
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.s08);
					break;
				case U16_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "u16";
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.u16);
					break;
				case S16_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "s16";
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.s16);
					break;
				case U32_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "u32";
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString((unsigned int)config[i].v.u32);
					break; 
				case S32_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "s32";
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.s32);
					break;
				case FLT_VALUE:
					dataGridView1->Rows[i]->Cells[3]->Value = "flt";
					//dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config[i].v.f);
					dataGridView1->Rows[i]->Cells[4]->Value = Convert::ToString(config_float_only[i].f);
					break;
				}
			}
			//s = dataGridView1->Rows[0]->Cells[2]->Value->ToString();
			//f = (float)(Convert::ToSingle(s));


			//
			//TODO: Add the constructor code here
			//
			ignore_parameter_changes = 0;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~f1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ComponentModel::BackgroundWorker^  bw1;
	protected: 
	private: System::Windows::Forms::Timer^  t1;




	private: System::Windows::Forms::ComboBox^  comboBox1;
	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TextBox^  tb_Vbatt;
	private: System::Windows::Forms::Label^  label9;








	private: System::Windows::Forms::TabPage^  tabSettings;






	private: System::Windows::Forms::TextBox^  textBox2;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::TabPage^  tabLog;
private: System::Windows::Forms::TextBox^  textBoxLog;






	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle5 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle8 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle6 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle7 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea3 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea4 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series4 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			this->bw1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->t1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabSettings = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->Group = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->id = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->name = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Type = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->val = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->inc = (gcnew System::Windows::Forms::DataGridViewButtonColumn());
			this->dec = (gcnew System::Windows::Forms::DataGridViewButtonColumn());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->comboBox5 = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox4 = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox3 = (gcnew System::Windows::Forms::ComboBox());
			this->comboBox2 = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
			this->chart1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->tabLog = (gcnew System::Windows::Forms::TabPage());
			this->textBoxLog = (gcnew System::Windows::Forms::TextBox());
			this->tb_Vbatt = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->btn_read_all = (gcnew System::Windows::Forms::Button());
			this->btn_write_all = (gcnew System::Windows::Forms::Button());
			this->tabControl1->SuspendLayout();
			this->tabSettings->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->tabPage2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->BeginInit();
			this->tabPage1->SuspendLayout();
			this->tabLog->SuspendLayout();
			this->SuspendLayout();
			// 
			// bw1
			// 
			this->bw1->WorkerReportsProgress = true;
			this->bw1->WorkerSupportsCancellation = true;
			this->bw1->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &f1::bw1_DoWork);
			// 
			// t1
			// 
			this->t1->Tick += gcnew System::EventHandler(this, &f1::t1_Tick);
			// 
			// comboBox1
			// 
			this->comboBox1->FormattingEnabled = true;
			this->comboBox1->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"11"});
			this->comboBox1->Location = System::Drawing::Point(67, 10);
			this->comboBox1->Name = L"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(36, 21);
			this->comboBox1->TabIndex = 1;
			this->comboBox1->Text = L"4";
			this->comboBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::comboBox1_SelectedIndexChanged);
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(109, 12);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(78, 17);
			this->checkBox1->TabIndex = 2;
			this->checkBox1->Text = L"Connected";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &f1::checkBox1_CheckedChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(5, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(56, 13);
			this->label1->TabIndex = 3;
			this->label1->Text = L"COM Port:";
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabSettings);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabLog);
			this->tabControl1->Location = System::Drawing::Point(0, 51);
			this->tabControl1->Margin = System::Windows::Forms::Padding(0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(1098, 484);
			this->tabControl1->TabIndex = 4;
			// 
			// tabSettings
			// 
			this->tabSettings->Controls->Add(this->btn_write_all);
			this->tabSettings->Controls->Add(this->btn_read_all);
			this->tabSettings->Controls->Add(this->dataGridView1);
			this->tabSettings->Location = System::Drawing::Point(4, 22);
			this->tabSettings->Name = L"tabSettings";
			this->tabSettings->Padding = System::Windows::Forms::Padding(3);
			this->tabSettings->Size = System::Drawing::Size(1090, 458);
			this->tabSettings->TabIndex = 2;
			this->tabSettings->Text = L"Parameters";
			this->tabSettings->UseVisualStyleBackColor = true;
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			dataGridViewCellStyle5->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle5->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle5->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle5->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle5->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle5->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->dataGridView1->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle5;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(7) {this->Group, 
				this->id, this->name, this->Type, this->val, this->inc, this->dec});
			dataGridViewCellStyle8->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle8->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle8->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle8->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle8->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle8->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
			this->dataGridView1->DefaultCellStyle = dataGridViewCellStyle8;
			this->dataGridView1->Location = System::Drawing::Point(0, 0);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->dataGridView1->Size = System::Drawing::Size(630, 458);
			this->dataGridView1->TabIndex = 3;
			this->dataGridView1->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::dataGridView1_CellContentClick);
			this->dataGridView1->CellContentDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::dataGridView1_CellContentDoubleClick);
			this->dataGridView1->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::dataGridView1_CellValueChanged);
			// 
			// Group
			// 
			dataGridViewCellStyle6->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			this->Group->DefaultCellStyle = dataGridViewCellStyle6;
			this->Group->HeaderText = L"Group";
			this->Group->Name = L"Group";
			this->Group->ReadOnly = true;
			this->Group->Width = 5;
			// 
			// id
			// 
			this->id->HeaderText = L"ID";
			this->id->Name = L"id";
			this->id->ReadOnly = true;
			this->id->Width = 50;
			// 
			// name
			// 
			dataGridViewCellStyle7->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			this->name->DefaultCellStyle = dataGridViewCellStyle7;
			this->name->HeaderText = L"Name";
			this->name->Name = L"name";
			this->name->ReadOnly = true;
			this->name->Width = 305;
			// 
			// Type
			// 
			this->Type->HeaderText = L"Type";
			this->Type->Name = L"Type";
			this->Type->ReadOnly = true;
			this->Type->Width = 70;
			// 
			// val
			// 
			this->val->HeaderText = L"Value";
			this->val->Name = L"val";
			// 
			// inc
			// 
			this->inc->HeaderText = L"+";
			this->inc->Name = L"inc";
			this->inc->ReadOnly = true;
			this->inc->Width = 20;
			// 
			// dec
			// 
			this->dec->HeaderText = L"-";
			this->dec->Name = L"dec";
			this->dec->ReadOnly = true;
			this->dec->Width = 20;
			// 
			// tabPage2
			// 
			this->tabPage2->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->tabPage2->Controls->Add(this->comboBox5);
			this->tabPage2->Controls->Add(this->comboBox4);
			this->tabPage2->Controls->Add(this->comboBox3);
			this->tabPage2->Controls->Add(this->comboBox2);
			this->tabPage2->Controls->Add(this->checkBox2);
			this->tabPage2->Controls->Add(this->chart1);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(1090, 458);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"Graphs";
			this->tabPage2->UseVisualStyleBackColor = true;
			this->tabPage2->Click += gcnew System::EventHandler(this, &f1::tabPage2_Click);
			// 
			// comboBox5
			// 
			this->comboBox5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->comboBox5->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox5->FormattingEnabled = true;
			this->comboBox5->Items->AddRange(gcnew cli::array< System::Object^  >(7) {L"1", L"2", L"3", L"4", L"5", L"6", L"7"});
			this->comboBox5->Location = System::Drawing::Point(892, 97);
			this->comboBox5->Name = L"comboBox5";
			this->comboBox5->Size = System::Drawing::Size(187, 21);
			this->comboBox5->TabIndex = 4;
			// 
			// comboBox4
			// 
			this->comboBox4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->comboBox4->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox4->FormattingEnabled = true;
			this->comboBox4->Items->AddRange(gcnew cli::array< System::Object^  >(7) {L"1", L"2", L"3", L"4", L"5", L"6", L"7"});
			this->comboBox4->Location = System::Drawing::Point(892, 70);
			this->comboBox4->Name = L"comboBox4";
			this->comboBox4->Size = System::Drawing::Size(187, 21);
			this->comboBox4->TabIndex = 4;
			// 
			// comboBox3
			// 
			this->comboBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->comboBox3->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox3->FormattingEnabled = true;
			this->comboBox3->Items->AddRange(gcnew cli::array< System::Object^  >(7) {L"1", L"2", L"3", L"4", L"5", L"6", L"7"});
			this->comboBox3->Location = System::Drawing::Point(892, 43);
			this->comboBox3->Name = L"comboBox3";
			this->comboBox3->Size = System::Drawing::Size(187, 21);
			this->comboBox3->TabIndex = 4;
			// 
			// comboBox2
			// 
			this->comboBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->comboBox2->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox2->FormattingEnabled = true;
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]"});
			this->comboBox2->Location = System::Drawing::Point(892, 16);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(187, 21);
			this->comboBox2->TabIndex = 4;
			// 
			// checkBox2
			// 
			this->checkBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->checkBox2->AutoSize = true;
			this->checkBox2->Location = System::Drawing::Point(904, 400);
			this->checkBox2->Name = L"checkBox2";
			this->checkBox2->Size = System::Drawing::Size(59, 17);
			this->checkBox2->TabIndex = 2;
			this->checkBox2->Text = L"Enable";
			this->checkBox2->UseVisualStyleBackColor = true;
			this->checkBox2->CheckedChanged += gcnew System::EventHandler(this, &f1::checkBox2_CheckedChanged);
			// 
			// chart1
			// 
			this->chart1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->chart1->AntiAliasing = System::Windows::Forms::DataVisualization::Charting::AntiAliasingStyles::None;
			chartArea3->AxisX->MajorGrid->Enabled = false;
			chartArea3->Name = L"ChartArea1";
			chartArea4->AxisX->MajorGrid->Enabled = false;
			chartArea4->Name = L"ChartArea2";
			this->chart1->ChartAreas->Add(chartArea3);
			this->chart1->ChartAreas->Add(chartArea4);
			legend2->Name = L"Legend1";
			this->chart1->Legends->Add(legend2);
			this->chart1->Location = System::Drawing::Point(-5, -1);
			this->chart1->Margin = System::Windows::Forms::Padding(0);
			this->chart1->Name = L"chart1";
			series3->ChartArea = L"ChartArea1";
			series3->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series3->IsXValueIndexed = true;
			series3->Legend = L"Legend1";
			series3->MarkerSize = 1;
			series3->Name = L"analog[7]";
			series3->YValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Int32;
			series4->ChartArea = L"ChartArea2";
			series4->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series4->IsXValueIndexed = true;
			series4->Legend = L"Legend1";
			series4->Name = L"analog[6]";
			this->chart1->Series->Add(series3);
			this->chart1->Series->Add(series4);
			this->chart1->Size = System::Drawing::Size(916, 458);
			this->chart1->TabIndex = 0;
			this->chart1->Text = L"chart1";
			this->chart1->AxisViewChanged += gcnew System::EventHandler<System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^ >(this, &f1::chart1_AxisViewChanged);
			// 
			// tabPage3
			// 
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(1090, 458);
			this->tabPage3->TabIndex = 4;
			this->tabPage3->Text = L"\"Radar\"";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->textBox1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(1090, 458);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Terminal";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Courier New", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBox1->Location = System::Drawing::Point(-4, 0);
			this->textBox1->Multiline = true;
			this->textBox1->Name = L"textBox1";
			this->textBox1->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBox1->Size = System::Drawing::Size(1094, 462);
			this->textBox1->TabIndex = 4;
			this->textBox1->WordWrap = false;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &f1::textBox1_TextChanged);
			this->textBox1->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &f1::textBox1_KeyDown);
			// 
			// tabLog
			// 
			this->tabLog->Controls->Add(this->textBoxLog);
			this->tabLog->Location = System::Drawing::Point(4, 22);
			this->tabLog->Name = L"tabLog";
			this->tabLog->Padding = System::Windows::Forms::Padding(3);
			this->tabLog->Size = System::Drawing::Size(1090, 458);
			this->tabLog->TabIndex = 3;
			this->tabLog->Text = L"Log";
			this->tabLog->UseVisualStyleBackColor = true;
			// 
			// textBoxLog
			// 
			this->textBoxLog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBoxLog->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->textBoxLog->Location = System::Drawing::Point(0, 0);
			this->textBoxLog->Multiline = true;
			this->textBoxLog->Name = L"textBoxLog";
			this->textBoxLog->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->textBoxLog->Size = System::Drawing::Size(1090, 455);
			this->textBoxLog->TabIndex = 0;
			// 
			// tb_Vbatt
			// 
			this->tb_Vbatt->Location = System::Drawing::Point(259, 9);
			this->tb_Vbatt->Name = L"tb_Vbatt";
			this->tb_Vbatt->ReadOnly = true;
			this->tb_Vbatt->Size = System::Drawing::Size(59, 20);
			this->tb_Vbatt->TabIndex = 0;
			this->tb_Vbatt->Text = L"0.0V";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(221, 13);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(32, 13);
			this->label9->TabIndex = 1;
			this->label9->Text = L"Vbatt";
			this->label9->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(394, 9);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(42, 20);
			this->textBox2->TabIndex = 5;
			this->textBox2->TextChanged += gcnew System::EventHandler(this, &f1::textBox2_TextChanged);
			this->textBox2->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &f1::textBox2_KeyDown);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(361, 13);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(28, 13);
			this->label5->TabIndex = 1;
			this->label5->Text = L"Cmd";
			this->label5->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// btn_read_all
			// 
			this->btn_read_all->Location = System::Drawing::Point(649, 16);
			this->btn_read_all->Name = L"btn_read_all";
			this->btn_read_all->Size = System::Drawing::Size(114, 26);
			this->btn_read_all->TabIndex = 4;
			this->btn_read_all->Text = L"Read All";
			this->btn_read_all->UseVisualStyleBackColor = true;
			// 
			// btn_write_all
			// 
			this->btn_write_all->Location = System::Drawing::Point(801, 16);
			this->btn_write_all->Name = L"btn_write_all";
			this->btn_write_all->Size = System::Drawing::Size(114, 26);
			this->btn_write_all->TabIndex = 4;
			this->btn_write_all->Text = L"Write All";
			this->btn_write_all->UseVisualStyleBackColor = true;
			// 
			// f1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1096, 534);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->tb_Vbatt);
			this->Name = L"f1";
			this->Text = L"Robot UI";
			this->Load += gcnew System::EventHandler(this, &f1::f1_Load);
			this->tabControl1->ResumeLayout(false);
			this->tabSettings->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->EndInit();
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabLog->ResumeLayout(false);
			this->tabLog->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void f1_Load(System::Object^  sender, System::EventArgs^  e) 
	{
		//p=serial_init("\\\\.\\COM11",115200);
		bw1->RunWorkerAsync();
	}

	private: System::Void bw1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e) 
	{
		static int x=0;
		int skip = 5;
		unsigned int t;
		double y;
		t_inputs *inputs;
		int result = 0;
		data_init();

		while(1)
		{
			if(s.p == INVALID_HANDLE_VALUE)
			{
				Sleep(20);
				//inputs_history[history_index].analog[0] = history_index;
				//history_index++;
				//log(".");
			}
			else
			{
				result = loop();
				if(result)
				{
					inputs = s.inputs;
					inputs_history[history_index] = *inputs;
					history_index++;
					//y = s.inputs->analog[7]; //sonar[0]; //analog[7];
					//t = GetTickCount();
					//log(Convert::ToString(t) + ": y = " + Convert::ToString(y) + " (bw1)\n");
					//this->Invoke( this->UpdateChart_delegate, "analog[7]", x, 0);
					update_ui();
					x++;
				}

				//skip--; if(skip==0) { term(s.msg); skip=5;}
			}
		}
	}


	private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
	{
		static HANDLE h;
		BOOL result;
		char port[30];
		System::String ^str;
		str = comboBox1->Text;
		char* str2 = (char*)(void*)Marshal::StringToHGlobalAnsi(str);
		//printf(str2);

		sprintf(port,"\\\\.\\COM%s",str2);
		sprintf(s.port,"\\\\.\\COM%s",str2);

		if(checkBox1->Checked)
		{
			log("Opening serial port COM" + str + "\r\n" );
			s.p = serial_init(port,115200,ONESTOPBIT);
			h = s.p;
			printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
			if(s.p == INVALID_HANDLE_VALUE) 
			{
				log("Unable to open the serial port\r\n");
			}
			else
			{
				//t1->Enabled = TRUE;
			}
		}
		else
		{
			sprintf(s.port,"");
			printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
			if(s.p != INVALID_HANDLE_VALUE)
			{
				s.p = INVALID_HANDLE_VALUE;
				Sleep(100);
				sprintf(s.port,"");
				log("Closing the serial port\r\n");
				result = CloseHandle(h); if(!result) show_last_error("CloseHandle()");
				printf("h,s.p:  0x%08x, 0x%08x\n",h,s.p);
				t1->Enabled = FALSE;
				checkBox2->Checked = false;
			}
		}

		Marshal::FreeHGlobal(System::IntPtr((void*)str2));
	}



private: System::Void tB_NW_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void tabPage2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void tabPage2_Paint(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void tb_SE_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void comboBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		 }
private: System::Void label9_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void textBox2_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) 
		 {
			 e->Handled = true;
			 log(e->KeyData.ToString() + "\n");
			 textBox2->Text = "";
			 key = e->KeyValue;

		 }


private: System::Void dataGridView1_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) 
		 {
			 int r,c;
			 float f;
			 c = e->ColumnIndex;
			 r = e->RowIndex;

			 //log("dataGridView1_CellContentClick\n");

			 //don't allow a change if this is a group name definition only
			 if(Convert::ToInt16(dataGridView1->Rows[r]->Cells[1]->Value)==0) return;

			 if(r>=0 && c==5)
			 {
				 f = (float)(Convert::ToSingle(dataGridView1->Rows[r]->Cells[4]->Value));
				 f+=1;
				 dataGridView1->Rows[r]->Cells[4]->Value = Convert::ToString(f);
			 }
			 if(r>=0 && c==6)
			 {
				 f = (float)(Convert::ToSingle(dataGridView1->Rows[r]->Cells[4]->Value));
				 f-=1;
				 dataGridView1->Rows[r]->Cells[4]->Value = Convert::ToString(f);
			 }
		 }


private: System::Void dataGridView1_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) 
		 {
			 t_config_value v;
			 int r,c,grp,id,i;

			 if(!ignore_parameter_changes)
			 {
				c = e->ColumnIndex;
				r = e->RowIndex;
				grp = Convert::ToInt32(dataGridView1->Rows[r]->Cells[0]->Value);
				id	= Convert::ToInt32(dataGridView1->Rows[r]->Cells[1]->Value);
				i = cfg_get_index_by_grp_and_id(grp,id);
				log("dataGridView1_CellValueChanged(): ");
				log(Convert::ToString(e->RowIndex) + "," + Convert::ToString(e->ColumnIndex) + " = ");
				switch(config[i].type)
				{
				case U08_VALUE:
					v.u08 = (uint8) Convert::ToUInt16(dataGridView1->Rows[r]->Cells[4]->Value);
					log(Convert::ToString(v.u08) + "\n");
					break;
				case S08_VALUE:
					v.s08 = (sint8) Convert::ToInt16(dataGridView1->Rows[r]->Cells[4]->Value);
					break;
				case U16_VALUE:
					v.u16 = Convert::ToUInt16(dataGridView1->Rows[r]->Cells[4]->Value);
					break;
				case S16_VALUE:
					v.s16 = Convert::ToInt16(dataGridView1->Rows[r]->Cells[4]->Value);
					log(Convert::ToString(v.s16));
					break;
				case U32_VALUE:
					v.u32 = Convert::ToUInt32(dataGridView1->Rows[r]->Cells[4]->Value);
					break; 
				case S32_VALUE:
					v.s32 = Convert::ToInt32(dataGridView1->Rows[r]->Cells[4]->Value);
					break;
				case FLT_VALUE:
					v.f = Convert::ToSingle(dataGridView1->Rows[r]->Cells[4]->Value);
					break;

				}
				log("\n");
				//for some reasone,  issuing commands from here results in some seq # skipping 
				CMD_set_config_value(grp,id, (uint8*)&v);
				CMD_send();
			}
		 }



private: System::Void t1_Tick(System::Object^  sender, System::EventArgs^  e) 
		{
			static int x=0;
			static int y=0;

			while(x<history_index)
			{
				chart1->Series[0]->Points->AddXY(x, inputs_history[x].analog[2]);
				chart1->Series[1]->Points->AddXY(x, inputs_history[x].ir[1]);
				x++;
			}
			// Keep a constant number of points by removing them from the left
			while(chart1->Series[0]->Points->Count > 2000) chart1->Series[0]->Points->RemoveAt(0);
			while(chart1->Series[1]->Points->Count > 2000) chart1->Series[1]->Points->RemoveAt(0);

			/*
			for(int pointNumber = 0; pointNumber <	5; pointNumber++)
			{
				chart1->Series[0]->Points->AddXY(x, random->Next(0, 200));
				chart1->Series[1]->Points->AddXY(x, random->Next(200, 400));
				chart1->Series[2]->Points->AddXY(x, random->Next(400, 600));
				x++;
			}

			// Keep a constant number of points by removing them from the left
			while(chart1->Series[0]->Points->Count > 500) chart1->Series[0]->Points->RemoveAt(0);
			while(chart1->Series[1]->Points->Count > 500) chart1->Series[1]->Points->RemoveAt(0);
			while(chart1->Series[2]->Points->Count > 500) chart1->Series[2]->Points->RemoveAt(0);
	
				// Adjust X axis scale
				//chart1->ChartAreas["Default"]->AxisX->Minimum = pointIndex - numberOfPointsAfterRemoval;
				//chart1->ChartAreas["Default"]->AxisX->Maximum = chart1->ChartAreas["Default"]->AxisX.Minimum + numberOfPointsInChart;
			// Redraw chart
			chart1->Invalidate();
			*/
		}


private: System::Void checkedListBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void checkedListBox1_SelectedValueChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void dataGridView1_CellContentDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if(checkBox2->Checked) t1->Enabled = TRUE;
			 else t1->Enabled = FALSE;
		 }
private: System::Void chart1_AxisViewChanged(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^  e) 
		 {
			 if(e->ChartArea->Name == "ChartArea1")
			 {
				 //chart1->ChartAreas["ChartArea2"]->AxisX->ScaleView = chart1->ChartAreas["ChartArea1"]->AxisX->ScaleView;
			 }
		 }
};


}

