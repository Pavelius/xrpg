#include "crt.h"
#include "draw.h"
#include "draw_button.h"
#include "draw_builder.h"
#include "draw_control.h"
#include "win.h"

using namespace draw;
//using namespace draw::controls;

struct menu_builder : builder {
	void*	hMenu;
	int		count;
	menu_builder() : builder(), hMenu(0), count(0) {
		hMenu = CreatePopupMenu();
	}
	~menu_builder() {
		DestroyMenu(hMenu);
		hMenu = 0;
	}
	static unsigned get_menu_data(void* handle, unsigned index) {
		MENUITEMINFO mi = {0};
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_DATA;
		if(!GetMenuItemInfoA(handle, index-1, 1, &mi))
			return 0;
		return mi.dwItemData;
	}
	const char* finish() const override {
		HWND hwnd = GetActiveWindow();
		if(!hwnd)
			return 0;
		POINT pt; pt.x = hot.mouse.x; pt.y = hot.mouse.y;
		ClientToScreen(hwnd, &pt);
		// show menu
		auto result = TrackPopupMenuEx(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, hwnd, 0);
		if(result)
			result = get_menu_data(hMenu, result);
		hot.pressed = false;
		return (const char*)result;
	}
	void add(const char* id) override {
		MENUITEMINFO mi = {0};
		mi.cbSize = sizeof(mi);
		mi.fMask = MIIM_STRING | MIIM_FTYPE | MIIM_STATE | MIIM_ID | MIIM_SUBMENU | MIIM_DATA;
		mi.fType = MFT_STRING;
		char temp[260]; temp[0] = 0; stringbuilder sb(temp);
		if(id)
			sb.add(getnm(id), sizeof(temp) - 1);
		auto key = getkey(id);
		if(key) {
			sb.add("\t");
			draw::key2str(sb, key);
		}
		temp[0] = szupper(temp[0]);
		if(!allowid || allowid(object, id))
			mi.fState = MFS_ENABLED;
		else
			mi.fState = MFS_DISABLED | MFS_GRAYED;
		mi.dwTypeData = temp;
		mi.wID = ++count;
		mi.dwItemData = (unsigned)id;
		InsertMenuItemA(hMenu, mi.wID, 0, &mi);
	}
	void addseparator() override {
		AppendMenuA(hMenu, MF_SEPARATOR, ++count, 0);
	}
};

const char* draw::contextmenu(const char** commands, const void* object, fnallowid allowid, fngetcommands getcommands) {
	menu_builder pm;
	return pm.choose(commands, object, allowid, getcommands);
}