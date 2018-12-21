SUBDIRS = po maximus mate-window-picker-applet

ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

DISTCHECK_CONFIGURE_FLAGS = \
	CFLAGS='-Wno-deprecated-declarations'

EXTRA_DIST = autogen.sh intltool-merge.in intltool-extract.in intltool-update.in
DISTCLEANFILES = intltool-merge intltool-update intltool-extract

MAINTAINERCLEANFILES = aclocal.m4 compile config.guess config.sub configure depcomp install-sh ltmain.sh Makefile.in missing

# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog

