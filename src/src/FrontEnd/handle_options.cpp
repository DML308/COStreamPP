#include "handle_options.h"
#include "defines.h"
#include "global.h"

//处理命令行输入的选项
int handle_options(int argc, char *argv[]){
    int c;
    //printf("err:%d\n",opterr);
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

float VersionNumber = 0.10;
const char *const CompiledDate = __DATE__;
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
    ////////////////////////////
    /* change '\t' with 8 ' ' */
    ////////////////////////////
    /*
    FILE *tempinfp, *tempoutfp;
    char *tempfile_name = (char *)malloc(sizeof(char) * (strlen(infile_name) + strlen(".temp.c") + 1));
    tempinfp = fopen(infile_name, "r");
    strcpy(tempfile_name, infile_name);
    strcat(tempfile_name, ".temp.c");
    tempoutfp = fopen(tempfile_name, "w");

    if (tempoutfp == NULL)
    {
        error("Can not open infile %s\n", infile_name);
        exit(0);
    }
    char ch;
    while ((ch = fgetc(tempinfp)) != EOF)
    {
        if (ch != '\t')
            fputc(ch, tempoutfp);
        else
        {
            ch = ' ';
            fputc(ch, tempoutfp);
            fputc(ch, tempoutfp);
            fputc(ch, tempoutfp);
            fputc(ch, tempoutfp);
        }
    }
    fclose(tempinfp);
    fclose(tempoutfp);
    ////////////////////////////////
    // change '\t' with 8 ' ' end 
    ////////////////////////////////
    infp = fopen(tempfile_name, "r");
    if (infp == NULL)
    {
        error("Can not open infile %s\n", tempfile_name);
        exit(0);
    }*/
}
