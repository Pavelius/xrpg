PRN = ../xrpg.out
LST = answers.o anyreq.o archive.o bstream.o bsreq.o code.o code_pack.o code_scan.o color.o command.o crt.o crt_file.o crt_translate.o draw.o draw_button.o draw_commands.o draw_control.o draw_figure.o draw_focus.o draw_input.o draw_text.o draw_textf.o draw_toolbar.o draw_theme.o draw_valuelist.o gender.o io_stream.o io_plugin.o io_plugin_bsreq.o io_plugin_json.o log.o moveable.o nix_clipboard.o nix_draw.o nix_io_file.o nix_menu.o markup.o menu.o point.o resource_dynamic.o screenshoot.o string.o stringbuilder.o surface_serial.o szdup.o variant.o varianta.o zip.o
all: build
clean:
	rm *.o $(PRN)
build: $(LST)
	gcc -o $(PRN) $(LST) -lstdc++
%.o: %.cpp
	gcc -c $< -g