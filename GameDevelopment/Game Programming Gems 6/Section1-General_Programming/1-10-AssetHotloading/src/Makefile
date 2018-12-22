subdirs = converter viewer bin/textures

all: $(subdirs)
	@for i in $(subdirs); do     $(MAKE) -C $$i all; done

clean:
	@for i in $(subdirs); do        (cd $$i; $(MAKE) clean); done
