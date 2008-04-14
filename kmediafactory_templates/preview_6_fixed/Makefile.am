SUBDIRS = images locale
kmediafactorytemplatedir = $(kde_datadir)/kmediafactory_template
kmediafactorytemplate_DATA = preview_6_fixed.kmft
kmediafactorytemplate_files = mimetype\nsettings.kcfg\nsettings.ui\ntemplate.xml\nicon.png\npreview.jpg\nimages/button.png\nimages/prev.png\nimages/back.png\nimages/next.png

preview_6_fixed.kmft:
	export builddir=`pwd` && cd $(srcdir) && \
	echo -e "$(kmediafactorytemplate_files)" | $(ZIP) -X $$builddir/preview_6_fixed.kmft -@
	$(ZIP) -X preview_6_fixed.kmft -r locale -x \*Makefile \*.svn*

kmft-messages:
	$(EXTRACTRC) --tag=text --tag-group=none $(srcdir)/template.xml > rc.cpp
	$(XGETTEXT) --keyword=i18n rc.cpp -o template.pot
	echo "//i18n: file ./template.xml" > rc.cpp
	echo "//xgettext: no-c-format" >> rc.cpp
	(tmp=`grep "<template" template.xml | sed -e 's/^.*name="//' | \
	     sed -e 's/".*//'`; echo "i18n(\"$$tmp\");") >> rc.cpp
	$(EXTRACTRC) $(srcdir)/settings.ui >> rc.cpp
	$(XGETTEXT) --keyword=i18n rc.cpp -o ui.pot

kmft-merge:
	@for f in locale/*; do \
		if test -d "$$f"; then \
			for pot in "template" "ui"; do \
				if test -e "$$pot.pot"; then \
					msgmerge -U "$$f/LC_MESSAGES/$$pot.po" "$$pot.pot"; \
				fi \
			done \
		fi \
	done

CLEANFILES = preview_6_fixed.kmft