#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
// #include<stdbool.h>
#include<sys/stat.h>
#include<ctype.h>

typedef enum { false, true } bool;

bool count_empty_lanes = true;
bool count_comments = true;
// bool in_comment = false;

static inline bool
is_empty_lane(char* lane)
{
    while(*lane)
    {
        if (!isspace((unsigned char) *lane)) return false;
        lane++;
    }
    return true;
}

static inline bool
is_comment(char* lane)
{
    char* str = lane;
    while(*str)
    {
        if(*str == '/' && *(str + 1) == '/')
        {
            return true;
        }
        str++;
    }
    return false;
}


static inline int
count_lanes(char* dir, char* file_path)
{
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", dir, file_path);
    FILE* file = fopen(path, "r");
    if(file == NULL)
    {
        printf("Error file %s cannot be opened\n", file_path);
        return 0;
    }
    int lanes = 0;
    char buffer[2048];

    while(1)
    {
        char* lane = fgets(buffer, sizeof(buffer), file);
        if(lane == NULL) break;

        if(!count_empty_lanes && count_comments)
        {
            if(!is_empty_lane(lane)) lanes++;
        }
        else if(!count_comments && count_empty_lanes)
        {
            if(!is_comment(lane)) lanes++;
        }
        else if(!count_comments && !count_empty_lanes)
        {
            if(!is_comment(lane) && !is_empty_lane(lane)) lanes++;
        }
        else lanes++;
    }
    fclose(file);
    return lanes;
}

static inline int
process_directory(char* path, char** extensions, int num_extensions, int* total_lanes)
{
    DIR* dir = opendir(path);
    if (dir == NULL) {
        printf("Error opening the directory %s\n", path);
        return 1;
    }

    struct dirent* entry;
    struct stat statbuf;
    char full_path[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        if (stat(full_path, &statbuf) == -1) {
            printf("Error getting file status\n");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            process_directory(full_path, extensions, num_extensions, total_lanes);
        } else {
            for (int i = 0; i < num_extensions; i++) {
                char extension[10] = ".";
                strcat(extension, extensions[i]);

                int len = strlen(entry->d_name);
                int ext_len = strlen(extension);

                if (len > ext_len && strcmp(entry->d_name + len - ext_len, extension) == 0) {
                    int lanes_file = count_lanes(path, entry->d_name);
                    printf("File: %s - Lanes: %d\n", full_path, lanes_file);
                    *total_lanes += lanes_file;
                    break;
                }
            }
        }
    }
    closedir(dir);
    return 0;
}

int
main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: %s <directory> <extension1> [extension2] ...\n", argv[0]);
        return 1;
    }

    char* path = argv[1];
    int total_lanes = 0;
    char** extensions;
    int num_flags = 0;

    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-e") == 0){
            num_flags++;
            count_empty_lanes = false;
        }
        if(strcmp(argv[i], "-c") == 0){
            num_flags++;
            count_comments = false;
        }
    }

    extensions = &argv[2 + num_flags];
    int num_extensions = argc - 2 + num_flags;

    process_directory(path, extensions, num_extensions, &total_lanes);
    printf("Total lanes: %d in dir %s and subdirectories\n", total_lanes, path);
    return 0;
}