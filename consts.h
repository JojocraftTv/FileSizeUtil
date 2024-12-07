#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define BOOL int
#define TRUE 1
#define FALSE 0
#define PATH_MAX 4096

#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"

void print(char * fullPath);

typedef struct {
    BOOL FLAG_PRINT_LIST;
    BOOL FLAG_PRINT_FILE_SIZE_IN_LIST; 
    BOOL FLAG_PRINT_TOTAL_SIZE; 
    BOOL FLAG_PRINT_TOTAL_COUNT; 
    BOOL FLAG_SORT; 
    BOOL FLAG_PRINT_FORMATTED;
    BOOL FLAG_PRINT_COLOR;
    BOOL FLAG_DEBUG;
    BOOL FLAG_PRINT_ABSOLUTE_PATH;
} FLAGS;
FLAGS flags;