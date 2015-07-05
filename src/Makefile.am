# EU-tire-label - Makefile.am
# Copyright (c) 2015 Arkadiusz Bokowy

bin_PROGRAMS = eu-tire-label

BUILT_SOURCES = \
	label-template.h

eu_tire_label_SOURCES = \
	label.c \
	main.c

if ENABLE_RSVG
eu_tire_label_SOURCES += raster.c
AM_CFLAGS = @LIBRSVG_CFLAGS@
LDADD = @LIBRSVG_LIBS@
endif

label-template.h: $(srcdir)/label.svg
	echo -n 'const char label_template[] = "' >$@
	sed -e 's/"/\\"/g' $< |tr -d '\t\n' >>$@
	echo '";' >>$@
