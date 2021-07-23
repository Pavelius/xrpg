PRN = ../spacex.out
LST = action.o answers.o bstream.o bsreq.o color.o converter.o crt.o datetime.o draw.o draw_text.o draw_textf.o fraction.o game.o information.o io.o io_plugin.o io_plugin_bsreq.o io_plugin_json.o landscape.o location.o main.o moveable.o nix_draw.o nix_io_file.o point.o population.o property.o protoship.o resource_dynamic.o ship.o size.o squad.o stringbuilder.o surface_serial.o szdup.o variant.o variants.o view.o waitable.o zip.o
all: build
clean:
	rm *.o $(PRN)
build: $(LST)
	gcc -o $(PRN) $(LST) -lstdc++
%.o: %.cpp
	gcc -c $< -Wno-builtin-declaration-mismatch -g