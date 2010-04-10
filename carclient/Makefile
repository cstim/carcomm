RBUIC4 = rbuic4
RBRCC = rbrcc

SOURCES = carclient.rb
#UI = main.ui
#QRC = main.qrc

ALL_FILES = $(UI:%.ui=%.rb) $(QRC:%.qrc=%.rb)

all: $(ALL_FILES)

# General rules

.ui.rb:
	$(RBUIC4) $< -o $@

.qrc.rb:
	$(RBRCC) $< -o $@

.SUFFIXES = .rb .ui .qrc
