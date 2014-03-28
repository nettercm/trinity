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
	private: System::Windows::Forms::Label^  main_lbl_rate;
	public: 

	private: System::Windows::Forms::ComboBox^  main_comboBox_rate;
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
	private: System::Void main_checkBox_connect_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void bw1_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e); 
	private: System::Void f1_Load(System::Object^  sender, System::EventArgs^  e); 

	public: void log(String ^str) { this->Invoke( this->Update_textBoxLog_delegate, str); }
	public: void log(char *str)  { this->Invoke( this->Update_textBoxLog_delegate, gcnew String(str)); }
	public: void term(char *str)  { this->Invoke( this->Update_textBox1_delegate, gcnew String(str)); }
	public: void update_ui(void) { this->Invoke( this->UpdateUI_delegate, gcnew String("")); }

	private: System::Void parameters_dataGridView_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;
	private: System::Void parameters_dataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;


			  
	private: System::Windows::Forms::DataGridView^  parameters_dataGridView;
	private: System::Windows::Forms::DataVisualization::Charting::Chart^  graphs_chart;
	private: System::Windows::Forms::CheckBox^  graphs_checkBox_enable;
	private: System::Windows::Forms::ComboBox^  graphs_comboBox_series_1a;
	private: System::Windows::Forms::ComboBox^  graphs_comboBox_series_2b;
	private: System::Windows::Forms::ComboBox^  graphs_comboBox_series_2a;
	private: System::Windows::Forms::ComboBox^  graphs_comboBox_series_1b;
	private: System::Windows::Forms::TabPage^  radar_tabPage;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Group;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  id;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  name;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Type;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  val;
	private: System::Windows::Forms::DataGridViewButtonColumn^  inc;
	private: System::Windows::Forms::DataGridViewButtonColumn^  dec;
	private: System::Windows::Forms::Button^  parameters_btn_write_all;
	private: System::Windows::Forms::Button^  parameters_btn_read_all;

	private: 

	private: System::Windows::Forms::CheckBox^  cb_show_ir_far_north;
	private: System::Windows::Forms::CheckBox^  cb_show_ir_north;
	private: System::Windows::Forms::Button^  main_btn_stop_scan;
	private: System::Windows::Forms::Button^  btn_start_scan;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  txt_scan_range;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  txt_speed;
	private: System::Windows::Forms::Button^  main_btn_estop;
	private: System::Windows::Forms::Button^  main_btn_up;
	private: System::Windows::Forms::Button^  main_btn_down;
	private: System::Windows::Forms::Button^  main_btn_left;
	private: System::Windows::Forms::Button^  main_btn_right;
	private: System::Windows::Forms::Button^  main_btn_stop;
	private: System::Windows::Forms::Timer^  radar_timer;
	private: System::Windows::Forms::Label^  main_lbl_behavior_control;

	private: System::Windows::Forms::ComboBox^  main_comboBox_behavior_id;
	private: System::Windows::Forms::ComboBox^  main_comboBox_behavior_state;
	private: System::Windows::Forms::Button^  main_btn_start_beh;
	private: System::Windows::Forms::Button^  main_main_btn_stop_beh;




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
	private: System::Windows::Forms::ComboBox^  main_comboBox_port;
	private: System::Windows::Forms::CheckBox^  main_checkBox_connect;
private: System::Windows::Forms::Label^  main_lbl_port;


	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  graphs_tab;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::TextBox^  main_textBox_vbatt;
private: System::Windows::Forms::Label^  main_lbl_battery;

	private: System::Windows::Forms::TabPage^  parameters_tab;
	private: System::Windows::Forms::TextBox^  main_textBox_keyb;
private: System::Windows::Forms::Label^  main_lbl_keyb;

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
			this->main_comboBox_port = (gcnew System::Windows::Forms::ComboBox());
			this->main_checkBox_connect = (gcnew System::Windows::Forms::CheckBox());
			this->main_lbl_port = (gcnew System::Windows::Forms::Label());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->parameters_tab = (gcnew System::Windows::Forms::TabPage());
			this->parameters_btn_write_all = (gcnew System::Windows::Forms::Button());
			this->parameters_btn_read_all = (gcnew System::Windows::Forms::Button());
			this->parameters_dataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->Group = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->id = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->name = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Type = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->val = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->inc = (gcnew System::Windows::Forms::DataGridViewButtonColumn());
			this->dec = (gcnew System::Windows::Forms::DataGridViewButtonColumn());
			this->graphs_tab = (gcnew System::Windows::Forms::TabPage());
			this->graphs_comboBox_series_2b = (gcnew System::Windows::Forms::ComboBox());
			this->graphs_comboBox_series_2a = (gcnew System::Windows::Forms::ComboBox());
			this->graphs_comboBox_series_1b = (gcnew System::Windows::Forms::ComboBox());
			this->graphs_comboBox_series_1a = (gcnew System::Windows::Forms::ComboBox());
			this->graphs_checkBox_enable = (gcnew System::Windows::Forms::CheckBox());
			this->graphs_chart = (gcnew System::Windows::Forms::DataVisualization::Charting::Chart());
			this->radar_tabPage = (gcnew System::Windows::Forms::TabPage());
			this->main_btn_stop_scan = (gcnew System::Windows::Forms::Button());
			this->btn_radar_clear = (gcnew System::Windows::Forms::Button());
			this->btn_start_scan = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txt_speed = (gcnew System::Windows::Forms::TextBox());
			this->txt_scan_range = (gcnew System::Windows::Forms::TextBox());
			this->cb_radar_enable_updates = (gcnew System::Windows::Forms::CheckBox());
			this->cb_radar_use_lines = (gcnew System::Windows::Forms::CheckBox());
			this->cb_show_ir_far_north = (gcnew System::Windows::Forms::CheckBox());
			this->cb_show_ir_north = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->tabLog = (gcnew System::Windows::Forms::TabPage());
			this->textBoxLog = (gcnew System::Windows::Forms::TextBox());
			this->main_textBox_vbatt = (gcnew System::Windows::Forms::TextBox());
			this->main_lbl_battery = (gcnew System::Windows::Forms::Label());
			this->main_textBox_keyb = (gcnew System::Windows::Forms::TextBox());
			this->main_lbl_keyb = (gcnew System::Windows::Forms::Label());
			this->main_btn_estop = (gcnew System::Windows::Forms::Button());
			this->main_btn_up = (gcnew System::Windows::Forms::Button());
			this->main_btn_down = (gcnew System::Windows::Forms::Button());
			this->main_btn_left = (gcnew System::Windows::Forms::Button());
			this->main_btn_right = (gcnew System::Windows::Forms::Button());
			this->main_btn_stop = (gcnew System::Windows::Forms::Button());
			this->radar_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->main_lbl_behavior_control = (gcnew System::Windows::Forms::Label());
			this->main_comboBox_behavior_id = (gcnew System::Windows::Forms::ComboBox());
			this->main_comboBox_behavior_state = (gcnew System::Windows::Forms::ComboBox());
			this->main_btn_start_beh = (gcnew System::Windows::Forms::Button());
			this->main_main_btn_stop_beh = (gcnew System::Windows::Forms::Button());
			this->main_lbl_rate = (gcnew System::Windows::Forms::Label());
			this->main_comboBox_rate = (gcnew System::Windows::Forms::ComboBox());
			this->tabControl1->SuspendLayout();
			this->parameters_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->parameters_dataGridView))->BeginInit();
			this->graphs_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->graphs_chart))->BeginInit();
			this->radar_tabPage->SuspendLayout();
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
			// main_comboBox_port
			// 
			this->main_comboBox_port->FormattingEnabled = true;
			this->main_comboBox_port->Items->AddRange(gcnew cli::array< System::Object^  >(1) {L"11"});
			this->main_comboBox_port->Location = System::Drawing::Point(67, 10);
			this->main_comboBox_port->Name = L"main_comboBox_port";
			this->main_comboBox_port->Size = System::Drawing::Size(36, 21);
			this->main_comboBox_port->TabIndex = 1;
			this->main_comboBox_port->Text = L"4";
			this->main_comboBox_port->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_port_SelectedIndexChanged);
			// 
			// main_checkBox_connect
			// 
			this->main_checkBox_connect->AutoSize = true;
			this->main_checkBox_connect->Location = System::Drawing::Point(109, 12);
			this->main_checkBox_connect->Name = L"main_checkBox_connect";
			this->main_checkBox_connect->Size = System::Drawing::Size(78, 17);
			this->main_checkBox_connect->TabIndex = 2;
			this->main_checkBox_connect->Text = L"Connected";
			this->main_checkBox_connect->UseVisualStyleBackColor = true;
			this->main_checkBox_connect->CheckedChanged += gcnew System::EventHandler(this, &f1::main_checkBox_connect_CheckedChanged);
			// 
			// main_lbl_port
			// 
			this->main_lbl_port->AutoSize = true;
			this->main_lbl_port->Location = System::Drawing::Point(5, 13);
			this->main_lbl_port->Name = L"main_lbl_port";
			this->main_lbl_port->Size = System::Drawing::Size(56, 13);
			this->main_lbl_port->TabIndex = 3;
			this->main_lbl_port->Text = L"COM Port:";
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->parameters_tab);
			this->tabControl1->Controls->Add(this->graphs_tab);
			this->tabControl1->Controls->Add(this->radar_tabPage);
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabLog);
			this->tabControl1->Location = System::Drawing::Point(0, 51);
			this->tabControl1->Margin = System::Windows::Forms::Padding(0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(1098, 657);
			this->tabControl1->TabIndex = 4;
			// 
			// parameters_tab
			// 
			this->parameters_tab->Controls->Add(this->parameters_btn_write_all);
			this->parameters_tab->Controls->Add(this->parameters_btn_read_all);
			this->parameters_tab->Controls->Add(this->parameters_dataGridView);
			this->parameters_tab->Location = System::Drawing::Point(4, 22);
			this->parameters_tab->Name = L"parameters_tab";
			this->parameters_tab->Padding = System::Windows::Forms::Padding(3);
			this->parameters_tab->Size = System::Drawing::Size(1090, 631);
			this->parameters_tab->TabIndex = 2;
			this->parameters_tab->Text = L"Parameters";
			this->parameters_tab->UseVisualStyleBackColor = true;
			// 
			// parameters_btn_write_all
			// 
			this->parameters_btn_write_all->Location = System::Drawing::Point(801, 16);
			this->parameters_btn_write_all->Name = L"parameters_btn_write_all";
			this->parameters_btn_write_all->Size = System::Drawing::Size(114, 26);
			this->parameters_btn_write_all->TabIndex = 4;
			this->parameters_btn_write_all->Text = L"Write All";
			this->parameters_btn_write_all->UseVisualStyleBackColor = true;
			this->parameters_btn_write_all->Click += gcnew System::EventHandler(this, &f1::parameters_btn_write_all_Click);
			// 
			// parameters_btn_read_all
			// 
			this->parameters_btn_read_all->Location = System::Drawing::Point(649, 16);
			this->parameters_btn_read_all->Name = L"parameters_btn_read_all";
			this->parameters_btn_read_all->Size = System::Drawing::Size(114, 26);
			this->parameters_btn_read_all->TabIndex = 4;
			this->parameters_btn_read_all->Text = L"Read All";
			this->parameters_btn_read_all->UseVisualStyleBackColor = true;
			// 
			// parameters_dataGridView
			// 
			this->parameters_dataGridView->AllowUserToAddRows = false;
			this->parameters_dataGridView->AllowUserToDeleteRows = false;
			this->parameters_dataGridView->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left));
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->parameters_dataGridView->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->parameters_dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->parameters_dataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(7) {this->Group, 
				this->id, this->name, this->Type, this->val, this->inc, this->dec});
			dataGridViewCellStyle4->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle4->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle4->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle4->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle4->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle4->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
			this->parameters_dataGridView->DefaultCellStyle = dataGridViewCellStyle4;
			this->parameters_dataGridView->Location = System::Drawing::Point(0, 0);
			this->parameters_dataGridView->Name = L"parameters_dataGridView";
			this->parameters_dataGridView->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->parameters_dataGridView->Size = System::Drawing::Size(630, 617);
			this->parameters_dataGridView->TabIndex = 3;
			this->parameters_dataGridView->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::parameters_dataGridView_CellContentClick);
			this->parameters_dataGridView->CellContentDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::parameters_dataGridView_CellContentDoubleClick);
			this->parameters_dataGridView->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::parameters_dataGridView_CellValueChanged);
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
			// graphs_tab
			// 
			this->graphs_tab->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->graphs_tab->Controls->Add(this->graphs_comboBox_series_2b);
			this->graphs_tab->Controls->Add(this->graphs_comboBox_series_2a);
			this->graphs_tab->Controls->Add(this->graphs_comboBox_series_1b);
			this->graphs_tab->Controls->Add(this->graphs_comboBox_series_1a);
			this->graphs_tab->Controls->Add(this->graphs_checkBox_enable);
			this->graphs_tab->Controls->Add(this->graphs_chart);
			this->graphs_tab->Location = System::Drawing::Point(4, 22);
			this->graphs_tab->Name = L"graphs_tab";
			this->graphs_tab->Padding = System::Windows::Forms::Padding(3);
			this->graphs_tab->Size = System::Drawing::Size(1090, 631);
			this->graphs_tab->TabIndex = 1;
			this->graphs_tab->Text = L"Graphs";
			this->graphs_tab->UseVisualStyleBackColor = true;
			this->graphs_tab->Click += gcnew System::EventHandler(this, &f1::graphs_tab_Click);
			// 
			// graphs_comboBox_series_2b
			// 
			this->graphs_comboBox_series_2b->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->graphs_comboBox_series_2b->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->graphs_comboBox_series_2b->FormattingEnabled = true;
			this->graphs_comboBox_series_2b->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->graphs_comboBox_series_2b->Location = System::Drawing::Point(892, 337);
			this->graphs_comboBox_series_2b->Name = L"graphs_comboBox_series_2b";
			this->graphs_comboBox_series_2b->Size = System::Drawing::Size(187, 21);
			this->graphs_comboBox_series_2b->TabIndex = 4;
			// 
			// graphs_comboBox_series_2a
			// 
			this->graphs_comboBox_series_2a->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->graphs_comboBox_series_2a->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->graphs_comboBox_series_2a->FormattingEnabled = true;
			this->graphs_comboBox_series_2a->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->graphs_comboBox_series_2a->Location = System::Drawing::Point(892, 310);
			this->graphs_comboBox_series_2a->Name = L"graphs_comboBox_series_2a";
			this->graphs_comboBox_series_2a->Size = System::Drawing::Size(187, 21);
			this->graphs_comboBox_series_2a->TabIndex = 4;
			// 
			// graphs_comboBox_series_1b
			// 
			this->graphs_comboBox_series_1b->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->graphs_comboBox_series_1b->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->graphs_comboBox_series_1b->FormattingEnabled = true;
			this->graphs_comboBox_series_1b->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->graphs_comboBox_series_1b->Location = System::Drawing::Point(892, 43);
			this->graphs_comboBox_series_1b->Name = L"graphs_comboBox_series_1b";
			this->graphs_comboBox_series_1b->Size = System::Drawing::Size(187, 21);
			this->graphs_comboBox_series_1b->TabIndex = 4;
			// 
			// graphs_comboBox_series_1a
			// 
			this->graphs_comboBox_series_1a->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->graphs_comboBox_series_1a->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->graphs_comboBox_series_1a->FormattingEnabled = true;
			this->graphs_comboBox_series_1a->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", L"analog[2]", 
				L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
				L"actual_speed[1]", L"target_speed[0]", L"target_speed[1]", L"x", L"y", L"theta", L"watch[0]", L"watch[1]", L"watch[2]", L"watch[3]"});
			this->graphs_comboBox_series_1a->Location = System::Drawing::Point(892, 16);
			this->graphs_comboBox_series_1a->Name = L"graphs_comboBox_series_1a";
			this->graphs_comboBox_series_1a->Size = System::Drawing::Size(187, 21);
			this->graphs_comboBox_series_1a->TabIndex = 4;
			this->graphs_comboBox_series_1a->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::graphs_comboBox_series_1a_SelectedIndexChanged);
			// 
			// graphs_checkBox_enable
			// 
			this->graphs_checkBox_enable->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->graphs_checkBox_enable->AutoSize = true;
			this->graphs_checkBox_enable->Location = System::Drawing::Point(1023, 592);
			this->graphs_checkBox_enable->Name = L"graphs_checkBox_enable";
			this->graphs_checkBox_enable->Size = System::Drawing::Size(59, 17);
			this->graphs_checkBox_enable->TabIndex = 2;
			this->graphs_checkBox_enable->Text = L"Enable";
			this->graphs_checkBox_enable->UseVisualStyleBackColor = true;
			this->graphs_checkBox_enable->CheckedChanged += gcnew System::EventHandler(this, &f1::graphs_checkBox_enable_CheckedChanged);
			// 
			// graphs_chart
			// 
			this->graphs_chart->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->graphs_chart->AntiAliasing = System::Windows::Forms::DataVisualization::Charting::AntiAliasingStyles::None;
			this->graphs_chart->BorderlineColor = System::Drawing::Color::RoyalBlue;
			chartArea1->AxisX->MajorGrid->Enabled = false;
			chartArea1->Name = L"ChartArea1";
			chartArea2->AxisX->MajorGrid->Enabled = false;
			chartArea2->Name = L"ChartArea2";
			this->graphs_chart->ChartAreas->Add(chartArea1);
			this->graphs_chart->ChartAreas->Add(chartArea2);
			legend1->Name = L"Legend1";
			this->graphs_chart->Legends->Add(legend1);
			this->graphs_chart->Location = System::Drawing::Point(-5, -1);
			this->graphs_chart->Margin = System::Windows::Forms::Padding(0);
			this->graphs_chart->Name = L"graphs_chart";
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
			this->graphs_chart->Series->Add(series1);
			this->graphs_chart->Series->Add(series2);
			this->graphs_chart->Size = System::Drawing::Size(894, 621);
			this->graphs_chart->TabIndex = 0;
			this->graphs_chart->Text = L"graphs_chart";
			this->graphs_chart->AxisViewChanged += gcnew System::EventHandler<System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^ >(this, &f1::graphs_chart_AxisViewChanged);
			// 
			// radar_tabPage
			// 
			this->radar_tabPage->Controls->Add(this->main_btn_stop_scan);
			this->radar_tabPage->Controls->Add(this->btn_radar_clear);
			this->radar_tabPage->Controls->Add(this->btn_start_scan);
			this->radar_tabPage->Controls->Add(this->label3);
			this->radar_tabPage->Controls->Add(this->label2);
			this->radar_tabPage->Controls->Add(this->txt_speed);
			this->radar_tabPage->Controls->Add(this->txt_scan_range);
			this->radar_tabPage->Controls->Add(this->cb_radar_enable_updates);
			this->radar_tabPage->Controls->Add(this->cb_radar_use_lines);
			this->radar_tabPage->Controls->Add(this->cb_show_ir_far_north);
			this->radar_tabPage->Controls->Add(this->cb_show_ir_north);
			this->radar_tabPage->Location = System::Drawing::Point(4, 22);
			this->radar_tabPage->Name = L"radar_tabPage";
			this->radar_tabPage->Padding = System::Windows::Forms::Padding(3);
			this->radar_tabPage->Size = System::Drawing::Size(1090, 631);
			this->radar_tabPage->TabIndex = 4;
			this->radar_tabPage->Text = L"\"Radar\"";
			this->radar_tabPage->UseVisualStyleBackColor = true;
			this->radar_tabPage->Click += gcnew System::EventHandler(this, &f1::radar_tabPage_Click);
			this->radar_tabPage->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &f1::radar_tabPage_Paint);
			// 
			// main_btn_stop_scan
			// 
			this->main_btn_stop_scan->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_stop_scan->Location = System::Drawing::Point(1006, 63);
			this->main_btn_stop_scan->Name = L"main_btn_stop_scan";
			this->main_btn_stop_scan->Size = System::Drawing::Size(67, 23);
			this->main_btn_stop_scan->TabIndex = 4;
			this->main_btn_stop_scan->Text = L"Stop";
			this->main_btn_stop_scan->UseVisualStyleBackColor = true;
			this->main_btn_stop_scan->Click += gcnew System::EventHandler(this, &f1::main_btn_stop_scan_Click);
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
			// main_textBox_vbatt
			// 
			this->main_textBox_vbatt->Location = System::Drawing::Point(275, 21);
			this->main_textBox_vbatt->Name = L"main_textBox_vbatt";
			this->main_textBox_vbatt->ReadOnly = true;
			this->main_textBox_vbatt->Size = System::Drawing::Size(59, 20);
			this->main_textBox_vbatt->TabIndex = 0;
			this->main_textBox_vbatt->Text = L"0.0V";
			// 
			// main_lbl_battery
			// 
			this->main_lbl_battery->AutoSize = true;
			this->main_lbl_battery->Location = System::Drawing::Point(272, 5);
			this->main_lbl_battery->Name = L"main_lbl_battery";
			this->main_lbl_battery->Size = System::Drawing::Size(43, 13);
			this->main_lbl_battery->TabIndex = 1;
			this->main_lbl_battery->Text = L"Battery:";
			this->main_lbl_battery->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// main_textBox_keyb
			// 
			this->main_textBox_keyb->Location = System::Drawing::Point(524, 23);
			this->main_textBox_keyb->Name = L"main_textBox_keyb";
			this->main_textBox_keyb->Size = System::Drawing::Size(77, 20);
			this->main_textBox_keyb->TabIndex = 5;
			this->main_textBox_keyb->TextChanged += gcnew System::EventHandler(this, &f1::main_textBox_keyb_TextChanged);
			this->main_textBox_keyb->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &f1::main_textBox_keyb_KeyDown);
			// 
			// main_lbl_keyb
			// 
			this->main_lbl_keyb->AutoSize = true;
			this->main_lbl_keyb->Location = System::Drawing::Point(521, 5);
			this->main_lbl_keyb->Name = L"main_lbl_keyb";
			this->main_lbl_keyb->Size = System::Drawing::Size(80, 13);
			this->main_lbl_keyb->TabIndex = 1;
			this->main_lbl_keyb->Text = L"Keyb. Shortcut:";
			this->main_lbl_keyb->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// main_btn_estop
			// 
			this->main_btn_estop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_estop->BackColor = System::Drawing::Color::Red;
			this->main_btn_estop->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"main_btn_estop.BackgroundImage")));
			this->main_btn_estop->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->main_btn_estop->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->main_btn_estop->Location = System::Drawing::Point(1024, 5);
			this->main_btn_estop->Name = L"main_btn_estop";
			this->main_btn_estop->Size = System::Drawing::Size(67, 61);
			this->main_btn_estop->TabIndex = 6;
			this->main_btn_estop->UseVisualStyleBackColor = false;
			this->main_btn_estop->Click += gcnew System::EventHandler(this, &f1::main_btn_estop_Click);
			// 
			// main_btn_up
			// 
			this->main_btn_up->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_up->Location = System::Drawing::Point(948, 1);
			this->main_btn_up->Name = L"main_btn_up";
			this->main_btn_up->Size = System::Drawing::Size(25, 21);
			this->main_btn_up->TabIndex = 7;
			this->main_btn_up->Text = L"u";
			this->main_btn_up->UseVisualStyleBackColor = true;
			// 
			// main_btn_down
			// 
			this->main_btn_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_down->Location = System::Drawing::Point(948, 47);
			this->main_btn_down->Name = L"main_btn_down";
			this->main_btn_down->Size = System::Drawing::Size(25, 21);
			this->main_btn_down->TabIndex = 7;
			this->main_btn_down->Text = L"d";
			this->main_btn_down->UseVisualStyleBackColor = true;
			// 
			// main_btn_left
			// 
			this->main_btn_left->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_left->Location = System::Drawing::Point(920, 24);
			this->main_btn_left->Name = L"main_btn_left";
			this->main_btn_left->Size = System::Drawing::Size(25, 21);
			this->main_btn_left->TabIndex = 7;
			this->main_btn_left->Text = L"<";
			this->main_btn_left->UseVisualStyleBackColor = true;
			// 
			// main_btn_right
			// 
			this->main_btn_right->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_right->Location = System::Drawing::Point(976, 24);
			this->main_btn_right->Name = L"main_btn_right";
			this->main_btn_right->Size = System::Drawing::Size(25, 21);
			this->main_btn_right->TabIndex = 7;
			this->main_btn_right->Text = L">";
			this->main_btn_right->UseVisualStyleBackColor = true;
			// 
			// main_btn_stop
			// 
			this->main_btn_stop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_stop->Location = System::Drawing::Point(948, 24);
			this->main_btn_stop->Name = L"main_btn_stop";
			this->main_btn_stop->Size = System::Drawing::Size(25, 21);
			this->main_btn_stop->TabIndex = 7;
			this->main_btn_stop->Text = L"s";
			this->main_btn_stop->UseVisualStyleBackColor = true;
			this->main_btn_stop->Click += gcnew System::EventHandler(this, &f1::main_btn_stop_Click);
			// 
			// radar_timer
			// 
			this->radar_timer->Enabled = true;
			this->radar_timer->Interval = 30;
			this->radar_timer->Tick += gcnew System::EventHandler(this, &f1::radar_timer_Tick);
			// 
			// main_lbl_behavior_control
			// 
			this->main_lbl_behavior_control->AutoSize = true;
			this->main_lbl_behavior_control->Location = System::Drawing::Point(624, 5);
			this->main_lbl_behavior_control->Name = L"main_lbl_behavior_control";
			this->main_lbl_behavior_control->Size = System::Drawing::Size(229, 13);
			this->main_lbl_behavior_control->TabIndex = 8;
			this->main_lbl_behavior_control->Text = L"Behavior:              Initial State:          Command:";
			// 
			// main_comboBox_behavior_id
			// 
			this->main_comboBox_behavior_id->FormattingEnabled = true;
			this->main_comboBox_behavior_id->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", 
				L"10", L"11", L"12", L"13", L"14", L"15", L"16"});
			this->main_comboBox_behavior_id->Location = System::Drawing::Point(625, 24);
			this->main_comboBox_behavior_id->Name = L"main_comboBox_behavior_id";
			this->main_comboBox_behavior_id->Size = System::Drawing::Size(61, 21);
			this->main_comboBox_behavior_id->TabIndex = 9;
			this->main_comboBox_behavior_id->Text = L"1";
			this->main_comboBox_behavior_id->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_behavior_id_SelectedIndexChanged);
			// 
			// main_comboBox_behavior_state
			// 
			this->main_comboBox_behavior_state->FormattingEnabled = true;
			this->main_comboBox_behavior_state->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", 
				L"9", L"10", L"11", L"12", L"13", L"14", L"15", L"16"});
			this->main_comboBox_behavior_state->Location = System::Drawing::Point(712, 24);
			this->main_comboBox_behavior_state->Name = L"main_comboBox_behavior_state";
			this->main_comboBox_behavior_state->Size = System::Drawing::Size(61, 21);
			this->main_comboBox_behavior_state->TabIndex = 9;
			this->main_comboBox_behavior_state->Text = L"1";
			// 
			// main_btn_start_beh
			// 
			this->main_btn_start_beh->Location = System::Drawing::Point(798, 21);
			this->main_btn_start_beh->Name = L"main_btn_start_beh";
			this->main_btn_start_beh->Size = System::Drawing::Size(42, 23);
			this->main_btn_start_beh->TabIndex = 10;
			this->main_btn_start_beh->Text = L"Start";
			this->main_btn_start_beh->UseVisualStyleBackColor = true;
			this->main_btn_start_beh->Click += gcnew System::EventHandler(this, &f1::main_btn_start_beh_Click);
			// 
			// main_main_btn_stop_beh
			// 
			this->main_main_btn_stop_beh->Location = System::Drawing::Point(846, 21);
			this->main_main_btn_stop_beh->Name = L"main_main_btn_stop_beh";
			this->main_main_btn_stop_beh->Size = System::Drawing::Size(42, 23);
			this->main_main_btn_stop_beh->TabIndex = 10;
			this->main_main_btn_stop_beh->Text = L"Stop";
			this->main_main_btn_stop_beh->UseVisualStyleBackColor = true;
			this->main_main_btn_stop_beh->Click += gcnew System::EventHandler(this, &f1::main_main_btn_stop_beh_Click);
			// 
			// main_lbl_rate
			// 
			this->main_lbl_rate->AutoSize = true;
			this->main_lbl_rate->Location = System::Drawing::Point(361, 5);
			this->main_lbl_rate->Name = L"main_lbl_rate";
			this->main_lbl_rate->Size = System::Drawing::Size(33, 13);
			this->main_lbl_rate->TabIndex = 1;
			this->main_lbl_rate->Text = L"Rate:";
			this->main_lbl_rate->Click += gcnew System::EventHandler(this, &f1::label9_Click);
			// 
			// main_comboBox_rate
			// 
			this->main_comboBox_rate->FormattingEnabled = true;
			this->main_comboBox_rate->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"0", L"10", L"100", L"1000", L"2000"});
			this->main_comboBox_rate->Location = System::Drawing::Point(364, 22);
			this->main_comboBox_rate->Name = L"main_comboBox_rate";
			this->main_comboBox_rate->Size = System::Drawing::Size(61, 21);
			this->main_comboBox_rate->TabIndex = 9;
			this->main_comboBox_rate->Text = L"1";
			this->main_comboBox_rate->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_behavior_id_SelectedIndexChanged);
			// 
			// f1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1096, 707);
			this->Controls->Add(this->main_main_btn_stop_beh);
			this->Controls->Add(this->main_btn_start_beh);
			this->Controls->Add(this->main_comboBox_behavior_state);
			this->Controls->Add(this->main_comboBox_rate);
			this->Controls->Add(this->main_comboBox_behavior_id);
			this->Controls->Add(this->main_lbl_behavior_control);
			this->Controls->Add(this->main_btn_right);
			this->Controls->Add(this->main_btn_left);
			this->Controls->Add(this->main_btn_down);
			this->Controls->Add(this->main_btn_stop);
			this->Controls->Add(this->main_btn_up);
			this->Controls->Add(this->main_btn_estop);
			this->Controls->Add(this->main_textBox_keyb);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->main_lbl_port);
			this->Controls->Add(this->main_checkBox_connect);
			this->Controls->Add(this->main_comboBox_port);
			this->Controls->Add(this->main_lbl_keyb);
			this->Controls->Add(this->main_lbl_rate);
			this->Controls->Add(this->main_lbl_battery);
			this->Controls->Add(this->main_textBox_vbatt);
			this->Name = L"f1";
			this->Text = L"Robot UI";
			this->Load += gcnew System::EventHandler(this, &f1::f1_Load);
			this->tabControl1->ResumeLayout(false);
			this->parameters_tab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->parameters_dataGridView))->EndInit();
			this->graphs_tab->ResumeLayout(false);
			this->graphs_tab->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->graphs_chart))->EndInit();
			this->radar_tabPage->ResumeLayout(false);
			this->radar_tabPage->PerformLayout();
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
private: System::Void graphs_tab_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void graphs_tab_Paint(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void tb_SE_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void main_comboBox_port_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
		 }
private: System::Void label9_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void main_textBox_keyb_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void main_textBox_keyb_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;

private: System::Void checkedListBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void checkedListBox1_SelectedValueChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void parameters_dataGridView_CellContentDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void radar_tabPage_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e); 

private: System::Void graphs_chart_AxisViewChanged(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^  e) ;
private: System::Void graphs_checkBox_enable_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void btn_start_scan_Click(System::Object^  sender, System::EventArgs^  e) {
			 radar_tabPage->Invalidate();
		 }
private: System::Void main_btn_stop_scan_Click(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void radar_timer_Tick(System::Object^  sender, System::EventArgs^  e); 

private: System::Void main_btn_stop_Click(System::Object^  sender, System::EventArgs^  e) 
		{

		}
private: System::Void graphs_comboBox_series_1a_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 }

private: System::Void main_comboBox_behavior_id_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 update_interval = Convert::ToInt32(main_comboBox_rate->Text);
		 }

private: System::Void main_btn_start_beh_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 CMD_set_behavior_state(Convert::ToByte(main_comboBox_behavior_id->Text), Convert::ToByte(main_comboBox_behavior_state->Text));
			 CMD_send();
		 }

private: System::Void main_main_btn_stop_beh_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 CMD_set_behavior_state(Convert::ToByte(main_comboBox_behavior_id->Text), 0);
			 CMD_send();
		 }
private: System::Void main_btn_estop_Click(System::Object^  sender, System::EventArgs^  e);

private: System::Void parameters_btn_write_all_Click(System::Object^  sender, System::EventArgs^  e) 
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
private: System::Void radar_tabPage_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
};


}

