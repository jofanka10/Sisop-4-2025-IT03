#define FUSE_USE_VERSION 31
#include <fuse3/fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>

#define MAX_PART_SIZE 1024
#define MAX_PARTS 1000

static char relics_dir[PATH_MAX];
static char log_path[PATH_MAX];

// --- Deklarasi Forward untuk Fungsi Helper dan Struktur ---
// Ini memberitahu compiler bahwa fungsi-fungsi ini ada,
// dan definisi lengkapnya akan menyusul.
void write_log(const char *format, ...);
static void make_part_path(char *buf, size_t size, const char *filename, int index);
static int count_parts(const char *filename);
static int read_full_file(const char *filename, char *buf, size_t size, off_t offset);

// Deklarasi struktur harus di atas jika ada fungsi yang menggunakannya
struct file_write_buffer {
    char *data;
    size_t size;
    size_t capacity;
};

// Fungsi helper untuk buffer tulis (ini harus dideklarasikan/didefinisikan sebelum baymax_write/release)
static struct file_write_buffer *get_write_buffer(struct fuse_file_info *fi);
static void set_write_buffer(struct fuse_file_info *fi, struct file_write_buffer *buf);
static int save_parts(const char *filename, const char *buf, size_t size); // Deklarasi save_parts


// --- Implementasi Fungsi Helper ---

// Helper untuk log aktivitas dengan timestamp
void write_log(const char *format, ...) {
    FILE *logfile = fopen(log_path, "a");
    if (!logfile) return;

    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "[%Y-%m-%d %H:%M:%S]", tm_info);

    fprintf(logfile, "%s ", timebuf);

    va_list args;
    va_start(args, format);
    vfprintf(logfile, format, args);
    va_end(args);

    fprintf(logfile, "\n");
    fclose(logfile);
}

// Helper untuk buat path file potongan relics
static void make_part_path(char *buf, size_t size, const char *filename, int index) {
    snprintf(buf, size, "%s/%s.%03d", relics_dir, filename, index);
}

// Helper hitung jumlah potongan file di relics untuk file nama tertentu
static int count_parts(const char *filename) {
    char part_path[PATH_MAX];
    int count = 0;
    while (count < MAX_PARTS) {
        make_part_path(part_path, sizeof(part_path), filename, count);
        if (access(part_path, F_OK) != 0) break;
        count++;
    }
    return count;
}

// Helper baca data gabungan dari pecahan relics ke buffer
static int read_full_file(const char *filename, char *buf, size_t size, off_t offset) {
    int part_count = count_parts(filename);
    if (part_count == 0) return -ENOENT;

    size_t total_size = part_count * MAX_PART_SIZE;
    if (offset >= total_size) return 0;

    size_t to_read = size;
    if (offset + to_read > total_size)
        to_read = total_size - offset;

    size_t read_bytes = 0;
    int part_index = offset / MAX_PART_SIZE;
    off_t part_offset = offset % MAX_PART_SIZE;

    while (to_read > 0 && part_index < part_count) {
        char part_path[PATH_MAX];
        make_part_path(part_path, sizeof(part_path), filename, part_index);

        FILE *f = fopen(part_path, "rb");
        if (!f) return -EIO;

        if (fseek(f, part_offset, SEEK_SET) != 0) {
            fclose(f);
            return -EIO;
        }

        size_t can_read = MAX_PART_SIZE - part_offset;
        if (can_read > to_read) can_read = to_read;

        size_t n = fread(buf + read_bytes, 1, can_read, f);
        fclose(f);

        if (n == 0) break;

        read_bytes += n;
        to_read -= n;
        part_index++;
        part_offset = 0;
    }
    return read_bytes;
}

// Implementasi helper untuk buffer sementara
static struct file_write_buffer *get_write_buffer(struct fuse_file_info *fi) {
    return (struct file_write_buffer *)(uintptr_t)fi->fh;
}

static void set_write_buffer(struct fuse_file_info *fi, struct file_write_buffer *buf) {
    fi->fh = (uint64_t)(uintptr_t)buf;
}

// Helper untuk simpan pecahan (dengan perbaikan chunk)
static int save_parts(const char *filename, const char *buf, size_t size) {
    int parts = 0;
    size_t offset = 0;

    if (size == 0) { // Jika ukuran file adalah 0, tidak ada bagian yang perlu disimpan
        write_log("DEBUG: save_parts called for %s with size 0", filename);
        return 0;
    }

    while (offset < size) {
        size_t chunk = MAX_PART_SIZE; // Deklarasi 'chunk' di sini
        if (size - offset < chunk) {
            chunk = size - offset;
        }

        char part_path[PATH_MAX];
        make_part_path(part_path, sizeof(part_path), filename, parts);

        FILE *f = fopen(part_path, "wb");
        if (!f) {
            write_log("ERROR: Failed to open part file %s for writing: %s", part_path, strerror(errno));
            return -EIO;
        }

        size_t written = fwrite(buf + offset, 1, chunk, f);
        fclose(f);
        if (written != chunk) {
            write_log("ERROR: Failed to write all data to part file %s. Expected %zu, wrote %zu", part_path, chunk, written);
            return -EIO;
        }

        parts++;
        offset += chunk;
    }
    write_log("DEBUG: save_parts finished for %s, total parts: %d", filename, parts);
    return parts;
}


// --- Implementasi Fungsi FUSE ---

// FUSE getattr
static int baymax_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0777;
        stbuf->st_nlink = 2;
        return 0;
    }

    const char *filename = path + 1;
    int parts = count_parts(filename);
    if (parts == 0) return -ENOENT;

    stbuf->st_mode = S_IFREG | 0777;
    stbuf->st_nlink = 1;
    stbuf->st_size = parts * MAX_PART_SIZE;
    return 0;
}

// FUSE readdir
static int baymax_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset,
                          struct fuse_file_info *fi, enum fuse_readdir_flags flags) {
    (void) offset; (void) fi; (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    DIR *d = opendir(relics_dir);
    if (!d) return 0;

    char lastname[512] = {0};
    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        int len = strlen(entry->d_name);
        if (len < 4) continue;
        if (entry->d_name[len - 4] != '.') continue;

        char base[512];
        strncpy(base, entry->d_name, len - 4);
        base[len - 4] = '\0';

        if (strcmp(base, lastname) != 0) {
            if (filler(buf, base, NULL, 0, 0) != 0)
                break;
            strcpy(lastname, base);
        }
    }

    closedir(d);
    return 0;
}

// FUSE open
static int baymax_open(const char *path, struct fuse_file_info *fi) {
    const char *filename = path + 1;
    int parts = count_parts(filename);
    if (parts == 0) return -ENOENT;
    // Untuk operasi tulis, kita perlu memastikan buffer siap jika ini adalah open untuk write
    // Tidak perlu inisialisasi di sini karena baymax_write akan menanganinya
    return 0;
}

// FUSE read
static int baymax_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    (void) fi;
    const char *filename = path + 1;
    int res = read_full_file(filename, buf, size, offset);
    if (res >= 0) {
        write_log("READ: %s", filename);
    }
    return res;
}

// FUSE create
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

    write_log("CREATE: %s -> %s", filename, part_path);

    // Ini adalah tempat yang bagus untuk menginisialisasi buffer tulis jika fi->fh diatur di sini
    // Namun, pendekatan yang ada di baymax_write (lazy allocation) juga berfungsi.
    // Jika Anda ingin menginisialisasi di sini:
    // struct file_write_buffer *wb = malloc(sizeof(struct file_write_buffer));
    // if (!wb) return -ENOMEM;
    // wb->data = NULL;
    // wb->size = 0;
    // wb->capacity = 0;
    // set_write_buffer(fi, wb);

    return 0; // Berhasil
}

static int baymax_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    struct file_write_buffer *wb = get_write_buffer(fi);
    if (!wb) {
        // Jika buffer belum dialokasikan (misalnya open tanpa create, atau open bukan untuk write)
        // Kita alokasikan di sini.
        wb = malloc(sizeof(struct file_write_buffer));
        if (!wb) return -ENOMEM;
        wb->data = NULL;
        wb->size = 0;
        wb->capacity = 0;
        set_write_buffer(fi, wb);
    }

    size_t newsize = offset + size;
    if (newsize > wb->capacity) {
        size_t newcap = newsize + MAX_PART_SIZE; // Alokasi lebih besar dari yang dibutuhkan
        char *newdata = realloc(wb->data, newcap);
        if (!newdata) return -ENOMEM;
        wb->data = newdata;
        wb->capacity = newcap;
    }

    memcpy(wb->data + offset, buf, size);
    if (newsize > wb->size) wb->size = newsize;

    // Log WRITE dipindahkan ke baymax_release
    return size;
}


// FUSE release
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

// FUSE unlink
static int baymax_unlink(const char *path) {
    const char *filename = path + 1;
    char part_path[PATH_MAX];
    char part_list[4096] = {0};
    int index = 0;
    bool deleted_any = false; // Flag untuk melacak apakah ada yang dihapus

    while (true) {
        make_part_path(part_path, sizeof(part_path), filename, index);
        if (access(part_path, F_OK) != 0) break; // Berhenti jika bagian tidak ditemukan

        char partname[64];
        snprintf(partname, sizeof(partname), "%s.%03d", filename, index);
        if (index > 0) strcat(part_list, ", ");
        strcat(part_list, partname);

        if (remove(part_path) == 0) { // Cek apakah penghapusan berhasil
            deleted_any = true;
        } else {
            write_log("ERROR: Failed to delete part file %s: %s", part_path, strerror(errno));
            // Anda bisa memilih untuk melanjutkan atau mengembalikan error di sini.
            // Untuk unlink, biasanya kita mencoba menghapus semua bagian yang bisa.
        }
        index++;
    }

    if (deleted_any) { // Log hanya jika setidaknya satu bagian berhasil dihapus
        write_log("DELETE: %s", part_list);
    } else {
        // Jika tidak ada yang dihapus, berarti file tidak ditemukan atau masalah izin
        write_log("DELETE: %s (file not found or permission issue)", filename);
        return -ENOENT; // Kembalikan ENOENT jika tidak ada bagian yang ditemukan untuk dihapus
    }

    return 0;
}

// FUSE operations
static struct fuse_operations baymax_oper = {
    .getattr = baymax_getattr,
    .readdir = baymax_readdir,
    .open    = baymax_open,
    .read    = baymax_read,
    .write   = baymax_write,
    .create  = baymax_create,
    .release = baymax_release,
    .unlink  = baymax_unlink,
};

// Main
int main(int argc, char *argv[]) {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        return 1;
    }

    snprintf(relics_dir, sizeof(relics_dir), "%s/relics", cwd);
    snprintf(log_path, sizeof(log_path), "%s/activity.log", cwd);

    // Coba buat direktori relics jika belum ada
    if (mkdir(relics_dir, 0777) == -1 && errno != EEXIST) {
        perror("mkdir relics_dir");
        return 1;
    }

    umask(0);
    return fuse_main(argc, argv, &baymax_oper, NULL);
}
