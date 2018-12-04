#include "handle_options.h"
#include "defines.h"
#include "global.h"
//处理命令行输入的选项
int handle_options(int argc, char *argv[]){
    int c;
    int version_flag = 0;
    int help_flag = 0;
    struct option long_options[] =
        {
            {"version", no_argument, &version_flag, 'v'},
            {"help", no_argument, &help_flag, 'h'},
            {"Cpucore", required_argument, NULL, 'j'},
            {"i",required_argument,NULL,'i'},
            {0, 0, 0, 0}};
    int option_index = 0;
    opterr = 0;

    while ((c = getopt_long(argc, argv, ":hvo:d:", long_options, &option_index)) != -1)
    {
        switch (c)
        {
        case 0:
            break;
        case 'h':
            help_flag = 1;
            break;
        case 'v':
            print_version_info();
            break;
        case 'o':
            outfile_name = optarg;
            break;
        case 'd':
            dumpfile_name = optarg;
            codefile_name = (char *)malloc(sizeof(char) * (200));
            strcpy(codefile_name, dumpfile_name);
            strcat(codefile_name, ".ll");
            codefp = fopen(codefile_name, "w");
            if (codefp == NULL)
            {
                error("Can not open codefpfile %s\n", codefile_name);
                exit(0);
            }
            break;
        case '?':
            error("Unknown option -%c\n", optopt);
            return false;
        case ':':
            if (optopt == 'o')
                error("Option -o requires an argument\n");
            else if (optopt == 'd')
                error("Option -d requires an argument\n");
            else
                error("Unknown option -%c\n", optopt);
            return false;
        default:
            error("Error in handle_opt()\n");
        }
    }

    if (help_flag)
    {
        printf("usage: parser  [options] [file]\n");
        printf("-h  --help     print this usage and exit\n");
        printf("-v  --version  print version and exit\n");
        printf("-o <file>      place the output into <file>\n");
        printf("-d <file>      dump AST into <file>\n");
        return false;
    }
    if (version_flag)
    {
        printf("parser V5.8 & AST & dumpdot\n");
        return false;
    }
    if (optind < argc)
        infile_name = argv[optind];
    if (infile_name == NULL)
        infp = stdin;
    else
    {
        ////////////////////////////
        /* change '\t' with 8 ' ' */
        ////////////////////////////
        FILE *tempinfp, *tempoutfp;
        char *tempfile_name = (char *)malloc(sizeof(char) * (strlen(infile_name) + strlen(".temp.c") + 1));
        tempinfp = fopen(infile_name, "r");
        strcpy(tempfile_name, infile_name);
        strcat(tempfile_name, ".temp.c");
        tempoutfp = fopen(tempfile_name, "w");

        if (tempoutfp == NULL)
        {
            error("Can not open infile %s\n", infile_name);
            return false;
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
        /* change '\t' with 8 ' ' end */
        ////////////////////////////////
        infp = fopen(tempfile_name, "r");
        if (infp == NULL)
        {
            error("Can not open infile %s\n", tempfile_name);
            return false;
        }
    }
    if (outfile_name == NULL)
        outfp = stdout;
    else
    {
        outfp = fopen(outfile_name, "w");
        if (outfp == NULL)
        {
            error("Can not open outfile %s\n", outfile_name);
            return false;
        }
    }
    if (dumpfile_name == NULL)
        dumpfp = NULL;
    else
    {
        dumpfp = fopen(dumpfile_name, "w");
        if (dumpfp == NULL)
        {
            error("Can not open dumpfile %s\n", dumpfile_name);
            return false;
        }
    }
    return true;
}

void print_version_info()
{
    fprintf(stderr, "Version %.02f (%s)\n",VersionNumber, CompiledDate);
    exit(0);
}
