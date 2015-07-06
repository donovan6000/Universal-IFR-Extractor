
CXXFLAGS=-std=c++11 -ggdb
SOURCES=main-cli.o UEFI.o EFI.o

all: uifr

clean: 
	rm -f *.o 
	rm -f ifrextract

uifr: $(SOURCES)  
	@echo -e "  LINK\t\tifrextract"
	@$(CXX) -o ifrextract $(SOURCES)
	
.cpp.o:
	@echo -e "  CXX\t\t"$<
	@$(CXX) $(CXXFLAGS) -c $< -o $@
