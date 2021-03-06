#include "crt.h"
#include "draw_clipboard.h"
#include "win.h"

void clipboard::copy(const char* string, long lenght) {
	if(!lenght)
		return;
	if(!OpenClipboard(0))
		return;
	EmptyClipboard();
	auto size = (lenght + 1) * sizeof(short);
	void* hglbCopy = GlobalAlloc(GMEM_MOVEABLE, size);
	if(!hglbCopy) {
		CloseClipboard();
		return;
	}
	// Lock the handle and copy the text to the buffer.
	auto ps = (char*)GlobalLock(hglbCopy);
	szencode(ps, size, codepages::U16LE, (const char*)string, lenght, codepages::W1251);
	GlobalUnlock(ps);
	SetClipboardData(CF_UNICODETEXT, hglbCopy);
	CloseClipboard();
}

bool clipboard::isallowpaste() {
	return IsClipboardFormatAvailable(CF_UNICODETEXT)
		|| IsClipboardFormatAvailable(CF_TEXT);
}

const char* clipboard::paste(long* size) {
	auto format = CF_UNICODETEXT;
	if(!IsClipboardFormatAvailable(format)) {
		format = CF_TEXT;
		if(!IsClipboardFormatAvailable(format))
			return 0;
	}
	if(!OpenClipboard(0))
		return 0;
	void* hglb = GetClipboardData(format);
	char* buffer = 0;
	if(hglb) {
		auto result = GlobalSize(hglb);
		if(result) {
			buffer = new char[result + 1];
			char* p = (char*)GlobalLock(hglb);
			if(p) {
				memcpy(buffer, p, result);
				GlobalUnlock(hglb);
			}
			if(format == CF_UNICODETEXT) {
				szencode(buffer, result, codepages::W1251, buffer, result, codepages::U16LE);
				result = result / 2;
				buffer[result] = 0;
			}
			if(size)
				*size = result;
		}
	}
	CloseClipboard();
	return buffer;
}