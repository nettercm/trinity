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
	extern t_inputs inputs_history[INPUTS_HISTORY_SIZE];
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


	private: System::Windows::Forms::TabPage^  map_tab;
	private: System::Windows::Forms::PictureBox^  map_picture;
	private: System::Windows::Forms::Timer^  serial_timer;
	private: System::Windows::Forms::Timer^  ui_timer;
	private: System::Windows::Forms::Label^  main_lbl_ip;
	private: System::Windows::Forms::ComboBox^  main_comboBox_ip;
	private: System::Windows::Forms::CheckBox^  main_checkBox_connect_ip;
	private: System::Windows::Forms::Label^  label9;
	private: System::Windows::Forms::Label^  label8;
	private: System::Windows::Forms::Label^  label7;
	private: System::Windows::Forms::Label^  label6;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_7_theta;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_7_y;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_6_theta;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_6_y;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_5_theta;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_7_x;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_5_y;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_6_x;

	private: System::Windows::Forms::TextBox^  radar_txt_calib_4_theta;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_5_x;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_4_y;
	private: System::Windows::Forms::TextBox^  radar_txt_calib_4_x;
	private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_7;
	private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_6;
private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_5;
private: System::Windows::Forms::CheckBox^  radar_checkBox_show_ir_4;
private: System::Windows::Forms::CheckBox^  log_txt_enabled;
private: System::Windows::Forms::TabPage^  capture_tab;
private: System::Windows::Forms::Button^  capture_btn_stop;
private: System::Windows::Forms::Button^  capture_btn_start;
private: System::Windows::Forms::ComboBox^  capture_comboBox_path;

private: System::Windows::Forms::Timer^  capture_timer;

private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::TextBox^  radar_textBox_max;
private: System::Windows::Forms::TextBox^  radar_textBox_dec;
private: System::Windows::Forms::TextBox^  radar_textBox_inc;
private: System::Windows::Forms::CheckBox^  radar_CheckBox_immediate_update;
private: System::Windows::Forms::CheckBox^  radar_checkBox_DrawRobot;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::TextBox^  radar_textBox_angle;
private: System::Windows::Forms::CheckBox^  radar_checkBox_grid;
private: System::Windows::Forms::Panel^  panel1;
private: System::Windows::Forms::CheckBox^  radar_CheckBox_ignore_odometry;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::TextBox^  radar_textBox_tsd;
private: System::Windows::Forms::TextBox^  radar_textBox_alfa;
private: System::Windows::Forms::TextBox^  radar_textBox_td;
private: System::Windows::Forms::TextBox^  radar_textBox_line;






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

	private: System::Text::StringBuilder sb;


	public: void UpdateChart_method(String^ series, double x, double y);
	public: void UpdateUI_method(String ^str);
	public: void f1::Update_log_txt_method(String ^str);
	public: void f1::Update_terminal_txt_method(String ^str);

	public: void InitializeParametersTab(void);
	public: void UpdateRadar(float theta, int measurement);
	public: void clear_grid(void);
	public: void update_grid(float d, float x0, float y0, float t0,float x1, float y1, float t1);

	public: void ShowRadar(int ex, int ey);
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

	private: System::Void radar_btn_Clear(System::Object^  sender, System::EventArgs^  e);
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
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea1 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::ChartArea^  chartArea2 = (gcnew System::Windows::Forms::DataVisualization::Charting::ChartArea());
			System::Windows::Forms::DataVisualization::Charting::Legend^  legend1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Legend());
			System::Windows::Forms::DataVisualization::Charting::Series^  series1 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series2 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
			System::Windows::Forms::DataVisualization::Charting::Series^  series3 = (gcnew System::Windows::Forms::DataVisualization::Charting::Series());
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
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->radar_btn_clear = (gcnew System::Windows::Forms::Button());
			this->radar_checkBox_show_ir_north = (gcnew System::Windows::Forms::CheckBox());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_far_north = (gcnew System::Windows::Forms::CheckBox());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_4 = (gcnew System::Windows::Forms::CheckBox());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_nw = (gcnew System::Windows::Forms::CheckBox());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_5 = (gcnew System::Windows::Forms::CheckBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_ne = (gcnew System::Windows::Forms::CheckBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_6 = (gcnew System::Windows::Forms::CheckBox());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_use_lines = (gcnew System::Windows::Forms::CheckBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_DrawRobot = (gcnew System::Windows::Forms::CheckBox());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->radar_CheckBox_ignore_odometry = (gcnew System::Windows::Forms::CheckBox());
			this->radar_CheckBox_immediate_update = (gcnew System::Windows::Forms::CheckBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_grid = (gcnew System::Windows::Forms::CheckBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_show_ir_7 = (gcnew System::Windows::Forms::CheckBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->radar_checkBox_enable_updates = (gcnew System::Windows::Forms::CheckBox());
			this->radar_txt_calib_7_theta = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->radar_txt_calib_ne_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_n_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_7_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_n_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_6_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_4_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_ne_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_fn_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_6_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_4_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_nw_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_n_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_5_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_5_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_textBox_angle = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_nw_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_nw_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_4_theta = (gcnew System::Windows::Forms::TextBox());
			this->radar_textBox_max = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_fn_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_textBox_dec = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_6_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_textBox_inc = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_ne_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_7_x = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_5_y = (gcnew System::Windows::Forms::TextBox());
			this->radar_txt_calib_fn_theta = (gcnew System::Windows::Forms::TextBox());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->map_tab = (gcnew System::Windows::Forms::TabPage());
			this->map_picture = (gcnew System::Windows::Forms::PictureBox());
			this->terminal_tab = (gcnew System::Windows::Forms::TabPage());
			this->terminal_txt = (gcnew System::Windows::Forms::TextBox());
			this->log_tab = (gcnew System::Windows::Forms::TabPage());
			this->log_txt_enabled = (gcnew System::Windows::Forms::CheckBox());
			this->log_txt = (gcnew System::Windows::Forms::TextBox());
			this->capture_tab = (gcnew System::Windows::Forms::TabPage());
			this->capture_comboBox_path = (gcnew System::Windows::Forms::ComboBox());
			this->capture_btn_stop = (gcnew System::Windows::Forms::Button());
			this->capture_btn_start = (gcnew System::Windows::Forms::Button());
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
			this->capture_timer = (gcnew System::Windows::Forms::Timer(this->components));
			this->radar_textBox_td = (gcnew System::Windows::Forms::TextBox());
			this->radar_textBox_alfa = (gcnew System::Windows::Forms::TextBox());
			this->radar_textBox_tsd = (gcnew System::Windows::Forms::TextBox());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->radar_textBox_line = (gcnew System::Windows::Forms::TextBox());
			this->tabControl1->SuspendLayout();
			this->parameters_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->parameters_dataGridView))->BeginInit();
			this->graphs_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->graphs_chart))->BeginInit();
			this->radar_tabPage->SuspendLayout();
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			this->map_tab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->map_picture))->BeginInit();
			this->terminal_tab->SuspendLayout();
			this->log_tab->SuspendLayout();
			this->capture_tab->SuspendLayout();
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
			this->main_comboBox_port->Items->AddRange(gcnew cli::array< System::Object^  >(11) {L"4", L"7", L"8", L"9", L"10", L"11", 
				L"12", L"13", L"19", L"20", L"55"});
			this->main_comboBox_port->Location = System::Drawing::Point(51, 20);
			this->main_comboBox_port->Name = L"main_comboBox_port";
			this->main_comboBox_port->Size = System::Drawing::Size(34, 21);
			this->main_comboBox_port->TabIndex = 1;
			this->main_comboBox_port->Text = L"27";
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
			this->tabControl1->Controls->Add(this->capture_tab);
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
			this->parameters_tab->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
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
			this->graphs_tab->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
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
			series1->Name = L"Series 1a";
			series1->YValueType = System::Windows::Forms::DataVisualization::Charting::ChartValueType::Int32;
			series2->ChartArea = L"ChartArea2";
			series2->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series2->Legend = L"Legend1";
			series2->Name = L"Series 2a";
			series3->ChartArea = L"ChartArea1";
			series3->ChartType = System::Windows::Forms::DataVisualization::Charting::SeriesChartType::FastLine;
			series3->Legend = L"Legend1";
			series3->Name = L"Series 1b";
			this->graphs_chart->Series->Add(series1);
			this->graphs_chart->Series->Add(series2);
			this->graphs_chart->Series->Add(series3);
			this->graphs_chart->Size = System::Drawing::Size(894, 613);
			this->graphs_chart->TabIndex = 0;
			this->graphs_chart->Text = L"graphs_chart";
			this->graphs_chart->AxisViewChanged += gcnew System::EventHandler<System::Windows::Forms::DataVisualization::Charting::ViewEventArgs^ >(this, &f1::graphs_chart_AxisViewChanged);
			// 
			// radar_tabPage
			// 
			this->radar_tabPage->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->radar_tabPage->Controls->Add(this->panel1);
			this->radar_tabPage->Controls->Add(this->pictureBox1);
			this->radar_tabPage->Location = System::Drawing::Point(4, 22);
			this->radar_tabPage->Name = L"radar_tabPage";
			this->radar_tabPage->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
			this->radar_tabPage->Size = System::Drawing::Size(1090, 623);
			this->radar_tabPage->TabIndex = 4;
			this->radar_tabPage->Text = L"Grid";
			this->radar_tabPage->UseVisualStyleBackColor = true;
			this->radar_tabPage->Click += gcnew System::EventHandler(this, &f1::radar_tabPage_Click);
			this->radar_tabPage->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &f1::radar_tabPage_Paint);
			// 
			// panel1
			// 
			this->panel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panel1->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(224)), static_cast<System::Int32>(static_cast<System::Byte>(224)), 
				static_cast<System::Int32>(static_cast<System::Byte>(224)));
			this->panel1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->panel1->Controls->Add(this->radar_btn_clear);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_north);
			this->panel1->Controls->Add(this->label16);
			this->panel1->Controls->Add(this->label12);
			this->panel1->Controls->Add(this->label15);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_far_north);
			this->panel1->Controls->Add(this->label11);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_4);
			this->panel1->Controls->Add(this->label13);
			this->panel1->Controls->Add(this->label14);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_nw);
			this->panel1->Controls->Add(this->label10);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_5);
			this->panel1->Controls->Add(this->label9);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_ne);
			this->panel1->Controls->Add(this->label5);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_6);
			this->panel1->Controls->Add(this->label8);
			this->panel1->Controls->Add(this->radar_checkBox_use_lines);
			this->panel1->Controls->Add(this->label4);
			this->panel1->Controls->Add(this->radar_checkBox_DrawRobot);
			this->panel1->Controls->Add(this->label7);
			this->panel1->Controls->Add(this->radar_CheckBox_ignore_odometry);
			this->panel1->Controls->Add(this->radar_CheckBox_immediate_update);
			this->panel1->Controls->Add(this->label3);
			this->panel1->Controls->Add(this->radar_checkBox_grid);
			this->panel1->Controls->Add(this->label6);
			this->panel1->Controls->Add(this->radar_checkBox_show_ir_7);
			this->panel1->Controls->Add(this->label2);
			this->panel1->Controls->Add(this->radar_checkBox_enable_updates);
			this->panel1->Controls->Add(this->radar_txt_calib_7_theta);
			this->panel1->Controls->Add(this->label1);
			this->panel1->Controls->Add(this->radar_txt_calib_ne_theta);
			this->panel1->Controls->Add(this->radar_txt_calib_n_x);
			this->panel1->Controls->Add(this->radar_txt_calib_7_y);
			this->panel1->Controls->Add(this->radar_txt_calib_n_y);
			this->panel1->Controls->Add(this->radar_txt_calib_6_theta);
			this->panel1->Controls->Add(this->radar_txt_calib_4_x);
			this->panel1->Controls->Add(this->radar_txt_calib_ne_y);
			this->panel1->Controls->Add(this->radar_txt_calib_fn_x);
			this->panel1->Controls->Add(this->radar_txt_calib_6_y);
			this->panel1->Controls->Add(this->radar_txt_calib_4_y);
			this->panel1->Controls->Add(this->radar_txt_calib_nw_theta);
			this->panel1->Controls->Add(this->radar_txt_calib_n_theta);
			this->panel1->Controls->Add(this->radar_txt_calib_5_theta);
			this->panel1->Controls->Add(this->radar_txt_calib_5_x);
			this->panel1->Controls->Add(this->radar_textBox_line);
			this->panel1->Controls->Add(this->radar_textBox_angle);
			this->panel1->Controls->Add(this->radar_txt_calib_nw_x);
			this->panel1->Controls->Add(this->radar_txt_calib_nw_y);
			this->panel1->Controls->Add(this->radar_textBox_tsd);
			this->panel1->Controls->Add(this->radar_txt_calib_4_theta);
			this->panel1->Controls->Add(this->radar_textBox_max);
			this->panel1->Controls->Add(this->radar_textBox_alfa);
			this->panel1->Controls->Add(this->radar_txt_calib_fn_y);
			this->panel1->Controls->Add(this->radar_textBox_dec);
			this->panel1->Controls->Add(this->radar_textBox_td);
			this->panel1->Controls->Add(this->radar_txt_calib_6_x);
			this->panel1->Controls->Add(this->radar_textBox_inc);
			this->panel1->Controls->Add(this->radar_txt_calib_ne_x);
			this->panel1->Controls->Add(this->radar_txt_calib_7_x);
			this->panel1->Controls->Add(this->radar_txt_calib_5_y);
			this->panel1->Controls->Add(this->radar_txt_calib_fn_theta);
			this->panel1->Location = System::Drawing::Point(882, -1);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(207, 623);
			this->panel1->TabIndex = 10;
			// 
			// radar_btn_clear
			// 
			this->radar_btn_clear->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_btn_clear->Location = System::Drawing::Point(28, 590);
			this->radar_btn_clear->Name = L"radar_btn_clear";
			this->radar_btn_clear->Size = System::Drawing::Size(100, 23);
			this->radar_btn_clear->TabIndex = 4;
			this->radar_btn_clear->Text = L"Clear";
			this->radar_btn_clear->UseVisualStyleBackColor = true;
			this->radar_btn_clear->Click += gcnew System::EventHandler(this, &f1::radar_btn_Clear);
			// 
			// radar_checkBox_show_ir_north
			// 
			this->radar_checkBox_show_ir_north->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_north->AutoSize = true;
			this->radar_checkBox_show_ir_north->Location = System::Drawing::Point(167, 25);
			this->radar_checkBox_show_ir_north->Name = L"radar_checkBox_show_ir_north";
			this->radar_checkBox_show_ir_north->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_north->TabIndex = 1;
			this->radar_checkBox_show_ir_north->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_north->Visible = false;
			// 
			// label12
			// 
			this->label12->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label12->AutoSize = true;
			this->label12->Location = System::Drawing::Point(63, 323);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(54, 13);
			this->label12->TabIndex = 7;
			this->label12->Text = L"Increment";
			this->label12->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_far_north
			// 
			this->radar_checkBox_show_ir_far_north->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_far_north->AutoSize = true;
			this->radar_checkBox_show_ir_far_north->Location = System::Drawing::Point(167, 51);
			this->radar_checkBox_show_ir_far_north->Name = L"radar_checkBox_show_ir_far_north";
			this->radar_checkBox_show_ir_far_north->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_far_north->TabIndex = 1;
			this->radar_checkBox_show_ir_far_north->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_far_north->Visible = false;
			// 
			// label11
			// 
			this->label11->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label11->AutoSize = true;
			this->label11->Location = System::Drawing::Point(63, 348);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(59, 13);
			this->label11->TabIndex = 7;
			this->label11->Text = L"Decrement";
			this->label11->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_4
			// 
			this->radar_checkBox_show_ir_4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_4->AutoSize = true;
			this->radar_checkBox_show_ir_4->Location = System::Drawing::Point(167, 129);
			this->radar_checkBox_show_ir_4->Name = L"radar_checkBox_show_ir_4";
			this->radar_checkBox_show_ir_4->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_4->TabIndex = 1;
			this->radar_checkBox_show_ir_4->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_4->Visible = false;
			// 
			// label13
			// 
			this->label13->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label13->AutoSize = true;
			this->label13->Location = System::Drawing::Point(63, 396);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(34, 13);
			this->label13->TabIndex = 7;
			this->label13->Text = L"Angle";
			this->label13->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_nw
			// 
			this->radar_checkBox_show_ir_nw->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_nw->AutoSize = true;
			this->radar_checkBox_show_ir_nw->Location = System::Drawing::Point(167, 76);
			this->radar_checkBox_show_ir_nw->Name = L"radar_checkBox_show_ir_nw";
			this->radar_checkBox_show_ir_nw->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_nw->TabIndex = 1;
			this->radar_checkBox_show_ir_nw->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_nw->Visible = false;
			// 
			// label10
			// 
			this->label10->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(63, 372);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(51, 13);
			this->label10->TabIndex = 7;
			this->label10->Text = L"Maximum";
			this->label10->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_5
			// 
			this->radar_checkBox_show_ir_5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_5->AutoSize = true;
			this->radar_checkBox_show_ir_5->Location = System::Drawing::Point(167, 155);
			this->radar_checkBox_show_ir_5->Name = L"radar_checkBox_show_ir_5";
			this->radar_checkBox_show_ir_5->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_5->TabIndex = 1;
			this->radar_checkBox_show_ir_5->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_5->Visible = false;
			// 
			// label9
			// 
			this->label9->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(24, 207);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(27, 13);
			this->label9->TabIndex = 7;
			this->label9->Text = L"IR 7";
			this->label9->Visible = false;
			this->label9->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_ne
			// 
			this->radar_checkBox_show_ir_ne->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_ne->AutoSize = true;
			this->radar_checkBox_show_ir_ne->Location = System::Drawing::Point(167, 102);
			this->radar_checkBox_show_ir_ne->Name = L"radar_checkBox_show_ir_ne";
			this->radar_checkBox_show_ir_ne->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_ne->TabIndex = 1;
			this->radar_checkBox_show_ir_ne->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_ne->Visible = false;
			// 
			// label5
			// 
			this->label5->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(24, 103);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(22, 13);
			this->label5->TabIndex = 7;
			this->label5->Text = L"NE";
			this->label5->Visible = false;
			this->label5->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_6
			// 
			this->radar_checkBox_show_ir_6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_6->AutoSize = true;
			this->radar_checkBox_show_ir_6->Location = System::Drawing::Point(167, 180);
			this->radar_checkBox_show_ir_6->Name = L"radar_checkBox_show_ir_6";
			this->radar_checkBox_show_ir_6->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_6->TabIndex = 1;
			this->radar_checkBox_show_ir_6->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_6->Visible = false;
			// 
			// label8
			// 
			this->label8->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(24, 181);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(27, 13);
			this->label8->TabIndex = 7;
			this->label8->Text = L"IR 6";
			this->label8->Visible = false;
			this->label8->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_use_lines
			// 
			this->radar_checkBox_use_lines->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_use_lines->AutoSize = true;
			this->radar_checkBox_use_lines->Location = System::Drawing::Point(28, 532);
			this->radar_checkBox_use_lines->Name = L"radar_checkBox_use_lines";
			this->radar_checkBox_use_lines->Size = System::Drawing::Size(69, 17);
			this->radar_checkBox_use_lines->TabIndex = 1;
			this->radar_checkBox_use_lines->Text = L"Use lines";
			this->radar_checkBox_use_lines->UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			this->label4->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(24, 77);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(26, 13);
			this->label4->TabIndex = 7;
			this->label4->Text = L"NW";
			this->label4->Visible = false;
			this->label4->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_DrawRobot
			// 
			this->radar_checkBox_DrawRobot->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_DrawRobot->AutoSize = true;
			this->radar_checkBox_DrawRobot->Location = System::Drawing::Point(28, 507);
			this->radar_checkBox_DrawRobot->Name = L"radar_checkBox_DrawRobot";
			this->radar_checkBox_DrawRobot->Size = System::Drawing::Size(83, 17);
			this->radar_checkBox_DrawRobot->TabIndex = 1;
			this->radar_checkBox_DrawRobot->Text = L"Draw Robot";
			this->radar_checkBox_DrawRobot->UseVisualStyleBackColor = true;
			// 
			// label7
			// 
			this->label7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(24, 155);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(27, 13);
			this->label7->TabIndex = 7;
			this->label7->Text = L"IR 5";
			this->label7->Visible = false;
			this->label7->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_CheckBox_ignore_odometry
			// 
			this->radar_CheckBox_ignore_odometry->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_CheckBox_ignore_odometry->AutoSize = true;
			this->radar_CheckBox_ignore_odometry->Location = System::Drawing::Point(27, 461);
			this->radar_CheckBox_ignore_odometry->Name = L"radar_CheckBox_ignore_odometry";
			this->radar_CheckBox_ignore_odometry->Size = System::Drawing::Size(102, 17);
			this->radar_CheckBox_ignore_odometry->TabIndex = 1;
			this->radar_CheckBox_ignore_odometry->Text = L"Ignore odometry";
			this->radar_CheckBox_ignore_odometry->UseVisualStyleBackColor = true;
			// 
			// radar_CheckBox_immediate_update
			// 
			this->radar_CheckBox_immediate_update->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_CheckBox_immediate_update->AutoSize = true;
			this->radar_CheckBox_immediate_update->Location = System::Drawing::Point(28, 485);
			this->radar_CheckBox_immediate_update->Name = L"radar_CheckBox_immediate_update";
			this->radar_CheckBox_immediate_update->Size = System::Drawing::Size(112, 17);
			this->radar_CheckBox_immediate_update->TabIndex = 1;
			this->radar_CheckBox_immediate_update->Text = L"Immediate Update";
			this->radar_CheckBox_immediate_update->UseVisualStyleBackColor = true;
			// 
			// label3
			// 
			this->label3->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(24, 51);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(33, 13);
			this->label3->TabIndex = 7;
			this->label3->Text = L"Far N";
			this->label3->Visible = false;
			this->label3->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_grid
			// 
			this->radar_checkBox_grid->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_grid->AutoSize = true;
			this->radar_checkBox_grid->Location = System::Drawing::Point(28, 438);
			this->radar_checkBox_grid->Name = L"radar_checkBox_grid";
			this->radar_checkBox_grid->Size = System::Drawing::Size(45, 17);
			this->radar_checkBox_grid->TabIndex = 1;
			this->radar_checkBox_grid->Text = L"Grid";
			this->radar_checkBox_grid->UseVisualStyleBackColor = true;
			// 
			// label6
			// 
			this->label6->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(24, 129);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(27, 13);
			this->label6->TabIndex = 7;
			this->label6->Text = L"IR 4";
			this->label6->Visible = false;
			this->label6->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_show_ir_7
			// 
			this->radar_checkBox_show_ir_7->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_show_ir_7->AutoSize = true;
			this->radar_checkBox_show_ir_7->Location = System::Drawing::Point(167, 206);
			this->radar_checkBox_show_ir_7->Name = L"radar_checkBox_show_ir_7";
			this->radar_checkBox_show_ir_7->Size = System::Drawing::Size(15, 14);
			this->radar_checkBox_show_ir_7->TabIndex = 1;
			this->radar_checkBox_show_ir_7->UseVisualStyleBackColor = true;
			this->radar_checkBox_show_ir_7->Visible = false;
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(24, 25);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(15, 13);
			this->label2->TabIndex = 7;
			this->label2->Text = L"N";
			this->label2->Visible = false;
			this->label2->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_checkBox_enable_updates
			// 
			this->radar_checkBox_enable_updates->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_checkBox_enable_updates->AutoSize = true;
			this->radar_checkBox_enable_updates->Location = System::Drawing::Point(28, 555);
			this->radar_checkBox_enable_updates->Name = L"radar_checkBox_enable_updates";
			this->radar_checkBox_enable_updates->Size = System::Drawing::Size(100, 17);
			this->radar_checkBox_enable_updates->TabIndex = 1;
			this->radar_checkBox_enable_updates->Text = L"Enable updates";
			this->radar_checkBox_enable_updates->UseVisualStyleBackColor = true;
			// 
			// radar_txt_calib_7_theta
			// 
			this->radar_txt_calib_7_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_7_theta->Location = System::Drawing::Point(133, 204);
			this->radar_txt_calib_7_theta->Name = L"radar_txt_calib_7_theta";
			this->radar_txt_calib_7_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_7_theta->TabIndex = 6;
			this->radar_txt_calib_7_theta->Text = L"90";
			this->radar_txt_calib_7_theta->Visible = false;
			// 
			// label1
			// 
			this->label1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(24, 3);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(92, 13);
			this->label1->TabIndex = 5;
			this->label1->Text = L"Offsets (x,y,theta):";
			this->label1->Visible = false;
			this->label1->Click += gcnew System::EventHandler(this, &f1::label1_Click);
			// 
			// radar_txt_calib_ne_theta
			// 
			this->radar_txt_calib_ne_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_ne_theta->Location = System::Drawing::Point(133, 100);
			this->radar_txt_calib_ne_theta->Name = L"radar_txt_calib_ne_theta";
			this->radar_txt_calib_ne_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_ne_theta->TabIndex = 6;
			this->radar_txt_calib_ne_theta->Text = L"-45";
			this->radar_txt_calib_ne_theta->Visible = false;
			// 
			// radar_txt_calib_n_x
			// 
			this->radar_txt_calib_n_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_n_x->Location = System::Drawing::Point(65, 22);
			this->radar_txt_calib_n_x->Name = L"radar_txt_calib_n_x";
			this->radar_txt_calib_n_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_n_x->TabIndex = 6;
			this->radar_txt_calib_n_x->Text = L"70";
			this->radar_txt_calib_n_x->Visible = false;
			// 
			// radar_txt_calib_7_y
			// 
			this->radar_txt_calib_7_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_7_y->Location = System::Drawing::Point(99, 204);
			this->radar_txt_calib_7_y->Name = L"radar_txt_calib_7_y";
			this->radar_txt_calib_7_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_7_y->TabIndex = 6;
			this->radar_txt_calib_7_y->Text = L"60";
			this->radar_txt_calib_7_y->Visible = false;
			// 
			// radar_txt_calib_n_y
			// 
			this->radar_txt_calib_n_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_n_y->Location = System::Drawing::Point(99, 22);
			this->radar_txt_calib_n_y->Name = L"radar_txt_calib_n_y";
			this->radar_txt_calib_n_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_n_y->TabIndex = 6;
			this->radar_txt_calib_n_y->Text = L"0";
			this->radar_txt_calib_n_y->Visible = false;
			// 
			// radar_txt_calib_6_theta
			// 
			this->radar_txt_calib_6_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_6_theta->Location = System::Drawing::Point(133, 178);
			this->radar_txt_calib_6_theta->Name = L"radar_txt_calib_6_theta";
			this->radar_txt_calib_6_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_6_theta->TabIndex = 6;
			this->radar_txt_calib_6_theta->Text = L"135";
			this->radar_txt_calib_6_theta->Visible = false;
			// 
			// radar_txt_calib_4_x
			// 
			this->radar_txt_calib_4_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_4_x->Location = System::Drawing::Point(65, 126);
			this->radar_txt_calib_4_x->Name = L"radar_txt_calib_4_x";
			this->radar_txt_calib_4_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_4_x->TabIndex = 6;
			this->radar_txt_calib_4_x->Text = L"0";
			this->radar_txt_calib_4_x->Visible = false;
			// 
			// radar_txt_calib_ne_y
			// 
			this->radar_txt_calib_ne_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_ne_y->Location = System::Drawing::Point(99, 100);
			this->radar_txt_calib_ne_y->Name = L"radar_txt_calib_ne_y";
			this->radar_txt_calib_ne_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_ne_y->TabIndex = 6;
			this->radar_txt_calib_ne_y->Text = L"-50";
			this->radar_txt_calib_ne_y->Visible = false;
			// 
			// radar_txt_calib_fn_x
			// 
			this->radar_txt_calib_fn_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_fn_x->Location = System::Drawing::Point(65, 48);
			this->radar_txt_calib_fn_x->Name = L"radar_txt_calib_fn_x";
			this->radar_txt_calib_fn_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_fn_x->TabIndex = 6;
			this->radar_txt_calib_fn_x->Text = L"70";
			this->radar_txt_calib_fn_x->Visible = false;
			// 
			// radar_txt_calib_6_y
			// 
			this->radar_txt_calib_6_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_6_y->Location = System::Drawing::Point(99, 178);
			this->radar_txt_calib_6_y->Name = L"radar_txt_calib_6_y";
			this->radar_txt_calib_6_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_6_y->TabIndex = 6;
			this->radar_txt_calib_6_y->Text = L"50";
			this->radar_txt_calib_6_y->Visible = false;
			// 
			// radar_txt_calib_4_y
			// 
			this->radar_txt_calib_4_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_4_y->Location = System::Drawing::Point(99, 126);
			this->radar_txt_calib_4_y->Name = L"radar_txt_calib_4_y";
			this->radar_txt_calib_4_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_4_y->TabIndex = 6;
			this->radar_txt_calib_4_y->Text = L"-60";
			this->radar_txt_calib_4_y->Visible = false;
			// 
			// radar_txt_calib_nw_theta
			// 
			this->radar_txt_calib_nw_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_nw_theta->Location = System::Drawing::Point(133, 74);
			this->radar_txt_calib_nw_theta->Name = L"radar_txt_calib_nw_theta";
			this->radar_txt_calib_nw_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_nw_theta->TabIndex = 6;
			this->radar_txt_calib_nw_theta->Text = L"45";
			this->radar_txt_calib_nw_theta->Visible = false;
			// 
			// radar_txt_calib_n_theta
			// 
			this->radar_txt_calib_n_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_n_theta->Location = System::Drawing::Point(133, 22);
			this->radar_txt_calib_n_theta->Name = L"radar_txt_calib_n_theta";
			this->radar_txt_calib_n_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_n_theta->TabIndex = 6;
			this->radar_txt_calib_n_theta->Text = L"0";
			this->radar_txt_calib_n_theta->Visible = false;
			// 
			// radar_txt_calib_5_theta
			// 
			this->radar_txt_calib_5_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_5_theta->Location = System::Drawing::Point(133, 152);
			this->radar_txt_calib_5_theta->Name = L"radar_txt_calib_5_theta";
			this->radar_txt_calib_5_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_5_theta->TabIndex = 6;
			this->radar_txt_calib_5_theta->Text = L"-135";
			this->radar_txt_calib_5_theta->Visible = false;
			// 
			// radar_txt_calib_5_x
			// 
			this->radar_txt_calib_5_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_5_x->Location = System::Drawing::Point(65, 152);
			this->radar_txt_calib_5_x->Name = L"radar_txt_calib_5_x";
			this->radar_txt_calib_5_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_5_x->TabIndex = 6;
			this->radar_txt_calib_5_x->Text = L"-50";
			this->radar_txt_calib_5_x->Visible = false;
			// 
			// radar_textBox_angle
			// 
			this->radar_textBox_angle->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_angle->Location = System::Drawing::Point(27, 394);
			this->radar_textBox_angle->Name = L"radar_textBox_angle";
			this->radar_textBox_angle->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_angle->TabIndex = 6;
			this->radar_textBox_angle->Text = L"180";
			this->radar_textBox_angle->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_nw_x
			// 
			this->radar_txt_calib_nw_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_nw_x->Location = System::Drawing::Point(65, 74);
			this->radar_txt_calib_nw_x->Name = L"radar_txt_calib_nw_x";
			this->radar_txt_calib_nw_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_nw_x->TabIndex = 6;
			this->radar_txt_calib_nw_x->Text = L"50";
			this->radar_txt_calib_nw_x->Visible = false;
			// 
			// radar_txt_calib_nw_y
			// 
			this->radar_txt_calib_nw_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_nw_y->Location = System::Drawing::Point(99, 74);
			this->radar_txt_calib_nw_y->Name = L"radar_txt_calib_nw_y";
			this->radar_txt_calib_nw_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_nw_y->TabIndex = 6;
			this->radar_txt_calib_nw_y->Text = L"50";
			this->radar_txt_calib_nw_y->Visible = false;
			// 
			// radar_txt_calib_4_theta
			// 
			this->radar_txt_calib_4_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_4_theta->Location = System::Drawing::Point(133, 126);
			this->radar_txt_calib_4_theta->Name = L"radar_txt_calib_4_theta";
			this->radar_txt_calib_4_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_4_theta->TabIndex = 6;
			this->radar_txt_calib_4_theta->Text = L"-90";
			this->radar_txt_calib_4_theta->Visible = false;
			// 
			// radar_textBox_max
			// 
			this->radar_textBox_max->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_max->Location = System::Drawing::Point(27, 370);
			this->radar_textBox_max->Name = L"radar_textBox_max";
			this->radar_textBox_max->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_max->TabIndex = 6;
			this->radar_textBox_max->Text = L"2";
			this->radar_textBox_max->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_fn_y
			// 
			this->radar_txt_calib_fn_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_fn_y->Location = System::Drawing::Point(99, 48);
			this->radar_txt_calib_fn_y->Name = L"radar_txt_calib_fn_y";
			this->radar_txt_calib_fn_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_fn_y->TabIndex = 6;
			this->radar_txt_calib_fn_y->Text = L"0";
			this->radar_txt_calib_fn_y->Visible = false;
			// 
			// radar_textBox_dec
			// 
			this->radar_textBox_dec->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_dec->Location = System::Drawing::Point(27, 345);
			this->radar_textBox_dec->Name = L"radar_textBox_dec";
			this->radar_textBox_dec->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_dec->TabIndex = 6;
			this->radar_textBox_dec->Text = L"-1";
			this->radar_textBox_dec->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_6_x
			// 
			this->radar_txt_calib_6_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_6_x->Location = System::Drawing::Point(65, 178);
			this->radar_txt_calib_6_x->Name = L"radar_txt_calib_6_x";
			this->radar_txt_calib_6_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_6_x->TabIndex = 6;
			this->radar_txt_calib_6_x->Text = L"-50";
			this->radar_txt_calib_6_x->Visible = false;
			// 
			// radar_textBox_inc
			// 
			this->radar_textBox_inc->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_inc->Location = System::Drawing::Point(27, 321);
			this->radar_textBox_inc->Name = L"radar_textBox_inc";
			this->radar_textBox_inc->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_inc->TabIndex = 6;
			this->radar_textBox_inc->Text = L"2";
			this->radar_textBox_inc->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_ne_x
			// 
			this->radar_txt_calib_ne_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_ne_x->Location = System::Drawing::Point(65, 100);
			this->radar_txt_calib_ne_x->Name = L"radar_txt_calib_ne_x";
			this->radar_txt_calib_ne_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_ne_x->TabIndex = 6;
			this->radar_txt_calib_ne_x->Text = L"50";
			this->radar_txt_calib_ne_x->Visible = false;
			this->radar_txt_calib_ne_x->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_7_x
			// 
			this->radar_txt_calib_7_x->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_7_x->Location = System::Drawing::Point(65, 204);
			this->radar_txt_calib_7_x->Name = L"radar_txt_calib_7_x";
			this->radar_txt_calib_7_x->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_7_x->TabIndex = 6;
			this->radar_txt_calib_7_x->Text = L"0";
			this->radar_txt_calib_7_x->Visible = false;
			this->radar_txt_calib_7_x->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_txt_calib_5_y
			// 
			this->radar_txt_calib_5_y->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_5_y->Location = System::Drawing::Point(99, 152);
			this->radar_txt_calib_5_y->Name = L"radar_txt_calib_5_y";
			this->radar_txt_calib_5_y->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_5_y->TabIndex = 6;
			this->radar_txt_calib_5_y->Text = L"-50";
			this->radar_txt_calib_5_y->Visible = false;
			// 
			// radar_txt_calib_fn_theta
			// 
			this->radar_txt_calib_fn_theta->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->radar_txt_calib_fn_theta->Location = System::Drawing::Point(133, 48);
			this->radar_txt_calib_fn_theta->Name = L"radar_txt_calib_fn_theta";
			this->radar_txt_calib_fn_theta->Size = System::Drawing::Size(28, 20);
			this->radar_txt_calib_fn_theta->TabIndex = 6;
			this->radar_txt_calib_fn_theta->Text = L"0";
			this->radar_txt_calib_fn_theta->Visible = false;
			// 
			// pictureBox1
			// 
			this->pictureBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->pictureBox1->BorderStyle = System::Windows::Forms::BorderStyle::FixedSingle;
			this->pictureBox1->Location = System::Drawing::Point(1, 1);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(527, 571);
			this->pictureBox1->TabIndex = 9;
			this->pictureBox1->TabStop = false;
			// 
			// map_tab
			// 
			this->map_tab->Controls->Add(this->map_picture);
			this->map_tab->Location = System::Drawing::Point(4, 22);
			this->map_tab->Name = L"map_tab";
			this->map_tab->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
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
			this->terminal_tab->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
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
			this->log_tab->Controls->Add(this->log_txt_enabled);
			this->log_tab->Controls->Add(this->log_txt);
			this->log_tab->Location = System::Drawing::Point(4, 22);
			this->log_tab->Name = L"log_tab";
			this->log_tab->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
			this->log_tab->Size = System::Drawing::Size(1090, 623);
			this->log_tab->TabIndex = 3;
			this->log_tab->Text = L"Debug Log";
			this->log_tab->UseVisualStyleBackColor = true;
			// 
			// log_txt_enabled
			// 
			this->log_txt_enabled->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->log_txt_enabled->AutoSize = true;
			this->log_txt_enabled->Checked = true;
			this->log_txt_enabled->CheckState = System::Windows::Forms::CheckState::Checked;
			this->log_txt_enabled->Location = System::Drawing::Point(1028, 601);
			this->log_txt_enabled->Name = L"log_txt_enabled";
			this->log_txt_enabled->Size = System::Drawing::Size(59, 17);
			this->log_txt_enabled->TabIndex = 1;
			this->log_txt_enabled->Text = L"Enable";
			this->log_txt_enabled->UseVisualStyleBackColor = true;
			// 
			// log_txt
			// 
			this->log_txt->AcceptsReturn = true;
			this->log_txt->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->log_txt->Font = (gcnew System::Drawing::Font(L"Consolas", 6.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->log_txt->Location = System::Drawing::Point(0, 0);
			this->log_txt->MaxLength = 32768;
			this->log_txt->Multiline = true;
			this->log_txt->Name = L"log_txt";
			this->log_txt->ScrollBars = System::Windows::Forms::ScrollBars::Both;
			this->log_txt->Size = System::Drawing::Size(1090, 595);
			this->log_txt->TabIndex = 0;
			this->log_txt->WordWrap = false;
			// 
			// capture_tab
			// 
			this->capture_tab->Controls->Add(this->capture_comboBox_path);
			this->capture_tab->Controls->Add(this->capture_btn_stop);
			this->capture_tab->Controls->Add(this->capture_btn_start);
			this->capture_tab->Location = System::Drawing::Point(4, 22);
			this->capture_tab->Name = L"capture_tab";
			this->capture_tab->Padding = System::Windows::Forms::Padding(3, 3, 3, 3);
			this->capture_tab->Size = System::Drawing::Size(1090, 623);
			this->capture_tab->TabIndex = 6;
			this->capture_tab->Text = L"Data Capture";
			this->capture_tab->UseVisualStyleBackColor = true;
			// 
			// capture_comboBox_path
			// 
			this->capture_comboBox_path->FormattingEnabled = true;
			this->capture_comboBox_path->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L".", L"c:\\temp", L"d:\\temp"});
			this->capture_comboBox_path->Location = System::Drawing::Point(199, 17);
			this->capture_comboBox_path->Name = L"capture_comboBox_path";
			this->capture_comboBox_path->Size = System::Drawing::Size(226, 21);
			this->capture_comboBox_path->TabIndex = 1;
			this->capture_comboBox_path->Text = L".";
			// 
			// capture_btn_stop
			// 
			this->capture_btn_stop->Enabled = false;
			this->capture_btn_stop->Location = System::Drawing::Point(12, 44);
			this->capture_btn_stop->Name = L"capture_btn_stop";
			this->capture_btn_stop->Size = System::Drawing::Size(121, 23);
			this->capture_btn_stop->TabIndex = 0;
			this->capture_btn_stop->Text = L"Stop Data Capture";
			this->capture_btn_stop->UseVisualStyleBackColor = true;
			this->capture_btn_stop->Click += gcnew System::EventHandler(this, &f1::capture_btn_stop_Click);
			// 
			// capture_btn_start
			// 
			this->capture_btn_start->Location = System::Drawing::Point(12, 15);
			this->capture_btn_start->Name = L"capture_btn_start";
			this->capture_btn_start->Size = System::Drawing::Size(121, 23);
			this->capture_btn_start->TabIndex = 0;
			this->capture_btn_start->Text = L"Start Data Capture";
			this->capture_btn_start->UseVisualStyleBackColor = true;
			this->capture_btn_start->Click += gcnew System::EventHandler(this, &f1::capture_btn_start_Click);
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
			this->radar_timer->Interval = 50;
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
			// capture_timer
			// 
			this->capture_timer->Tick += gcnew System::EventHandler(this, &f1::capture_timer_Tick);
			// 
			// radar_textBox_td
			// 
			this->radar_textBox_td->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_td->Location = System::Drawing::Point(27, 243);
			this->radar_textBox_td->Name = L"radar_textBox_td";
			this->radar_textBox_td->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_td->TabIndex = 6;
			this->radar_textBox_td->Text = L"5";
			this->radar_textBox_td->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_textBox_alfa
			// 
			this->radar_textBox_alfa->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_alfa->Location = System::Drawing::Point(27, 267);
			this->radar_textBox_alfa->Name = L"radar_textBox_alfa";
			this->radar_textBox_alfa->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_alfa->TabIndex = 6;
			this->radar_textBox_alfa->Text = L"0.1";
			this->radar_textBox_alfa->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// radar_textBox_tsd
			// 
			this->radar_textBox_tsd->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_tsd->Location = System::Drawing::Point(27, 292);
			this->radar_textBox_tsd->Name = L"radar_textBox_tsd";
			this->radar_textBox_tsd->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_tsd->TabIndex = 6;
			this->radar_textBox_tsd->Text = L"1";
			this->radar_textBox_tsd->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
			// 
			// label14
			// 
			this->label14->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label14->AutoSize = true;
			this->label14->Location = System::Drawing::Point(63, 294);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(97, 13);
			this->label14->TabIndex = 7;
			this->label14->Text = L"thresholdSmallestD";
			this->label14->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// label15
			// 
			this->label15->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(63, 270);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(24, 13);
			this->label15->TabIndex = 7;
			this->label15->Text = L"alfa";
			this->label15->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// label16
			// 
			this->label16->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->label16->AutoSize = true;
			this->label16->Location = System::Drawing::Point(63, 245);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(68, 13);
			this->label16->TabIndex = 7;
			this->label16->Text = L"thresholdDist";
			this->label16->Click += gcnew System::EventHandler(this, &f1::label2_Click);
			// 
			// radar_textBox_line
			// 
			this->radar_textBox_line->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->radar_textBox_line->Location = System::Drawing::Point(103, 530);
			this->radar_textBox_line->Name = L"radar_textBox_line";
			this->radar_textBox_line->Size = System::Drawing::Size(28, 20);
			this->radar_textBox_line->TabIndex = 6;
			this->radar_textBox_line->Text = L"5";
			this->radar_textBox_line->TextChanged += gcnew System::EventHandler(this, &f1::radar_txt_calib_ne_x_TextChanged);
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
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			this->map_tab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->map_picture))->EndInit();
			this->terminal_tab->ResumeLayout(false);
			this->terminal_tab->PerformLayout();
			this->log_tab->ResumeLayout(false);
			this->log_tab->PerformLayout();
			this->capture_tab->ResumeLayout(false);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion



	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) { }
	private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) { }
	
	private: System::Void serial_timer_Tick(System::Object^  sender, System::EventArgs^  e);
	private: System::Void ui_timer_Tick(System::Object^  sender, System::EventArgs^  e);


	private: System::Void radar_txt_calib_ne_x_TextChanged(System::Object^  sender, System::EventArgs^  e) { }

	private: System::Void capture_btn_start_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void capture_btn_stop_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void capture_timer_Tick(System::Object^  sender, System::EventArgs^  e);
};


}

