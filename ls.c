#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
long long int findInode(const char path[]){
    int fd, inode;
    fd = open(path, 'r');

    struct stat file_stat;
    int ret;
    ret = fstat (fd, &file_stat);

    inode = file_stat.st_ino;
    return inode;
}
int isFile(const char* name)
{
    DIR* directory = opendir(name);

    if(directory != NULL)
    {
        closedir(directory);
        return 0;
    }

    if(errno == ENOTDIR)
    {
        return 1;
    }

    return -1;
}
char f_type(mode_t mode)
{
    char c;

    switch (mode & S_IFMT)
    {
        case S_IFBLK:
            c = 'b';
            break;
        case S_IFCHR:
            c = 'c';
            break;
        case S_IFDIR:
            c = 'd';
            break;
        case S_IFIFO:
            c = 'p';
            break;
        case S_IFLNK:
            c = 'l';
            break;
        case S_IFREG:
            c = '-';
            break;
        case S_IFSOCK:
            c = 's';
            break;
        default:
            c = '?';
            break;
    }
    return (c);
}



static char *lsperms(int mode)
{
    static const char *rwx[] = {"---", "--x", "-w-", "-wx",
                                "r--", "r-x", "rw-", "rwx"};
    static char bits[11];

    bits[0] = f_type(mode);
    strcpy(&bits[1], rwx[(mode >> 6)& 7]);
    strcpy(&bits[4], rwx[(mode >> 3)& 7]);
    strcpy(&bits[7], rwx[(mode & 7)]);
    if (mode & S_ISUID)
        bits[3] = (mode & S_IXUSR) ? 's' : 'S';
    if (mode & S_ISGID)
        bits[6] = (mode & S_IXGRP) ? 's' : 'l';
    if (mode & S_ISVTX)
        bits[9] = (mode & S_IXOTH) ? 't' : 'T';
    bits[10] = '\0';
    return(bits);
}


void printFilePermissions(char fileName[256]){
    struct stat fileStat;
    printf("%s\t",lsperms(fileStat.st_mode));
}

void ls(const char * dir,int op_a,int op_l, int op_i, int op_R, int op_d)
{
    struct dirent *d;
    DIR *dh = opendir(dir);
    if (!dh)
    {
        if (errno == ENOENT)
        {
            perror("Directory doesn't exist");
        }
        else{
            perror("Unable to read directory");
        }
        exit(EXIT_FAILURE);
    }
    while ((d = readdir(dh)) != NULL)
    {
        char* path = (char*) malloc(200);
        sprintf(path, "%s/%s", dir, d->d_name);
        if(d->d_name[0] == '.' && d->d_name[1] == '\0') continue;
        if(!op_d || isFile(path) == 0){
            if (!op_a && d->d_name[0] == '.' && !op_R)
                continue;
            if(op_l) printFilePermissions(d->d_name);
            if(op_i) printf("%lld\t", findInode(d->d_name));
            if(op_R && isFile(path) == 0 ) {
                printf("%s:  \n", path);
                ls(path, op_a, op_l, op_i, op_R, op_d);
            }
            else{
                printf("%s  ", d->d_name);
            }
            printf("\n");
        }

    }
    printf("\n");
}
int main(int argc, const char *argv[])
{
    if (argc == 1)
    {
        ls(".",0,0, 0, 0, 0);
    }
    else if (argc == 2)
    {
        if (argv[1][0] == '-')
        {
            int op_a = 0, op_l = 0, op_i = 0, op_R = 0, op_d = 0;
            char *p = (char*)(argv[1] + 1);
            while(*p){
                if(*p == 'a') op_a = 1;
                else if(*p == 'l') op_l = 1;
                else if(*p == 'i') op_i = 1;
                else if(*p == 'R') op_R = 1;
                else if(*p == 'd'){
                    op_d = 1;
                }
                else{
                    perror("Option not available");
                    exit(EXIT_FAILURE);
                }
                p++;
            }
            ls(".",op_a,op_l, op_i, op_R, op_d);
        }
    }
    return 0;
}
