include src_list.make

INC_EX_LIBS = $(PHYSFS_INC) $(SDL_INC) $(LUA_INC)
INC_INTERN = -I. -Iutil -Icoldet -Imath -Iopensteer/include

INC = $(INC_INTERN) $(INC_EX_LIBS)

FLAGS = $(WARN) $(DEBUG) $(OPT)

BUILD_FOR = $(shell if [ -n "$(HOST)" ]; then echo $(HOST) ; else echo "LINUX"; fi)

#SDL_GL_LIB = -lGL -lGLU

#COLDET_LIB = -Wl,-Rcoldet -Lcoldet -lcoldet

CXXFLAGS=$(FLAGS) $(DEFS) \
$(INC)

src_list.make: prepare_build.sh
	./prepare_build.sh $(BUILD_FOR)
	$(MAKE) depend

%.o: %.cpp
	$(CXX) $(FLAGS) $(DEFS) \
  $(INC) \
    -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) \
	-c -o $@ $<


ctags:
	ctags *.cpp util/*.cpp

coldet/libcoldet.a:
	make -C coldet -f makefile.g++ all

clean:
	rm -f tags depend *.o tools/*.o coldet/*.o lua_addon/*.o math/*.o util/*.o opensteer/src/Clock.o \
	spriteplayer gfxextract viewer $(TOOLS) objdump objdump_map minimap slopeview luaviewer g24

html: doxy_main.h
	doxygen
doxygen: doxy_main.h
	doxygen

doxy_main.h: doc/hacking.txt tools/doxy_doc.sh
	./tools/doxy_doc.sh > doxy_main.h

doxyclean:
	$(RM) doxy_main.h -r doc/html

package: release_files_sorted
	sh tools/cvslog.sh
	(cd .. && tar jvcf ogta_src_`date +%F`.tar.bz2 -T ogta/release_files_sorted)
	@echo "saved as: ogta_src_`date +%F.tar.bz2`"
	
rclean:
	make libclean
	make clean
	make doxyclean
	rm -f src_list.make

libclean:
	make -C coldet -f makefile.g++ clean
	rm -f $(OSTEER_OBJ)
	

depend: src_list.make
	$(RM) depend
	$(CXX) $(CXXFLAGS) -E -MM $(GL_SRC) $(OGTA_SRC) $(UTIL_SRC) > depend
