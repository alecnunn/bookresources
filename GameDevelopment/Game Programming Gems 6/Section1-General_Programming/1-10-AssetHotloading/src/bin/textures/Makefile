CONVERTER = ../converter

tgas = texture1.tga texture2.tga texture3.tga
.SUFFIXES: .tga .texture

textures = $(patsubst %.tga, %.texture, $(tgas))

all: $(textures)

.tga.texture:
	$(CONVERTER) $<
	
clean:
	rm $(textures)
