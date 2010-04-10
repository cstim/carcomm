RBUIC4 = rbuic4
RBRCC = rbrcc

SOURCES = carclient.rb
UI = mainwindow.ui
QRC = ressources.qrc

GENERATEDFILES = $(UI:%.ui=%.ui.rb) $(QRC:%.qrc=%.qrc.rb)
ALL_FILES = $(GENERATEDFILES)

all: $(ALL_FILES)

clean:
	rm -f $(GENERATEDFILES)

# General rules

.SUFFIXES: .ui .ui.rb .qrc .qrc.rb

.ui.ui.rb:
	$(RBUIC4) $< -o $@

.qrc.qrc.rb:
	$(RBRCC) $< -o $@

