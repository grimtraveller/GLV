include ./Makefile.config
CFLAGS += -I./include/
SRCDIR = ./src

SRC = 	$(SRCDIR)/glv_buttons.cpp \
	$(SRCDIR)/glv_color.cpp \
	$(SRCDIR)/glv_core.cpp \
	$(SRCDIR)/glv_draw.cpp \
	$(SRCDIR)/glv_glv.cpp \
	$(SRCDIR)/glv_inputdevice.cpp \
	$(SRCDIR)/glv_layout.cpp \
 	$(SRCDIR)/glv_plots.cpp \
	$(SRCDIR)/glv_sliders.cpp \
	$(SRCDIR)/glv_texture.cpp \
	$(SRCDIR)/glv_textview.cpp \
	$(SRCDIR)/glv_view.cpp \
	$(SRCDIR)/glv_view3D.cpp

ifndef BINDING_NONE
	SRC += $(SRCDIR)/glv_binding.cpp $(SRCDIR)/$(BINDING_SRC)
endif

OBJ = $(SRC:.cpp=.o)

.cpp.o:
	@echo CC $<
	@$(CC) -c $(CFLAGS) -o $*.o $<

libglv.a: $(OBJ)
	@echo AR $@
	@$(AR) $@ $(OBJ)
	@$(RANLIB) $@

.PHONY: tests

test: libglv.a
	@cd test && make all

clean:
	@rm -f $(OBJ) libglv.a
	@cd test && make clean

current:
	libglv.a

all: libglv.a test
