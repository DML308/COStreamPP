#include "0.handle_options.h"

float VersionNumber = 0.30;
const char *const CompiledDate = __DATE__;
int WarningLevel = 99; //大于99级作用域 Level 后不报警告,即默认大部分情况都报警告

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
            WarningLevel = 0;//大于等于0级的作用域中需要报警告时直接 return, 即不报警告
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

