#pragma once

#include "serial.h"
#include "logic.h"
#include "data.h"
#include "kalman.h"
#include "commands.h"
#include "..\robot\config.h"

#include <io.h>
#include <fcntl.h>     /* for _O_TEXT and _O_BINARY */
#include <math.h>


extern "C" 
{ 
	extern float PI;;
	extern volatile unsigned int key; 
	extern int loop(void);
	extern void show_last_error(char *s);
	extern volatile t_inputs inputs_history[200000];
	extern volatile int history_index;
	extern int update_interval;
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
	public: f1(void);

	private: int ignore_parameter_changes;
	public: Graphics^ g;

	public: delegate void UpdateUI(String ^str);
	public: delegate void UpdateChart(String^ series, double x, double y);
	public: UpdateUI^ UpdateUI_delegate;
	public: UpdateChart^ UpdateChart_delegate; 
	public: UpdateUI^ Update_textBox1_delegate;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::ComboBox^  cB_Rate;
	private: System::Windows::Forms::Button^  btn_radar_clear;
	private: System::Windows::Forms::CheckBox^  cb_radar_enable_updates;
	private: System::Windows::Forms::CheckBox^  cb_radar_use_lines;
	public: 

	public: UpdateUI^ Update_textBoxLog_delegate;

	public: void UpdateChart_method(String^ series, double x, double y);
	public: void UpdateUI_method(String ^str);
	public: void f1::Update_textBoxLog_method(String ^str);
	public: void f1::Update_textBox1_method(String ^str);

	public: void InitializeParametersTab(void);
	public: void UpdateRadar(float theta, int measurement);
	public: void DrawGrid(Graphics ^g);
	public: void InitializeGraphsTab(void);

	private: System::Void t1_Tick(System::Object^  sender, System::EventArgs^  e); 
	private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void bw1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e); 
	private: System::Void f1_Load(System::Object^  sender, System::EventArgs^  e); 

	public: void log(String ^str) { this->Invoke( this->Update_textBoxLog_delegate, str); }
	public: void log(char *str)  { this->Invoke( this->Update_textBoxLog_delegate, gcnew String(str)); }
	public: void term(char *str)  { this->Invoke( this->Update_textBox1_delegate, gcnew String(str)); }
	public: void update_ui(void) { this->Invoke( this->UpdateUI_delegate, gcnew String("")); }

	private: System::Void dataGridView1_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;
	private: System::Void dataGridView1_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;


			  
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
	public: System::Windows::Forms::Panel^  panel1;
	private: 

	private: System::Windows::Forms::CheckBox^  cb_show_ir_far_north;
	private: System::Windows::Forms::CheckBox^  cb_show_ir_north;
	private: System::Windows::Forms::Button^  btn_stop_scan;
	private: System::Windows::Forms::Button^  btn_start_scan;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  txt_scan_range;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  txt_speed;
	private: System::Windows::Forms::Button^  btn_estop;
	private: System::Windows::Forms::Button^  btn_up;
	private: System::Windows::Forms::Button^  btn_down;
	private: System::Windows::Forms::Button^  btn_left;
	private: System::Windows::Forms::Button^  btn_right;
	private: System::Windows::Forms::Button^  btn_stop;
	private: System::Windows::Forms::Timer^  radar_timer;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ComboBox^  cB_beh;
	private: System::Windows::Forms::ComboBox^  cb_state;
	private: System::Windows::Forms::Button^  btn_start_beh;
	private: System::Windows::Forms::Button^  btn_stop_beh;




	private: System::Random^	random;
	private: System::Windows::Forms::ComboBox::ObjectCollection^ chartitems;

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
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea2 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(f1::typeid));
			this->bw1 = (gcnew System::ComponentModel::BackgroundWorker());
			this->t1 = (gcnew System::Windows::Forms::Timer(this->components));
			this->comboBox1 = (gcnew System::Windows::Forms::ComboBox());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabSettings = (gcnew System::Windows::Forms::TabPage());
			this->btn_write_all = (gcnew System::Windows::Forms::Button());
			this->btn_read_all = (gcnew System::Windows::Forms::Button());
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
			this->btn_stop_scan = (gcnew System::Windows::Forms::Button());
			this->btn_radar_clear = (gcnew System::Windows::Forms::Button());
			this->btn_start_scan = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txt_speed = (gcnew System::Windows::Forms::TextBox());
			this->txt_scan_range = (gcnew System::Windows::Forms::TextBox());
			this->cb_radar_enable_updates = (gcnew System::Windows::Forms::CheckBox());
			this->cb_show_ir_far_north = (gcnew System::Windows::Forms::CheckBox());
			this->cb_show_ir_north = (gcnew System::Windows::Forms::CheckBox());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->tabLog = (gcnew System::Windows::Forms::TabPage());
			this->textBoxLog = (gcnew System::Windows::Forms::TextBox());
			this->tb_Vbatt = (gcnew System::Windows::Forms::TextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->btn_estop = (gcnew System::Windows::Forms::Button());
			this->btn_up = (gcnew System::Windows::Forms::Button());
			this->btn_down = (gcnew System::Windows::Forms::Button());
			this->btn_left = (gcnew System::Windows::Forms::Button());
			this->btn_right = (gcnew System::Windows::Forms::Button());
			this->btn_stop = (gcnew System::Windows::Forms::Button());
			this->radar_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->cB_beh = (gcnew System::Windows::Forms::ComboBox());
			this->cb_state = (gcnew System::Windows::Forms::ComboBox());
			this->btn_start_beh = (gcnew System::Windows::Forms::Button());
			this->btn_stop_beh = (gcnew System::Windows::Forms::Button());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->cB_Rate = (gcnew System::Windows::Forms::ComboBox());
			this->cb_radar_use_lines = (gcnew System::Windows::Forms::CheckBox());
			this->tabControl1->SuspendLayout();
			this->tabSettings->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->tabPage2->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->chart1))->BeginInit();
			this->tabPage3->SuspendLayout();
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
			this->tabControl1->Size = System::Drawing::Size(1098, 657);
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
			this->tabSettings->Size = System::Drawing::Size(1090, 631);
			this->tabSettings->TabIndex = 2;
			this->tabSettings->Text = L"Parameters";
			this->tabSettings->UseVisualStyleBackColor = true;
			// 
			// btn_write_all
			// 
			this->btn_write_all->Location = System::Drawing::Point(801, 16);
			this->btn_write_all->Name = L"btn_write_all";
			this->btn_write_all->Size = System::Drawing::Size(114, 26);
			this->btn_write_all->TabIndex = 4;
			this->btn_write_all->Text = L"Write All";
			this->btn_write_all->UseVisualStyleBackColor = true;
			this->btn_write_all->Click += gcnew System::EventHandler(this, &f1::btn_write_all_Click);
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
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->dataGridView1->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(7) {this->Group, 
				this->id, this->name, this->Type, this->val, this->inc, this->dec});
			dataGridViewCellStyle4->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle4->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle4->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle4->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle4->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle4->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
			this->dataGridView1->DefaultCellStyle = dataGridViewCellStyle4;
			this->dataGridView1->Location = System::Drawing::Point(0, 0);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->dataGridView1->Size = System::Drawing::Size(630, 617);
			this->dataGridView1->TabIndex = 3;
			this->dataGridView1->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::dataGridView1_CellContentClick);
			this->dataGridView1->CellContentDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::dataGridView1_CellContentDoubleClick);
			this->dataGridView1->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::dataGridView1_CellValueChanged);
			// 
			// Group
			// 
			dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			this->Group->DefaultCellStyle = dataGridViewCellStyle2;
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
			dataGridViewCellStyle3->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			this->name->DefaultCellStyle = dataGridViewCellStyle3;
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
			this->tabPage2->Size = System::Drawing::Size(1090, 631);
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
			this->comboBox5->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->comboBox5->Location = System::Drawing::Point(892, 337);
			this->comboBox5->Name = L"comboBox5";
			this->comboBox5->Size = System::Drawing::Size(187, 21);
			this->comboBox5->TabIndex = 4;
			// 
			// comboBox4
			// 
			this->comboBox4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->comboBox4->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox4->FormattingEnabled = true;
			this->comboBox4->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->comboBox4->Location = System::Drawing::Point(892, 310);
			this->comboBox4->Name = L"comboBox4";
			this->comboBox4->Size = System::Drawing::Size(187, 21);
			this->comboBox4->TabIndex = 4;
			// 
			// comboBox3
			// 
			this->comboBox3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->comboBox3->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->comboBox3->FormattingEnabled = true;
			this->comboBox3->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
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
			this->comboBox2->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->comboBox2->Location = System::Drawing::Point(892, 16);
			this->comboBox2->Name = L"comboBox2";
			this->comboBox2->Size = System::Drawing::Size(187, 21);
			this->comboBox2->TabIndex = 4;
			this->comboBox2->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::comboBox2_SelectedIndexChanged);
			// 
			// checkBox2
			// 
			this->checkBox2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->checkBox2->AutoSize = true;
			this->checkBox2->Location = System::Drawing::Point(1023, 592);
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
			this->chart1->BorderlineColor = System::Drawing::Color::RoyalBlue;
			chartArea1->AxisX->MajorGrid->Enabled = false;
			chartArea1->Name = L"ChartArea1";
			chartArea2->AxisX->MajorGrid->Enabled = false;
			chartArea2->Name = L"ChartArea2";
			this->chart1->ChartAreas->Add(chartArea1);
			this->chart1->ChartAreas->Add(chartArea2);
			legend1->Name = L"Legend1";
			this->chart1->Legends->Add(legend1);
			this->chart1->Location = System::Drawing::Point(-5, -1);
			this->chart1->Margin = System::Windows::Forms::Padding(0);
			this->chart1->Name = L"chart1";
			series1->ChartArea = L"ChartArea1";
			series1->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series1->IsXValueIndexed = true;
			series1->Legend = L"Legend1";
			series1->MarkerSize = 1;
			series1->Name = L"Series 1";
			series1->YValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Int32;
			series2->ChartArea = L"ChartArea2";
			series2->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series2->IsXValueIndexed = true;
			series2->Legend = L"Legend1";
			series2->Name = L"Series 2";
			this->chart1->Series->Add(series1);
			this->chart1->Series->Add(series2);
			this->chart1->Size = System::Drawing::Size(894, 621);
			this->chart1->TabIndex = 0;
			this->chart1->Text = L"chart1";
			this->chart1->AxisViewChanged += gcnew System::EventHandler<System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^ >(this, &f1::chart1_AxisViewChanged);
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->btn_stop_scan);
			this->tabPage3->Controls->Add(this->btn_radar_clear);
			this->tabPage3->Controls->Add(this->btn_start_scan);
			this->tabPage3->Controls->Add(this->label3);
			this->tabPage3->Controls->Add(this->label2);
			this->tabPage3->Controls->Add(this->txt_speed);
			this->tabPage3->Controls->Add(this->txt_scan_range);
			this->tabPage3->Controls->Add(this->cb_radar_enable_updates);
			this->tabPage3->Controls->Add(this->cb_radar_use_lines);
			this->tabPage3->Controls->Add(this->cb_show_ir_far_north);
			this->tabPage3->Controls->Add(this->cb_show_ir_north);
			this->tabPage3->Controls->Add(this->panel1);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(1090, 631);
			this->tabPage3->TabIndex = 4;
			this->tabPage3->Text = L"\"Radar\"";
			this->tabPage3->UseVisualStyleBackColor = true;
			this->tabPage3->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &f1::tabPage3_Paint);
			// 
			// btn_stop_scan
			// 
			this->btn_stop_scan->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_stop_scan->Location = System::Drawing::Point(1006, 63);
			this->btn_stop_scan->Name = L"btn_stop_scan";
			this->btn_stop_scan->Size = System::Drawing::Size(67, 23);
			this->btn_stop_scan->TabIndex = 4;
			this->btn_stop_scan->Text = L"Stop";
			this->btn_stop_scan->UseVisualStyleBackColor = true;
			this->btn_stop_scan->Click += gcnew System::EventHandler(this, &f1::btn_stop_scan_Click);
			// 
			// btn_radar_clear
			// 
			this->btn_radar_clear->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_radar_clear->Location = System::Drawing::Point(932, 278);
			this->btn_radar_clear->Name = L"btn_radar_clear";
			this->btn_radar_clear->Size = System::Drawing::Size(100, 23);
			this->btn_radar_clear->TabIndex = 4;
			this->btn_radar_clear->Text = L"Clear";
			this->btn_radar_clear->UseVisualStyleBackColor = true;
			this->btn_radar_clear->Click += gcnew System::EventHandler(this, &f1::btn_start_scan_Click);
			// 
			// btn_start_scan
			// 
			this->btn_start_scan->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_start_scan->Location = System::Drawing::Point(933, 63);
			this->btn_start_scan->Name = L"btn_start_scan";
			this->btn_start_scan->Size = System::Drawing::Size(67, 23);
			this->btn_start_scan->TabIndex = 4;
			this->btn_start_scan->Text = L"Start";
			this->btn_start_scan->UseVisualStyleBackColor = true;
			this->btn_start_scan->Click += gcnew System::EventHandler(this, &f1::btn_start_scan_Click);
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(958, 40);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(41, 13);
			this->label3->TabIndex = 3;
			this->label3->Text = L"Speed:";
			this->label3->Click += gcnew System::EventHandler(this, &f1::label3_Click);
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(929, 14);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(70, 13);
			this->label2->TabIndex = 3;
			this->label2->Text = L"Scan Range:";
			// 
			// txt_speed
			// 
			this->txt_speed->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->txt_speed->Location = System::Drawing::Point(1006, 37);
			this->txt_speed->Name = L"txt_speed";
			this->txt_speed->Size = System::Drawing::Size(67, 20);
			this->txt_speed->TabIndex = 2;
			this->txt_speed->Text = L"90";
			this->txt_speed->TextChanged += gcnew System::EventHandler(this, &f1::textBox3_TextChanged);
			// 
			// txt_scan_range
			// 
			this->txt_scan_range->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->txt_scan_range->Location = System::Drawing::Point(1006, 11);
			this->txt_scan_range->Name = L"txt_scan_range";
			this->txt_scan_range->Size = System::Drawing::Size(67, 20);
			this->txt_scan_range->TabIndex = 2;
			this->txt_scan_range->Text = L"90";
			// 
			// cb_radar_enable_updates
			// 
			this->cb_radar_enable_updates->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->cb_radar_enable_updates->AutoSize = true;
			this->cb_radar_enable_updates->Location = System::Drawing::Point(932, 255);
			this->cb_radar_enable_updates->Name = L"cb_radar_enable_updates";
			this->cb_radar_enable_updates->Size = System::Drawing::Size(100, 17);
			this->cb_radar_enable_updates->TabIndex = 1;
			this->cb_radar_enable_updates->Text = L"Enable updates";
			this->cb_radar_enable_updates->UseVisualStyleBackColor = true;
			// 
			// cb_show_ir_far_north
			// 
			this->cb_show_ir_far_north->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->cb_show_ir_far_north->AutoSize = true;
			this->cb_show_ir_far_north->Location = System::Drawing::Point(933, 159);
			this->cb_show_ir_far_north->Name = L"cb_show_ir_far_north";
			this->cb_show_ir_far_north->Size = System::Drawing::Size(84, 17);
			this->cb_show_ir_far_north->TabIndex = 1;
			this->cb_show_ir_far_north->Text = L"IR Far North";
			this->cb_show_ir_far_north->UseVisualStyleBackColor = true;
			// 
			// cb_show_ir_north
			// 
			this->cb_show_ir_north->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->cb_show_ir_north->AutoSize = true;
			this->cb_show_ir_north->Location = System::Drawing::Point(933, 136);
			this->cb_show_ir_north->Name = L"cb_show_ir_north";
			this->cb_show_ir_north->Size = System::Drawing::Size(66, 17);
			this->cb_show_ir_north->TabIndex = 1;
			this->cb_show_ir_north->Text = L"IR North";
			this->cb_show_ir_north->UseVisualStyleBackColor = true;
			// 
			// panel1
			// 
			this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			this->panel1->Location = System::Drawing::Point(-4, 0);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(914, 631);
			this->panel1->TabIndex = 0;
			this->panel1->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &f1::panel1_Paint);
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->textBox1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(1090, 631);
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
			this->textBox1->Size = System::Drawing::Size(1094, 600);
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
			this->tabLog->Size = System::Drawing::Size(1090, 631);
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
			this->textBoxLog->Size = System::Drawing::Size(1090, 617);
			this->textBoxLog->TabIndex = 0;
			// 
			// tb_Vbatt
			// 
			this->tb_Vbatt->Location = System::Drawing::Point(275, 21);
			this->tb_Vbatt->Name = L"tb_Vbatt";
			this->tb_Vbatt->ReadOnly = true;
			this->tb_Vbatt->Size = System::Drawing::Size(59, 20);
			this->tb_Vbatt->TabIndex = 0;
			this->tb_Vbatt->Text = L"0.0V";
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(272, 5);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(43, 13);
			this->label9->TabIndex = 1;
			this->label9->Text = L"Battery:";
			this->label9->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(524, 23);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(77, 20);
			this->textBox2->TabIndex = 5;
			this->textBox2->TextChanged += gcnew System::EventHandler(this, &f1::textBox2_TextChanged);
			this->textBox2->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &f1::textBox2_KeyDown);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(521, 5);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(80, 13);
			this->label5->TabIndex = 1;
			this->label5->Text = L"Keyb. Shortcut:";
			this->label5->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// btn_estop
			// 
			this->btn_estop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_estop->BackColor = System::Drawing::Color::Red;
			this->btn_estop->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"btn_estop.BackgroundImage")));
			this->btn_estop->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->btn_estop->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->btn_estop->Location = System::Drawing::Point(1024, 5);
			this->btn_estop->Name = L"btn_estop";
			this->btn_estop->Size = System::Drawing::Size(67, 61);
			this->btn_estop->TabIndex = 6;
			this->btn_estop->UseVisualStyleBackColor = false;
			this->btn_estop->Click += gcnew System::EventHandler(this, &f1::btn_estop_Click);
			// 
			// btn_up
			// 
			this->btn_up->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_up->Location = System::Drawing::Point(948, 1);
			this->btn_up->Name = L"btn_up";
			this->btn_up->Size = System::Drawing::Size(25, 21);
			this->btn_up->TabIndex = 7;
			this->btn_up->Text = L"u";
			this->btn_up->UseVisualStyleBackColor = true;
			// 
			// btn_down
			// 
			this->btn_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_down->Location = System::Drawing::Point(948, 47);
			this->btn_down->Name = L"btn_down";
			this->btn_down->Size = System::Drawing::Size(25, 21);
			this->btn_down->TabIndex = 7;
			this->btn_down->Text = L"d";
			this->btn_down->UseVisualStyleBackColor = true;
			// 
			// btn_left
			// 
			this->btn_left->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_left->Location = System::Drawing::Point(920, 24);
			this->btn_left->Name = L"btn_left";
			this->btn_left->Size = System::Drawing::Size(25, 21);
			this->btn_left->TabIndex = 7;
			this->btn_left->Text = L"<";
			this->btn_left->UseVisualStyleBackColor = true;
			// 
			// btn_right
			// 
			this->btn_right->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_right->Location = System::Drawing::Point(976, 24);
			this->btn_right->Name = L"btn_right";
			this->btn_right->Size = System::Drawing::Size(25, 21);
			this->btn_right->TabIndex = 7;
			this->btn_right->Text = L">";
			this->btn_right->UseVisualStyleBackColor = true;
			// 
			// btn_stop
			// 
			this->btn_stop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->btn_stop->Location = System::Drawing::Point(948, 24);
			this->btn_stop->Name = L"btn_stop";
			this->btn_stop->Size = System::Drawing::Size(25, 21);
			this->btn_stop->TabIndex = 7;
			this->btn_stop->Text = L"s";
			this->btn_stop->UseVisualStyleBackColor = true;
			this->btn_stop->Click += gcnew System::EventHandler(this, &f1::btn_stop_Click);
			// 
			// radar_timer
			// 
			this->radar_timer->Enabled = true;
			this->radar_timer->Interval = 30;
			this->radar_timer->Tick += gcnew System::EventHandler(this, &f1::radar_timer_Tick);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(624, 5);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(229, 13);
			this->label4->TabIndex = 8;
			this->label4->Text = L"Behavior:              Initial State:          Command:";
			// 
			// cB_beh
			// 
			this->cB_beh->FormattingEnabled = true;
			this->cB_beh->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", 
				L"10", L"11", L"12", L"13", L"14", L"15", L"16"});
			this->cB_beh->Location = System::Drawing::Point(625, 24);
			this->cB_beh->Name = L"cB_beh";
			this->cB_beh->Size = System::Drawing::Size(61, 21);
			this->cB_beh->TabIndex = 9;
			this->cB_beh->Text = L"1";
			this->cB_beh->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::cB_beh_SelectedIndexChanged);
			// 
			// cb_state
			// 
			this->cb_state->FormattingEnabled = true;
			this->cb_state->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", 
				L"9", L"10", L"11", L"12", L"13", L"14", L"15", L"16"});
			this->cb_state->Location = System::Drawing::Point(712, 24);
			this->cb_state->Name = L"cb_state";
			this->cb_state->Size = System::Drawing::Size(61, 21);
			this->cb_state->TabIndex = 9;
			this->cb_state->Text = L"1";
			// 
			// btn_start_beh
			// 
			this->btn_start_beh->Location = System::Drawing::Point(798, 21);
			this->btn_start_beh->Name = L"btn_start_beh";
			this->btn_start_beh->Size = System::Drawing::Size(42, 23);
			this->btn_start_beh->TabIndex = 10;
			this->btn_start_beh->Text = L"Start";
			this->btn_start_beh->UseVisualStyleBackColor = true;
			this->btn_start_beh->Click += gcnew System::EventHandler(this, &f1::btn_start_beh_Click);
			// 
			// btn_stop_beh
			// 
			this->btn_stop_beh->Location = System::Drawing::Point(846, 21);
			this->btn_stop_beh->Name = L"btn_stop_beh";
			this->btn_stop_beh->Size = System::Drawing::Size(42, 23);
			this->btn_stop_beh->TabIndex = 10;
			this->btn_stop_beh->Text = L"Stop";
			this->btn_stop_beh->UseVisualStyleBackColor = true;
			this->btn_stop_beh->Click += gcnew System::EventHandler(this, &f1::btn_stop_beh_Click);
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(361, 5);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(33, 13);
			this->label6->TabIndex = 1;
			this->label6->Text = L"Rate:";
			this->label6->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// cB_Rate
			// 
			this->cB_Rate->FormattingEnabled = true;
			this->cB_Rate->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"0", L"10", L"100", L"1000", L"2000"});
			this->cB_Rate->Location = System::Drawing::Point(364, 22);
			this->cB_Rate->Name = L"cB_Rate";
			this->cB_Rate->Size = System::Drawing::Size(61, 21);
			this->cB_Rate->TabIndex = 9;
			this->cB_Rate->Text = L"1";
			this->cB_Rate->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::cB_beh_SelectedIndexChanged);
			// 
			// cb_radar_use_lines
			// 
			this->cb_radar_use_lines->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->cb_radar_use_lines->AutoSize = true;
			this->cb_radar_use_lines->Location = System::Drawing::Point(932, 210);
			this->cb_radar_use_lines->Name = L"cb_radar_use_lines";
			this->cb_radar_use_lines->Size = System::Drawing::Size(69, 17);
			this->cb_radar_use_lines->TabIndex = 1;
			this->cb_radar_use_lines->Text = L"Use lines";
			this->cb_radar_use_lines->UseVisualStyleBackColor = true;
			// 
			// f1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1096, 707);
			this->Controls->Add(this->btn_stop_beh);
			this->Controls->Add(this->btn_start_beh);
			this->Controls->Add(this->cb_state);
			this->Controls->Add(this->cB_Rate);
			this->Controls->Add(this->cB_beh);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->btn_right);
			this->Controls->Add(this->btn_left);
			this->Controls->Add(this->btn_down);
			this->Controls->Add(this->btn_stop);
			this->Controls->Add(this->btn_up);
			this->Controls->Add(this->btn_estop);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->checkBox1);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label6);
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
			this->tabPage3->ResumeLayout(false);
			this->tabPage3->PerformLayout();
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabLog->ResumeLayout(false);
			this->tabLog->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion




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

private: System::Void textBox2_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;

private: System::Void checkedListBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void checkedListBox1_SelectedValueChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void dataGridView1_CellContentDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void tabPage3_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) 
	{
	}
private: System::Void chart1_AxisViewChanged(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^  e) ;
private: System::Void checkBox2_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void panel1_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) ;
private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void btn_start_scan_Click(System::Object^  sender, System::EventArgs^  e) {
			 panel1->Invalidate();
		 }
private: System::Void btn_stop_scan_Click(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void radar_timer_Tick(System::Object^  sender, System::EventArgs^  e); 

private: System::Void btn_stop_Click(System::Object^  sender, System::EventArgs^  e) 
		{

		}
private: System::Void comboBox2_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void cB_beh_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 update_interval = Convert::ToInt32(cB_Rate->Text);
		 }

private: System::Void btn_start_beh_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 CMD_set_behavior_state(Convert::ToByte(cB_beh->Text), Convert::ToByte(cb_state->Text));
			 CMD_send();
		 }

private: System::Void btn_stop_beh_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 CMD_set_behavior_state(Convert::ToByte(cB_beh->Text), 0);
			 CMD_send();
		 }
private: System::Void btn_estop_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void btn_write_all_Click(System::Object^  sender, System::EventArgs^  e) 
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
};


}

