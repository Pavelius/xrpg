#include "draw.h"

void set_dark_theme() {
	colors::active = color(172, 128, 0);
	colors::border = color(73, 73, 80);
	colors::button = color(0, 122, 204);
	colors::form = color(64, 64, 64);
	colors::window = color(32, 32, 32);
	colors::text = color(255, 255, 255);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = colors::text.mix(colors::button, 128);
	colors::special = color(255, 244, 32);
	colors::border = colors::window.mix(colors::text, 128);
	colors::tips::text = color(255, 255, 255);
	colors::tips::back = color(100, 100, 120);
}

void set_light_theme() {
	colors::active = color(0, 128, 172);
	colors::button = color(223, 223, 223);
	colors::form = color(240, 240, 240);
	colors::window = color(255, 255, 255);
	colors::text = color(0, 0, 0);
	colors::h1 = colors::text.mix(colors::button, 64);
	colors::h2 = colors::text.mix(colors::button, 96);
	colors::h3 = colors::text.mix(colors::button, 128);
	colors::special = color(0, 0, 255);
	colors::border = color(172, 172, 172);
	colors::tips::text = color(255, 255, 255);
	colors::tips::back = color(80, 80, 120);
}