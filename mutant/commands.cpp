#include "command.h"
#include "main.h"

static void close_active_window() {
	draw::form.window = 0;
}

static void next_window() {
	draw::hot.param = 0;
}

static void character_sheet() {
	auto player = game.getplayer();
	if(!player)
		return;
	auto gender = player->getgender();
	if(!gender)
		return;
	int x, y, width;
	draw::dialogul(x, y, width);
	draw::header(x, y, width, "%+2 %1",
		player->getname(),
		getnm(bsdata<rolei>::elements[player->getrole()].id),
		getnm(bsdata<genderi>::elements[gender].name));
	char temp[2048]; stringbuilder sb(temp);
	sb.addn("###%1", getnm("Attributes"));
	for(auto& e : bsdata<attributei>()) {
		auto id = (attribute_s)variant(&e).value;
		sb.addn("%1\t%2i", getnm(e.id), player->get(id));
	}
	sb.addn("###%1", getnm("Skills"));
	for(auto& e : bsdata<skilli>()) {
		auto id = (skill_s)variant(&e).value;
		auto value = player->get(id);
		if(!value)
			continue;
		sb.addn("%1\t%2i", getnm(e.id), value);
	}
	auto x1 = x + gui.padding;
	auto w1 = width / 2 - gui.padding * 2;
	draw::textf(x1, y, w1, temp, 0, 0, 0, 0, 120);
	auto x2 = x + width;
	auto y2 = 300 - gui.border;
	draw::buttonr(x2, y2, getnm("Close"), close_active_window, KeyEscape);
	draw::buttonr(x2, y2, getnm("Next"), next_window, 'N');
	draw::rectb({100, 100, 200, 200}, 8);
}

BSDATA(commandi) = {
	{"CharacterSheet", character_sheet},
};
BSDATAF(commandi)

fnevent getcommand(const char* id) {
	if(!id || !id[0])
		return 0;
	for(auto& e : bsdata<commandi>()) {
		if(equal(e.id, id))
			return e.proc;
	}
	return 0;
}