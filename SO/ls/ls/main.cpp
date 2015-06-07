#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>

//S_IRUSR    00400     owner has read permission
//S_IWUSR    00200     owner has write permission
//S_IXUSR    00100     owner has execute permission
//S_IRWXG    00070     mask for group permissions
//S_IRGRP    00040     group has read permission
//S_IWGRP    00020     group has write permission
//S_IXGRP    00010     group has execute permission
//S_IRWXO    00007     mask for permissions for others (not in group)
//S_IROTH    00004     others have read permission
//S_IWOTH    00002     others have write permission
//S_IXOTH    00001     others have execute permission


void ls_dir(const char *directory_path) {
    DIR *dirp = opendir(directory_path);
    if (dirp != NULL) {
        printf("%s\n", directory_path);
        dirent *dp = NULL;
        while ((dp = readdir(dirp)) != NULL) {
            if(dp->d_name[0] != '.') { // ., .., .*
                char file_path[PATH_MAX] = {0};
                sprintf(file_path, "%s/%s", directory_path, dp->d_name);
                struct stat st;
                stat(file_path, &st);
                char d = '-';
                if(st.st_mode & S_IFDIR)
                    d = 'd';
                if(st.st_mode & S_IFLNK)
                    d = 'l';
                printf("%c%c%c%c%c%c%c%c%c%c ",
                       ((st.st_mode & S_IFLNK) ? 'l' : ((st.st_mode & S_IFDIR) ? 'd' : '-')),
                       ((st.st_mode & S_IRUSR) ? 'r' : '-'),
                       ((st.st_mode & S_IWUSR) ? 'w' : '-'),
                       ((st.st_mode & S_IXUSR) ? 'x' : '-'),
                       ((st.st_mode & S_IRGRP) ? 'r' : '-'),
                       ((st.st_mode & S_IWGRP) ? 'w' : '-'),
                       ((st.st_mode & S_IXGRP) ? 'x' : '-'),
                       ((st.st_mode & S_IROTH) ? 'r' : '-'),
                       ((st.st_mode & S_IWOTH) ? 'w' : '-'),
                       ((st.st_mode & S_IXOTH) ? 'x' : '-')
                );
                printf("%3d ", st.st_nlink);
                passwd *pw = getpwuid(st.st_uid);
                group *gr = getgrgid(st.st_gid);
                printf("%4s %5s ", pw ? pw->pw_name : "", gr ? gr->gr_name : "");
                printf("%7lld ", st.st_size);
                char time_buf[128] = {0};
                strftime(time_buf, sizeof time_buf, "%c", localtime(&(st.st_ctime)));
                printf("%s ", time_buf);
                printf("%s", file_path);
                printf("\n");
                

            }
        }
        printf("\n");
        (void)closedir(dirp);
        dirp = opendir(directory_path);
        if(dirp != NULL) {
            while ((dp = readdir(dirp)) != NULL) {
                if(dp->d_name[0] != '.') { // ., .., .*
                    char file_path[PATH_MAX] = {0};
                    sprintf(file_path, "%s/%s", directory_path, dp->d_name);
                    struct stat st;
                    stat(file_path, &st);
                    if(st.st_mode & S_IFDIR) {
                        ls_dir(file_path);
                    }
                }
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    if(argc == 1) {
        ls_dir(".");
    } else if(argc == 2) {
        const char *path = argv[1];
        ls_dir(path);
    } else {
        
    }
    return 0;
}
