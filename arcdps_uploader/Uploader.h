#pragma once

#include "arcdps_defs.h"
#include "SimpleIni.h"
#include <cpr/cpr.h>
#include <filesystem>
#include <future>
#include <deque>
#include "Revtc.h"
#include "sqlite_orm.h"
#include "Log.h"
#include "Settings.h"

namespace fs = std::filesystem;

struct StatusMessage
{
	std::string msg;
	int log_id;
};

struct UserToken
{
	int id;
	std::string value;
	bool disabled;
	char value_buf[128];
};

struct Webhook
{
	int id;
	std::string name;
	std::string url;
	bool raids;
	bool fractals;
	bool strikes;
	bool golems;
	bool wvw;
	std::string filter;
	int filter_min;
	bool success;

	char name_buf[64];
	char url_buf[192];
	char filter_buf[128];
};

class Uploader
{
	Settings settings;

	fs::path log_path;
	std::vector<Log> logs;
	std::future<decltype(logs)> ft_file_list;
	std::chrono::system_clock::time_point refresh_time;

	std::deque<int> upload_queue;
	std::vector<std::future<cpr::Response>> ft_uploads;
	std::vector<UserToken> userTokens;
	UserToken userToken;
	std::vector<Webhook> webhooks;
	std::mutex wh_mutex;
	std::deque<int> wh_queue;

	std::vector<StatusMessage> status_messages;
	std::mutex ts_msg_mutex;
	std::vector<StatusMessage> thread_status_messages;

	std::thread upload_thread;
	std::atomic<bool> upload_thread_run;
	std::mutex ut_mutex;
	std::condition_variable ut_cv;

	void imgui_draw_logs();
	void imgui_draw_status();
	void imgui_draw_options();
	void imgui_draw_options_aleeva();
	void create_log_table(Log& l);

	void check_webhooks(int log_id);
	void check_gw2bot(int log_id);
	void check_aleeva(int log_id);

	void upload_thread_loop();
	void add_pending_upload_logs(std::vector<int>& queue);
	void poll_async_refresh_log_list();

	void queue_status_message(const std::string& msg, int log_id = -1);
	void queue_status_message(const StatusMessage& status);

	std::string format_msg(Log log);
	std::string build_msg(std::vector<Log> logs);
	std::string build_sorted_msg(std::vector<Log> logs, std::string msg);

	std::vector<int> raid_order = { 0x3C4E,0x3C45,0x3C0F,0x3EFB,0x3ED8,0x3F09,0x3EFD,0x3EF3,0x3F71,0x3F7D,0x3F6B,0x3F77,0x3F76,0x3F9E,0x432A,0x4314,0x4324,0x4302,0x4D37,0x4D74,0x4CEB,0x4C50,0x4CC3,0x4D84,0x4BFA,0xABC6,0x5271,0x5261,0x51C6,0x5231,0x55CC,0x55F6,0x55F0 };
	std::vector<int> strike_order = { 0x568A,0x57DC,0x5747,0x57D1,0x572B,0x58B7,0x57F9,0x5DE1,0x5D95,0x5FA5,0xA9E0,0x6345,0x6469,0x6585 };
	std::vector<int> fractal_order = { 0x427D,0x4284,0x4234,0x44E0,0x461D,0x455F,0x5AD6 };

 public:
	bool is_open;
	bool in_combat;

	Uploader(fs::path data_path, std::optional<fs::path> custom_log_path);
	~Uploader();

	uintptr_t imgui_tick();
	void imgui_window_checkbox();
	
	void start_async_refresh_log_list();

	void start_upload_thread();
};