# SoalShiftSISOP20_modul4_D07

1. Maisie Chiara Salsabila - 05111840000057
2. David Bintang - 05111840000090

## SOAL
Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidak amanan dan ketidak efisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil. Sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut. Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)

Berikut adalah detail filesystem rancangan jasir:
### 1. Enkripsi versi 1:
- Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
- Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
- Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
- Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
- Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.
``` 9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO ```
Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10
“encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg
Note : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.
- Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

### 2. Enkripsi versi 2:
- Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
- Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v2.
- Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekrip.
- Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
- Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil yakni: File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004.
- Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada didalam direktori tersebut (rekursif).

### 3. Sinkronisasi direktori otomatis:

Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:
- Kedua directory memiliki parent directory yang sama.
- Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi:
  - Nama dari setiap berkas di dalamnya sama.
  - Modified time dari setiap berkas di dalamnya tidak berselisih lebih dari 0.1 detik.
- Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.
- Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory.
Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.
Implementasi dilarang menggunakan symbolic links dan thread.

### 4. Log system:

- Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
- Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
- Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
- Sisanya, akan dicatat dengan level INFO.
- Format untuk logging yaitu:
```[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]```

LEVEL    : Level logging
yy        : Tahun dua digit
mm         : Bulan dua digit
dd         : Hari dua digit
HH         : Jam dua digit
MM         : Menit dua digit
SS         : Detik dua digit
CMD          : System call yang terpanggil
DESC      : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)

Contoh format logging nantinya seperti:
```
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
```


## JAWABAN

### 1. ENCRIPSI V1
#### FUSE OPERATIONS
```
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
```
xmp berfungsi untuk mengenkripsi dan mendekripsi file

#### ENKRIPSI
fungsi encrypt akan dipanggil di ```readdir``` dan ```rename```
```
void encrypt_1 (char* str)
{
	if(!strcmp(str,".") || !strcmp(str,"..")) return;
	int panjang = strlen(str);
	for(int i=0; i<panjang; i++)
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
```
Program di atas adalah untuk mendapatkan indeks file tanpa slash yang akan dienkripsi atau di dekripsi.

```
int encrypt(char *src){
    DIR *dp;
    struct dirent *d;
    char name[100];
    char path[1000], path1[1000]; 
    char path2[3000];

    dp = opendir(src);
    if(dp == NULL){
        return -errno;
    }

    while ((d=readdir(dp)) != NULL){
        struct stat stat;
        memset(&stat, 0, sizeof(stat));

        strcpy(name, d->d_name);
        sprintf(path, "%s/%s", src, name);

        if(strcmp(name, ".") && strcmp(name, "..") && d->d_type == DT_DIR){
            char folder[1000];

            strcpy(folder, name);
            encrypt_1(folder);

            strcpy(path1, path);
            sprintf(path2, "%s/%s", path1, folder);

            int res = rename(path1, path2);
            if(res!=0){
                return -errno;
            }
        }
        if(d->d_type == DT_REG){
            char *ext;

            ext = strrchr(name, '.');

            if(ext == NULL){
                encrypt_1(name);
                sprintf(path2, "%s/%s", path, name);
            } else {
                int z = strlen(ext);
                size_t n = sizeof(name)/sizeof(name[0]);
                
                int noext = z-n+1;
                char noname[1000];

                snprintf(noname, noext, "%s", name);
                encrypt_1(noname);

                strcat(noname, ext);
                sprintf(path2, "%s/%s", path, noname);
            }

            int res = rename(path, path2);
            if(res!=0){
                return -errno;
            }
        }
    }

    closedir(dp);
    return 0;

}
```

#### DEKRIPSI
fungsi dekripsi akan dipanggil di ```getattr```, ```readdir```, ```read```, ```mkdir```, ```open```, dll.
```
void decrypt_1 (char* str)
{
	if(!strcmp(str,".") || !strcmp(str,"..")) 
        return;
	int panjang = strlen(str);
	for(int i=0; i<panjang; i++)
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

```


### 2. ENCRIPSI V2


### 3. SINKRONISASI DIREKTORI OTOMATIS


### 4. LOG SYSTEM
```
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
```
