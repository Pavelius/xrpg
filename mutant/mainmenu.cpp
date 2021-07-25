#include "main.h"

BSDATA(menui) = {
	{"main", "Новая игра", {0, character::choose_role}},
	{"main", "Загрузить игру"},
	{"main", "Настройки", {"setting"}, "Что хотите настроить?", "machine", {MenuBack}},
	{"main", "Об авторах"},
	{"main", "Выход"},
	{"setting", "Видео"},
	{"setting", "Звук"},
	{"setting", "Правила"},
};
BSDATAF(menui)