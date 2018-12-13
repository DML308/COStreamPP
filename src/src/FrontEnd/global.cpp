#include "global.h"

char *infile_name = NULL;   // input file's name
char *outfile_name = NULL;  // output file's name
FILE *infp = NULL;   // input file's pointer, default is stdin
FILE *outfp = NULL;  // output file's pointer, default is stdout

static void show_line(int line, const char *file_name)
{
    std::string line_str;
    line_str = "";
    line_str += "./bin/sed.sh '";
    char temp[10];
    sprintf(temp, "%d", line);
    line_str += temp;
    line_str += "p' ";
    line_str += file_name;
    line_str += ".temp.c";
    system(line_str.c_str());
}
static void printWaveLine(int column)
{
    int i = 0;
    for (i = 0; i < column - 1; i++)
        line(" ");
    line("^\n");
};
void SyntaxError(const char *msg, const char *file_name, int line, int column = 0)
{
    line("Line:%-4d",line);
    error("%s\n", msg);
    show_line(line,file_name);
    printWaveLine(column);
}
void SyntaxWarning(const char *msg, const char *file_name, int line, int column = 0)
{
    line("Line:%-4d", line);
    error("%s\n", msg);
    show_line(line, file_name);
}
