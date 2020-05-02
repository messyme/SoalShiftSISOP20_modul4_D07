#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/xattr.h>
#include <sys/wait.h>
#include <pthread.h>

//No 1
static  const  char * dirpath = "/home/maisie/Documents";
char cipher[100]="9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

int ext (char *path){
    for(int i=strlen(path)-1; i>=0; i--){
        if(path[i] == '.') return i;
    }
    return strlen(path);
}

int slash (char *path, int akhir){
    for(int i = 0; i < strlen(path); i++){
        if(path[i] == '/') return i + 1;
    }
    return akhir;
}

void encrypt_1 (char* str)
{
	if(!strcmp(str,".") || !strcmp(str,"..")) return;
	int awal = slash (nama,0);
    int akhir = ext (nama);
	for(int i=awal; i<akhir; i++)
	{
		for(int j=0; j<87; j++)
		{
            if (str[i] == '/') 
                continue;
			if(str[i] == cipher[j])
			{
				str[i] = cipher[(j+10)%87];
				break;
			}
		}
	}
}


void decrypt_1 (char* str)
{
	if(!strcmp(str,".") || !strcmp(str,"..")) 
        return;
	int awal = slash (nama,0);
    int akhir = ext (nama);
	for(int i=awal; i<akhir; i++)
	{
		for(int j=0; j<87; j++)
		{
            if (str[i] == '/')
                continue;
			if(str[i]==cipher[j])
			{
				str[i] = cipher[(j+77)%87];
				break;
			}
		}
	}
}


//NO 4
void logInfo (char * str)
{
	FILE * logFile;
    logfile = fopen("/home/maisie/fs.log", "a");
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime (&rawtime);
    
    int year, month, day, hour, minute, second;
    year = timeinfo->tm_year+1900;
    month = timeinfo->tm_mon+1;
    day = timeinfo->tm_mday;
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;
    second = timeinfo->tm_sec;

	fprintf(logFile, "INFO::%d%d%d-%d:%d:%d::%s\n", year, month, day, hour, minute, second, str);
	fclose(logFile);
}

void logWarning (char * str){
    FILE * logFile = fopen("/home/maisie/fs.log", "a");
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime (&rawtime);

    int year, month, day, hour, minute, second;
    year = timeinfo->tm_year+1900;
    month = timeinfo->tm_mon+1;
    day = timeinfo->tm_mday;
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;
    second = timeinfo->tm_sec;

    fprintf(logFile, "WARNING::%d%d%d-%d:%d:%d::%s\n", year, month, day, hour, minute, second, str);
    fclose(logFile);
}


//FUSE
static int xmp_getattr(const char *path, struct stat *stbuf){
    //get file attributes
    logInfo ("GETATTR");
    int res;
	char fpath[1000], new_name[1000];
    
    sprintf (new_name, "%s", path);
    decrypt_1 (new_name);
	sprintf (fpath,"%s%s", dirpath, new_name);
	res = lstat (fpath, stbuf);
	if (res == -1) 
        return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){
    // read directory
    logInfo ("READDIR");
    int res;
    DIR *dp;
    struct dirent *de;
 
    (void) offset;
    (void) fi;
    char fpath[1000];
    char name[1000];
    if (strcmp(path, "/") == 0){
        sprintf(fpath, "%s", dirpath);
    }
    else {
        sprintf(name,"%s",path);
        decrypt_1 (name);
        sprintf(fpath, "%s%s",dirpath,name);
    }
    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
 
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        char fullpathname[1000];
        sprintf(fullpathname, "%s/%s", fpath, de->d_name);
        char temp[1000];
        strcpy (temp,de->d_name);
        encrypt_1 (temp);
    res = (filler(buf, temp, &st, 0));
    if (res!=0) 
        break;
    }
 
    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    //read data from an open file
    char fpath[1000];
    char name[1000];
    if(strcmp(path,"/") == 0){
        path = dirpath;
        sprintf(fpath,"%s", path);
    }
    else {
        sprintf(name,"%s",path);
        decrypt_1 (name);
        sprintf(fpath, "%s%s", dirpath, path);
    }
    int fd = 0;
    int res = 0;
    (void) fi;
    fd = open(fpath, O_RDONLY);
    if (fd == -1)
        return -errno;    
    
    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;
    
    close(fd);
    logInfo ("READ");
    return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){
    //create a file node  
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s", dirpath, path);
    res = mknod(fpath, mode, rdev);
    if(res == -1){
        return -errno;
    }
    logInfo ("CREATE");
    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){
	//create a directory
    logInfo ("MKDIR");
    int res;
	char fpath[1000];
    char spath[1000];
	sprintf(spath,"%s",path);
	decrypt_1(spath);
	
	sprintf(fpath, "%s%s", dirpath, spath);
	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
    //write data to an open file
    char fpath[1000], name[1000];
    int fd, res;

    sprintf (name,"%s",path);

    if (strcmp(path,"/") == 0) {
        path = dirpath;
        sprintf (fpath,"%s", path);
    }
    else {
        sprintf (fpath, "%s%s", dirpath, name);
    }

    (void) fi;
    fd = open (fpath, O_WRONLY);
    res = pwrite (fd, buf, size, offset);

    if (fd == -1) {
        return -errno;
    }
    
    if (res == -1) {
        res = -errno;
    }

    logInfo ("WRITE");
    close(fd);
    return res;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){
	//file open operation
    int res;
    char fpath[1000];
    char name[1000];
	sprintf(name,"%s",path);
    decrypt_1 (name);
	sprintf(fpath, "%s%s",dirpath,name);
	res = open(fpath, fi->flags);
    logInfo ("OPEN");
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_truncate(const char *path, off_t size){
	//change the size of a file
    int res;
    char fpath[1000];
    char name[1000];
	sprintf(name,"%s",path);
    decrypt_1(name);
	sprintf(fpath, "%s%s",dirpath,name);
	res = truncate(fpath, size);
	logInfo ("TRUNCATE");
    if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path){
    //remove a file
    int res;
    char fpath[1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = unlink(fpath);
    logWarning ("UNLINK");
    if(res == -1){
        return -errno;
    }

    return 0;
}

static int xmp_rmdir(const char *path){
    //remove a directory
    logWarning ("RMDIR");
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = rmdir(fpath);
    if(res==-1){
        return -errno;
    }
    return 0;
}

static int xmp_rename(const char *from, const char *to){
	//rename a file
    logInfo ("RENAME");
    int res;
    char fpath[1000], fpath1[1000], name[1000];

    sprintf(fpath, "%s%s", dirpath, from);
    sprintf(fpath1, "%s%s", dirpath, to);

    if(strncmp(to, enc1, 7) == 0){
        sprintf(name, "%s", to);
        encrypt_1(fpath);
    }

	res = rename(fpath, fpath1);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask){
	//systemcell
    int res;

	char fpath[1000];
    char name[1000];
	sprintf(name,"%s",path);
    decrypt_1(name);
	sprintf(fpath, "%s%s",dirpath,name);

	res = access(fpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}


static int xmp_utimens(const char *path, const struct timespec ts[2]){
	//update last access time
    int res;
	struct timeval tv[2];
	char fpath[1000];
    char spath[1000];
	sprintf(spath,"%s",path);
	decrypt_1(spath);
	sprintf(fpath,"%s%s",dirpath,spath);

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
    logInfo ("UTIMENS");
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi){
    (void) fi;

    int res;
    char fpath[1000];
	char spath[1000];
	sprintf(spath,"%s",path);

	decrypt_1(spath);
    
	sprintf(fpath,"%s%s",dirpath,spath);

    res = creat(fpath, mode);
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}

static struct fuse_operations xmp_oper = 
{
    .getattr  = xmp_getattr,
    .readdir  = xmp_readdir,
    .read     = xmp_read,
    .mknod    = xmp_mknod,
    .mkdir    = xmp_mkdir,
    .write    = xmp_write,
    .open     = xmp_open,
    .truncate = xmp_truncate,
    .unlink   = xmp_unlink,
    .rmdir    = xmp_rmdir,
    .rename   = xmp_rename,
    .access = xmp_access,
	.utimens = xmp_utimens,
    .create = xmp_create,
};


//MAIN
int main(int argc, char *argv[]) {
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}