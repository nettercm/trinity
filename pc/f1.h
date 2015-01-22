#pragma once

#include "serial.h"
#include "logic.h"
#include "data.h"
#include "kalman.h"
#include "commands.h"
#include "..\robot\config.h"
#include "ip.h"
#include "debug.h"

#include <io.h>
#include <fcntl.h>     /* for _O_TEXT and _O_BINARY */
#include <math.h>

//
// Global variables
//


#define NOP()  _dummy_++

extern "C" 
{ 
	extern float PI;;
	extern volatile unsigned int key; 
	extern int loop(void);
	extern void show_last_error(char *s);
	extern volatile t_inputs inputs_history[200000];
	extern volatile int history_index;
	extern int update_interval;
	extern char log_buffer[];
	extern volatile int log_write_index;
	extern volatile int log_read_index;
	extern int	log_printf(const char *__fmt, ...);
	extern void ParseRawInput(PRAWINPUT pRawInput);
	extern void process_wm_input(LPARAM lParam);
	extern void register_joystick(HWND hwnd);
	extern void enumerate_raw_input(void);
	extern void UpdateControllerState(void);
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
	using namespace System::Security::Permissions;


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
	public:
	volatile int _dummy_;

	public:
		[SecurityPermissionAttribute(SecurityAction::LinkDemand, Flags = SecurityPermissionFlag::UnmanagedCode)]
		[SecurityPermissionAttribute(SecurityAction::InheritanceDemand, Flags = SecurityPermissionFlag::UnmanagedCode)]
		virtual  void WndProc( Message% m ) override
		{
			static unsigned long t1=0,t2,count=0;

			count++;
			t2=timeGetTime(); //GetTickCount();
			if(t2-t1>=1000)
			{
				t1=t2;
				//log_printf("%lu messages per second\n",count);
				count=0;
			}

			// Listen for operating system messages. 
			switch ( m.Msg )
			{
			case WM_ACTIVATEAPP:
				NOP();
				break;
			case WM_INPUT:
				process_wm_input(static_cast<LPARAM>(m.LParam.ToInt32()));
				//log_printf("WM_INPUT\n");
				break;
			}
			Form::WndProc( m );
		}


	private: int ignore_parameter_changes;
	public: Graphics^ g;
	private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_ne;
	public: 
	private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_nw;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_n_x;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_n_theta;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_n_y;

	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_ne_theta;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_ne_y;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_nw_theta;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_nw_y;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_fn_theta;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_ne_x;


	private: System::Windows::Forms::TextBox^  radar_txt_calib_fn_y;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_nw_x;


	private: System::Windows::Forms::TextBox^  radar_txt_calib_fn_x;

	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TabPage^  map_tab;
	private: System::Windows::Forms::PictureBox^  map_picture;
	private: System::Windows::Forms::Timer^  serial_timer;
	private: System::Windows::Forms::Timer^  ui_timer;
	private: System::Windows::Forms::Label^  main_lbl_ip;
	private: System::Windows::Forms::ComboBox^  main_comboBox_ip;
	private: System::Windows::Forms::CheckBox^  main_checkBox_connect_ip;

	private: System::Random^	random;

	public: f1(void);
	protected: 	~f1()	{	if (components) 	{	delete components;		}	}

	public: delegate void UpdateUI(String ^str);
	public: delegate void UpdateChart(String^ series, double x, double y);
	public: UpdateUI^ UpdateUI_delegate;
	public: UpdateChart^ UpdateChart_delegate; 
	public: UpdateUI^ Update_terminal_txt_delegate;
	public: UpdateUI^ Update_log_txt_delegate;

	private: System::Windows::Forms::Label^  main_lbl_rate;
	private: System::Windows::Forms::ComboBox^  main_comboBox_rate;
	private: System::Windows::Forms::Button^  radar_btn_clear;
	private: System::Windows::Forms::CheckBox^  radar_checkBox_enable_updates;
	private: System::Windows::Forms::CheckBox^  radar_checkBox_use_lines;


	public: void UpdateChart_method(String^ series, double x, double y);
	public: void UpdateUI_method(String ^str);
	public: void f1::Update_log_txt_method(String ^str);
	public: void f1::Update_terminal_txt_method(String ^str);

	public: void InitializeParametersTab(void);
	public: void UpdateRadar(float theta, int measurement);
	public: void DrawGrid(Graphics ^g);
	public: void InitializeGraphsTab(void);

	public: void log(String ^str) { this->Invoke( this->Update_log_txt_delegate, str); }
	public: void log(char *str)  { this->Invoke( this->Update_log_txt_delegate, gcnew String(str)); }
	public: void term(char *str)  { this->Invoke( this->Update_terminal_txt_delegate, gcnew String(str)); }
	public: void update_ui(void) { this->Invoke( this->UpdateUI_delegate, gcnew String("")); }

	private: System::Void f1_Load(System::Object^  sender, System::EventArgs^  e); 

	private: System::Void graphs_tab_Click(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void graphs_tab_Paint(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void graphs_chart_AxisViewChanged(System::Object^  sender, System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^  e) ;
	private: System::Void graphs_checkBox_enable_CheckedChanged(System::Object^  sender, System::EventArgs^  e) ;
	private: System::Void graphs_comboBox_series_1a_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {	 }
	private: System::Void graphs_timer_Tick(System::Object^  sender, System::EventArgs^  e); 

	private: System::Void main_checkBox_connect_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void main_checkBox_connect_ip_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void main_serial_thread_DoWork(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e); 
	private: System::Void main_comboBox_port_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void main_comboBox_ip_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void main_textBox_keyb_TextChanged(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void main_textBox_keyb_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) ;
	private: System::Void main_btn_stop_Click(System::Object^  sender, System::EventArgs^  e) 	{	}
	private: System::Void main_comboBox_behavior_id_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e);
	private: System::Void main_btn_start_beh_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void main_main_btn_stop_beh_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void main_btn_estop_Click(System::Object^  sender, System::EventArgs^  e);

	private: System::Void terminal_txt_TextChanged(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void terminal_txt_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {		 }

	private: System::Void parameters_dataGridView_CellContentDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {		 }
	private: System::Void parameters_btn_write_all_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void parameters_dataGridView_CellValueChanged(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;
	private: System::Void parameters_dataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) ;

	private: System::Void radar_btn_start_scan_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void radar_btn_stop_scan_Click(System::Object^  sender, System::EventArgs^  e) {	 }
	private: System::Void radar_timer_Tick(System::Object^  sender, System::EventArgs^  e); 
	private: System::Void radar_tabPage_Click(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void radar_tabPage_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e); 

	private: System::Void tb_SE_TextChanged(System::Object^  sender, System::EventArgs^  e) {	 }
	private: System::Void main_lbl_battery_Click(System::Object^  sender, System::EventArgs^  e) {		 }
	private: System::Void radar_txt_speed_TextChanged(System::Object^  sender, System::EventArgs^  e) {	 }
	private: System::Void radar_lbl_speed_Click(System::Object^  sender, System::EventArgs^  e) {	 }
	private: System::Void tB_NW_TextChanged(System::Object^  sender, System::EventArgs^  e) {		 }

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
	private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_far_north;
	private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_north;
	private: System::Windows::Forms::Button^  radar_btn_stop_scan;
	private: System::Windows::Forms::Button^  radar_btn_start_scan;
	private: System::Windows::Forms::Label^  radar_lbl_scan_range;
	private: System::Windows::Forms::TextBox^  radar_txt_scan_range;
	private: System::Windows::Forms::Label^  radar_lbl_speed;
	private: System::Windows::Forms::TextBox^  radar_txt_speed;
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
	private: System::Windows::Forms::ComboBox::ObjectCollection^ chartitems;
	private: System::ComponentModel::BackgroundWorker^  main_serial_thread;
	private: System::Windows::Forms::Timer^  graphs_timer;
	private: System::Windows::Forms::ComboBox^  main_comboBox_port;
	private: System::Windows::Forms::CheckBox^  main_checkBox_connect;
	private: System::Windows::Forms::Label^  main_lbl_port;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  terminal_tab;
	private: System::Windows::Forms::TabPage^  graphs_tab;
	private: System::Windows::Forms::TextBox^  terminal_txt;
	private: System::Windows::Forms::TextBox^  main_textBox_vbatt;
	private: System::Windows::Forms::Label^  main_lbl_battery;
	private: System::Windows::Forms::TabPage^  parameters_tab;
	private: System::Windows::Forms::TextBox^  main_textBox_keyb;
	private: System::Windows::Forms::Label^  main_lbl_keyb;
	private: System::Windows::Forms::TabPage^  log_tab;
	private: System::Windows::Forms::TextBox^  log_txt;
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
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle17 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle20 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle18 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle19 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea9 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea10 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend5 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series9 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series10 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(f1::typeid));
			this->main_serial_thread = (gcnew System::ComponentModel::BackgroundWorker());
			this->graphs_timer = (gcnew System::Windows::Forms::Timer(this->components));
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
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->radar_txt_calib_ne_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_ne_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_nw_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_nw_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_fn_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_ne_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_fn_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_nw_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_n_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_fn_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_n_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_n_x = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->radar_btn_stop_scan = (gcnew System::Windows::Forms::Button());
			this->radar_btn_clear = (gcnew System::Windows::Forms::Button());
			this->radar_btn_start_scan = (gcnew System::Windows::Forms::Button());
			this->radar_lbl_speed = (gcnew System::Windows::Forms::Label());
			this->radar_lbl_scan_range = (gcnew System::Windows::Forms::Label());
			this->radar_txt_speed = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_scan_range = (gcnew System::Windows::Forms::TextBox());
			this->radar_checkBox_enable_updates = (gcnew System::Windows::Forms::CheckBox());
			this->radar_checkBox_use_lines = (gcnew System::Windows::Forms::CheckBox());
			this->radar_checkBox_show_ir_ne = (gcnew System::Windows::Forms::CheckBox());
			this->radar_checkBox_show_ir_nw = (gcnew System::Windows::Forms::CheckBox());
			this->radar_checkBox_show_ir_far_north = (gcnew System::Windows::Forms::CheckBox());
			this->radar_checkBox_show_ir_north = (gcnew System::Windows::Forms::CheckBox());
			this->map_tab = (gcnew System::Windows::Forms::TabPage());
			this->map_picture = (gcnew System::Windows::Forms::PictureBox());
			this->terminal_tab = (gcnew System::Windows::Forms::TabPage());
			this->terminal_txt = (gcnew System::Windows::Forms::TextBox());
			this->log_tab = (gcnew System::Windows::Forms::TabPage());
			this->log_txt = (gcnew System::Windows::Forms::TextBox());
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
			this->serial_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->ui_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->main_lbl_ip = (gcnew System::Windows::Forms::Label());
			this->main_comboBox_ip = (gcnew System::Windows::Forms::ComboBox());
			this->main_checkBox_connect_ip = (gcnew System::Windows::Forms::CheckBox());
			this->tabControl1->SuspendLayout();
			this->parameters_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->parameters_dataGridView))->BeginInit();
			this->graphs_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->graphs_chart))->BeginInit();
			this->radar_tabPage->SuspendLayout();
			this->map_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->map_picture))->BeginInit();
			this->terminal_tab->SuspendLayout();
			this->log_tab->SuspendLayout();
			this->SuspendLayout();
			// 
			// main_serial_thread
			// 
			this->main_serial_thread->WorkerReportsProgress = true;
			this->main_serial_thread->WorkerSupportsCancellation = true;
			this->main_serial_thread->DoWork += gcnew System::ComponentModel::DoWorkEventHandler(this, &f1::main_serial_thread_DoWork);
			// 
			// graphs_timer
			// 
			this->graphs_timer->Tick += gcnew System::EventHandler(this, &f1::graphs_timer_Tick);
			// 
			// main_comboBox_port
			// 
			this->main_comboBox_port->FormattingEnabled = true;
			this->main_comboBox_port->Items->AddRange(gcnew cli::array< System::Object^  >(9) {L"4", L"7", L"8", L"9", L"10", L"11", 
				L"12", L"13", L"55"});
			this->main_comboBox_port->Location = System::Drawing::Point(51, 20);
			this->main_comboBox_port->Name = L"main_comboBox_port";
			this->main_comboBox_port->Size = System::Drawing::Size(34, 21);
			this->main_comboBox_port->TabIndex = 1;
			this->main_comboBox_port->Text = L"4";
			this->main_comboBox_port->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_port_SelectedIndexChanged);
			// 
			// main_checkBox_connect
			// 
			this->main_checkBox_connect->AutoSize = true;
			this->main_checkBox_connect->Location = System::Drawing::Point(91, 23);
			this->main_checkBox_connect->Name = L"main_checkBox_connect";
			this->main_checkBox_connect->Size = System::Drawing::Size(15, 14);
			this->main_checkBox_connect->TabIndex = 2;
			this->main_checkBox_connect->UseVisualStyleBackColor = true;
			this->main_checkBox_connect->CheckedChanged += gcnew System::EventHandler(this, &f1::main_checkBox_connect_CheckedChanged);
			// 
			// main_lbl_port
			// 
			this->main_lbl_port->AutoSize = true;
			this->main_lbl_port->Location = System::Drawing::Point(13, 24);
			this->main_lbl_port->Name = L"main_lbl_port";
			this->main_lbl_port->Size = System::Drawing::Size(36, 13);
			this->main_lbl_port->TabIndex = 3;
			this->main_lbl_port->Text = L"Serial:";
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->parameters_tab);
			this->tabControl1->Controls->Add(this->graphs_tab);
			this->tabControl1->Controls->Add(this->radar_tabPage);
			this->tabControl1->Controls->Add(this->map_tab);
			this->tabControl1->Controls->Add(this->terminal_tab);
			this->tabControl1->Controls->Add(this->log_tab);
			this->tabControl1->Location = System::Drawing::Point(0, 58);
			this->tabControl1->Margin = System::Windows::Forms::Padding(0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(1098, 649);
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
			this->parameters_tab->Size = System::Drawing::Size(1090, 623);
			this->parameters_tab->TabIndex = 2;
			this->parameters_tab->Text = L"Parameters";
			this->parameters_tab->UseVisualStyleBackColor = true;
			// 
			// parameters_btn_write_all
			// 
			this->parameters_btn_write_all->Location = System::Drawing::Point(883, 16);
			this->parameters_btn_write_all->Name = L"parameters_btn_write_all";
			this->parameters_btn_write_all->Size = System::Drawing::Size(114, 26);
			this->parameters_btn_write_all->TabIndex = 4;
			this->parameters_btn_write_all->Text = L"Write All";
			this->parameters_btn_write_all->UseVisualStyleBackColor = true;
			this->parameters_btn_write_all->Click += gcnew System::EventHandler(this, &f1::parameters_btn_write_all_Click);
			// 
			// parameters_btn_read_all
			// 
			this->parameters_btn_read_all->Location = System::Drawing::Point(735, 16);
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
			dataGridViewCellStyle17->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle17->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle17->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle17->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle17->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle17->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->parameters_dataGridView->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle17;
			this->parameters_dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->parameters_dataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(7) {this->Group, 
				this->id, this->name, this->Type, this->val, this->inc, this->dec});
			dataGridViewCellStyle20->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleCenter;
			dataGridViewCellStyle20->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle20->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle20->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle20->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle20->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle20->WrapMode = System::Windows::Forms::DataGridViewTriState::False;
			this->parameters_dataGridView->DefaultCellStyle = dataGridViewCellStyle20;
			this->parameters_dataGridView->Location = System::Drawing::Point(18, 16);
			this->parameters_dataGridView->Name = L"parameters_dataGridView";
			this->parameters_dataGridView->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->parameters_dataGridView->Size = System::Drawing::Size(630, 593);
			this->parameters_dataGridView->TabIndex = 3;
			this->parameters_dataGridView->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::parameters_dataGridView_CellContentClick);
			this->parameters_dataGridView->CellContentDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::parameters_dataGridView_CellContentDoubleClick);
			this->parameters_dataGridView->CellValueChanged += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &f1::parameters_dataGridView_CellValueChanged);
			// 
			// Group
			// 
			dataGridViewCellStyle18->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			this->Group->DefaultCellStyle = dataGridViewCellStyle18;
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
			dataGridViewCellStyle19->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			this->name->DefaultCellStyle = dataGridViewCellStyle19;
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
			this->graphs_tab->Size = System::Drawing::Size(1090, 623);
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
			this->graphs_comboBox_series_2b->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", 
				L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
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
			this->graphs_comboBox_series_2a->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", 
				L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
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
			this->graphs_comboBox_series_1b->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", 
				L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
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
			this->graphs_comboBox_series_1a->Items->AddRange(gcnew cli::array< System::Object^  >(24) {L"--NONE--", L"analog[0]", L"analog[1]", 
				L"analog[2]", L"analog[3]", L"analog[4]", L"analog[5]", L"analog[6]", L"analog[7]", L"ir[0]", L"ir[1]", L"ir[2]", L"ir[3]", L"actual_speed[0]", 
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
			this->graphs_checkBox_enable->Location = System::Drawing::Point(1023, 584);
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
			chartArea9->AxisX->MajorGrid->Enabled = false;
			chartArea9->Name = L"ChartArea1";
			chartArea10->AxisX->MajorGrid->Enabled = false;
			chartArea10->Name = L"ChartArea2";
			this->graphs_chart->ChartAreas->Add(chartArea9);
			this->graphs_chart->ChartAreas->Add(chartArea10);
			legend5->Name = L"Legend1";
			this->graphs_chart->Legends->Add(legend5);
			this->graphs_chart->Location = System::Drawing::Point(-5, -1);
			this->graphs_chart->Margin = System::Windows::Forms::Padding(0);
			this->graphs_chart->Name = L"graphs_chart";
			series9->ChartArea = L"ChartArea1";
			series9->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series9->IsXValueIndexed = true;
			series9->Legend = L"Legend1";
			series9->MarkerSize = 1;
			series9->Name = L"Series 1";
			series9->YValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Int32;
			series10->ChartArea = L"ChartArea2";
			series10->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series10->IsXValueIndexed = true;
			series10->Legend = L"Legend1";
			series10->Name = L"Series 2";
			this->graphs_chart->Series->Add(series9);
			this->graphs_chart->Series->Add(series10);
			this->graphs_chart->Size = System::Drawing::Size(894, 613);
			this->graphs_chart->TabIndex = 0;
			this->graphs_chart->Text = L"graphs_chart";
			this->graphs_chart->AxisViewChanged += gcnew System::EventHandler<System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^ >(this, &f1::graphs_chart_AxisViewChanged);
			// 
			// radar_tabPage
			// 
			this->radar_tabPage->Controls->Add(this->label5);
			this->radar_tabPage->Controls->Add(this->label4);
			this->radar_tabPage->Controls->Add(this->label3);
			this->radar_tabPage->Controls->Add(this->label2);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_ne_theta);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_ne_y);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_nw_theta);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_nw_y);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_fn_theta);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_ne_x);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_fn_y);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_nw_x);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_n_theta);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_fn_x);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_n_y);
			this->radar_tabPage->Controls->Add(this->radar_txt_calib_n_x);
			this->radar_tabPage->Controls->Add(this->label6);
			this->radar_tabPage->Controls->Add(this->label1);
			this->radar_tabPage->Controls->Add(this->radar_btn_stop_scan);
			this->radar_tabPage->Controls->Add(this->radar_btn_clear);
			this->radar_tabPage->Controls->Add(this->radar_btn_start_scan);
			this->radar_tabPage->Controls->Add(this->radar_lbl_speed);
			this->radar_tabPage->Controls->Add(this->radar_lbl_scan_range);
			this->radar_tabPage->Controls->Add(this->radar_txt_speed);
			this->radar_tabPage->Controls->Add(this->radar_txt_scan_range);
			this->radar_tabPage->Controls->Add(this->radar_checkBox_enable_updates);
			this->radar_tabPage->Controls->Add(this->radar_checkBox_use_lines);
			this->radar_tabPage->Controls->Add(this->radar_checkBox_show_ir_ne);
			this->radar_tabPage->Controls->Add(this->radar_checkBox_show_ir_nw);
			this->radar_tabPage->Controls->Add(this->radar_checkBox_show_ir_far_north);
			this->radar_tabPage->Controls->Add(this->radar_checkBox_show_ir_north);
			this->radar_tabPage->Location = System::Drawing::Point(4, 22);
			this->radar_tabPage->Name = L"radar_tabPage";
			this->radar_tabPage->Padding = System::Windows::Forms::Padding(3);
			this->radar_tabPage->Size = System::Drawing::Size(1090, 623);
			this->radar_tabPage->TabIndex = 4;
			this->radar_tabPage->Text = L"\"Radar\"";
			this->radar_tabPage->UseVisualStyleBackColor = true;
			this->radar_tabPage->Click += gcnew System::EventHandler(this, &f1::radar_tabPage_Click);
			this->radar_tabPage->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &f1::radar_tabPage_Paint);
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(894, 402);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(22, 13);
			this->label5->TabIndex = 7;
			this->label5->Text = L"NE";
			this->label5->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(894, 376);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(26, 13);
			this->label4->TabIndex = 7;
			this->label4->Text = L"NW";
			this->label4->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(894, 350);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(33, 13);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Far N";
			this->label3->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(894, 324);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(15, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"N";
			this->label2->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_txt_calib_ne_theta
			// 
			this->radar_txt_calib_ne_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_ne_theta->Location = System::Drawing::Point(1029, 399);
			this->radar_txt_calib_ne_theta->Name = L"radar_txt_calib_ne_theta";
			this->radar_txt_calib_ne_theta->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_ne_theta->TabIndex = 6;
			this->radar_txt_calib_ne_theta->Text = L"-45";
			// 
			// radar_txt_calib_ne_y
			// 
			this->radar_txt_calib_ne_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_ne_y->Location = System::Drawing::Point(982, 399);
			this->radar_txt_calib_ne_y->Name = L"radar_txt_calib_ne_y";
			this->radar_txt_calib_ne_y->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_ne_y->TabIndex = 6;
			this->radar_txt_calib_ne_y->Text = L"-50";
			// 
			// radar_txt_calib_nw_theta
			// 
			this->radar_txt_calib_nw_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_nw_theta->Location = System::Drawing::Point(1029, 373);
			this->radar_txt_calib_nw_theta->Name = L"radar_txt_calib_nw_theta";
			this->radar_txt_calib_nw_theta->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_nw_theta->TabIndex = 6;
			this->radar_txt_calib_nw_theta->Text = L"45";
			// 
			// radar_txt_calib_nw_y
			// 
			this->radar_txt_calib_nw_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_nw_y->Location = System::Drawing::Point(982, 373);
			this->radar_txt_calib_nw_y->Name = L"radar_txt_calib_nw_y";
			this->radar_txt_calib_nw_y->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_nw_y->TabIndex = 6;
			this->radar_txt_calib_nw_y->Text = L"50";
			// 
			// radar_txt_calib_fn_theta
			// 
			this->radar_txt_calib_fn_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_fn_theta->Location = System::Drawing::Point(1029, 347);
			this->radar_txt_calib_fn_theta->Name = L"radar_txt_calib_fn_theta";
			this->radar_txt_calib_fn_theta->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_fn_theta->TabIndex = 6;
			this->radar_txt_calib_fn_theta->Text = L"0";
			// 
			// radar_txt_calib_ne_x
			// 
			this->radar_txt_calib_ne_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_ne_x->Location = System::Drawing::Point(935, 399);
			this->radar_txt_calib_ne_x->Name = L"radar_txt_calib_ne_x";
			this->radar_txt_calib_ne_x->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_ne_x->TabIndex = 6;
			this->radar_txt_calib_ne_x->Text = L"50";
			this->radar_txt_calib_ne_x->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_fn_y
			// 
			this->radar_txt_calib_fn_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_fn_y->Location = System::Drawing::Point(982, 347);
			this->radar_txt_calib_fn_y->Name = L"radar_txt_calib_fn_y";
			this->radar_txt_calib_fn_y->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_fn_y->TabIndex = 6;
			this->radar_txt_calib_fn_y->Text = L"0";
			// 
			// radar_txt_calib_nw_x
			// 
			this->radar_txt_calib_nw_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_nw_x->Location = System::Drawing::Point(935, 373);
			this->radar_txt_calib_nw_x->Name = L"radar_txt_calib_nw_x";
			this->radar_txt_calib_nw_x->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_nw_x->TabIndex = 6;
			this->radar_txt_calib_nw_x->Text = L"50";
			// 
			// radar_txt_calib_n_theta
			// 
			this->radar_txt_calib_n_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_n_theta->Location = System::Drawing::Point(1029, 321);
			this->radar_txt_calib_n_theta->Name = L"radar_txt_calib_n_theta";
			this->radar_txt_calib_n_theta->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_n_theta->TabIndex = 6;
			this->radar_txt_calib_n_theta->Text = L"0";
			// 
			// radar_txt_calib_fn_x
			// 
			this->radar_txt_calib_fn_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_fn_x->Location = System::Drawing::Point(935, 347);
			this->radar_txt_calib_fn_x->Name = L"radar_txt_calib_fn_x";
			this->radar_txt_calib_fn_x->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_fn_x->TabIndex = 6;
			this->radar_txt_calib_fn_x->Text = L"60";
			// 
			// radar_txt_calib_n_y
			// 
			this->radar_txt_calib_n_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_n_y->Location = System::Drawing::Point(982, 321);
			this->radar_txt_calib_n_y->Name = L"radar_txt_calib_n_y";
			this->radar_txt_calib_n_y->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_n_y->TabIndex = 6;
			this->radar_txt_calib_n_y->Text = L"0";
			// 
			// radar_txt_calib_n_x
			// 
			this->radar_txt_calib_n_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_n_x->Location = System::Drawing::Point(935, 321);
			this->radar_txt_calib_n_x->Name = L"radar_txt_calib_n_x";
			this->radar_txt_calib_n_x->Size = System::Drawing::Size(41, 20);
			this->radar_txt_calib_n_x->TabIndex = 6;
			this->radar_txt_calib_n_x->Text = L"60";
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(894, 156);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(69, 13);
			this->label6->TabIndex = 5;
			this->label6->Text = L"Make visible:";
			this->label6->Click += gcnew System::EventHandler(this, &f1::label1_Click);
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(894, 302);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(92, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Offsets (x,y,theta):";
			this->label1->Click += gcnew System::EventHandler(this, &f1::label1_Click);
			// 
			// radar_btn_stop_scan
			// 
			this->radar_btn_stop_scan->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_btn_stop_scan->Location = System::Drawing::Point(970, 88);
			this->radar_btn_stop_scan->Name = L"radar_btn_stop_scan";
			this->radar_btn_stop_scan->Size = System::Drawing::Size(67, 23);
			this->radar_btn_stop_scan->TabIndex = 4;
			this->radar_btn_stop_scan->Text = L"Stop";
			this->radar_btn_stop_scan->UseVisualStyleBackColor = true;
			this->radar_btn_stop_scan->Click += gcnew System::EventHandler(this, &f1::radar_btn_stop_scan_Click);
			// 
			// radar_btn_clear
			// 
			this->radar_btn_clear->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_btn_clear->Location = System::Drawing::Point(897, 515);
			this->radar_btn_clear->Name = L"radar_btn_clear";
			this->radar_btn_clear->Size = System::Drawing::Size(100, 23);
			this->radar_btn_clear->TabIndex = 4;
			this->radar_btn_clear->Text = L"Clear";
			this->radar_btn_clear->UseVisualStyleBackColor = true;
			this->radar_btn_clear->Click += gcnew System::EventHandler(this, &f1::radar_btn_start_scan_Click);
			// 
			// radar_btn_start_scan
			// 
			this->radar_btn_start_scan->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_btn_start_scan->Location = System::Drawing::Point(897, 88);
			this->radar_btn_start_scan->Name = L"radar_btn_start_scan";
			this->radar_btn_start_scan->Size = System::Drawing::Size(67, 23);
			this->radar_btn_start_scan->TabIndex = 4;
			this->radar_btn_start_scan->Text = L"Start";
			this->radar_btn_start_scan->UseVisualStyleBackColor = true;
			this->radar_btn_start_scan->Click += gcnew System::EventHandler(this, &f1::radar_btn_start_scan_Click);
			// 
			// radar_lbl_speed
			// 
			this->radar_lbl_speed->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_lbl_speed->AutoSize = true;
			this->radar_lbl_speed->Location = System::Drawing::Point(922, 65);
			this->radar_lbl_speed->Name = L"radar_lbl_speed";
			this->radar_lbl_speed->Size = System::Drawing::Size(41, 13);
			this->radar_lbl_speed->TabIndex = 3;
			this->radar_lbl_speed->Text = L"Speed:";
			this->radar_lbl_speed->Click += gcnew System::EventHandler(this, &f1::radar_lbl_speed_Click);
			// 
			// radar_lbl_scan_range
			// 
			this->radar_lbl_scan_range->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_lbl_scan_range->AutoSize = true;
			this->radar_lbl_scan_range->Location = System::Drawing::Point(893, 39);
			this->radar_lbl_scan_range->Name = L"radar_lbl_scan_range";
			this->radar_lbl_scan_range->Size = System::Drawing::Size(70, 13);
			this->radar_lbl_scan_range->TabIndex = 3;
			this->radar_lbl_scan_range->Text = L"Scan Range:";
			// 
			// radar_txt_speed
			// 
			this->radar_txt_speed->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_speed->Location = System::Drawing::Point(970, 62);
			this->radar_txt_speed->Name = L"radar_txt_speed";
			this->radar_txt_speed->Size = System::Drawing::Size(67, 20);
			this->radar_txt_speed->TabIndex = 2;
			this->radar_txt_speed->Text = L"90";
			this->radar_txt_speed->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_speed_TextChanged);
			// 
			// radar_txt_scan_range
			// 
			this->radar_txt_scan_range->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_scan_range->Location = System::Drawing::Point(970, 36);
			this->radar_txt_scan_range->Name = L"radar_txt_scan_range";
			this->radar_txt_scan_range->Size = System::Drawing::Size(67, 20);
			this->radar_txt_scan_range->TabIndex = 2;
			this->radar_txt_scan_range->Text = L"90";
			// 
			// radar_checkBox_enable_updates
			// 
			this->radar_checkBox_enable_updates->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_enable_updates->AutoSize = true;
			this->radar_checkBox_enable_updates->Location = System::Drawing::Point(897, 492);
			this->radar_checkBox_enable_updates->Name = L"radar_checkBox_enable_updates";
			this->radar_checkBox_enable_updates->Size = System::Drawing::Size(100, 17);
			this->radar_checkBox_enable_updates->TabIndex = 1;
			this->radar_checkBox_enable_updates->Text = L"Enable updates";
			this->radar_checkBox_enable_updates->UseVisualStyleBackColor = true;
			// 
			// radar_checkBox_use_lines
			// 
			this->radar_checkBox_use_lines->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_use_lines->AutoSize = true;
			this->radar_checkBox_use_lines->Location = System::Drawing::Point(897, 469);
			this->radar_checkBox_use_lines->Name = L"radar_checkBox_use_lines";
			this->radar_checkBox_use_lines->Size = System::Drawing::Size(69, 17);
			this->radar_checkBox_use_lines->TabIndex = 1;
			this->radar_checkBox_use_lines->Text = L"Use lines";
			this->radar_checkBox_use_lines->UseVisualStyleBackColor = true;
			// 
			// radar_checkBox_show_ir_ne
			// 
			this->radar_checkBox_show_ir_ne->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_ne->AutoSize = true;
			this->radar_checkBox_show_ir_ne->Location = System::Drawing::Point(897, 244);
			this->radar_checkBox_show_ir_ne->Name = L"radar_checkBox_show_ir_ne";
			this->radar_checkBox_show_ir_ne->Size = System::Drawing::Size(124, 17);
			this->radar_checkBox_show_ir_ne->TabIndex = 1;
			this->radar_checkBox_show_ir_ne->Text = L"IR North East (Right)";
			this->radar_checkBox_show_ir_ne->UseVisualStyleBackColor = true;
			// 
			// radar_checkBox_show_ir_nw
			// 
			this->radar_checkBox_show_ir_nw->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_nw->AutoSize = true;
			this->radar_checkBox_show_ir_nw->Location = System::Drawing::Point(897, 221);
			this->radar_checkBox_show_ir_nw->Name = L"radar_checkBox_show_ir_nw";
			this->radar_checkBox_show_ir_nw->Size = System::Drawing::Size(121, 17);
			this->radar_checkBox_show_ir_nw->TabIndex = 1;
			this->radar_checkBox_show_ir_nw->Text = L"IR North West (Left)";
			this->radar_checkBox_show_ir_nw->UseVisualStyleBackColor = true;
			// 
			// radar_checkBox_show_ir_far_north
			// 
			this->radar_checkBox_show_ir_far_north->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_far_north->AutoSize = true;
			this->radar_checkBox_show_ir_far_north->Location = System::Drawing::Point(897, 198);
			this->radar_checkBox_show_ir_far_north->Name = L"radar_checkBox_show_ir_far_north";
			this->radar_checkBox_show_ir_far_north->Size = System::Drawing::Size(84, 17);
			this->radar_checkBox_show_ir_far_north->TabIndex = 1;
			this->radar_checkBox_show_ir_far_north->Text = L"IR Far North";
			this->radar_checkBox_show_ir_far_north->UseVisualStyleBackColor = true;
			// 
			// radar_checkBox_show_ir_north
			// 
			this->radar_checkBox_show_ir_north->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_north->AutoSize = true;
			this->radar_checkBox_show_ir_north->Location = System::Drawing::Point(897, 175);
			this->radar_checkBox_show_ir_north->Name = L"radar_checkBox_show_ir_north";
			this->radar_checkBox_show_ir_north->Size = System::Drawing::Size(66, 17);
			this->radar_checkBox_show_ir_north->TabIndex = 1;
			this->radar_checkBox_show_ir_north->Text = L"IR North";
			this->radar_checkBox_show_ir_north->UseVisualStyleBackColor = true;
			// 
			// map_tab
			// 
			this->map_tab->Controls->Add(this->map_picture);
			this->map_tab->Location = System::Drawing::Point(4, 22);
			this->map_tab->Name = L"map_tab";
			this->map_tab->Padding = System::Windows::Forms::Padding(3);
			this->map_tab->Size = System::Drawing::Size(1090, 623);
			this->map_tab->TabIndex = 5;
			this->map_tab->Text = L"Map";
			this->map_tab->UseVisualStyleBackColor = true;
			// 
			// map_picture
			// 
			this->map_picture->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->map_picture->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"map_picture.BackgroundImage")));
			this->map_picture->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->map_picture->Location = System::Drawing::Point(0, 0);
			this->map_picture->Name = L"map_picture";
			this->map_picture->Size = System::Drawing::Size(682, 623);
			this->map_picture->TabIndex = 0;
			this->map_picture->TabStop = false;
			// 
			// terminal_tab
			// 
			this->terminal_tab->Controls->Add(this->terminal_txt);
			this->terminal_tab->Location = System::Drawing::Point(4, 22);
			this->terminal_tab->Name = L"terminal_tab";
			this->terminal_tab->Padding = System::Windows::Forms::Padding(3);
			this->terminal_tab->Size = System::Drawing::Size(1090, 623);
			this->terminal_tab->TabIndex = 0;
			this->terminal_tab->Text = L"Terminal";
			this->terminal_tab->UseVisualStyleBackColor = true;
			// 
			// terminal_txt
			// 
			this->terminal_txt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->terminal_txt->Font = (gcnew System::Drawing::Font(L"Courier New", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->terminal_txt->Location = System::Drawing::Point(-4, 0);
			this->terminal_txt->Multiline = true;
			this->terminal_txt->Name = L"terminal_txt";
			this->terminal_txt->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->terminal_txt->Size = System::Drawing::Size(1094, 627);
			this->terminal_txt->TabIndex = 4;
			this->terminal_txt->WordWrap = false;
			this->terminal_txt->TextChanged += gcnew System::EventHandler(this, &f1::terminal_txt_TextChanged);
			this->terminal_txt->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &f1::terminal_txt_KeyDown);
			// 
			// log_tab
			// 
			this->log_tab->Controls->Add(this->log_txt);
			this->log_tab->Location = System::Drawing::Point(4, 22);
			this->log_tab->Name = L"log_tab";
			this->log_tab->Padding = System::Windows::Forms::Padding(3);
			this->log_tab->Size = System::Drawing::Size(1090, 623);
			this->log_tab->TabIndex = 3;
			this->log_tab->Text = L"Log";
			this->log_tab->UseVisualStyleBackColor = true;
			// 
			// log_txt
			// 
			this->log_txt->AcceptsReturn = true;
			this->log_txt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->log_txt->Font = (gcnew System::Drawing::Font(L"Courier New", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->log_txt->Location = System::Drawing::Point(0, 0);
			this->log_txt->Multiline = true;
			this->log_txt->Name = L"log_txt";
			this->log_txt->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->log_txt->Size = System::Drawing::Size(1090, 623);
			this->log_txt->TabIndex = 0;
			this->log_txt->WordWrap = false;
			// 
			// main_textBox_vbatt
			// 
			this->main_textBox_vbatt->Location = System::Drawing::Point(370, 25);
			this->main_textBox_vbatt->Name = L"main_textBox_vbatt";
			this->main_textBox_vbatt->ReadOnly = true;
			this->main_textBox_vbatt->Size = System::Drawing::Size(59, 20);
			this->main_textBox_vbatt->TabIndex = 0;
			this->main_textBox_vbatt->Text = L"0.0V";
			// 
			// main_lbl_battery
			// 
			this->main_lbl_battery->AutoSize = true;
			this->main_lbl_battery->Location = System::Drawing::Point(367, 9);
			this->main_lbl_battery->Name = L"main_lbl_battery";
			this->main_lbl_battery->Size = System::Drawing::Size(43, 13);
			this->main_lbl_battery->TabIndex = 1;
			this->main_lbl_battery->Text = L"Battery:";
			this->main_lbl_battery->Click += gcnew System::EventHandler(this, &f1::main_lbl_battery_Click);
			// 
			// main_textBox_keyb
			// 
			this->main_textBox_keyb->Location = System::Drawing::Point(524, 26);
			this->main_textBox_keyb->Name = L"main_textBox_keyb";
			this->main_textBox_keyb->Size = System::Drawing::Size(77, 20);
			this->main_textBox_keyb->TabIndex = 5;
			this->main_textBox_keyb->TextChanged += gcnew System::EventHandler(this, &f1::main_textBox_keyb_TextChanged);
			this->main_textBox_keyb->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &f1::main_textBox_keyb_KeyDown);
			// 
			// main_lbl_keyb
			// 
			this->main_lbl_keyb->AutoSize = true;
			this->main_lbl_keyb->Location = System::Drawing::Point(521, 8);
			this->main_lbl_keyb->Name = L"main_lbl_keyb";
			this->main_lbl_keyb->Size = System::Drawing::Size(80, 13);
			this->main_lbl_keyb->TabIndex = 1;
			this->main_lbl_keyb->Text = L"Keyb. Shortcut:";
			this->main_lbl_keyb->Click += gcnew System::EventHandler(this, &f1::main_lbl_battery_Click);
			// 
			// main_btn_estop
			// 
			this->main_btn_estop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_estop->BackColor = System::Drawing::Color::Red;
			this->main_btn_estop->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"main_btn_estop.BackgroundImage")));
			this->main_btn_estop->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Stretch;
			this->main_btn_estop->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->main_btn_estop->Location = System::Drawing::Point(1024, 8);
			this->main_btn_estop->Name = L"main_btn_estop";
			this->main_btn_estop->Size = System::Drawing::Size(67, 61);
			this->main_btn_estop->TabIndex = 6;
			this->main_btn_estop->UseVisualStyleBackColor = false;
			this->main_btn_estop->Click += gcnew System::EventHandler(this, &f1::main_btn_estop_Click);
			// 
			// main_btn_up
			// 
			this->main_btn_up->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_up->Location = System::Drawing::Point(948, 4);
			this->main_btn_up->Name = L"main_btn_up";
			this->main_btn_up->Size = System::Drawing::Size(25, 21);
			this->main_btn_up->TabIndex = 7;
			this->main_btn_up->Text = L"u";
			this->main_btn_up->UseVisualStyleBackColor = true;
			// 
			// main_btn_down
			// 
			this->main_btn_down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_down->Location = System::Drawing::Point(948, 50);
			this->main_btn_down->Name = L"main_btn_down";
			this->main_btn_down->Size = System::Drawing::Size(25, 21);
			this->main_btn_down->TabIndex = 7;
			this->main_btn_down->Text = L"d";
			this->main_btn_down->UseVisualStyleBackColor = true;
			// 
			// main_btn_left
			// 
			this->main_btn_left->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_left->Location = System::Drawing::Point(920, 27);
			this->main_btn_left->Name = L"main_btn_left";
			this->main_btn_left->Size = System::Drawing::Size(25, 21);
			this->main_btn_left->TabIndex = 7;
			this->main_btn_left->Text = L"<";
			this->main_btn_left->UseVisualStyleBackColor = true;
			// 
			// main_btn_right
			// 
			this->main_btn_right->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_right->Location = System::Drawing::Point(976, 27);
			this->main_btn_right->Name = L"main_btn_right";
			this->main_btn_right->Size = System::Drawing::Size(25, 21);
			this->main_btn_right->TabIndex = 7;
			this->main_btn_right->Text = L">";
			this->main_btn_right->UseVisualStyleBackColor = true;
			// 
			// main_btn_stop
			// 
			this->main_btn_stop->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->main_btn_stop->Location = System::Drawing::Point(948, 27);
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
			this->main_lbl_behavior_control->Location = System::Drawing::Point(624, 8);
			this->main_lbl_behavior_control->Name = L"main_lbl_behavior_control";
			this->main_lbl_behavior_control->Size = System::Drawing::Size(229, 13);
			this->main_lbl_behavior_control->TabIndex = 8;
			this->main_lbl_behavior_control->Text = L"Behavior:              Initial State:          Command:";
			// 
			// main_comboBox_behavior_id
			// 
			this->main_comboBox_behavior_id->FormattingEnabled = true;
			this->main_comboBox_behavior_id->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"1", L"2", L"3", L"4", L"5", L"6", 
				L"7", L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15", L"16"});
			this->main_comboBox_behavior_id->Location = System::Drawing::Point(625, 27);
			this->main_comboBox_behavior_id->Name = L"main_comboBox_behavior_id";
			this->main_comboBox_behavior_id->Size = System::Drawing::Size(61, 21);
			this->main_comboBox_behavior_id->TabIndex = 9;
			this->main_comboBox_behavior_id->Text = L"1";
			this->main_comboBox_behavior_id->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_behavior_id_SelectedIndexChanged);
			// 
			// main_comboBox_behavior_state
			// 
			this->main_comboBox_behavior_state->FormattingEnabled = true;
			this->main_comboBox_behavior_state->Items->AddRange(gcnew cli::array< System::Object^  >(16) {L"1", L"2", L"3", L"4", L"5", 
				L"6", L"7", L"8", L"9", L"10", L"11", L"12", L"13", L"14", L"15", L"16"});
			this->main_comboBox_behavior_state->Location = System::Drawing::Point(712, 27);
			this->main_comboBox_behavior_state->Name = L"main_comboBox_behavior_state";
			this->main_comboBox_behavior_state->Size = System::Drawing::Size(61, 21);
			this->main_comboBox_behavior_state->TabIndex = 9;
			this->main_comboBox_behavior_state->Text = L"1";
			// 
			// main_btn_start_beh
			// 
			this->main_btn_start_beh->Location = System::Drawing::Point(798, 24);
			this->main_btn_start_beh->Name = L"main_btn_start_beh";
			this->main_btn_start_beh->Size = System::Drawing::Size(42, 23);
			this->main_btn_start_beh->TabIndex = 10;
			this->main_btn_start_beh->Text = L"Start";
			this->main_btn_start_beh->UseVisualStyleBackColor = true;
			this->main_btn_start_beh->Click += gcnew System::EventHandler(this, &f1::main_btn_start_beh_Click);
			// 
			// main_main_btn_stop_beh
			// 
			this->main_main_btn_stop_beh->Location = System::Drawing::Point(846, 24);
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
			this->main_lbl_rate->Location = System::Drawing::Point(439, 8);
			this->main_lbl_rate->Name = L"main_lbl_rate";
			this->main_lbl_rate->Size = System::Drawing::Size(33, 13);
			this->main_lbl_rate->TabIndex = 1;
			this->main_lbl_rate->Text = L"Rate:";
			this->main_lbl_rate->Click += gcnew System::EventHandler(this, &f1::main_lbl_battery_Click);
			// 
			// main_comboBox_rate
			// 
			this->main_comboBox_rate->FormattingEnabled = true;
			this->main_comboBox_rate->Items->AddRange(gcnew cli::array< System::Object^  >(5) {L"0", L"10", L"100", L"1000", L"2000"});
			this->main_comboBox_rate->Location = System::Drawing::Point(442, 25);
			this->main_comboBox_rate->Name = L"main_comboBox_rate";
			this->main_comboBox_rate->Size = System::Drawing::Size(61, 21);
			this->main_comboBox_rate->TabIndex = 9;
			this->main_comboBox_rate->Text = L"0";
			this->main_comboBox_rate->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_behavior_id_SelectedIndexChanged);
			// 
			// serial_timer
			// 
			this->serial_timer->Enabled = true;
			this->serial_timer->Interval = 10;
			this->serial_timer->Tick += gcnew System::EventHandler(this, &f1::serial_timer_Tick);
			// 
			// ui_timer
			// 
			this->ui_timer->Enabled = true;
			this->ui_timer->Interval = 30;
			this->ui_timer->Tick += gcnew System::EventHandler(this, &f1::ui_timer_Tick);
			// 
			// main_lbl_ip
			// 
			this->main_lbl_ip->AutoSize = true;
			this->main_lbl_ip->Location = System::Drawing::Point(141, 25);
			this->main_lbl_ip->Name = L"main_lbl_ip";
			this->main_lbl_ip->Size = System::Drawing::Size(20, 13);
			this->main_lbl_ip->TabIndex = 3;
			this->main_lbl_ip->Text = L"IP:";
			// 
			// main_comboBox_ip
			// 
			this->main_comboBox_ip->FormattingEnabled = true;
			this->main_comboBox_ip->Items->AddRange(gcnew cli::array< System::Object^  >(2) {L"127.0.0.1", L"192.168.2.213"});
			this->main_comboBox_ip->Location = System::Drawing::Point(163, 21);
			this->main_comboBox_ip->Name = L"main_comboBox_ip";
			this->main_comboBox_ip->Size = System::Drawing::Size(77, 21);
			this->main_comboBox_ip->TabIndex = 1;
			this->main_comboBox_ip->Text = L"127.0.0.1";
			this->main_comboBox_ip->SelectedIndexChanged += gcnew System::EventHandler(this, &f1::main_comboBox_ip_SelectedIndexChanged);
			// 
			// main_checkBox_connect_ip
			// 
			this->main_checkBox_connect_ip->AutoSize = true;
			this->main_checkBox_connect_ip->Location = System::Drawing::Point(246, 24);
			this->main_checkBox_connect_ip->Name = L"main_checkBox_connect_ip";
			this->main_checkBox_connect_ip->Size = System::Drawing::Size(15, 14);
			this->main_checkBox_connect_ip->TabIndex = 2;
			this->main_checkBox_connect_ip->UseVisualStyleBackColor = true;
			this->main_checkBox_connect_ip->CheckedChanged += gcnew System::EventHandler(this, &f1::main_checkBox_connect_ip_CheckedChanged);
			// 
			// f1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::Color::DarkSeaGreen;
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
			this->Controls->Add(this->main_lbl_ip);
			this->Controls->Add(this->main_lbl_port);
			this->Controls->Add(this->main_checkBox_connect_ip);
			this->Controls->Add(this->main_checkBox_connect);
			this->Controls->Add(this->main_comboBox_ip);
			this->Controls->Add(this->main_comboBox_port);
			this->Controls->Add(this->main_lbl_keyb);
			this->Controls->Add(this->main_lbl_rate);
			this->Controls->Add(this->main_lbl_battery);
			this->Controls->Add(this->main_textBox_vbatt);
			this->DoubleBuffered = true;
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
			this->map_tab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->map_picture))->EndInit();
			this->terminal_tab->ResumeLayout(false);
			this->terminal_tab->PerformLayout();
			this->log_tab->ResumeLayout(false);
			this->log_tab->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion



	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void serial_timer_Tick(System::Object^  sender, System::EventArgs^  e);

private: System::Void ui_timer_Tick(System::Object^  sender, System::EventArgs^  e) 
		 {
			 char s[500];
			 int i;
			 if(log_read_index != log_write_index)
			 {
				 i=0;
				 while( (log_read_index != log_write_index)  && (i<499) )
				 {
					 s[i]=log_buffer[log_read_index];
					 log_read_index++;
					 if(s[i] == 10)
					 {
						 s[i++] = 0;
						 log_txt->AppendText(gcnew String(s));
						 log_txt->AppendText(System::Environment::NewLine);
						 i=0;
					 }
					 else i++;
					 if(log_read_index>=LOG_BUFFER_SIZE) log_read_index=0;
				 }
				 s[i]=0;
				 log_txt->AppendText(gcnew String(s));
			 }
		 }
private: System::Void radar_txt_calib_ne_x_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
};


}

