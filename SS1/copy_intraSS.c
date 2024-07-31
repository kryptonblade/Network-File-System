// #include "headers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

int copy_file(const char *source_path, const char *destination_path)
{
    FILE *source_file = fopen(source_path, "r");
    FILE *dest_file = fopen(destination_path, "w");

    if (!source_file || !dest_file)
    {
        perror("Error opening files for copying");
        return 1;
    }

    char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0)
    {
        // Using fprintf to write binary data to the destination file
        fprintf(dest_file, "%.*s", (int)bytesRead, buffer);
    }

    fclose(source_file);
    fclose(dest_file);
    return 0;
}

int copy_directory(const char *source_path, const char *destination_path)
{
    DIR *source_dir = opendir(source_path);
    if (!source_dir)
    {
        perror("Error opening source directory");
        return 1;
    }

    // Create the destination directory
    if (mkdir(destination_path, 0777) != 0)
    {
        perror("Error creating destination directory");
        closedir(source_dir);
        return 1;
    }

    struct dirent *entry;

    while ((entry = readdir(source_dir)) != NULL)
    {
        if (entry->d_type == DT_REG) // Check if it's a regular file
        {
            char source_file[1024], dest_file[1024];
            sprintf(source_file, "%s/%s", source_path, entry->d_name);
            sprintf(dest_file, "%s/%s", destination_path, entry->d_name);
            int kk = copy_file(source_file, dest_file);
            if (kk == 1)
            {
                return 1;
            }
        }
        else if (entry->d_type == DT_DIR) // Check if it's a subdirectory
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            {
                char source_subdir[1024], dest_subdir[1024];
                sprintf(source_subdir, "%s/%s", source_path, entry->d_name);
                sprintf(dest_subdir, "%s/%s", destination_path, entry->d_name);
                int kk = copy_directory(source_subdir, dest_subdir);
                if (kk == 1)
                {
                    return 1;
                }
            }
        }
    }

    closedir(source_dir);
    return 0;
}

// int main()
// {
//     const char *source_path = "/home/krishna/1.txt";
//     const char *destination_path = "/home/krishna/2.txt";

//     copy_file(source_path, destination_path);

//     return 0;
// }