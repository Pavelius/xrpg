#include "command.h"
#include "draw.h"
#include "main.h"
#include "draw_figure.h"
#include "draw_simple.h"

using namespace draw;

static void character_sheet() {
	char temp[2048]; stringbuilder sb(temp);
	auto player = game.getplayer();
	if(!player)
		return;
	auto gender = player->getgender();
	if(!gender)
		return;
	customwindow();
	sb.clear(); sb.add("%+2 %1",
		player->getname(),
		getnm(bsdata<rolei>::elements[player->getrole()].id),
		getnm(bsdata<genderi>::elements[gender].name));
	//sheader(temp);
	sb.clear(); sb.addn("###%Attributes");
	for(auto& e : bsdata<attributei>()) {
		auto id = (attribute_s)variant(&e).value;
		sb.addn("%1\t%2i", getnm(e.id), player->get(id));
	}
	sb.addn("###%Skills");
	for(auto& e : bsdata<skilli>()) {
		auto id = (skill_s)variant(&e).value;
		auto value = player->get(id);
		if(!value)
			continue;
		sb.addn("%1\t%2i", getnm(e.id), value);
	}
	auto push_tab = draw::tab_pixels;
	draw::tab_pixels = 120;
	text(temp);
	draw::tab_pixels = push_tab;
}

static command commands[] = {
	{"CharacterSheet", character_sheet, 'C'},
{}};

void initialize_commands() {
	//pcommands = commands;
}