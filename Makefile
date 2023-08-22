PACKAGE = CompareLEDRuns

ROOTFLAGS = $(shell root-config --cflags)
ROOTLIBS = $(shell root-config --glibs)


CXXFLAGS = -I.  $(ROOTFLAGS) -I$(ONLINE_MAIN)/include -I$(OFFLINE_MAIN)/include
RCFLAGS = -I.  -I$(ONLINE_MAIN)/include -I$(OFFLINE_MAIN)/include

LDFLAGS = -Wl,--no-as-needed  -L$(ONLINE_MAIN)/lib -L$(OFFLINE_MAIN)/lib  -lEvent -lNoRootEvent -lmessage  $(ROOTLIBS) -fPIC



HDRFILES = $(PACKAGE).h
LINKFILE = $(PACKAGE).h


ADDITIONAL_SOURCES = GetLEDData.cc 
ADDITIONAL_LIBS = 


SO = lib$(PACKAGE).so

#$(SO) : $(PACKAGE).cc $(PACKAGE)_dict.C $(ADDITIONAL_SOURCES) $(LINKFILE)
#	$(CXX) $(CXXFLAGS) -o $@ -shared  $<  $(ADDITIONAL_SOURCES) $(PACKAGE)_dict.C $(LDFLAGS)  $(ADDITIONAL_LIBS)
$(PACKAGE): $(PACKAGE).o GetLEDData.o
$(PACKAGE).o : $(PACKAGE).cc $(PACKAGE)_dict.C $(ADDITIONAL_SOURCES) $(LINKFILE)
	$(CXX) $(CXXFLAGS) -o $@ -shared  $<  $(ADDITIONAL_SOURCES) $(PACKAGE)_dict.C $(LDFLAGS)  $(ADDITIONAL_LIBS)

$(PACKAGE)_dict.C : $(HDRFILES) $(LINKFILE)
	rootcint -f $@  -c $(RCFLAGS) $^

GetLEDData.o: $(ADDITIONAL_SOURCES) $(CXX) 
	 $(CXXFLAGS) -o $@ -shared $< $(LDFLAGS) $(ADDITIONAL_LIBS)

.PHONY: clean

clean: 
	rm -f $(SO) $(PACKAGE)_dict.C $(PACKAGE)_dict.h

