# Modul 4

## soal_1
Pada soal ini, program diminta untuk mengubah file txt hexadecimal menjadi sebuah gambar.
### a. Hex to Byte Converter
Fungsi ini digunakan untuk mengubah string hexadecimal menjadi format biner (byte). Untuk kodenya seperti ini
```
unsigned char hex_byte_converter(const char *hex) {
    unsigned char byte;
    sscanf(hex, "%2hhx", &byte);
    return byte;
}
```
Dimana fungsi akan mengconvert dengan kode 
```
sscanf(hex, "%2hhx", &byte);
```
lalu mengembalikan nilai berupa kode biner.

### b. Fungsi convert to Image
Fungsi ini digunakan untuk mengubah text menjadi gambar. Untuk kodenya seperti ini.
```
void convert_to_image(const char *filename) {
    printf(">> Memproses file: %s\n", filename);

    FILE *input = fopen(filename, "r");
    if (!input) {
        perror("Gagal membuka file input");
        return;
    }

    fseek(input, 0, SEEK_END);
    long length = ftell(input);
    rewind(input);

    if (length <= 0) {
        printf("!! File kosong: %s\n", filename);
        fclose(input);
        return;
    }

    char *hex_data = malloc(length + 1);
    fread(hex_data, 1, length, input);
    hex_data[length] = '\0';
    fclose(input);

    // Hapus semua newline dan carriage return
    char *clean_hex = malloc(length + 1);
    int j = 0;
    for (int i = 0; i < length; i++) {
        if (hex_data[i] != '\n' && hex_data[i] != '\r') {
            clean_hex[j++] = hex_data[i];
        }
    }
    clean_hex[j] = '\0';
    free(hex_data);

    size_t hex_len = strlen(clean_hex);
    if (hex_len % 2 != 0) {
        printf("!! Hex length ganjil, tidak valid: %s\n", filename);
        free(clean_hex);
        return;
    }

    size_t bin_len = hex_len / 2;
    unsigned char *bin_data = malloc(bin_len);
    if (!bin_data) {
        free(clean_hex);
        return;
    }

    for (size_t i = 0; i < bin_len; i++) {
        bin_data[i] = hex_byte_converter(&clean_hex[i * 2]);
    }
    free(clean_hex);

    // Buat folder image di dalam source_dir
    char image_dir[PATH_MAX];
    snprintf(image_dir, sizeof(image_dir), "%s/image", source_dir);
    struct stat st = {0};
    if (stat(image_dir, &st) == -1) {
        if (mkdir(image_dir, 0755) == -1) {
            perror("Gagal membuat folder image");
            free(bin_data);
            return;
        }
    }

    // Ambil nama file tanpa path dan ekstensi
    const char *base_name = strrchr(filename, '/');
    base_name = base_name ? base_name + 1 : filename;

    char name_part[128];
    sscanf(base_name, "%[^.]", name_part);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    char output_filename[PATH_MAX];
    snprintf(output_filename, sizeof(output_filename),
        "%s/%s_image_%04d-%02d-%02d_%02d-%02d-%02d.png",
        image_dir, name_part,
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec);

    FILE *output = fopen(output_filename, "wb");
    if (output) {
        fwrite(bin_data, 1, bin_len, output);
        fclose(output);

    char log_path[PATH_MAX];
    snprintf(log_path, sizeof(log_path), "%s/conversion.log", source_dir);
    FILE *log = fopen(log_path, "a");
        if (log) {
            fprintf(log, "[%04d-%02d-%02d][%02d:%02d:%02d]: Successfully converted hexadecimal text %s to %s.\n",
                tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                filename, output_filename);
            fclose(log);
        }

        printf("✓ Converted: %s -> %s\n", filename, output_filename);
    } else {
        printf("!! Gagal membuat file gambar: %s\n", output_filename);
    }

    free(bin_data);

}
```
Dimana untuk cara kerjanya sebagai berikut.
1. Fungsi akan mengambil input berupa `*char filename`.
2. Setelah itu, fungsi akan membuka filename terebut dalam mode read.
3. Jika file tidak ada, maka muncul pesan `Gagal membuka file input`.
4. Setelah itu, file menentukan ukuran (panjang) dari file yang sedang dibuka.
5. Jika panjangnya <= 0, makka program akan muncul pesan `!! File kosong: %s`.
6. Program memuat seluruh konten file heksadesimal ke dalam memori untuk pemrosesan lebih lanjut, dan kemudian menutup file tersebut.
7. Program membersihkan data heksadesimal yang telah dibaca dari file (`hex_data`) dengan menghapus semua karakter newline (`\n`) dan carriage return (`\r`).
8. Setelah itu, memori yang berisi hex_data dibersihkan untuk emnghindari terjadinya memory leak.
9. Program memvalidasi panjang string heksadesimal.
10. Jika panjangnya ganjil, menandakan format tidak valid (karena 2 karakter hex = 1 byte biner), sehingga program akan muncul pesan `!! Hex length ganjil, tidak valid: %s`.
11. Fungsi menghitung ukutan data biner yang akan dihasilkan (setengah dari panjang hex) dan mengalokasikan memori yang diperlukan untuk menyimpannya.
12. menggunakan for loop, fugnsi ini menjalankan fungsi lain, yaitu fungsi `hex_byte_converter` pada poin a.
13. Lalu, fugnsi membuat folder image di dalam source_dir.
14. Jika gagal, maka muncul pesan error berupa `Gagal membuat folder image`.
15. Setelah itu, fungsi mengekstrak nama dasar file dari sebuah path lengkap.
16. Jika nama dasarnya tidak null, maka pointer `base_name` akan digeser saru posisi kedepan, melewati karakter `/` terakhir.
17. Jika nama dasarnya bernilai `NULL`, maka file tersebut sudah merupakan file dasar.
18. Kode ini mengekstraksi bagian nama file dari variabel base_name (yang sudah berisi nama file tanpa path direktori) hingga karakter titik (.) pertama ditemukan. Hasilnya disimpan di name_part.
19. Selanjutnya, fungsi mendapatkan waktu dan tanggal sistem saat ini.
20. Kemudian, fungsi membuat nama file lengkap untuk gambar output. Nama ini akan menggabungkan path direktori gambar `image_dir`), `name_part`, dan timestamp (tahun, bulan, hari, jam, menit, detik) ke dalam format `[direktori_image]/[nama_file]_image_[YYYY-MM-DD_HH-MM-SS].png`.
21. Fungsi ini mencoba membuka file output dengan nama yang telah dibuat dalam mode tulis biner ("wb").
22. Jika file output berhasil dibuka:
    - Data biner yang sudah dikonversi (bin_data) ditulis ke file gambar.
    - File gambar kemudian ditutup.
    -= Kode mencoba membuka file log bernama conversion.log (di dalam source_dir) dalam mode append ("a").
    - Jika file log berhasil dibuka, sebuah entri log yang berisi timestamp dan detail konversi (file input dan file output) ditulis ke dalamnya, lalu file log ditutup.
23. Jika file output gagal dibuka, pesan kesalahan akan dicetak ke konsol.
24. Terakhir, memori yang dialokasikan untuk `bin_data` dibebaskan untuk menghindari terjadinya memory leak.

### c. Fungsi `is_converted`
Fungsi ini berfungsi untuk memeriksa apakah file telah dikonversi sebelumnya berdasarkan pathnya. Untuk kodenya seperti ini.
```
static bool is_converted(const char *filepath) {
    for (int i = 0; i < converted_count; i++) {
        if (strcmp(converted_files[i], filepath) == 0)
            return true;
    }
    return false;
}
```
Untuk cara kerjanya yaitu mulanya fungsi mengiterasi daftar konversi, lalu membndingkan path dan pengembalian true jika ditemukan, false jika ditemukan.

### d. Fungsi `mark_converted`
Fungsi mark_converted berfungsi untuk menambahkan path sebuah file ke dalam daftar (cache) file yang telah berhasil dikonversi. Untuk kodenya seperti ini.
```
static void mark_converted(const char *filepath) {
    if (converted_count < MAX_CONVERTED_FILES) {
        strncpy(converted_files[converted_count], filepath, PATH_MAX);
        converted_files[converted_count][PATH_MAX - 1] = '\0'; // safety null terminator
        converted_count++;
    }
}
```
Untuk cara kerjanya sebagai berikut.
1. Fungsi memeriksa apakah jumlah file yang sudah tercatat (`converted_count`) belum mencapai batas maksimum yang diizinkan (`MAX_CONVERTED_FILES`).
2. Jika kapasitas belum penuh, path file yang diberikan (`filepath`) akan disalin ke lokasi kosong berikutnya dalam array `converted_files`.
3. Setelah berhasil disalin, penghitung `converted_count` akan ditingkatkan satu untuk mencatat bahwa ada satu file lagi yang sudah ditandai.
4. Jika kapasitas `MAX_CONVERTED_FILES` sudah tercapai, fungsi ini tidak melakukan apa pun dan tidak menambahkan path file baru.

### e. `fs_getattr()`
Fungsi ini digunakan untuk mendapatkan atribut dari sebuah file atau direktori, seperti tipe (file atau direktori), izin akses, ukuran, dan jumlah hard link. Untuk kodenya seperti ini
```
static int fs_getattr(const char *path, struct stat *st, struct fuse_file_info *fi) {
    (void) fi;
    char realpath[PATH_MAX];
    snprintf(realpath, sizeof(realpath), "%s%s", source_dir, path);

    printf("[getattr] path=%s, realpath=%s\n", path, realpath);

    if (strcmp(path, "/") == 0) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
        return 0;
    }

    struct stat temp_stat;
    if (stat(realpath, &temp_stat) == -1)
        return -ENOENT;

    if (S_ISREG(temp_stat.st_mode)) {
        st->st_mode = S_IFREG | 0444;
        st->st_nlink = 1;
        st->st_size = temp_stat.st_size;
        return 0;
    }

    if (S_ISDIR(temp_stat.st_mode)) {
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2;
        return 0;
    }

    return -ENOENT;
}
```
dimana untuk cara kerjanya sebagai berikut.
1. Fungsi ini menggabungkan source_dir (direktori sumber asli) dengan path (jalur yang diminta oleh sistem FUSE) untuk membentuk jalur file yang sebenarnya di sistem operasi.
2. Jika path yang diminta adalah / (direktori root FUSE), fungsi ini secara langsung mengatur atributnya sebagai direktori dengan izin `0755`.
3. Memeriksa Keberadaan File: Untuk path lainnya, fungsi ini mencoba mendapatkan atribut dari file atau direktori nyata menggunakan `stat()`. Jika tidak ditemukan, fungsi mengembalikan `-ENOENT`.
4. Jika `stat()` berhasil, fungsi memeriksa apakah itu file biasa. Jika ya, atributnya diatur sebagai file dengan izin hanya baca (`0444`) dan ukurannya disalin.
5. Jika itu direktori, atributnya diatur sebagai direktori dengan izin `0755`.
6. Jika atribut berhasil ditentukan, fungsi mengembalikan 0 (berhasil). Jika tipe entri tidak didukung atau ada masalah lain, fungsi mengembalikan `-ENOENT`.

### f. `x_readdir()`
Fungsi ini digunakan untuk membaca sebuah direktori dalam FUSE. Untuk kodenya seperti ini
```
static int x_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                     off_t offset, struct fuse_file_info *fi,
                     enum fuse_readdir_flags flags)
{
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    DIR *dp = opendir(source_dir);
    if (!dp)
        return -errno;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));

        if (de->d_type == DT_DIR)
            st.st_mode = S_IFDIR | 0755;
        else if (de->d_type == DT_REG)
            st.st_mode = S_IFREG | 0444;
        else
            st.st_mode = S_IFREG | 0444;

        filler(buf, de->d_name, &st, 0, 0);
    }

    closedir(dp);
    return 0;
}
```
dimana untuk cara kerjanya sebagai berikut.
1. Fungsi memastikan path yang diminta adalah direktori root (`/`). Jika bukan, fungsi akan mengembalikan `-ENOENT`.
2. Fungsi menambahkan entri `.` (direktori saat ini) dan `..` (direktori induk) ke buffer daftar direktori.
3. Fungsi membuka direktori source_dir (direktori fisik yang dipetakan oleh FUSE) untuk membaca isinya. Jika gagal, fungsi mengembalikan error.
4. Fungsi mengulang setiap entri di `source_dir`. Untuk setiap entri, fungsi menentukan apakah itu direktori atau file biasa, menetapkan izin yang sesuai (`0755` untuk direktori, `0444` untuk file), dan menambahkannya ke buffer daftar direktori yang akan ditampilkan ke pengguna.
5. Setelah semua entri dibaca, fungsi menutup direktori sumber.
6. Fungsi mengembalikan `0` jika operasi berhasil.

### g. `fs_open()`
Fungsi ini berfungsi tidak hanya "membuka" file dalam arti tradisional, tetapi juga mengintegrasikan logika konversi heksadesimal ke gambar. Untuk kodenya seperti ini
```
static int fs_open(const char *path, struct fuse_file_info *fi) {
    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s%s", source_dir, path);

    printf("[open] path=%s, realpath=%s\n", path, fullpath);

    int fd = open(fullpath, O_RDONLY);
    if (fd == -1) return -ENOENT;
    close(fd);

    if (!is_converted(fullpath)) {
        convert_to_image(fullpath);
        mark_converted(fullpath);
    }

    return 0;
}
```
dimana untuk cara kerjanya seperti ini.
1. Fungsi menggabungkan source_dir dan path FUSE untuk mendapatkan jalur file yang sebenarnya di sistem file fisik.
2. Fungsi mencoba membuka file fisik dalam mode hanya-baca.
3. Jika file tidak ada atau gagal dibuka, fungsi mengembalikan error berupa `-ENOENT`.
4. File segera ditutup kembali karena tujuan utama open FUSE ini bukan untuk menyimpan file descriptor.
5. Fungsi memeriksa apakah file sudah dikonversi menggunakan fungsi `is_converted()`.
6. ika belum dikonversi, fungsi memanggil fungsi `convert_to_image()` untuk mengubah konten heksadesimal file menjadi gambar. Setelah konversi, file tersebut ditandai sebagai sudah dikonversi menggunakan `mark_converted()`.
7. Fungsi mengembalikan `0` untuk menandakan bahwa operasi pembukaan file berhasil.

### `h. fs_read()`
Fungsi ini digunakan untuk membaca sebuah file dalam FUSE. Untuk kodenya seperti ini
```
static int fs_read(const char *path, char *buf, size_t size, off_t offset,
                   struct fuse_file_info *fi) {
    (void) fi;

    char fullpath[PATH_MAX];
    snprintf(fullpath, sizeof(fullpath), "%s%s", source_dir, path);

    printf("[read] path=%s, realpath=%s, size=%zu, offset=%ld\n", path, fullpath, size, offset);

    int fd = open(fullpath, O_RDONLY);
    if (fd == -1) return -errno;

    int res = pread(fd, buf, size, offset);
    if (res == -1) res = -errno;
    close(fd);
    return res;
}
```
dimana untuk cara kerjanya sebagai berikut.
1. Fungsi menggabungkan source_dir dan path FUSE untuk mendapatkan jalur file yang sebenarnya di sistem file fisik.
2. Fungsi membuka file fisik dalam mode hanya-baca. Jika gagal, fungsi mengembalikan error.
3. Fungsi membaca data dari file fisik dimulai dari offset tertentu dengan ukuran size yang diminta, lalu menyimpannya ke buffer yang disediakan (buf).
4. Setelah membaca, fungsi menutup file yang telah dibuka.
5. Fungsi mengembalikan jumlah byte yang berhasil dibaca, atau nilai error jika terjadi masalah selama pembacaan.

### `i. Fuse Operations`
Fungsi ini digunakan untuk menjalankan beberapa operasi yang diperlukan pada FUSE. Untuk kodenya seperti ini.
```
static struct fuse_operations fs_oper = {
    .getattr = fs_getattr,
    .readdir = x_readdir,
    .open = fs_open,
    .read = fs_read,
};
```

### `j. int main()`
Fungsi ini digunaka sebagai program utama. Untuk kodenya seperti ini.
```
int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return 1;
    }
    snprintf(source_dir, sizeof(source_dir), "%s/anomali", cwd);
    printf("Source directory set to: %s\n", source_dir);

    return fuse_main(argc, argv, &fs_oper, NULL);
}
```
Dimana cara kerjanya sebagai berikut.
1. Program mencoba mendapatkan path direktori kerja saat ini (cwd) tempat program dijalankan.
2. Jika gagal, program akan mencetak kesalahan dan keluar.
3. Program membuat jalur absolut ke source_dir dengan menggabungkan direktori kerja saat ini dan sub-direktori "anomali".
4. Program memulai FUSE filesystem, menyerahkan kontrol ke library FUSE dengan argumen command-line yang diterima (argc, argv) dan struktur operasi (fs_oper) yang mendefinisikan perilaku filesystem.

### Revisi
Revisi pada soal ini terletak pada download file `anomali.zip`. Solusinya adalah menambahkan kode remmove dan download. Untuk kodenya seperti ini
```
int remove_directory_recursive(const char *path) {
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = 0; // 0 for success

    if (!d) {
        // Direktori tidak ada atau tidak dapat dibuka.
        // Jika errno adalah ENOENT, berarti direktori memang tidak ada, jadi anggap berhasil "dihapus".
        if (errno == ENOENT) return 0;
        perror("opendir for recursive delete");
        return -1;
    }

    struct dirent *p;
    while ((p = readdir(d)) != NULL) {
        // Lewati entri "." dan ".."
        if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
            continue;
        }

        char *buf = NULL;
        // Alokasikan buffer untuk path lengkap (path_lama/nama_file_baru)
        size_t len = path_len + strlen(p->d_name) + 2; // +2 untuk '/' dan null terminator
        buf = malloc(len);
        if (!buf) {
            perror("malloc for path buffer");
            r = -1;
            break;
        }
        snprintf(buf, len, "%s/%s", path, p->d_name);

        struct stat st_entry;
        if (stat(buf, &st_entry) == -1) {
            perror("stat on entry for recursive delete");
            free(buf);
            r = -1;
            break;
        }

        if (S_ISDIR(st_entry.st_mode)) { // Jika entri adalah direktori
            if (remove_directory_recursive(buf) == -1) { // Panggil rekursif
                r = -1;
                free(buf);
                break;
            }
        } else { // Jika entri adalah file
            if (unlink(buf) == -1) { // Hapus file
                perror("unlink file during recursive delete");
                r = -1;
                free(buf);
                break;
            }
        }
        free(buf);
    }
    closedir(d);

    if (r == 0) { // Jika tidak ada kesalahan sejauh ini, coba hapus direktori itu sendiri
        if (rmdir(path) == -1) {
            perror("rmdir");
            r = -1;
        }
    }

    return r; // 0 jika berhasil, -1 jika ada kesalahan
}

// --- Fungsi Download Anomali ---
void download()
{
    // Cek apakah folder 'anomali' sudah ada dan merupakan direktori
    struct stat st_anomali_dir;
    int anomali_dir_exists = (stat("anomali", &st_anomali_dir) == 0 && S_ISDIR(st_anomali_dir.st_mode));

    // Cek apakah file 'anomali.zip' sudah ada dan merupakan file biasa
    struct stat st_anomali_zip;
    int anomali_zip_exists = (stat("anomali.zip", &st_anomali_zip) == 0 && S_ISREG(st_anomali_zip.st_mode));

    bool proceed_with_download = true; // Asumsi default untuk melanjutkan

    if (anomali_dir_exists || anomali_zip_exists) {
        char response[10];
        printf("File atau folder 'anomali' atau 'anomali.zip' sudah ada.\n");
        printf("Apakah Anda ingin menggantinya? (y/n): ");
        if (fgets(response, sizeof(response), stdin) == NULL) {
            fprintf(stderr, "Gagal membaca input. Batalkan pengunduhan.\n");
            proceed_with_download = false;
        } else {
            // Hapus karakter newline jika ada
            response[strcspn(response, "\n")] = 0;
            if (strcmp(response, "y") != 0 && strcmp(response, "Y") != 0) {
                printf("Penggantian dibatalkan. Pengunduhan dilewati.\n");
                proceed_with_download = false;
            }
        }
    }

    if (!proceed_with_download) {
        return; // Keluar dari fungsi download jika tidak ada konfirmasi
    }

    // Jika folder 'anomali' sudah ada, hapus secara rekursif
    if (anomali_dir_exists) {
        printf("Menghapus folder 'anomali'...\n");
        if (remove_directory_recursive("anomali") == -1) {
            fprintf(stderr, "Gagal menghapus folder 'anomali'. Batalkan pengunduhan.\n");
            return;
        }
        printf("Folder 'anomali' berhasil dihapus.\n");
    }

    if (!opendir("anomali.zip"))
    {
        pid_t pid_download = fork();
        if (pid_download == 0)
        {
            char *download = "mkdir anomali && wget 'https://drive.google.com/uc?export=download&id=1hi_GDdP51Kn2JJMw02WmCOxuc3qrXzh5' -O anomali.zip && sudo unzip anomali.zip && rm anomali.zip";
            char *argv_download[] = {"sh", "-c", download, NULL};
            execv("/bin/sh", argv_download);
        }
        else if (pid_download > 0) wait(NULL);
        else perror("fork");
    }
}
```
Dimana untuk cara kerjanya yaitu:
1. Fungsi mengecek apakah di direktori saat ini ada folder `anomali.zip`.
2. Jika ada, maka program muncul pesan agar folder tersebut ingin di-replace atau tidak.
3. jika ya, maka program langsung me-replace folder anomali beserta isinya dengan download file yang sudah ditentukan.

## soal_2
Pada soal ini, program dapat menyatukan beberapa file yang terpecah menjadi 14 bagian dengan format `.000`, `.001`, sampai `.013`. 

### A. `baymax_getattr()`
Pertama-tama, kita perlu mendapatkan atribut file berdasarkan path yuang diminta. Untuk kodenya seperti ini

```
static int baymax_getattr(const char *path, struct stat *stbuf) {
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
    } else {
        char *filename = basename(strdup(path + 1));
        char fragment_path[PATH_MAX];
        snprintf(fragment_path, sizeof(fragment_path), "%s/%s.000", options.relics_dir, filename);

        if (access(fragment_path, F_OK) == 0) {
            stbuf->st_mode = S_IFREG | 0777;
            stbuf->st_nlink = 1;

            size_t total_size = 0;
            int fragment_count = 0;
            char current_fragment_path[PATH_MAX];

            while (1) {
                snprintf(current_fragment_path, sizeof(current_fragment_path), "%s/%s.%03d", options.relics_dir, filename, fragment_count);
                if (access(current_fragment_path, F_OK) == 0) {
                    struct stat fragment_stat;
                    if (stat(current_fragment_path, &fragment_stat) == 0) {
                        total_size += fragment_stat.st_size;
                    }
                    fragment_count++;
                } else {
                    break;
                }
            }
            stbuf->st_size = total_size;

        } else {
            res = -ENOENT;
        }
        free(filename);
    }
    return res;
}
```
Untuk cara kerjanya seperti ini.
1. Untuk mencegah terjadinya error, diperlukan `memset` untuk membersihkan memori.
2. Jika program menangani direktori root, maka permission diubah menjadi 0777.
3. Jika program menangani file, maka program akan mencari atribut file berupa informasi yang terkait dengan file tersebut.
4. Jika file ada, maka program dapat menjalankan fungsi `if (access(fragment_path, F_OK) == 0)`.
5. Setelah itu, file virtual dibuat di dalam fungsi `while(1)`.
6. Jika file tidak ada, maka akan muncul error dengan kode `-ENOENT`.
7. Memori dibersihakn untuk menghindari terjadinya memory leak.
8. Program akan return 0 jika berhasil (`return res`).

### B. `baymax_readdir()`
Selanjutnya, program akan membaca file dengan fungsi ini. Untuk kodenya seperti ini
```
static int baymax_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;
    
    if (strcmp(path, "/") != 0)
        return -ENOENT;
    
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    
    DIR *dir;
    struct dirent *ent;
    char *files[MAX_FRAGMENTS];
    int file_count = 0;
    
    if ((dir = opendir(options.relics_dir))) {
        while ((ent = readdir(dir)) && file_count < MAX_FRAGMENTS) {
            char *dot = strrchr(ent->d_name, '.');
            if (dot && strlen(dot) == 4 && isdigit(dot[1]) && isdigit(dot[2]) && isdigit(dot[3])) {
                *dot = '\0';
                int found = 0;
                for (int i = 0; i < file_count; i++) {
                    if (strcmp(files[i], ent->d_name) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    files[file_count] = strdup(ent->d_name);
                    file_count++;
                }
                *dot = '.';
            }
        }
        closedir(dir);
        
        for (int i = 0; i < file_count; i++) {
            filler(buf, files[i], NULL, 0);
            free(files[i]);
        }
    }
    return 0;
}
```

Dimana untuk cara kerjanya seperti ini.
1. Untuk mencegah terjadinya error, diperlukan `(void)` untuk paramter yang mungkin tidak dimasukkan.
2. Pengecekan path dilakukan, jika tidak ada maka fungsi akan mengembalikan error berupa `-ENOENT`.
3. Fungsi `filler(buf, ".", NULL, 0);` diperlukan untuk menambahkan nama entri ke dalam buffer yang dikembalikan oleh FUSE.
4. Setelah itu, fungsi membaca direktori `relics`.
5. Lalu, fungsi `while()` dijalankan untuk membaca setiap entri hingga tidak ada lagi entri yang harus dibaca.
6. `char *dot = strrchr(ent->d_name, '.');` digunakan untuk mencari kemunculan katakter terskhir, misalnya `.000`, `.001`, dan seterusnya.
7. Lalu, fungsi `if()` dijalankan. Jika syarat terpenuhi, maka karakter terakhir ditambahkan null terminator berupa `'\0'`.
8. Karena sebuah file virtual mungkin terdiri dari banyak fragmen (misalnya `document.000`, `document.001`, `document.002`), kita tidak ingin menampilkan document berkali-kali di direktori FUSE. Kode ini memeriksa apakah nama file (setelah dipotong ekstensi fragmen) sudah ada di array files yang menyimpan nama-nama unik. Jika belum ada, nama tersebut disalin ke files dan file_count ditingkatkan.
9. Setelah loop pembacaan direktori `relics` selesai dan semua file unik dikumpulakn di array `files`, maka kode `filler(buf, files[i], NULL, 0)` digunakan untuk menambahkan nama file ke buffer yang akan dikembalikan oleh FUSE.
10. `free(files[i]` digunakan untuk membersihkan memori untuk menghindari terjadinya memory leak.

### C. `baymax_open()`
Pada fungsi ini, program akan membuka file virtual dalam FUSE. Untuk kodenya seperti ini.
```
static int baymax_open(const char *path, struct fuse_file_info *fi) {
    char *filename = basename(strdup(path + 1));
    log_activity("READ", filename);
    free(filename);
    return 0;
}
```
Dimana untuk log_activity akan dijabarkan pada poin K.

### D. `baymax_read()`
Fungsi ini diperlukan untuk membaca file. Perbedaannya dengan `baymax_readdir` terletak pada jenis entitas yang dibaca. pada `baymax_readdir`, FUSE akan membaca sebuah dir, sedangkan pada `baymax_read` akan membaca sebuah file. Untuk kodenya seperti ini
```
static int baymax_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    char *fragments[MAX_FRAGMENTS];
    char *filename = basename(strdup(path + 1));
    int count = get_fragments(filename, fragments);
    
    if (count == 0) {
        free(filename);
        return -ENOENT;
    }

    size_t total_size = 0;
    size_t fragment_sizes[MAX_FRAGMENTS];

    for (int i = 0; i < count; i++) {
        char fragment_path[PATH_MAX];
        snprintf(fragment_path, sizeof(fragment_path), "%s/%s", options.relics_dir, fragments[i]);
        struct stat st;
        if (stat(fragment_path, &st) == 0) {
            fragment_sizes[i] = st.st_size;
            total_size += st.st_size;
        } else {
            fragment_sizes[i] = 0;
        }
    }

    size_t total_read = 0;
    size_t current_offset = 0;

    for (int i = 0; i < count && total_read < size; i++) {
        char fragment_path[PATH_MAX];
        snprintf(fragment_path, sizeof(fragment_path), "%s/%s", options.relics_dir, fragments[i]);
        
        FILE *f = fopen(fragment_path, "rb");
        if (f) {
            size_t fragment_size = fragment_sizes[i];
            if (offset < current_offset + fragment_size) {
                size_t fragment_offset = (offset > current_offset) ? offset - current_offset : 0;
                size_t read_size = fragment_size - fragment_offset;
                if (read_size > size - total_read)
                    read_size = size - total_read;

                if (fragment_offset < fragment_size) {
                    fseek(f, fragment_offset, SEEK_SET);
                    total_read += fread(buf + total_read, 1, read_size, f);
                }
            }
            current_offset += fragment_size;
            fclose(f);
        }
        free(fragments[i]);
    }

    free(filename);
    return total_read;
}
```
Dimana cara kerjanya sebagai berikut.
1. `memset(stbuf, 0, sizeof(struct stat));` digunakan untuk membersihkan memori agar tidak ada sampah yang mengganggu jalannya kode.
2. Jika path yang diminta adalah "/", artinya sistem operasi menanyakan atribut untuk direktori root.
3. Jika path adalah file, ia mendapatkan nama file dasar dan memeriksa apakah fragmen pertamanya (.000) ada di direktori relics.
4. Apabila fragmen .000 ditemukan, ia mulai menghitung ukuran total file virtual dengan menjumlahkan ukuran semua fragmen (.000, .001, dst.) yang terkait dengan nama file dasar tersebut.
5. Fungsi akan menghitung ukuran file virtual dengan menjumlahkan semua fragmen yang terkait dengan nama file tersebut.
6. Jika proses read selesai, maka memori dibersihkan untuk menghindari terjadinya memory leak.
7. Fungsi akan me-return 0 jika berhasil.

### E. `baymax_unink()`
Fungsi ini digunakan untuk menghapus file yang ada di mountpoint. Jika file ini dihapus, maka file fisik yang terkait juga dihapus. Untuk kodenya seperti ini
```
static int baymax_unlink(const char *path) {
    char *fragments[MAX_FRAGMENTS];
    char *filename = basename(strdup(path + 1));
    int count = get_fragments(filename, fragments);
    if (count == 0) {
        free(filename);
        return -ENOENT;
    }
    char details[1024] = {0};
    strcat(details, fragments[0]);
    for (int i = 1; i < count; i++) {
        strcat(details, " - ");
        strcat(details, fragments[i]);
    }
    log_activity("DELETE", details);
    for (int i = 0; i < count; i++) {
        char fragment_path[PATH_MAX];
        snprintf(fragment_path, sizeof(fragment_path), "%s/%s", options.relics_dir, fragments[i]);
        unlink(fragment_path);
        free(fragments[i]);
    }
    free(filename);
    return 0;
}
```
Dimana cara kerjanya sebagai berikut.
1. Program mendapatkan nama file dasar dari jalur yang diminta, lalu mencari semua fragmen fisik yang menyusun file virtual tersebut.
2. Jika fragmen tidak ditemukan, maka berarti file virtual tidak ada dan program mengembalikan error berupa `-ENOENT`.
3. Fungsi mencatat aktivitas "DELETE" ke file log.
4. Program kemudian mengiterasi setiap fragmen, membangun jalur lengkapnya, dan memanggil fungsi `unlink()` untuk menghapus fragmen fisik tersebut dari disk.
5. Setelah semua fragmen dihapus, memori yang dialokasikan akan dibebaskan untuk menghindari terjadinya memory leak.
6. Fungsi mengembalikan 0 jika penghapusan file di mountpoint berhasil.

### F. `baymax_create()`
Fungsi ini digunakan untuk membuat file, untuk kodenya seperti ini
```
static int baymax_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char *filename = basename(strdup(path + 1));
    char fragment_path[PATH_MAX];
    snprintf(fragment_path, sizeof(fragment_path), "%s/%s.000", options.relics_dir, filename);
    int fd = open(fragment_path, O_CREAT | O_EXCL | O_WRONLY, mode);
    if (fd == -1) {
        free(filename);
        return -errno;
    }
    close(fd);
    log_activity("CREATE", filename);
    free(filename);
    return 0;
}
```
Dimana untuk cara kerjanya sebagai berikut.
1. Fungsi mendapatkan nama file dasar yang diminta untuk dibuat di path.
2. Fungsi membuat path lengkap.
3. Fungsi `open()` dipanggil untuk membuat file fisik fragmen `.000` di lokasi `relics`, dengan bendera O_CREAT (buat jika tidak ada), `O_EXCL` (error jika sudah ada), dan `O_WRONLY` (hanya tulis), serta mode izin yang diberikan (`mode`).
4. Jika `fd` bernilai `-1`, maka memori dibersihkan dan mengembalikan error berupa `-errno`.
5. Setleh file berhasil dibuat, maka `fd` akan ditutup menggunakan `close(fd)`.
6. Setelah itu, fungsi mencatat log jika berhasil.
7. Terakhir, memori dibersihkan untuk menghindari terjadinya memory leak.

### G. `baymax_truncate()`
Fungsi ini digunakan untuk mengubah ukuran file. Untuk kodenya sepertii ini.
```
static int baymax_truncate(const char *path, off_t size) {
 char *filename = basename(strdup(path + 1));
    char base_filename[256];
    strncpy(base_filename, filename, sizeof(base_filename) - 1);
    base_filename[sizeof(base_filename) - 1] = '\0';
    char fragment_path[PATH_MAX];
    snprintf(fragment_path, sizeof(fragment_path), "%s/%s.000", options.relics_dir, base_filename);

    int res = truncate(fragment_path, size);
     if (res == -1) {
        free(filename);
        return -errno;
    }
    log_activity("TRUNCATE", filename);
    free(filename);
    return 0;
}
```
Dimana untuk cara kerjanya sebagai berikut.
1. Program mendapatkan nama file dasar yang diminta untuk diubah ukurannya dari path.
2. Nama file dasar tersebut disalin ke buffer `base_filename` untuk memastikan keamanan string dan penanganan basename yang dapat mengubah input aslinya.
3. Fungsi membentuk jalur lengkap ke fragmen pertama file ini (`.000`) di direktori `relics`.
4. Fungsi `truncate()` dipanggil pada fragmen fisik `.000` tersebut untuk mengubah ukurannya menjadi size yang diminta.
5. Jika `truncate()` mengembalikan -1, berarti ada kesalahan dalam mengubah ukuran fragmen dan program mengembalikan kode error berupa `-errno`.
6. Fungsi mencatat log truncate.
7. Memori dibersihkan untuk menghindari terjadinya memory leak.
8. Fungsi mengembalikan 0 jika berhasil.

### H. `baymax_utiments()`
Fungsi ini diguankan untuk mengubah waktu akses dan waktu modifikasi dari sebuah file atau direktori. Untuk kodenya seperti ini.
```
static int baymax_utimens(const char *path, const struct timespec tv[2]) {
    char *filename = basename(strdup(path + 1));
    char fragment_path[PATH_MAX];
    snprintf(fragment_path, sizeof(fragment_path), "%s/%s.000", options.relics_dir, filename);
    int res = utimensat(0, fragment_path, tv, AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        free(filename);
        return -errno;
    }
    log_activity("UTIMENS", filename);
    free(filename);
    return 0;
}
```
Dimana untuk cara kerjanya sebagai berikut.
1. Program mendapatkan nama file dasar yang diminta untuk diubah timestamps-nya.
2. Ia membentuk jalur ke fragmen pertama file tersebut (`.000`) di direktori `relics`.
3. Fungsi utimensat() sistem kemudian dipanggil pada fragmen fisik `.000` tersebut untuk mengubah waktu akses dan modifikasinya.
4. Jika ada kesalahan, program mengembalikan kode error berupa `-errno`.
5. Aktivitas `"UTIMENS"` dicatat ke file log.
6. Memori yang dialokasikan dibebaskan untuk menghindari terjadinya memory leak.
7. Fungsi mengembalikan 0 jika operasi berhasil.

### I. `baymax_write()`
ungsi ini digunakan untuk menulis data ke sebuah file dalam FUSE. Untuk kodenya seperti ini
```
static int baymax_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char *filename = basename(strdup(path + 1));
    char base_filename[256];
    strncpy(base_filename, filename, sizeof(base_filename) - 1);
    base_filename[sizeof(base_filename) - 1] = '\0';
    char log_details[PATH_MAX * 2] = {0};
    int fragment_count = offset / FRAGMENT_SIZE;
    size_t bytes_written = 0;
    size_t current_offset_in_fragment = offset % FRAGMENT_SIZE;

    while (bytes_written < size) {
        char fragment_name[PATH_MAX];
        snprintf(fragment_name, sizeof(fragment_name), "%s.%03d", base_filename, fragment_count);
        char fragment_path[PATH_MAX];
        snprintf(fragment_path, sizeof(fragment_path), "%s/%s", options.relics_dir, fragment_name);
        size_t fragment_size = FRAGMENT_SIZE - current_offset_in_fragment;
        if (size - bytes_written < fragment_size) {
            fragment_size = size - bytes_written;
        }
        int fragment_fd = open(fragment_path, O_WRONLY | O_CREAT, 0644);
        if (fragment_fd == -1) {
            free(filename);
            return -errno;
        }
        ssize_t written = pwrite(fragment_fd, buf + bytes_written, fragment_size, current_offset_in_fragment);
        if (written == -1) {
            close(fragment_fd);
            free(filename);
            return -errno;
        }
        close(fragment_fd);
        bytes_written += written;
        current_offset_in_fragment = 0;
        fragment_count++;
    }
    snprintf(log_details, sizeof(log_details), "%s -> %s.000", filename, filename);
    log_activity("WRITE", log_details);
    free(filename);
    return bytes_written;
}
```
Dimana untuk cara kerjanya seperti ini.
1. Program mendapatkan nama file dasar tempat data akan ditulis dari `path`.
2. Fungsi menginisialisasi variabel untuk melacak jumlah byte yang sudah ditulis (`bytes_written`), nomor fragmen yang sedang diproses (`fragment_count`), dan offset di dalam fragmen saat ini (`current_offset_in_fragment`) berdasarkan offset permintaan.
3. Loop utama dimulai dan terus berjalan selama masih ada data yang perlu ditulis.
4. Di setiap iterasi, program menentukan nama fragmen yang relevan (misal: filename.000, filename.001, dst.) dan membangun jalur fisiknya.
5. Ia menghitung ukuran data yang akan ditulis ke fragmen saat ini, memastikan tidak melebihi ukuran fragmen atau total size yang diminta.
6. Fragmen fisik dibuka dengan bendera O_WRONLY (tulis saja) dan O_CREAT (buat jika belum ada); jika gagal, error dikembalikan.
7. Fungsi pwrite() digunakan untuk menulis data dari buf ke fragmen fisik pada current_offset_in_fragment yang tepat, memastikan penulisan dimulai dari posisi yang benar dalam fragmen tersebut.
8. Jika `pwrite()` gagal, error dikembalikan, dan file descriptor ditutup.
9. Setelah penulisan berhasil pada fragmen, file descriptor ditutup, bytes_written diperbarui, current_offset_in_fragment direset ke 0 (karena fragmen berikutnya akan ditulis dari awal), dan fragment_count ditingkatkan.
10. Setelah semua data ditulis, program mencatat aktivitas "WRITE" ke file log, dan membebaskan memori yang dialokasikan.
11. Fungsi mengembalikan jumlah total byte yang berhasil ditulis.

### J. `baymax_release()`
Fungsi ini digunakan untuk menangani saat file ditutup setelah file tersebut dibuka. Untuk kodenya seperti ini.
```
static int baymax_release(const char *path, struct fuse_file_info *fi) {
    char *filename = basename(strdup(path + 1));
    char log_details[PATH_MAX * 3] = {0};
    char source_path[PATH_MAX] = {0};
    char real_mountpoint[PATH_MAX];
    if (realpath(mountpoint_path, real_mountpoint) == NULL) {
        strncpy(real_mountpoint, mountpoint_path, sizeof(real_mountpoint) - 1);
        real_mountpoint[sizeof(real_mountpoint) - 1] = '\0';
    }
    if (snprintf(source_path, sizeof(source_path), "%s/%s", real_mountpoint, filename) >= (int)sizeof(source_path)) {
        strncpy(source_path, real_mountpoint, sizeof(source_path) - 1);
        source_path[sizeof(source_path) - 1] = '\0';
        strncat(source_path, "/", sizeof(source_path) - strlen(source_path) - 1);
        strncat(source_path, filename, sizeof(source_path) - strlen(source_path) - 1);
    }
   
    free(filename);
    return 0;
}
```
Dimana untuk cara kerjanya sebagai berikut.
1. Program mendapatkan nama file dasar yang sedang ditutup dari path.
2. Ia mencoba mendapatkan jalur absolut (realpath) dari mount point FUSE. Jika gagal, ia menggunakan jalur mount point yang disimpan.
3. Program kemudian mencoba membangun jalur lengkap ke file virtual tersebut di mount point (source_path). Ini mencakup penanganan potensi buffer overflow.
4. Pada implementasi yang diberikan, tidak ada operasi signifikan yang dilakukan setelah membangun source_path (seperti pencatatan log "RELEASE" atau penyalinan file ke tujuan yang ditentukan oleh options.copy_dest_dir).
5. Memori yang dialokasikan untuk filename dibebaskan.
6. Fungsi mengembalikan 0 untuk menunjukkan operasi penutupan file berhasil.

### K. `log_activity()`
Fungsi ini digunakan untuk mencatat beberapa fungsi yang berhasil dijalankan. Untuk kodenya seperti ini.
Dimana untuk cara kerjanya sebagai berikut.
```
void log_activity(const char *action, const char *details) {
    time_t now;
    struct tm tm_info;
    char timestamp[64];
    time(&now);
    localtime_r(&now, &tm_info);
    strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S]", &tm_info);
    FILE *log = fopen(LOG_FILE, "a");
    if (log == NULL) {
        perror("Failed to open log file");
        return;
    }
    fprintf(log, "%s %s: %s\n", timestamp, action, details);
    fclose(log);
}
```
1. Pertama, fungsi ini mengambil waktu saat ini dan memformatnya menjadi timestamp.
2. Lalu, fungsi membuka file log bernama activity.log dalam mode append, yang berarti tulisan baru akan ditambahkan di akhir file.
3. Jika file gagal dibuka, fungsi akan menampilkan pesan error.
4. Selanjutnya, fungsi mencatat aktivitas dengan menuliskan timestamp, jenis tindakan, dan detailnya ke dalam file log.
5. Terakhir, fungsi menutup file log untuk memastikan semua data tersimpan dengan benar.

### L. `get_fragments()`
Fungsi ini digunakan untuk menemukan semua fragmen file yang terkait dengan sebuah nama file dasar. Fungsi ini bertugas mengumpulkan daftar semua nama fragmen tersebut agar konten file asli dapat dibaca atau dimanipulasi secara keseluruhan. Untuk kodenya seperti ini
```
int get_fragments(const char *filename, char **fragments) {
    DIR *dir;
    struct dirent *ent;
    int count = 0;
    dir = opendir(options.relics_dir);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL && count < MAX_FRAGMENTS) {
            if (strncmp(ent->d_name, filename, strlen(filename)) == 0) {
                char *dot = strrchr(ent->d_name, '.');
                if (dot != NULL && strlen(dot) == 4 && isdigit(dot[1]) && isdigit(dot[2]) && isdigit(dot[3])) {
                    fragments[count] = strdup(ent->d_name);
                    count++;
                }
            }
        }
        closedir(dir);
    }
    return count;
}
```
Dimana untuk cara kerjanya seperti ini.
1. Pertama, fungsi ini membuka direktori relics_dir yang telah ditentukan.
2. Lalu, fungsi membaca setiap entry (file atau direktori) di dalamnya.
3. Untuk setiap entry, fungsi memeriksa apakah nama file cocok dengan nama file yang dicari dan memiliki ekstensi numerik .xxx (misalnya, .000, .001, dst.).
4. Jika cocok, nama fragmen tersebut akan disalin ke dalam array fragments.
5. Proses ini berlanjut hingga semua fragmen ditemukan atau batas MAX_FRAGMENTS tercapai.
6. Terakhir, fungsi mengembalikan jumlah fragmen yang ditemukan.
### M. `static struct fuse_opt option_specs[]`
Fungsi dari `static struct fuse_opt option_specs[]` adalah menentukan opsi-opsi baris perintah yang bisa digunakan oleh filesystem FUSE, seperti direktori relik, direktori tujuan salinan, dan nama file tujuan. Untuk kodenya seperti ini.
```
static struct fuse_opt option_specs[] = {

    OPTION("--relics-dir=%s", relics_dir),

    OPTION("--copy-dest=%s", copy_dest_dir),

    OPTION("--dest-filename=%s", dest_filename),

    FUSE_OPT_END

};
```

### N. Fungsi `find_mountpoint()`
Fungsi ini diguankan untuk mencari mountpoint. Untuk kodenya seperti ini
```
static void find_mountpoint(int argc, char *argv[]) {

    for (int i = argc - 1; i > 0; i--) {

        if (argv[i][0] != '-') {

            strncpy(mountpoint_path, argv[i], sizeof(mountpoint_path) - 1);

            mountpoint_path[sizeof(mountpoint_path) - 1] = '\0';

            size_t len = strlen(mountpoint_path);

            if (len > 1 && mountpoint_path[len - 1] == '/') {

                mountpoint_path[len - 1] = '\0';

            }

            break;

        }

    }

}
```
Dimana untuk cara kerjanya sebagai berikut.
1. Fungsi mencari mountpoint Fungsi ini mencari argumen terakhir di baris perintah yang tidak diawali dengan tanda strip (-).
2. Setelah itu, fungsi menyimpan path Argumen yang ditemukan ini dianggap sebagai mountpoint dan disimpan ke mountpoint_path.
3. Fungsi menghilangkan garis miring Jika path berakhir dengan garis miring (/), garis miring itu dihilangkan untuk normalisasi.
4. Terakhir, fungsi mengakhiri pencarian Setelah mountpoint ditemukan, proses pencarian dihentikan.

### O. `int_main()`
Untuk int_main sendiri berisi inisialisasi, fing_mountpoint, fuse_operation, fungsi dari FUSE itu sendiri, dan pembersihan memori agar tidak terjadi memory leak. Untuk kodenya seperti ini.
```
int main(int argc, char *argv[]) {
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
    options.relics_dir = strdup("relics");
    options.copy_dest_dir = NULL;
    options.dest_filename = NULL;
    if (fuse_opt_parse(&args, &options, option_specs, NULL) == -1)
        return 1;
    find_mountpoint(args.argc, args.argv);
    mkdir(options.relics_dir, 0777);

    static struct fuse_operations baymax_oper = {
        .getattr    = baymax_getattr,
        .readdir    = baymax_readdir,
        .open       = baymax_open,
        .read       = baymax_read,
        .unlink     = baymax_unlink,
        .create     = baymax_create,
        .truncate   = baymax_truncate,
        .utimens    = baymax_utimens,
        .write      = baymax_write,
        .release = baymax_release
    };

    int ret = fuse_main(args.argc, args.argv, &baymax_oper, NULL);

    free(options.relics_dir);
    if (options.copy_dest_dir)
        free(options.copy_dest_dir);
    if (options.dest_filename)
        free(options.dest_filename);

    return ret;
}
```
Untuk cara kerjanya sebagai berikut.
1. Pertama, program menyiapkan argumen dan opsi FUSE dari baris perintah.
2. Lalu, program menentukan titik mount dan memastikan direktori "relics" ada.
3. Setelah itu, program mendefinisikan cara filesystem akan berinteraksi (baca, tulis, dan sebagainya).
4. Kemudian, program menjalankan FUSE, membuat filesystem virtual berfungsi.
5. Terakhir, program membersihkan memori setelah sistem file tidak lagi digunakan.

### P. Revisi
Revisi pada soal ini terletak pada penulisan log dan fitur copy paste antar mount_dir dan file fisik. Ada cukup banyak fungsi yang direvisi, diantaranya yang direvisi (sebagai penulisan write) adalah fungsi `baymax_write()` dan `baymax_release()`. Untuk fungsi `baymax_create()` seperti ini.
```
static int baymax_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    (void)mode; // Mode tidak digunakan secara langsung di sini

    const char *filename = path + 1;
    char part_path[PATH_MAX];

    make_part_path(part_path, sizeof(part_path), filename, 0);

    FILE *f = fopen(part_path, "wb"); // Buka file sebagai binary write
    if (!f) {
        write_log("ERROR: Failed to create initial part for %s: %s", filename, strerror(errno));
        return -EIO; // Error I/O
    }
    fclose(f); // Tutup file kosong

    write_log("WRITE: %s -> %s", filename, part_path);
    return 0; // Berhasil
}
```
Untuk fungsi `baymax_release()` seperti ini.
```
static int baymax_release(const char *path, struct fuse_file_info *fi) {
    struct file_write_buffer *wb = get_write_buffer(fi);
    if (wb) {
        const char *filename = path + 1;
        int parts = save_parts(filename, wb->data, wb->size);

        if (parts > 0) {
            char part_list[4096] = {0};
            for (int i = 0; i < parts; i++) {
                char partname[256];
                snprintf(partname, sizeof(partname), "%s.%03d", filename, i);
                strcat(part_list, partname);
                if (i != parts - 1) {
                    strcat(part_list, ", ");
                }
            }
            write_log("WRITE: %s -> %s", filename, part_list);
        } else {
            // Ini akan mencatat file kosong atau jika save_parts mengembalikan 0 karena error
            write_log("WRITE: %s (no parts saved or empty file)", filename);
        }

        free(wb->data);
        free(wb);
        set_write_buffer(fi, NULL);
    }
    return 0;
}
```
Untuk outputnya seperti ini.
```
[2025-05-23 23:10:00] WRITE: .misal2.txt.swp -> /home/jofanka/Sistem_Operasi/Modul_4/soal_2/relics/.misal2.txt.swp.000
[2025-05-23 23:10:03] WRITE: misal2.txt -> /home/jofanka/Sistem_Operasi/Modul_4/soal_2/relics/misal2.txt.000
```
## soal_3

pada soal kali ini kita diminta untuk membuat sebuah program yang dapat mendeteksi file berbahaya kemudian membalikan nama file tersebut  lalu mencetak log serta mengenkriipsi file yang tidak berbahaya dengan ROT13

<h3>
    antink.c
</h3>

```c
#define FUSE_USE_VERSION 30
```
Mengaktifkan penggunaan FUSE versi 3.0 (FUSE_USE_VERSION 30)

```c
void rot13(char *str) {
    for (; *str; ++str) {
        if ((*str >= 'a' && *str <= 'm') || (*str >= 'A' && *str <= 'M')) {
            *str += 13;
        } else if ((*str >= 'n' && *str <= 'z') || (*str >= 'N' && *str <= 'Z')) {
            *str -= 13;
        }
    }
}
```
<li> Mengubah huruf alfabet menjadi bentuk ROT13 (menggeser 13 karakter) </li>
<li> hanya berlaku untuk file non-berbahaya saat dibaca. </li>

```c
void reverse_string(char *str) {
    if (!str) return;
    int i = 0, j = strlen(str) - 1;
    while (i < j) {
        char c = str[i];
        str[i++] = str[j];
        str[j--] = c;
    }
}
```
Membalikkan isi string, digunakan untuk menampilkan nama file berbahaya dalam kondisi terbalik.

```c
int is_dangerous(const char *filename) {
    return strstr(filename, KEYWORD1) || strstr(filename, KEYWORD2);
}
```
Mengembalikan nilai true jika nama file mengandung "nafis" atau "kimcun".

```c
void log_activity(const char *filename, const char *action) {
    FILE *log = fopen(LOG_FILE, "a");
    if (log) {
        time_t now = time(NULL);
        fprintf(log, "[%s] %s: %s\n", ctime(&now), action, filename);
        fclose(log);
    }
}
```
Membuka file log /var/log/it24.log lalu menuliskan waktu, aksi (DANGEROUS_FILE), dan nama file.

getattr
```c
static int antink_getattr(const char *path, struct stat *st) {
    char full_path[MAX_PATH_LEN];
    snprintf(full_path, sizeof(full_path), "/it24_host%s", path);
    return lstat(full_path, st) ? -errno : 0;
}

```
<li>Menyediakan metadata file seperti ukuran, mode, dll.</li>
<li>File asli dicari di /it24_host.</li>

readdir
```c
static int antink_readdir(const char *path, void *buf, fuse_fill_dir_t filler, 
                         off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char full_path[MAX_PATH_LEN];
    
    snprintf(full_path, sizeof(full_path), "/it24_host%s", path);
    if (!(dp = opendir(full_path))) return -errno;

    while ((de = readdir(dp))) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        char disp_name[MAX_PATH_LEN];
        strcpy(disp_name, de->d_name);

        if (is_dangerous(de->d_name)) {
            reverse_string(disp_name);
            log_activity(de->d_name, "DANGEROUS_FILE");
        }

        if (filler(buf, disp_name, &st, 0)) break;
    }
    closedir(dp);
    return 0;
}
```

<li>Menyediakan metadata file seperti ukuran, mode, dll.</li>
<li>File asli dicari di /it24_host.</li>
<li>Membuka direktori /it24_host/<path>.</li>
<li>Untuk setiap entry file/direktori:<br>
Disiapkan info stat-nya <br>
Nama file disalin ke buffer disp_name</li>
<li>Jika berbahaya:
<br>
Nama file dibalik
<br>
Dicatat ke log</li>
<li>Mengisi hasil isi direktori yang ditampilkan ke user</li>

Struktur fuse_operations
```c
static struct fuse_operations antink_ops = {
    .getattr = antink_getattr,
    .readdir = antink_readdir,
    .open = antink_open,
    .read = antink_read,
};
```
Struktur fuse_operations adalah tabel fungsi callback yang memberitahu FUSE bagaimana menangani operasi sistem file tertentu.
Setiap entri di struktur ini mengarah ke fungsi handler yang telah kita implementasikan.

<table border="1">
  <thead>
    <tr>
      <th>Baris</th>
      <th>Makna</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><code>.getattr = antink_getattr,</code></td>
      <td>Digunakan untuk menangani permintaan metadata file, misalnya saat user menjalankan <code>ls -l</code> atau <code>stat</code>. FUSE akan memanggil <code>antink_getattr()</code> untuk mengisi informasi seperti ukuran file, permission, dll.</td>
    </tr>
    <tr>
      <td><code>.readdir = antink_readdir,</code></td>
      <td>Digunakan saat user ingin melihat isi direktori, misalnya dengan <code>ls</code>. Fungsi <code>antink_readdir()</code> akan memberikan daftar nama file dan folder di direktori tersebut.</td>
    </tr>
    <tr>
      <td><code>.open = antink_open,</code></td>
      <td>Digunakan saat user mencoba membuka file. Fungsi <code>antink_open()</code> memverifikasi bahwa file bisa dibuka, dan menyiapkan file descriptor.</td>
    </tr>
    <tr>
      <td><code>.read = antink_read,</code></td>
      <td>Digunakan saat user membaca file. Fungsi <code>antink_read()</code> akan membaca isi file dari sistem file backend dan memberikan hasilnya (yang bisa dimodifikasi, seperti ROT13).</td>
    </tr>
  </tbody>
</table>

Fungsi main()

```c
int main(int argc, char *argv[]) {
    umask(0);
    return fuse_main(argc, argv, &antink_ops, NULL);
}
```
Fungsi dan Tujuan:
Fungsi main() adalah titik awal program seperti biasa. Di dalamnya kita memanggil fuse_main() yang akan:
<br>
Menyambungkan sistem file kita ke mount point
<br>
Menangani request dari user (melalui kernel)
<br>
Menjalankan loop yang menunggu dan merespon operasi sistem file.

<h3>
    Dockerfile
</h3>

```dockerfile
FROM ubuntu:22.04
```
Menggunakan image dasar Ubuntu versi 22.04.
<br>
Ini adalah sistem operasi minimal yang cocok untuk membangun dan menjalankan aplikasi berbasis FUSE (Filesystem in Userspace).

```dockerfile
RUN apt-get update && \
    apt-get install -y \
    fuse \
    libfuse-dev \
    gcc \
    make \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*
```
<ul>
  <li> <code>apt-get update</code>: Memperbarui daftar paket.</li>
  <li> <code>apt-get install -y</code>: Menginstal beberapa dependensi penting:
    <ul>
      <li><code>fuse</code>: Binary utama untuk menjalankan filesystem FUSE.</li>
      <li><code>libfuse-dev</code>: Header dan library untuk mengembangkan aplikasi berbasis FUSE.</li>
      <li><code>gcc</code>: Compiler C yang digunakan untuk menyusun program <code>antink.c</code>.</li>
      <li><code>make</code>: Utilitas build system (meskipun tidak digunakan langsung di Dockerfile ini).</li>
      <li><code>pkg-config</code>: Digunakan untuk mencari informasi build library (bisa membantu saat build proyek yang kompleks).</li>
    </ul>
  </li>
  <li> <code>rm -rf /var/lib/apt/lists/*</code>: Membersihkan cache apt untuk mengurangi ukuran image Docker.</li>
</ul>

```dockerfile
WORKDIR /app
COPY antink.c .
RUN gcc antink.c -o antink -D_FILE_OFFSET_BITS=64 -lfuse
HEALTHCHECK --interval=5s --timeout=3s --retries=3 \
  CMD ls /antink_mount && [ -f /var/log/it24.log ]
```
Menetapkan direktori kerja menjadi /app.
Semua perintah selanjutnya akan dijalankan dari folder ini.

<br>

Menyalin file sumber antink.c dari direktori lokal ke folder /app di dalam container.
<br>
Menyusun program antink.c menggunakan gcc.
<br>
-o antink: Outputnya adalah file binary bernama antink.
<br>
-D_FILE_OFFSET_BITS=64: Flag penting agar filesystem bisa menangani file besar (>2GB).
<br>
-lfuse: Melink ke library FUSE. Catatan: karena image Ubuntu ini menggunakan libfuse2, kita pakai -lfuse, bukan -lfuse3.


```dockerfile
HEALTHCHECK --interval=5s --timeout=3s --retries=3 \
  CMD ls /antink_mount && [ -f /var/log/it24.log ]
CMD ["/app/antink", "-f", "/antink_mount"]
```

Penambahan health check pada container bertujuan untuk memastikan bahwa sistem berjalan dengan benar. Container dianggap sehat jika direktori mount /antink_mount dapat diakses, yang menandakan bahwa sistem FUSE aktif, dan jika file log /var/log/it24.log telah dibuat, menunjukkan bahwa program utama sudah berjalan dan melakukan logging. Pemeriksaan dilakukan secara berkala setiap 5 detik, dengan batas waktu (timeout) 3 detik, dan akan dicoba hingga 3 kali sebelum container dinyatakan gagal. Saat container dimulai, perintah utama yang dijalankan adalah menjalankan program antink dengan flag -f, yang memastikan program berjalan di foreground mode—hal ini penting dalam konteks Docker agar proses utama tidak berjalan sebagai daemon. Direktori /antink_mount berfungsi sebagai mount point FUSE dan harus dipastikan sudah tersedia di host atau di dalam container saat eksekusi.

<h3>docker-compose.yml </h3>
File ini mendefinisikan dua container layanan: antink-server dan antink-logger. Container ini bekerja bersama untuk menjalankan filesystem berbasis FUSE (antink) dan mengakses log-nya secara terpisah.

```yml
version: '3.8'
```
Menentukan versi docker-compose schema yang digunakan (3.8) — stabil dan kompatibel dengan Docker Desktop dan Docker Engine terbaru.

```yml
services:
  antink-server:
    build: .
    container_name: antink-server
    privileged: true
    volumes:
      - ./it24_host:/it24_host
      - ./antink_mount:/antink_mount:rshared
      - ./antink-logs:/var/log
```
Membangun image dari Dockerfile yang berada di direktori saat ini (.).
<br>
Menetapkan nama container menjadi antink-server.
<br>
Mengaktifkan akses istimewa penuh ke host. Wajib untuk filesystem FUSE agar bisa mount/akses /dev/fuse.
<br>
Mount direktori lokal ke dalam container:
<ul>
  <li><code>./it24_host</code>: Sumber file nyata di host.</li>
  <li><code>./antink_mount</code>: Tempat filesystem FUSE dimount oleh <code>antink</code>.</li>
  <li><code>:rshared</code>: Opsi <em>mount propagation</em> agar perubahan mount terlihat oleh host atau container lain. Penting untuk FUSE agar berfungsi dengan benar.</li>
  <li><code>./antink-logs</code>: Folder di host untuk menyimpan file log dari container, yaitu <code>/var/log/it24.log</code>.</li>
</ul>

```yml
    cap_add:
      - SYS_ADMIN
    devices:
      - /dev/fuse
    restart: unless-stopped
  antink-logger:
    image: alpine:latest
    container_name: antink-logger
    volumes:
      - ./antink-logs:/var/log
    depends_on:
      antink-server:
        condition: service_healthy
    restart: unless-stopped
```

Bagian ini mengatur konfigurasi lanjutan untuk dua layanan. Pada antink-server, opsi cap_add: - SYS_ADMIN memberi hak administratif ke container agar dapat melakukan operasi mount FUSE, sementara devices: - /dev/fuse memberi akses langsung ke perangkat FUSE di host. Opsi restart: unless-stopped memastikan container akan otomatis dimulai ulang kecuali dihentikan secara manual. Layanan antink-logger menggunakan image ringan alpine:latest, dan diberi akses ke volume log yang sama (./antink-logs:/var/log) agar dapat membaca log aktivitas dari antink-server. depends_on dengan condition: service_healthy memastikan antink-logger hanya berjalan setelah antink-server dipastikan sehat oleh healthcheck, menjaga urutan inisialisasi layanan dengan benar.
## soal_4 
