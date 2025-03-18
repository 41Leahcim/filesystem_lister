#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

char entry_type(const char type){
    switch(type){
        case 4: return 'd';
        case 10: return 'l';
        case 8: return 'f';
        default: return 'u';
    }
}

int read_directory_recursively(const char *directory_path, const size_t directory_path_length){
    // Make sure the path is shorter than 2 times the maximum path length
    if(directory_path_length >= 256){
        return 0;
    }

    // Open the directory for iterating
    DIR *directory = opendir(directory_path);
    if(directory == NULL){
        perror("Failed to open directory");
        return -1;
    }

    // Iterate through the files
    struct dirent *entry;
    while((entry = readdir(directory)) != NULL){
        // Create the full path of the entry
        char entry_path[512] = { 0 };
        strncpy(entry_path, directory_path, directory_path_length);
        size_t file_path_index = directory_path_length;
        if(strcmp(entry_path, "/") != 0){
            entry_path[directory_path_length] = '/';
            file_path_index++;
        }
        strncpy(entry_path + file_path_index, entry->d_name, sizeof(entry_path) - file_path_index);

        // Retrieve the file's metadata
        struct stat metadata;
        if(stat(entry_path, &metadata) != 0){
            perror("Failed to read metadata");
            continue;
        }

        // Retrieve the entry type, read directories recursively, print file information
        const char type = entry_type(entry->d_type);
        if(type == 'd' && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){
            read_directory_recursively(entry_path, strlen(entry_path));
        }else{
            printf("\"%s\" (%c): %ld\n", entry_path, type, metadata.st_size);
        }
    }

    // Display an error message if the application failed to read the last entry
    if(errno != 0){
        perror("Failed to read entry");
        closedir(directory);
        return -1;
    }

    // Close the directory
    closedir(directory);
    return 0;
}

int main(){
    // Start measuring performance
    clock_t start = clock();

    // Read the directory recursively
    read_directory_recursively("/", 1);

    // Display how long it took to display all entries
    printf("%lf\n", (double)(clock() - start) / CLOCKS_PER_SEC);
}
