#include "consts.h"

void print_help(char *name);


int main(int argc, char *argv[]) {
    if (argc <= 1) {
        print_help(argv[0]);
        return 1;
    }

    flags.FLAG_PRINT_LIST = FALSE;
    flags.FLAG_PRINT_FILE_SIZE_IN_LIST = FALSE;
    flags.FLAG_PRINT_TOTAL_SIZE = FALSE;
    flags.FLAG_PRINT_TOTAL_COUNT = FALSE;
    flags.FLAG_SORT = FALSE;
    flags.FLAG_PRINT_FORMATTED = TRUE;
    flags.FLAG_PRINT_COLOR = TRUE;
    flags.FLAG_DEBUG = FALSE;
    flags.FLAG_PRINT_ABSOLUTE_PATH = FALSE;

    char full_path[PATH_MAX];
    if (!_fullpath(full_path, argv[1], PATH_MAX) || 
        (full_path[strlen(full_path) - 1] == '\\' || full_path[strlen(full_path) - 1] == '/')) {
        full_path[strlen(full_path) - 1] = '\0';
    }

    for (int i = 2; i < argc; ++i) {
        if (!strcmp(argv[i], "--help")) return print_help(argv[0]), 0;
        if (!strcmp(argv[i], "--list")) flags.FLAG_PRINT_LIST = !flags.FLAG_PRINT_LIST;
        else if (!strcmp(argv[i], "--list-size")) flags.FLAG_PRINT_FILE_SIZE_IN_LIST = !flags.FLAG_PRINT_FILE_SIZE_IN_LIST;
        else if (!strcmp(argv[i], "--total-size")) flags.FLAG_PRINT_TOTAL_SIZE = !flags.FLAG_PRINT_TOTAL_SIZE;
        else if (!strcmp(argv[i], "--total-count")) flags.FLAG_PRINT_TOTAL_COUNT = !flags.FLAG_PRINT_TOTAL_COUNT;
        else if (!strcmp(argv[i], "--sort")) flags.FLAG_SORT = !flags.FLAG_SORT;
        else if (!strcmp(argv[i], "--formatted")) flags.FLAG_PRINT_FORMATTED = !flags.FLAG_PRINT_FORMATTED;
        else if (!strcmp(argv[i], "--color")) flags.FLAG_PRINT_COLOR = !flags.FLAG_PRINT_COLOR;
        else if (!strcmp(argv[i], "--debug")) flags.FLAG_DEBUG = !flags.FLAG_DEBUG;
        else if (!strcmp(argv[i], "--absolute")) flags.FLAG_PRINT_ABSOLUTE_PATH = !flags.FLAG_PRINT_ABSOLUTE_PATH;
        else return fprintf(stderr, "Unknown option: %s\n", argv[i]), 1;
    }

    if ((flags.FLAG_PRINT_FILE_SIZE_IN_LIST || flags.FLAG_SORT) && !flags.FLAG_PRINT_LIST)
        return fprintf(stderr, "Error: --list-size and --sort require --list.\n"), 1;

    if (!(flags.FLAG_PRINT_LIST || flags.FLAG_PRINT_TOTAL_SIZE || flags.FLAG_PRINT_TOTAL_COUNT))
        return fprintf(stderr, "Error: No valid flags provided for output.\n"), 1;

    print(full_path);
}

void print_help(char *name) {
    printf("--- HELP ---\n");
    printf("Usage: %s [OPTIONS]\n", name);
    printf("Options:\n");
    printf("  --help           Display this help message and exit.\n");
    printf("  --list           Toggle printing the list of files or items.\n");
    printf("  --list-size      Toggle printing the size of each file in the list (Default On).\n");
    printf("  --total-size     Toggle printing the total size of all files.\n");
    printf("  --total-count    Toggle printing the total count of items.\n");
    printf("  --sort           Toggle sorting of the list.\n");
    printf("  --formatted      Toggle formatted output (Default On).\n");
    printf("  --color          Toggle colored output (Default On).\n");
    printf("  --absolute       Toggle the printing of the absolute Path.\n");
    printf("  --debug          Toggle debug information.\n");
    printf("\nExamples:\n");
    printf("  %s --list --sort\n", name);
    printf("  %s --total-size --total-count\n", name);
}