#include "color.h"
#include "point.h"
#include "stringbuilder.h"

#pragma once

typedef void(*fnevent)();
typedef bool(*fnbutton)(const char* title);

enum draw_event_s : unsigned {
	KeyBackspace = 8, KeyTab = 9, KeyEnter = 10, KeyEscape = 0x1B, KeySpace = 0x20, KeyDelete = 0x7F,
	// input events
	InputSymbol = 0x80, InputTimer, InputKeyUp, InputIdle, InputNeedUpdate, InputUpdate, InputNoUpdate,
	// Keyboard and mouse input (can be overrided by flags)
	MouseLeft, MouseLeftDBL, MouseRight,
	MouseMove, MouseWheelUp, MouseWheelDown,
	KeyLeft, KeyRight, KeyUp, KeyDown, KeyPageUp, KeyPageDown, KeyHome, KeyEnd,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	// support
	CommandMask = 0x000000FF,
	// misc events can be combination with previous
	Ctrl = 0x00000100,
	Alt = 0x00000200,
	Shift = 0x00000400,
};
enum image_flag_s {
	ImageMirrorV = 0x0001,
	ImageMirrorH = 0x0002,
	ImageGrayscale = 0x0004,
	ImageNoOffset = 0x0008,
	ImageTransparent = 0x0010,
	ImageColor = 0x0020,
	ImagePallette = 0x0040,
	TextStroke = 0x0080,
	TextItalic = 0x0100,
	TextBold = 0x0200,
	TextUscope = 0x0400,
	TextSingleLine = 0x0800, // Text would be showed as single line
	AlignLeft = 0x0000,
	AlignCenter = 0x1000,
	AlignRight = 0x2000,
	AlignLeftCenter = 0x3000,
	AlignCenterCenter = 0x4000,
	AlignRightCenter = 0x5000,
	AlignLeftBottom = 0x6000,
	AlignCenterBottom = 0x7000,
	AlignRightBottom = 0x8000,
	AlignWidth = 0xE000,
	AlignMask = 0xF000,
};
enum window_flags {
	WFResize = 0x0010,
	WFMinmax = 0x0020,
	WFMaximized = 0x0040,
	WFAbsolutePos = 0x0080,
};
enum class cursor : unsigned char {
	Arrow, Hand, LeftRight, UpDown, All, No, Edit, Wait,
};
struct pma {
	char				name[4]; // Identifier of current block
	int					size; // Size of all block
	int					count; // Count of records in this block
	int					find(const char* name) const;
	const pma*			getheader(const char* name) const;
	const char*			getstring(int id) const;
};
struct sprite : pma {
	enum encodes { Auto, RAW, RLE, ALC, RAW8, RLE8 };
	struct frame {
		short 			sx, sy;
		short			ox, oy;
		encodes			encode;
		unsigned		pallette;
		unsigned		offset;
		rect			getrect(int x, int y, unsigned flags) const;
	};
	struct cicle {
		short unsigned	start;
		short unsigned	count;
	};
	short int			width, height; // common size of all frames (if applicable)
	short int			ascend, descend; // top or down ascend
	short unsigned		flags; // must be zero
	unsigned			cicles; // count of anim structure
	unsigned			cicles_offset;
	frame				frames[1];
	int					esize() const { return frames[0].offset - (sizeof(sprite) + sizeof(frame)*(count - 1)); }
	const unsigned char* edata() const { return (const unsigned char*)this + sizeof(sprite) + sizeof(frame)*(count - 1); }
	int					ganim(int index, int tick);
	const frame&		get(int id) const { return frames[(id >= count) ? 0 : id]; }
	cicle*				gcicle(int index) { return (cicle*)ptr(cicles_offset) + index; }
	int					glyph(unsigned sym) const;
	const unsigned char* ptr(unsigned o) const { return (unsigned char*)this + o; }
};
namespace colors {
extern color			button, form, window;
extern color			border, active;
extern color			text, h1, h2, h3, special;
namespace tips {
extern color			back;
extern color			text;
}
}
namespace metrics {
extern sprite*			font;
extern sprite*			small;
extern sprite*			h1;
extern sprite*			h2;
extern sprite*			h3;
extern sprite*			icons;
extern int				border, padding;
}
namespace draw {
struct hoti {
	enum cursor			cursor; // set this mouse cursor
	unsigned			key; // if pressed key or mouse this field has key
	point				mouse; // current mouse coordinates
	bool				pressed; // flag if any of mouse keys is pressed
	long				param, param2; // command context or parameters
	const void*			object; // command object
	rect				hilite, focus;
	explicit operator bool() const { return key != 0; }
};
extern hoti				hot;
struct surface {
	struct plugin {
		const char*		name;
		const char*		filter;
		plugin*			next;
		static plugin*	first;
		plugin(const char* name, const char* filter);
		virtual bool	decode(unsigned char* output, int output_bpp, const unsigned char* input, unsigned input_size) = 0;
		virtual bool	inspect(int& w, int& h, int& bpp, const unsigned char* input, unsigned size) = 0;
	};
	int					width;
	int					height;
	int					scanline;
	int					bpp;
	unsigned char*		bits;
	surface();
	surface(int width, int height, int bpp);
	surface(const char* url, color* pallette = 0);
	~surface();
	constexpr explicit operator bool() const { return bits != 0; }
	static unsigned char* allocator(unsigned char* bits, unsigned size);
	void				clear() { resize(0, 0, 0, true); }
	void				convert(int bpp, color* pallette);
	void				flipv();
	unsigned char*		ptr(int x, int y);
	bool				read(const char* url, color* pallette = 0, int need_bpp = 0);
	void				resize(int width, int height, int bpp, bool alloc_memory);
	void				rotate();
	void				write(const char* url, color* pallette);
};
extern surface*			canvas;
extern rect				clipping; // Clipping area
extern fnevent			domodal;
extern point			dragmouse, caret;
extern color			fore, fore_stroke;
extern unsigned char    alpha;
extern int				width, height, width_maximum, height_maximum;
extern bool				text_clipped, control_hilited;
extern const sprite*	font; // Currently selected font
extern double			linw;
extern char				link[4096];
extern color*			palt;
extern int				tab_pixels;
extern bool				block_mode;
extern fnevent          pbackground, ptips, pwindow, pinput;
extern fnevent          pbeforemodal, pleavemodal, psetfocus;
struct rectpush {
	point				caret;
	int					width, height;
	constexpr rectpush() : caret(draw::caret), width(draw::width), height(draw::height) {}
	~rectpush() { draw::caret = caret; draw::width = width; draw::height = height; }
};
int						aligned(int x, int width, unsigned state, int string_width);
int						alignedh(const rect& rc, const char* string, unsigned state);
void					bezier(int x0, int y0, int x1, int y1, int x2, int y2);
void					bezierseg(int x0, int y0, int x1, int y1, int x2, int y2);
void					blit(surface& dest, int x, int y, int width, int height, unsigned flags, const surface& source, int x_source, int y_source);
void					blit(surface& dest, int x, int y, int width, int height, unsigned flags, const surface& source, int x_source, int y_source, int width_source, int height_source);
void					circle(int size);
void					circlef(int size);
void					create(int x, int y, int width, int height, unsigned flags, int bpp);
bool					dragactive(const void* p);
bool					dragactive();
void					dragbegin(const void* p);
void					dropshadow();
void					execute(fnevent proc, long value = 0, long value2 = 0, const void* object = 0);
int						getbpp();
inline rect				getrect() { return {caret.x, caret.y, caret.x + width, caret.y + height}; }
int						getheight();
int						getwidth();
void					getwindowpos(point& pos, point& size, unsigned* flags);
void					glyph(int sym, unsigned flags);
void					gradv(const color c1, const color c2, int skip = 0);
void					gradh(const color c1, const color c2, int skip = 0);
const sprite*			gres(const char* name, const char* folder = 0, point size = {});
int						hittest(int x, int test_x, const char* string, int lenght);
int						hittest(rect rc, const char* string, unsigned state, point mouse);
bool					ishilite(const rect& rc);
inline bool				ishilite() { return ishilite({caret.x, caret.y, caret.x + width, caret.y + height}); }
inline bool				ishilite(int size) { return ishilite({caret.x-size, caret.y - size, caret.x + size, caret.y + size}); }
void					image(int x, int y, const sprite* e, int id, int flags);
inline void				image(const sprite* e, int id, int flags) { image(caret.x, caret.y, e, id, flags); }
void					image(const sprite* e, int id, int flags, color* pal);
void					key2str(stringbuilder& sb, int key);
void					line(int x, int y); // Draw line
void					linet(int x, int y);
void					pixel(int x, int y);
void					pixel(int x, int y, unsigned char alpha);
unsigned char*			ptr(int x, int y);
int						rawinput();
void					rectb(); // Draw rectangle border
void					rectb(rect rc, int radius);
void					rectb3d(); // Draw rectangle border
void					rectf(); // Draw rectangle area. Right and bottom side is one pixel less.
void					rectfe(rect rc, int radius);
void					rectx();
void					rectfocus();
void					setcaption(const char* string);
void					setclip(rect rc);
inline void				setclip() { clipping.set(0, 0, getwidth(), getheight()); }
void					setoffset(int x, int y);
void					setpos(int x, int y);
void					setpos(int x, int y, int width, int height);
void					settimer(unsigned milleseconds);
const char*				skiptr(const char* string);
//void					spline(point* points, int n);
void					stroke(int x, int y, const sprite* e, int id, int flags, unsigned char thin = 1, unsigned char* koeff = 0);
void					syscursor(bool enable);
void					sysredraw();
void					text(const char* string, int count = -1, unsigned flags = 0);
int						text(rect rc, const char* string, unsigned state = 0, int* max_width = 0);
void					texta(const char* string, unsigned state = 0);
void					textas(const char* string);
void					textc(const char* string, int count = -1, unsigned flags = 0);
int						textbc(const char* string, int width);
int						texte(rect rc, const char* string, unsigned flags, int i1, int i2);
void					textf(const char* text, int min_height = 0, int* cashe_height = 0, const char** cashe_string = 0);
int						textf(rect& rc, const char* string);
int						textfs(const char* string);
int						texth();
int						texth(const char* string, int width);
int						textw(int sym);
int						textw(const char* string, int count = -1);
int						textw(rect& rc, const char* string);
int						textw(const sprite* font);
void					triangle(point v1, point v2, point v3);
void					triangle(point v1, point v2, point v3, color c1);
void					updatewindow();
void					write(const char* url, unsigned char* bits, int width, int height, int bpp, int scanline, color* pallette);
}
namespace draw {
struct awindowi {
	int				x, y, width, height;
	int				header_width;
	unsigned		flags;
};
extern awindowi		awindow;
void				applicationinitialize();
void				applicationafterinitialize();
void				fieldsetfocus();
void				simpleinitialize();
void				statusbeforemodal();
void				statuspaint();
void				tooltipsbeforemodal();
void				tooltipspaint();
void				loginitialize();
}
namespace draw {
void				breakmodal(long result);
bool				button(const char* title, unsigned key, fnbutton proc);
void				buttoncancel();
void				buttonok();
void				buttonparam();
void				cbsetsht();
void				cbsetint();
void				cbsetptr();
void				doredraw();
void				fire(bool run, fnevent proc, long value = 0, long value2 = 0, const void* object = 0);
long				getresult();
bool				isblocking();
bool				isnext();
void				initialize(const char* title);
bool				ismodal();
long				scene(fnevent proc);
long				sceneblocking(fnevent proc);
void				scene();
void				setneedupdate();
void				setnext(fnevent v);
//void				showscene(fnevent v);
void				start();
void				tooltips(const char* format, ...);
void				tooltips(int x, int y, int width);
}
namespace draw {
bool				isclipped(int size);
void				setposru();
void				setposlu();
void				setposld();
}
