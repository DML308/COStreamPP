#include "1.firstScan.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
extern int errno; //用于创建文件夹的报错, 参考https://blog.csdn.net/u011866460/article/details/40888943

FILE *changeTabToSpace()
{
    /* 使用一个 xx.cos.temp 的文件来把所有\t 转化为4个空格 ' ' */
    
    FILE *temp;
    temp_name = getFileNameAll(string(infile_name) + ".temp");
    infp = fopen(infile_name.c_str(), "r"); // infp 是在 global.h 中注册的输入文件指针
    if (infp == NULL)
    {
        error("%s:%d 无法打开该文件名对应的文件: \"%s\"\n", __FILE__, __LINE__, infile_name.c_str());
        exit(0);
    }
    temp = fopen(temp_name.c_str(), "w");
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
    infp = fopen(temp_name.c_str(), "r");
    assert(infp != NULL);
    return infp;
}

//在第一遍扫描时, 将函数和 Composite 变量名存入符号表 S 的对应位置中方便后续使用
//考虑带注释的情形
FILE *recordFunctionAndCompositeName()
{
    char line[1000];
    int comment_flag = 0;
    while (!feof(infp))
    {
        fgets(line, 1000, infp);
        cmatch cm;
        //检查注释
        // 情况1:如果现在是多行注释状态
        if (comment_flag == 1)
        {
            regex comment2("\\*/");
            if (regex_search(line, comment2))
                comment_flag = 0;
        }
        // 情况2:如果不是多行注释状态
        else
        {
            //如果是单行注释
            regex comment1("\/\/");
            if (regex_search(line, comment1))
                continue;
            //如果遇到了多行注释的开头
            regex comment3("\\*");
            if (regex_search(line, comment3))
            {
                //如果在这一行里直接检测到了多行注释的结尾
                regex comment4("\\*/");
                if(regex_search(line, comment4))
                    continue;
                //如果在这一行里检测不到多行注释的结尾
                comment_flag = 1;
                continue;
            }
            //终于到了非注释文本的识别
            regex efunc("\\s*(int|double|string|float|long)\\s+([a-zA-Z_][0-9a-zA-Z_]*)\\s*\\(");
            if (regex_search(line, cm, efunc))
            {
                S.firstScanFuncTable[cm.str(2)] = true;
            }
            regex ecomp("\\s*(composite)\\s+([a-zA-Z_][0-9a-zA-Z_]*)\\s*\\(");
            if (regex_search(line, cm, ecomp))
            {
                S.firstScanCompTable[cm.str(2)] = true;
            }
        }
    }
    //重新打开文件(即将文件读取的锚点移动回文件头)
    fclose(infp);
    infp = fopen(temp_name.c_str(), "r");
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

//切割输入文件字符串获取文件名, 不包括后缀
string getFileName(string path)
{
    int pos = path.find_last_of('/');
    string nameWithSuffix(path.substr(pos + 1));
    string pureName = nameWithSuffix.substr(0, nameWithSuffix.rfind("."));
    return pureName;
}
void setOutputPath()
{
    origin_path = getcwd(NULL, NULL);
    if (output_path == "stdout")
    {
        /* 获取当前文件目录 用于拼接文件目录*/
        /* 默认输出路径为 ./StaticDistCode/filename */
        char buf[1024];
        char *path = getcwd(buf, 1024);
        umask(0); //重置文件夹权限继承
        mkdir("StaticDistCode", 0754);
        //切换文件目录
        chdir("StaticDistCode");
        string localOutputPath = getFileName(infile_name);
        mkdir(localOutputPath.c_str(), 0754);
        chdir(localOutputPath.c_str());
    }
    else
    {
        /* 用户输入了指定路径,例如 -o output , 则应该新建文件夹 */
        umask(0); //重置文件夹权限继承
        mkdir(infile_name.c_str(), 0754);
        chdir(infile_name.c_str());
    }
}

//在语法树使用完毕后删除 xx.cos.temp 临时文件
void removeTempFile()
{
    chdir(origin_path.c_str());
    const string temp_name = getFileNameAll(string(infile_name) + ".temp").c_str();
    if (remove(temp_name.c_str()))
    {
        error("删除 temp 文件失败! Can't delete temp file!");
        exit(0);
    }
}
