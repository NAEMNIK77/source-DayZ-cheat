#ifndef __GCONSOLE__
#define __GCONSOLE__

#include "GSDK.h"

class GKeyboardInput {
public:
	GKeyboardInput();
	void OnKeyPressDown(int key);
	void OnKeyPressUp(int key);
	char* GetText();
	void SetText(const char *text);
	void Cut(int index);
	void Clear();

private:
	std::string cur_text;
	bool shift;
	bool altgr;
};

class GConsole {
public:
	struct console_cmd_args {
		std::string argv[20];
		int argc;
	};
	struct console_output_entry {
		std::string entries[20];
		GColor color[20];
		int count;
	};

	GConsole();
	GConsole(std::string Title);
	GConsole(int X, int Y, int W, int H, std::string Title = "");
	~GConsole();

	void Initialize();

	void SetDimensions(int X, int Y, int W, int H);
	void SetTitle(std::string Title = "");

	void DrawBox();
	void DrawLogBox();
	void DrawLogText();
	void DrawInputBox();

	void Print(const char *text, ...);
	void Print(const GColor color, const char *text, ...);
	void PrintC(const char *text, ...);
	void ClearLogEntries();

	void SetHistory(int index);
	void HandleHistory();

	void RegisterCommand(const std::string cmd, void *function_ptr);
	void ExecuteCommand(const std::string cmd);

	void Render();
	void DrawColoredText(int x, int y, console_output_entry entries);

	GKeyboardInput *GetInput();
	console_cmd_args *GetCmdArgs();
	void SetCmdArgs(std::string cmd);
private:
	struct rect {
		int x, y, w, h;
	};

	struct console_function {
		std::string cmd;
		void *function_ptr;
	};

	struct console_input_entry {
		std::string entry;
		//int index;
	};

	std::list<console_function>functions;
	std::list<console_output_entry>entries;
	std::list<console_input_entry>input_entries;

	int cur_input_entry_index;

	rect main_box;
	rect console_box;
	rect console_text;
	rect input_box;

	console_cmd_args cmd_args;

	GKeyboardInput input;

	std::string title;
};

extern GConsole *g_pConsole;

#endif