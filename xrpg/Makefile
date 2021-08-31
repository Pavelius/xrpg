LST = answers.o anyreq.o archive.o bstream.o bsreq.o code.o\
 code_pack.o code_scan.o color.o command.o crt.o crt_file.o crt_translate.o datetime.o\
 draw.o draw_builder.o draw_button.o draw_commands.o draw_control.o draw_combobox.o draw_dockbar.o\
 draw_field.o draw_figure.o draw_focus.o draw_input.o draw_list.o draw_scroll.o draw_scrollable.o\
 draw_simple.o draw_statusbar.o draw_splitter.o\
 draw_table.o draw_tabs.o draw_text.o draw_textf.o draw_toolbar.o draw_theme.o\
 draw_valuelist.o\
 gender.o io_stream.o io_plugin.o io_plugin_bsreq.o io_plugin_json.o log.o moveable.o markup.o menu.o point.o\
 resource_dynamic.o\
 screenshoot.o setting.o string.o stringbuilder.o surface_serial.o szdup.o\
 variant.o varianta.o
PLATFORM_LST = nix_clipboard.o nix_draw.o nix_io_file.o nix_menu.o nix_draw_shell.o
PLUGIN_LST = png.o zip.o
WIDGET_LST = widget_application.o widget_codetree.o widget_codeview.o widget_logging.o widget_picture.o
all: libxface.a
clean:
	rm *.o *.a
libxface.a: $(LST) $(PLATFORM_LST) $(PLUGIN_LST) $(WIDGET_LST)
	ar cr libxface.a $^
	rm *.o
%.o: %.cpp
	gcc -c $< -g -O2