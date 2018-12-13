#include "global.h"

char *infile_name = "stdin";      // input file's name
char *outfile_name = "stdout";    // output file's name
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
/**
 * @brief 输出一段波浪线来强调一行源码中的一段代码片段
 * @param column 要强调位置的中心点
 */
static void printWaveLine(int column)
{
    int i = 0;
    for (i = 0; i < column - 1; i++)
        line(" ");
    line("^\n");
};
/**
 * @brief 发现输入的.cos文件有错误时输出报错信息
 * @param msg       报错信息
 * @param file_name 当前文件名,默认为 stdin
 * @param line      出错行号
 * @param column    需要强调的标识符的列号
 */
void SyntaxError(const char *msg, const char *file_name, int line, int column)
{
    error("[%s:%d]  error: %s\n", file_name, line, msg);
    show_line(line,file_name);
    printWaveLine(column);
}
/** 和上面函数类似,处理 Warning **/
void SyntaxWarning(const char *msg, const char *file_name, int line, int column)
{
    warning("[%s:%d]  warning: %s\n", file_name, line, msg);
    show_line(line, file_name);
    printWaveLine(column);
}
