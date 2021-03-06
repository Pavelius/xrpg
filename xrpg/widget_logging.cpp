#include "crt.h"
#include "datetime.h"
#include "draw_button.h"
#include "draw_control.h"
#include "draw.h"
#include "handler.h"
#include "io_stream.h"
#include "setting.h"
#include "stringbuilder.h"

using namespace	draw;
using namespace	draw::controls;

namespace {
struct logi {
	datetime		stamp;
	const char*		text;
};
}
static bool			save_log_file;
static vector<logi>	messages;

void draw::logmsgv(const char* format, const char* arguments) {
	logi e = {0};
	char temp[8192]; stringbuilder sb(temp);
	sb.addv(format, arguments);
	e.stamp = datetime::now();
	e.text = szdup(temp);
	messages.add(e);
}

static void addsb(stringbuilder& sb, const datetime& d) {
	sb.add("%1.2i.%2.2i.%3.2i %4.2i:%5.2i",
		d.day(), d.month(), d.year(),
		d.hour(), d.minute());
}

static void write_log_file() {
	io::file file("log.txt", StreamRead | StreamWrite | StreamText);
	if(!file)
		return;
	file.seek(0, SeekEnd);
	for(auto& e : messages) {
		char temp[64]; stringbuilder sb(temp);
		addsb(sb, e.stamp);
		file << temp << " " << e.text << "\r\n";
	}
}

static void before_application_exit() {
	if(save_log_file)
		write_log_file();
}

static class widget_logging : public plugin, public table {
	array	rows;
	control* getcontrol() override {
		return this;
	}
	const char* getvalue(const char* id, stringbuilder& sb) const override {
		if(equal(id, "Name"))
			return getnm(this->id);
		return 0;
	}
	void* get(int line) const {
		return messages.ptr(line);
	}
	int	getmaximum() const override {
		return messages.getcount();
	}
	bool hastoolbar() const override {
		return false;
	}
public:
	widget_logging() : plugin("MessageList", dock::Bottom), table(rows), rows(sizeof(logi)) {
		no_change_count = true;
		readonly = true;
		select_mode = selection::Row;
	}
} widget;

void logmsg(const char* format, ...) {
	logmsgv(format, xva_start(format));
}

void draw::loginitialize() {
	widget.show_header = false;
	widget.show_grid_lines = false;
	widget.addcol("Message", ANREQ(logi, text), "Text").set(widtht::Auto);
	widget.addcol("Date", ANREQ(logi, stamp), "Date").set(widtht::Fixed);
	atexit(before_application_exit);
}

static setting::element logging_common[] = {
	{"SaveMessageFile", save_log_file},
};
static setting::header headers[] = {
	{"Log", "General", 0, logging_common},
};