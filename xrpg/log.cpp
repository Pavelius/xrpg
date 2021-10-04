#include "io_stream.h"
#include "log.h"
#include "stringbuilder.h"

static int error_count;
static const char* current_url;

void log::seturl(const char* v) {
    current_url = v;
}

void log::errorv(const char* format) {
	static io::file file("errors.txt", StreamWrite|StreamText);
	if(!file)
		return;
	error_count++;
	file << format << "\n";
}

void log::error(const char* format, ...) {
	char temp[4096]; stringbuilder sb(temp);
	if(current_url) {
        sb.add("In file %1", current_url);
        current_url = 0;
        errorv(temp);
        sb.clear();
	}
	sb.addv(format, xva_start(format));
	errorv(temp);
}

int log::geterrors() {
	return error_count;
}