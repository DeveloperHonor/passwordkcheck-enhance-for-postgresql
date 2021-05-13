# contrib/passwordcheck_enhance/Makefile

MODULE_big = passwordcheck_enhance
OBJS = passwordcheck_enhance.o $(WIN32RES)
PGFILEDESC = "passwordcheck_enhance - strengthen user password checks"

# uncomment the following two lines to enable cracklib support
PG_CPPFLAGS = -DUSE_CRACKLIB '-DCRACKLIB_DICTPATH="/usr/lib/cracklib_dict"'
SHLIB_LINK = -lcrack

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/passwordcheck_enhance
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
