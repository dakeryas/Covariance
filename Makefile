ODIR := ./objects
SDIR := ./src
IDIR := ./include
MAIN := Covariance.cpp
EXECUTABLE := $(patsubst %.cpp,%, $(MAIN))
DESTDIR := /usr/local

MAKEFLAGS := -j$(shell nproc)

CPPFLAGS += -I$(IDIR)
CPPFLAGS += -I$(shell root-config --incdir)
CPPFLAGS += -I$(APPLICATIONS_PATH)
CPPFLAGS += -I$(BOOST_PATH)/include

OPTFLAGS :=  -O3
CXXFLAGS += $(shell root-config --auxcflags)
CXXFLAGS += -Wall -Wextra -MMD -MP $(OPTFLAGS)

LDFLAGS +=  $(shell root-config --libs)
LDFLAGS += -lrt
LDFLAGS += -L$(BOOST_PATH)/lib -lboost_filesystem -lboost_system -lboost_program_options

OBJS := $(patsubst %.cpp,%.o,$(addprefix $(ODIR)/,$(wildcard *.cpp)))
OBJS += $(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(wildcard $(SDIR)/*.cpp))

DEPS := $(patsubst %.o,%.d, $(OBJS))

.PHONY: all debug install clean

all: $(EXECUTABLE)  

debug: OPTFLAGS := -O0 -g -DDEBUG
debug: all

install: $(EXECUTABLE)
	mkdir -p $(DESTDIR)/bin
	cp $< $(DESTDIR)/bin

$(OBJS): | $(ODIR)
$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/$(MAIN:.cpp=.o): $(MAIN)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

$(ODIR)/%.o:$(SDIR)/%.cpp $(IDIR)/%.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	
$(EXECUTABLE):$(OBJS)
	$(CXX) -o $@  $^ $(LDFLAGS)

clean:
	rm -f $(ODIR)/*.o $(DEPS) $(SDIR)/*~ $(IDIR)/*~ $(EXECUTABLE) *~
	
-include $(DEPS)





