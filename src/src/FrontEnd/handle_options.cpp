#include "handle_options.h"

float VersionNumber = 0.10;
const char *const CompiledDate = __DATE__;

//处理命令行输入的选项
int handle_options(int argc, char *argv[]){
    int c;
    struct option long_options[] =
        {
            {"version", no_argument, NULL, 'v'},
            {"help", no_argument, NULL, 'h'},
            {"nCpucore", required_argument, NULL, 'j'},
            {"nowarning",no_argument,NULL,'w'},
            {"output",required_argument,NULL,'o'},
            {0, 0, 0, 0}};
    int option_index = 0;
    while ((c = getopt_long(argc, argv, ":hvwo:j:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 0:
            break;
        case 'h':
            print_usage();
            break;
        case 'v':
            print_version_info();
            break;
        case 'o':
            outfile_name = optarg;
            break;
        case 'w':
            error("-w 还没写完呢\n");
            break;
        case 'j':
            error("-j 还没写完呢\n");
            break;
        case '?':
            error("unknown option: %c\n", optopt);
            break;
        case ':':
            if (optopt == 'o')
                error("Option -o requires an argument\n");
            else if (optopt == 'j')
                error("Option -j requires an argument\n");
            else
                error("Unknown option -%c\n", optopt);
            return false;
        default:
            error("Error in handle_opt()\n");
        }
    }
    if (optind < argc)
        infile_name = argv[optind];
    if (infile_name == NULL)
        infp = stdin;
    else
    {
        changeTabToSpace();
    }
    if (outfile_name == NULL)
        outfp = stdout;
    else
    {

    }
    return true;
}
void print_version_info()
{
    fprintf(stderr, "COStream\nVersion %.02f (Compiled Date: %s)\n\n",VersionNumber, CompiledDate);
    exit(0);
}
void print_usage(){
    fprintf(stderr, "Usage: COStream  [options] [file]\n");
    fprintf(stderr,
            "\n"
            "Parses <file> as a COStream program, reporting syntax and type errors, and writes\n"
            "paralleled program out to <file>. If <file> is null, uses "
            "stdin and stdout.\n"
            "\n");
    fprintf(stderr, "General Options:\n");
    fprintf(stderr, "      -h, --help              print this usage and exit\n");
    fprintf(stderr, "      -v, --version           print Version Number and Compiled Date\n");
    fprintf(stderr, "      -w, --nowarning         ignore warnings\n");
    fprintf(stderr, "      -o, <dir>               place the output files into <dir>\n");
    fprintf(stderr, "      -j, --nCpucore <number> set number of threads\n");
    fprintf(stderr, "\n");
    exit(0);
}
FILE * changeTabToSpace(){
    /* 使用一个 xx.cos.temp 的文件来把所有\t 转化为4个空格 ' ' */
    FILE *temp;
    const char *temp_name = getFileNameAll(string(infile_name) + ".temp").c_str() ;
    infp = fopen(infile_name, "r");  // infp 是在 global.h 中注册的输入文件指针
    if(infp == NULL){
        error("%s:%d 无法打开该文件名对应的文件: \"%s\"\n", __FILE__ , __LINE__ , infile_name);
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
            fputs("    ",temp);
        }
    }
    fclose(infp);
    fclose(temp);
    infp = fopen(temp_name, "r");
    assert(infp != NULL);
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
