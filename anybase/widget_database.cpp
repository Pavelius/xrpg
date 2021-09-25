#include "draw.h"
#include "draw_button.h"
#include "draw_clipboard.h"
#include "draw_input.h"
#include "draw_focus.h"
#include "draw_scroll.h"
#include "draw_valuelist.h"
#include "io_stream.h"
#include "lexer.h"
#include "handler.h"
#include "package.h"
#include "pointl.h"
#include "setting.h"

using namespace draw;

struct widget_database : public controls::table {

};