#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int create_file(char *path, char *name, int Type)
{
    char full_path[256];

    snprintf(full_path, sizeof(full_path), "%s%s", path, name);

    struct stat st;
    if (stat(full_path, &st) == 0)
    {
        printf("Error: %s already exists.\n", Type ? "Directory" : "File");
        return 1;
    }

    // if (!Type && mkdir(path, 0777) != 0)
    // {
    //     perror("Error creating directory");
    //     return 1;
    // }

    // if (chdir(path) != 0)
    // {
    //     perror("Error changing directory");
    //     return 1;
    // }
    printf("full_path %s\n",full_path);
    if (Type)
    {
        if (mkdir(full_path, 0777) != 0)
        {
            perror("Error creating directory");
            return 1;
        }
    }
    else
    {
        FILE *file = fopen(full_path, "w");
        if (file == NULL)
        {
            perror("Error creating file");
            return 1;
        }

        fclose(file);
    }

    return 0;
}

int delete_file(char *path)
{
    char full_path[256]; 
    strcpy(full_path,path);
    // snprintf(full_path, sizeof(full_path), "%s%s", patH);

    if (access(full_path, F_OK) == -1)
    {
        printf("Error: %s does not exist.\n", full_path);
        return 1;
    }

    // if (chdir(path) != 0)
    // {
    //     perror("Error changing directory");
    //     return 1;
    // }

    struct stat st;
    printf("full_path  :%s",full_path);
    if (stat(full_path, &st) == 0)
    {
        if (S_ISDIR(st.st_mode))
        {
            if (rmdir(full_path) != 0)
            {
                perror("Error deleting directory");
                return 1;
            }
        }
        else
        {
            if (remove(full_path) != 0)
            {
                perror("Error deleting file");
                return 1;
            }
        }
    }

    // Success
    return 0;
}



