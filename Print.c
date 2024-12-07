#include "consts.h"

typedef struct {
    char filePath[PATH_MAX];
    DWORD size;
    double totalPercentage;      
    double subDirPercentage;    
} FileEntry;

typedef struct {
    char dirPath[PATH_MAX];
    FileEntry *files;
    int fileCount;
    unsigned long long totalSize;
    double totalPercentage; 
} DirDetails;


void process_directory(const char *path, DirDetails **directories, int *dirCount);
void sort_directories(DirDetails *directories, int dirCount);
void sort_files(FileEntry *files, int fileCount);
int compare_dirs(const void *a, const void *b);
int compare_files(const void *a, const void *b);
unsigned long long calculate_total_size(DirDetails *directories, int dirCount);
unsigned long long calculate_subdir_size(DirDetails *directories, int dirCount, const char *dirPath);
void print_hierarchy(const DirDetails *directories, int dirCount, const char *topDir, const char *currentDir, int depth);

void print(char * fullPath) {
    DirDetails *directories = NULL;
    int dirCount = 0;
    process_directory(fullPath, &directories, &dirCount);
    unsigned long long grandTotal = 0;
    if(flags.FLAG_PRINT_TOTAL_SIZE || flags.FLAG_SORT) grandTotal = calculate_total_size(directories, dirCount);
    if(flags.FLAG_SORT) {
        if(flags.FLAG_DEBUG)  printf("[DEBUG] Sorting Index...\n");
        for (int i = 0; i < dirCount; i++) {
            directories[i].totalPercentage = ((double)directories[i].totalSize / grandTotal) * 100.0;
            unsigned long long subDirSize = calculate_subdir_size(directories, dirCount, directories[i].dirPath);

            for (int j = 0; j < directories[i].fileCount; j++) {
                directories[i].files[j].totalPercentage =
                    ((double)directories[i].files[j].size / grandTotal) * 100.0;
                directories[i].files[j].subDirPercentage =
                    ((double)directories[i].files[j].size / subDirSize) * 100.0;
            }
        }
        sort_directories(directories, dirCount);
        for (int i = 0; i < dirCount; i++) {
            sort_files(directories[i].files, directories[i].fileCount);
        }
    }
    if(flags.FLAG_PRINT_LIST) {
        if(flags.FLAG_DEBUG)  printf("[DEBUG] Printing Output... \n");
        if(flags.FLAG_PRINT_COLOR) printf(COLOR_BLUE "%s" COLOR_RESET "\n", fullPath); else printf("%s\n", fullPath);
        print_hierarchy(directories, dirCount, fullPath, fullPath, 0);
    }
    if(flags.FLAG_PRINT_TOTAL_SIZE)
        printf("Total-Size: %llu bytes\n", grandTotal);
      if(flags.FLAG_PRINT_TOTAL_COUNT) {
        int totalFileCount = 0;
        for (int i = 0; i < dirCount; i++) {
            totalFileCount += directories[i].fileCount;
        }
        printf("Total-Files: %d\n", totalFileCount);
    }   
    if(flags.FLAG_DEBUG)  printf("[DEBUG] Freeing Memory... \n");
    for (int i = 0; i < dirCount; i++) {
        free(directories[i].files);
    }
    free(directories);
}

void process_directory(const char *path, DirDetails **directories, int *dirCount) {
    if (flags.FLAG_DEBUG) printf("[DEBUG] Processing directory: %s\n", path);
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char searchPath[PATH_MAX];
    snprintf(searchPath, PATH_MAX, "%s\\*", path);
    hFind = FindFirstFile(searchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        DWORD err = GetLastError();
        printf("[ERROR] Unable to access directory: %s\n", path);
        return;
    }
    DirDetails dirDetails = {0};
    strncpy(dirDetails.dirPath, path, PATH_MAX);
    dirDetails.files = NULL;
    dirDetails.fileCount = 0;
    dirDetails.totalSize = 0;
    do {
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }
        char fullPath[PATH_MAX];
        snprintf(fullPath, PATH_MAX, "%s\\%s", path, findFileData.cFileName);
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            process_directory(fullPath, directories, dirCount);
        } else {
            dirDetails.files = realloc(dirDetails.files, (dirDetails.fileCount + 1) * sizeof(FileEntry));
            strncpy(dirDetails.files[dirDetails.fileCount].filePath, fullPath, PATH_MAX);
            dirDetails.files[dirDetails.fileCount].size = findFileData.nFileSizeLow;
            dirDetails.totalSize += findFileData.nFileSizeLow;
            dirDetails.fileCount++;
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
    FindClose(hFind);
    *directories = realloc(*directories, (*dirCount + 1) * sizeof(DirDetails));
    (*directories)[*dirCount] = dirDetails;
    (*dirCount)++;
}
void sort_directories(DirDetails *directories, int dirCount) {
    qsort(directories, dirCount, sizeof(DirDetails), compare_dirs);
}
void sort_files(FileEntry *files, int fileCount) {
    qsort(files, fileCount, sizeof(FileEntry), compare_files);
}
int compare_dirs(const void *a, const void *b) {
    const DirDetails *dirA = (const DirDetails *)a;
    const DirDetails *dirB = (const DirDetails *)b;
    if (dirB->totalSize > dirA->totalSize) return 1;
    if (dirB->totalSize < dirA->totalSize) return -1;
    return 0;
}
int compare_files(const void *a, const void *b) {
    const FileEntry *fileA = (const FileEntry *)a;
    const FileEntry *fileB = (const FileEntry *)b;
    return (fileB->size - fileA->size);
}
unsigned long long calculate_total_size(DirDetails *directories, int dirCount) {
    unsigned long long grandTotal = 0;
    for (int i = 0; i < dirCount; i++) {
        grandTotal += directories[i].totalSize;
    }
    return grandTotal;
}
unsigned long long calculate_subdir_size(DirDetails *directories, int dirCount, const char *dirPath) {
    unsigned long long subDirTotal = 0;
    for (int i = 0; i < dirCount; i++) {
        if (strstr(directories[i].dirPath, dirPath) == directories[i].dirPath) {
            subDirTotal += directories[i].totalSize;
        }
    }
    return subDirTotal;
}
void print_hierarchy(const DirDetails *directories, int dirCount, const char *topDir, const char *currentDir, int depth) {
    for (int i = 0; i < dirCount; i++) {
        if (strcmp(directories[i].dirPath, currentDir) == 0 || strstr(directories[i].dirPath, currentDir) == directories[i].dirPath) {
            const char *relativePath = directories[i].dirPath + strlen(currentDir);
            if (relativePath[0] == '\\' && strchr(relativePath + 1, '\\') == NULL) {
                    if(flags.FLAG_PRINT_FORMATTED)
                    for (int d = -1; d < depth; d++) {
                        printf("   ");
                    }
                    const char *dirName = flags.FLAG_PRINT_ABSOLUTE_PATH 
                                      ? directories[i].dirPath 
                                      : strrchr(directories[i].dirPath, '\\') + 1;
                    if(flags.FLAG_PRINT_FILE_SIZE_IN_LIST || !flags.FLAG_SORT)
                        printf("%s%s%s\n",
                        flags.FLAG_PRINT_COLOR ? COLOR_BLUE : "",
                        dirName,
                        flags.FLAG_PRINT_COLOR ? COLOR_RESET : "");
                    else
                     printf("%s%s%s %.2f%%\n",
                        flags.FLAG_PRINT_COLOR ? COLOR_BLUE : "",
                        dirName,
                        flags.FLAG_PRINT_COLOR ? COLOR_RESET : "", 
                        directories[i].totalPercentage);
                for (int j = 0; j < directories[i].fileCount; j++) {
                    if(flags.FLAG_PRINT_FORMATTED)
                    for (int d = -1; d < depth + 1; d++) {
                        printf("   ");
                    }
                    const char *fileName = flags.FLAG_PRINT_ABSOLUTE_PATH 
                                           ? directories[i].files[j].filePath 
                                           : strrchr(directories[i].files[j].filePath, '\\') + 1;

                if (flags.FLAG_PRINT_FILE_SIZE_IN_LIST) {
                    printf("%s%s%s\n",
                        flags.FLAG_PRINT_COLOR ? COLOR_GREEN : "",
                        fileName,
                        flags.FLAG_PRINT_COLOR ? COLOR_RESET : "");
                } else {
                    if(flags.FLAG_SORT) {
                         printf("%s%s%s %lu bytes %.2f%% %.2f%%\n",
                        flags.FLAG_PRINT_COLOR ? COLOR_GREEN : "",
                        fileName,
                        flags.FLAG_PRINT_COLOR ? COLOR_RESET : "",
                        directories[i].files[j].size,
                        directories[i].files[j].totalPercentage,
                        directories[i].files[j].subDirPercentage);
                    } else {
                         printf("%s%s%s %lu bytes\n",
                        flags.FLAG_PRINT_COLOR ? COLOR_GREEN : "",
                        fileName,
                        flags.FLAG_PRINT_COLOR ? COLOR_RESET : "",
                        directories[i].files[j].size);
                    }
                }
                }
                print_hierarchy(directories, dirCount, topDir, directories[i].dirPath, depth + 1);
            }
        }
    }
}
