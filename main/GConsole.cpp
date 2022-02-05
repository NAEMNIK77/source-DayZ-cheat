#include "GConsole.h"
#include "GOverlay.h"

GConsole *g_pConsole = new GConsole();

GConsole::GConsole()
{
	SetDimensions(100, 100, 500, 350);
	SetTitle();
	Initialize();

	cur_input_entry_index = 0;

	//Print(color_green, ":: Console initialized © 2014, Gordon` ::");
}

GConsole::GConsole(std::string Title)
{
	SetDimensions(100, 100, 500, 350);
	SetTitle(Title);
	Initialize();
}

GConsole::GConsole(int X, int Y, int W, int H, std::string Title)
{
	SetDimensions(X, Y, W, H);
	SetTitle(Title);
	Initialize();
}

GConsole::~GConsole()
{
	ClearLogEntries();
}

void GConsole::Initialize()
{
	console_box.x = main_box.x + 10;
	console_box.y = main_box.y + 30;
	console_box.w = main_box.w - 20;
	console_box.h = main_box.h - 70;

	console_text.x = main_box.x + 15;
	console_text.y = main_box.y + 30;
	console_text.w = main_box.w - 20;
	console_text.h = main_box.h - 70;

	input_box.x = main_box.x + 10;
	input_box.y = main_box.y + main_box.h - 30;
	input_box.w = main_box.w - 20;
	input_box.h = 20;
}

GKeyboardInput *GConsole::GetInput()
{
	return &input;
}

void GConsole::SetDimensions(int X, int Y, int W, int H)
{
	main_box = { X, Y, W, H };
}
void GConsole::SetTitle(std::string Title)
{
	if (!Title.empty())
		title = Title;
	else
		title = "Console";
}

void GConsole::DrawBox()
{
	g_pOverlay->DirectX->Draw->FilledQuad(main_box.x, main_box.y, main_box.w, main_box.h, GColor(0, 0, 0, 200));
	g_pOverlay->DirectX->Draw->Quad(main_box.x, main_box.y, main_box.w, main_box.h, 2, GColor(255, 255, 255, 200));

	g_pOverlay->DirectX->Draw->TextCentered(main_box.x + main_box.w / 2, main_box.y + 5, color_green, const_cast<char*>(title.c_str()));
	g_pOverlay->DirectX->Draw->Line(main_box.x, main_box.y + 20, main_box.x + main_box.w, main_box.y + 20, GColor(255, 255, 255, 200));
}

void GConsole::DrawLogBox()
{
	console_box.x = main_box.x + 10;
	console_box.y = main_box.y + 30;
	console_box.w = main_box.w - 20;
	console_box.h = main_box.h - 70;

	g_pOverlay->DirectX->Draw->Quad(console_box.x, console_box.y, console_box.w, console_box.h, 1, GColor(255, 255, 255, 230));
}

void GConsole::DrawLogText()
{
	console_text.x = main_box.x + 15;
	console_text.y = main_box.y + 30;
	console_text.w = main_box.w - 20;
	console_text.h = main_box.h - 70;

	int height = 0;
	int font_height = g_pOverlay->DirectX->Draw->GetFontHeight() + 1;

	for (auto &it : entries) {

		DrawColoredText(console_text.x, console_text.y - height + console_text.h - font_height - 1, it);
		height += font_height;

		if (height > console_text.h - font_height)
			break;
	}
}

void GConsole::DrawInputBox()
{
	input_box.x = main_box.x + 10;
	input_box.y = main_box.y + main_box.h - 30;
	input_box.w = main_box.w - 20;
	input_box.h = 20;

	g_pOverlay->DirectX->Draw->Text(input_box.x, input_box.y + 3, color_white, "Input:");

	input_box.x += 50;
	input_box.w -= 50;

	g_pOverlay->DirectX->Draw->Quad(input_box.x, input_box.y, input_box.w, input_box.h, 1, color_white);

	std::string key_text = input.GetText();

	g_pOverlay->DirectX->Draw->Text(input_box.x + 5, input_box.y + 5, color_green, "%s", key_text.c_str());

	HandleHistory();

	if (!key_text.empty() && key_text.front() == '\n') {

		input.Clear();
		return;
	}

	for (unsigned int i = 0; i < key_text.length(); i++) {

		if (key_text.at(i) != '\n')
			continue;

		key_text.erase(i);

		Print("> %s", key_text.c_str());
		ExecuteCommand(key_text);

		console_input_entry buf;
		buf.entry = key_text;
		input_entries.push_front(buf);

		input.Clear();
		cur_input_entry_index = 0;
	}
}

/*

TODO:
-line splitting (DONE)
--optional: split at space when available

*/

void GConsole::Print(const char *text, ...)
{
	va_list va_alist;
	char logbuf[1024];

	va_start(va_alist, text);
	_vsnprintf(logbuf, sizeof(logbuf), text, va_alist);
	va_end(va_alist);

	if (strlen(logbuf) == 0)
		return;

	std::istringstream buf_cmd(logbuf);
	std::string arg;

	Print(color_white, logbuf);
}

void GConsole::Print(const GColor color, const char *text, ...)
{
	va_list va_alist;
	char logbuf[1024];

	va_start(va_alist, text);
	_vsnprintf(logbuf, sizeof(logbuf), text, va_alist);
	va_end(va_alist);

	if (strlen(logbuf) == 0)
		return;

	int console_width = console_text.w;

	std::string string_text = logbuf;

	textsize full_text_size;
	g_pOverlay->DirectX->Draw->GetTextLength(logbuf, &full_text_size);

	bool multiline = full_text_size.w > console_width && console_width;

	if (multiline) {

		std::string split = string_text;
		textsize split_text_size;

		size_t len = 0;
		do {

			split = string_text.substr(len);

			do {

				g_pOverlay->DirectX->Draw->GetTextLength((char*)split.c_str(), &split_text_size);
				split.erase(split.length() - 1);

			} while (split_text_size.w > console_width);

			len += split.length();

			if (split[0] == ' ')
				split.erase(0, 1);


			std::istringstream buf_cmd(split);
			std::string arg;

			int count = 0;

			while (std::getline(buf_cmd, arg, '\n')) {

				console_output_entry buffer;
				buffer.color[0] = color;
				buffer.entries[0] = arg;
				buffer.count = 1;

				entries.push_front(buffer);

				count++;
			}

			if (!count) {

				console_output_entry buffer;
				buffer.color[0] = color;
				buffer.entries[0] = split;
				buffer.count = 1;

				entries.push_front(buffer);
			}

		} while (len < string_text.length() - 1);
	}
	else {

		std::istringstream buf_cmd(logbuf);
		std::string arg;

		int count = 0;
		bool nl = string_text[string_text.length() - 1] == '\n' && string_text[0] != '\n';

		while (std::getline(buf_cmd, arg, '\n')) {

			console_output_entry buffer;
			buffer.color[0] = color;
			buffer.entries[0] = arg;
			buffer.count = 1;

			entries.push_front(buffer);

			count++;
		}

		if (!count) {

			console_output_entry buffer;
			buffer.color[0] = color;
			buffer.entries[0] = logbuf;
			buffer.count = 1;

			entries.push_front(buffer);
		}

		if (nl) {
			console_output_entry buffer;
			buffer.count = 1;
			entries.push_front(buffer);
		}
	}
}

void GConsole::PrintC(const char *text, ...)
{
	va_list va_alist;
	char logbuf[256];

	va_start(va_alist, text);
	_vsnprintf(logbuf, sizeof(logbuf), text, va_alist);
	va_end(va_alist);

	if (strlen(logbuf) == 0)
		return;

	std::string string_text = logbuf;
	console_output_entry buffer;

	std::istringstream buf_nl(logbuf);
	std::string arg_nl;

	int count_nl = 0;

	bool nl = string_text[string_text.length() - 1] == '\n' && string_text[0] != '\n';

	while (std::getline(buf_nl, arg_nl, '\n')) {


		std::istringstream buf_cmd(arg_nl);
		std::string arg;

		int count = 0;

		while (std::getline(buf_cmd, arg, '^')) {

			/*if (arg.size() == 0)
			break;*/

			char c = strtol(&arg[0], 0, 10);
			//if (c == 0) buffer.color[count] = color_white;
			if (c == 1) buffer.color[count] = color_pink;
			else if (c == 2) buffer.color[count] = color_orange;
			else if (c == 3) buffer.color[count] = color_yellow;
			else if (c == 4) buffer.color[count] = color_lightblue;
			else if (c == 5) buffer.color[count] = color_red;
			else if (c == 6) buffer.color[count] = color_chocolate;
			else if (c == 7) buffer.color[count] = color_cadetblue;
			else if (c == 8) buffer.color[count] = color_green;
			else if (c == 9) buffer.color[count] = color_white;
			else buffer.color[count] = color_white;

			buffer.entries[count] = arg.substr(c > 0 && c <= 9 ? 1 : 0);

			count++;
		}

		buffer.count = count;
		entries.push_front(buffer);

		count_nl++;
	}

	if (nl) {
		console_output_entry buffer;
		buffer.count = 1;
		entries.push_front(buffer);
	}
}

void GConsole::ClearLogEntries()
{
	entries.clear();
}

void GConsole::RegisterCommand(const std::string cmd, void *function_ptr)
{
	console_function buf = { cmd, function_ptr };
	functions.push_back(buf);
}

void GConsole::ExecuteCommand(const std::string cmd)
{
	std::string buf_cmd = cmd;

	unsigned int i = 0;
	for (i = 0; i < buf_cmd.length(); i++) {

		if (buf_cmd.at(i) == 0x20) {

			buf_cmd.erase(i);
			break;
		}
	}

	std::string con_cmd;
	if (cmd.length() > buf_cmd.length())
		con_cmd = cmd.substr(i + 1);

	SetCmdArgs(cmd);

	bool recognized = false;

	for (auto &it : functions) {

		if (it.cmd != buf_cmd)
			continue;

		recognized = true;

		typedef void(*func_t)(GConsole *this_ptr);
		func_t func = (func_t)it.function_ptr;

		func(this);
	}

	if (!recognized) {

		if (buf_cmd == "echo")
			PrintC(con_cmd.c_str());
		else if (buf_cmd == "clear")
			ClearLogEntries();
		else if (buf_cmd == "exit")
			exit(1);
		else
			Print(">> unknown command \'%s\'", buf_cmd.c_str());
	}
}

void GConsole::SetHistory(int index)
{
	bool found = false;

	int i = 0;

	for (auto &it : input_entries) {

		if (++i != index)
			continue;

		input.SetText(it.entry.c_str());
		found = true;
	}

	if (!found)
		input.Clear();
}

void GConsole::HandleHistory()
{
	bool up = GetAsyncKeyState(VK_UP) & 1;
	bool down = GetAsyncKeyState(VK_DOWN) & 1;

	if (!up && !down)
		return;

	int &i = cur_input_entry_index;

	if (up)
		i++;
	else
		i--;

	if (i < 0)
		i = 0;

	if (i > input_entries.size())
		i = input_entries.size();

	SetHistory(i ? i : 0);
}

GConsole::console_cmd_args *GConsole::GetCmdArgs()
{
	return &cmd_args;
}

void GConsole::SetCmdArgs(std::string cmd)
{
	std::istringstream buf_cmd(cmd);
	std::string arg;

	int count = 0;

	while (std::getline(buf_cmd, arg, ' '))
		cmd_args.argv[count++] = arg;

	cmd_args.argc = count;
}

void GConsole::DrawColoredText(int x, int y, console_output_entry entries)
{
	int last_x = x;

	for (int i = 0; i < entries.count; i++) {

		if (entries.entries[i].length() == 0 || entries.entries[i] == "")
			continue;

		textsize size;
		g_pOverlay->DirectX->Draw->GetTextLength(const_cast<char*>(entries.entries[i].c_str()), &size);
		g_pOverlay->DirectX->Draw->Text(last_x, y, entries.color[i], const_cast<char*>(entries.entries[i].c_str()));

		last_x += size.w;
	}
}

void GConsole::Render()
{
	DrawBox();
	DrawLogBox();
	DrawLogText();
	DrawInputBox();
}

GKeyboardInput::GKeyboardInput()
{
	cur_text.clear();
	shift = false;
	altgr = false;
}

void GKeyboardInput::OnKeyPressDown(int key)
{
	if (key > 255)
		return;

	if (key == 16)
		shift = true;
	if (key == 18)
		altgr = true;

	if (key == 8 || key == 46)
		if (cur_text.length())
			cur_text.erase(cur_text.length() - 1);

	char buf = '\0';

	if (altgr) {

		if (key == 50) buf = '²';
		if (key == 51) buf = '³';
		if (key == 55) buf = '{';
		if (key == 56) buf = '[';
		if (key == 57) buf = ']';
		if (key == 48) buf = '}';
		if (key == 219) buf = '\\';
		if (key == 187) buf = '~';
		if (key == 226) buf = '|';
		if (key == 'M') buf = 'µ';
		if (key == 'Q') buf = '@';
		if (key == 'E') buf = '€';
	}
	else {

		if (key == 222) shift ? buf = 'Ä' : buf = 'ä';
		if (key == 186) shift ? buf = 'Ü' : buf = 'ü';
		if (key == 192) shift ? buf = 'Ö' : buf = 'ö';
		if (key == 190) shift ? buf = ':' : buf = '.';
		if (key == 110) shift ? buf = ';' : buf = ',';
		if (key == 188) shift ? buf = ';' : buf = ',';
		if (key == 109) shift ? buf = '_' : buf = '-';
		if (key == 189) shift ? buf = '_' : buf = '-';
		if (key == 191) shift ? buf = '\'' : buf = '#';
		if (key == 221) shift ? buf = '`' : buf = '´';
		if (key == 220) shift ? buf = '°' : buf = '^';
		if (key == 107) shift ? buf = '*' : buf = '+';
		if (key == 187) shift ? buf = '*' : buf = '+';
		if (key == 219) shift ? buf = '?' : buf = 'ß';
		if (key == 226) shift ? buf = '>' : buf = '<';

		if (key == ' ') buf = ' ';
		if (key == 13) buf = '\n';

		if (shift) {

			if (key == 48) buf = '=';
			if (key == 49) buf = '!';
			if (key == 50) buf = '\"';
			if (key == 51) buf = '§';
			if (key == 52) buf = '$';
			if (key == 53) buf = '%';
			if (key == 54) buf = '&';
			if (key == 55) buf = '/';
			if (key == 56) buf = '(';
			if (key == 57) buf = ')';

			if (key >= 'A' && key <= 'Z')
				buf = toupper(key);
		}
		else {

			if (key >= 'A' && key <= 'Z')
				buf = tolower(key);

			if (!altgr) {
				if (key >= '0' && key <= '9')
					buf = key;
			}
		}
	}

	if (buf)
		cur_text += buf;
}

void GKeyboardInput::OnKeyPressUp(int key)
{
	if (key == 16)
		shift = false;
	if (key == 18)
		altgr = false;
}

char* GKeyboardInput::GetText()
{
	return const_cast<char*>(cur_text.c_str());
}

void GKeyboardInput::SetText(const char *text)
{
	cur_text = text;
}

void GKeyboardInput::Cut(int index)
{
	cur_text.erase(index);
}

void GKeyboardInput::Clear()
{
	cur_text.clear();
}