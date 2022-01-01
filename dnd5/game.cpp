#include "archive.h"
#include "draw_background.h"
#include "main.h"

static bool serial_map(const char* name, bool writemode) {
	char temp[260]; stringbuilder sb(temp);
	sb.add("art/background/%1.dsc", name);
	io::file file(temp, writemode ? StreamWrite : StreamRead);
	if(!file)
		return false;
	archive a(file, writemode);
	if(!a.signature("MAP"))
		return false;
	if(!a.version(1, 0))
		return false;
	a.set(map::blocks);
	return true;
}

static void toggle_block() {
	if(draw::hilite_index == Blocked)
		return;
	if(blocks[draw::hilite_index] != Inpassable)
		blocks[draw::hilite_index] = Inpassable;
	else
		blocks[draw::hilite_index] = Passable;
}

void gamei::readmap() {
	serial_map(draw::background::url, false);
}

void gamei::writemap() {
	serial_map(draw::background::url, true);
}

static void run_game() {
	draw::setnext(gamei::rungame);
}

static void editor_proc() {
	answers an;
	an.add(gamei::writemap, getnm("Save"));
	an.add(run_game, getnm("Play"));
	an.modal(0, getnm("Cancel"));
}

void gamei::rungame() {
	auto p1 = bsdata<creature>::elements + 0;
	auto p2 = bsdata<creature>::elements + 1;
	readmap();
	while(p1->is(Alive) && p2->is(Alive)) {
		p1->fight();
		//p2->fight();
	}
}

void gamei::runeditor() {
	readmap();
	//draw::modalscene(draw::painteditor, editor_proc, toggle_block);
}

