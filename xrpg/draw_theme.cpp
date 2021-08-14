#include "draw.h"
#include "draw_button.h"
#include "handler.h"

handler* after_theme_change;

void set_dark_theme() {
	colors::window = color(32, 32, 32);
	colors::active = color(172, 128, 0);
	colors::border = color(73, 73, 80);
	colors::button = color(0, 122, 204);
	colors::form = color(45, 45, 48);
	colors::text = color(255, 255, 255);
	colors::special = color(255, 244, 32);
	colors::border = color(63, 63, 70);
	colors::tips::text = color(255, 255, 255);
	colors::tips::back = color(100, 100, 120);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = colors::text.mix(colors::button, 128);
	after_theme_change->execute();
}

void set_light_theme() {
	colors::window = color(255, 255, 255);
	colors::active = color(0, 128, 172);
	colors::button = color(223, 223, 223);
	colors::form = color(240, 240, 240);
	colors::text = color(0, 0, 0);
	colors::special = color(0, 0, 255);
	colors::border = color(172, 172, 172);
	colors::tips::text = color(255, 255, 255);
	colors::tips::back = color(80, 80, 120);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = colors::text.mix(colors::button, 128);
	after_theme_change->execute();
}