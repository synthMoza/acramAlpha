CXX = g++
CXXFLAGS = -Wall -g -MD -O3

# Files and directories to be removed after cleaning
RMEXEC = diff
RMFILES = *.d *.o *.tex *.aux *.log *.out

.PHONY: all clean

all: diff

diff: main.o differentiator.o expression.o
	$(CXX) -o $@ $^

%.o: %.c
	$(CXX) -c -o $@ $< $(CXXFLAGS)

# Generate and open PDF file from generated LaTeX file
latex: output.tex
	pdflatex $<
	xdg-open output.pdf

clean:
	rm -rf $(RMEXEC) $(RMFILES)

-include *.d