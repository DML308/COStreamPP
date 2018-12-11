#include "1.firstScan.h"

FILE *changeTabToSpace()
{
    /* 使用一个 xx.cos.temp 的文件来把所有\t 转化为4个空格 ' ' */
    FILE *temp;
    const char *temp_name = getFileNameAll(string(infile_name) + ".temp").c_str();
    infp = fopen(infile_name, "r"); // infp 是在 global.h 中注册的输入文件指针
    if (infp == NULL)
    {
        error("%s:%d 无法打开该文件名对应的文件: \"%s\"\n", __FILE__, __LINE__, infile_name);
        exit(0);
    }
    temp = fopen(temp_name, "w");
    assert(temp != NULL);
    char ch;
    while ((ch = fgetc(infp)) != EOF)
    {
        if (ch != '\t')
            fputc(ch, temp);
        else
        {
            fputs("    ", temp);
        }
    }
    fclose(infp);
    fclose(temp);
    infp = fopen(temp_name, "r");
    assert(infp != NULL);
    return infp;
}

//获取
FILE * recordFunctionAndCompositeName()
{
    char line[1000];
    while (!feof(infp))
    {
        fgets(line, 1000, infp);
        cmatch cm;
        regex ef("\\s*(int|double|string|float|long)\\s+([a-zA-Z_][0-9a-zA-Z_]*)\\s*\\(");
        if (regex_search(line, cm, ef))
        {
            S.firstScanFuncTable[cm.str(2)] = true;
        }
        regex ec("\\s*(composite)\\s+([a-zA-Z_][0-9a-zA-Z_]*)\\s*\\(");
        if (regex_search(line, cm, ec))
        {
            S.firstScanCompTable[cm.str(2)] = true;
        }
    }
    //重新打开文件(即将文件读取的锚点移动回文件头)
    fclose(infp);
    const char *temp_name = getFileNameAll(string(infile_name) + ".temp").c_str();
    infp = fopen(temp_name, "r");
    assert(infp != NULL);
    return infp;
}
// 取文件名字 包括后缀 https://www.jianshu.com/p/4ea92d9688d1
static string getFileNameAll(string str)
{
    string::size_type idx = str.rfind('/', str.length());
    string name_all = str.substr(idx + 1, str.length());
    return name_all;
}

//在语法树使用完毕后删除 xx.cos.temp 临时文件
void removeTempFile()
{
    const char *temp_name = getFileNameAll(string(infile_name) + ".temp").c_str();
    if (remove(temp_name))
    {
        error("删除 temp 文件失败! Can't delete temp file!");
        exit(0);
    }
}
