ODIR = ./objects
SDIR = ./src
IDIR = ./include
MAIN = Covariance.cpp
EXECUTABLE = $(patsubst %.cpp,%, $(MAIN))

MAKEFLAGS = -j$(shell nproc)
FLAGS := $(shell root-config --cflags)
FLAGS += -I. -I$(IDIR)
FLAGS += -I$(APPLICATIONS_PATH)
FLAGS += -I$(BOOST_PATH)/include
OPTFLAG = $(FLAGS) -Wall -Wextra -O3 -MMD -MP

LIBS :=  $(shell root-config --libs)
LIBS += -lrt
LIBS += -L$(BOOST_PATH)/lib -lboost_filesystem -lboost_system -lboost_program_options

OBJS = $(patsubst %.cpp,%.o,$(addprefix $(ODIR)/,$(wildcard *.cpp)))
OBJS += $(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(wildcard $(SDIR)/*.cpp))

DEPS = $(patsubst %.o,%.d, $(OBJS))

.PHONY: clean

all: $(EXECUTABLE)

debug:OPTFLAG = $(FLAGS) -Wall -Wextra -O0 -g
debug:$(EXECUTABLE)

$(OBJS): | $(ODIR)
$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/$(MAIN:.cpp=.o): $(MAIN)
	$(CXX) $(OPTFLAG) -c -o $@ $<

$(ODIR)/%.o:$(SDIR)/%.cpp $(IDIR)/%.hpp
	$(CXX) $(OPTFLAG) -c -o $@ $<


$(EXECUTABLE): $(OBJS)
	$(CXX) -o $@  $^ $(LIBS)

clean:
	rm -f $(ODIR)/*.o $(DEPS) $(SDIR)/*~ $(IDIR)/*~ $(EXECUTABLE) *.root *~

-include $(DEPS)
