Pada soal ini kita mendapatkan pekerjaan di SEGA sebagai char designer untuk game maimai dan dipromosikan menjadi administrator, lalu kita diberi tugas untuk memastikan 7 area di maimai berfungsi seperti semestinya.<br>
1. Starter Chiho sebagai menyimpan file yang masuk secara normal.<br>
2. Metropolis Chiho sebagai file directory asli di shift sesuai lokasinya.<br>
3. Dragon Chiho sebagai File yang disimpan di area ini dienkripsi menggunakan ROT13 di direktori asli. <br>
4. Black Rose Chiho untuk menyimpan data dalam format biner murni, tanpa enkripsi atau encoding tambahan.
5. Heaven Chiho untuk melindungi file dengan nkripsi AES-256-CBC.<br>
6. Skystreet Chiho untuk dikompresi menggunakan gzip untuk menghemat storage.<br>
7. 7sRef Chiho adalah area spesial yang dapat mengakses semua area lain melalui sistem penamaan khusus.



```c
static const char *CHIHO_BASE_DIR = "chiho"; 
static char REAL_CHIHO_BASE_PATH[PATH_MAX];
```

memiliki fungsi sebagai penyimpan informasi direktori dasar (base directory) dan path absolut dari direktori tersebut dalam sistem C, yang biasanya digunakan dalam program seperti FUSE filesystem atau program yang mengelola struktur direktori khusus.

```c
static const unsigned char g_aes_key[32] = "0123456789abcdef0123456789abcdef";
#define AES_BLOCK_SIZE 16
```

Kode tersebut berfungsi untuk Mendefinisikan kunci enkripsi/dekripsi berukuran 256-bit (32 byte) untuk digunakan dalam algoritma AES-256.

```c
typedef enum {
    TYPE_STARTER,
    TYPE_METRO,
    TYPE_DRAGON,
    TYPE_BLACKROSE,
    TYPE_HEAVEN,
    TYPE_YOUTH,
    TYPE_7SREF,
    TYPE_ROOT,
    TYPE_UNKNOWN
} ChihoType;
```

Enumerasi yang mendefinisikan berbagai jenis "chiho" (area/direktori) dalam sistem universe maimai (seperti yang sedang kamu bangun dengan FUSE).

```c
ChihoType get_chiho_type_and_real_path(const char *path, char *real_path_buf, char *original_filename_buf) {
    char temp_path[PATH_MAX];
    strncpy(temp_path, path, PATH_MAX -1); // Salin path agar bisa dimodifikasi
    temp_path[PATH_MAX-1] = '\0';

    if (original_filename_buf) original_filename_buf[0] = '\0';

    if (strcmp(path, "/") == 0) {
        strncpy(real_path_buf, REAL_CHIHO_BASE_PATH, PATH_MAX -1);
        real_path_buf[PATH_MAX-1] = '\0';
        return TYPE_ROOT;
    }

    char *first_slash = strchr(temp_path + 1, '/');
    char chiho_name[256] = {0};
    char filename_component[PATH_MAX] = {0}; 

    if (first_slash) {
        size_t chiho_len_val = first_slash - (temp_path + 1);
        if (chiho_len_val >= sizeof(chiho_name)) {
            chiho_len_val = sizeof(chiho_name) - 1;
        }
        strncpy(chiho_name, temp_path + 1, chiho_len_val);
        chiho_name[chiho_len_val] = '\0';

        strncpy(filename_component, first_slash + 1, sizeof(filename_component) - 1);

        if (original_filename_buf) {
            strncpy(original_filename_buf, filename_component, PATH_MAX - 1); 
            original_filename_buf[PATH_MAX - 1] = '\0';
        }
    } else {
        strncpy(chiho_name, temp_path + 1, sizeof(chiho_name) -1);
    }

    ChihoType type = TYPE_UNKNOWN;
    size_t len_base = strlen(REAL_CHIHO_BASE_PATH);
    size_t len_filename_comp = strlen(filename_component);

    if (strcmp(chiho_name, "starter") == 0) {
        type = TYPE_STARTER;
        if (strlen(filename_component) > 0) {
            if (len_base + 1 + 7 + 1 + len_filename_comp + 4 + 1 > PATH_MAX) {
                real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/starter/%s.mai", REAL_CHIHO_BASE_PATH, filename_component);
            }
        } else {
            if (len_base + 1 + 7 + 1 > PATH_MAX) {
                 real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/starter", REAL_CHIHO_BASE_PATH);
            }
        }
    } else if (strcmp(chiho_name, "metro") == 0) {
        type = TYPE_METRO;
        if (strlen(filename_component) > 0) {
            if (len_base + 1 + 5 + 1 + len_filename_comp + 1 > PATH_MAX) {
                real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/metro/%s", REAL_CHIHO_BASE_PATH, filename_component);
            }
        } else {
             if (len_base + 1 + 5 + 1 > PATH_MAX) {
                 real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/metro", REAL_CHIHO_BASE_PATH);
            }
        }
    } else if (strcmp(chiho_name, "dragon") == 0) {
        type = TYPE_DRAGON;
         if (strlen(filename_component) > 0) {
            if (len_base + 1 + 6 + 1 + len_filename_comp + 1 > PATH_MAX) {
                real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/dragon/%s", REAL_CHIHO_BASE_PATH, filename_component);
            }
        } else {
            if (len_base + 1 + 6 + 1 > PATH_MAX) {
                 real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/dragon", REAL_CHIHO_BASE_PATH);
            }
        }
    } else if (strcmp(chiho_name, "blackrose") == 0) {
        type = TYPE_BLACKROSE;
        if (strlen(filename_component) > 0) {
            if (len_base + 1 + 9 + 1 + len_filename_comp + 1 > PATH_MAX) {
                real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/blackrose/%s", REAL_CHIHO_BASE_PATH, filename_component);
            }
        } else {
            if (len_base + 1 + 9 + 1 > PATH_MAX) {
                 real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/blackrose", REAL_CHIHO_BASE_PATH);
            }
        }
    } else if (strcmp(chiho_name, "heaven") == 0) {
        type = TYPE_HEAVEN;
        if (strlen(filename_component) > 0) {
            if (len_base + 1 + 6 + 1 + len_filename_comp + 1 > PATH_MAX) {
                real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/heaven/%s", REAL_CHIHO_BASE_PATH, filename_component);
            }
        } else {
            if (len_base + 1 + 6 + 1 > PATH_MAX) {
                 real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/heaven", REAL_CHIHO_BASE_PATH);
            }
        }
    } else if (strcmp(chiho_name, "youth") == 0) {
        type = TYPE_YOUTH;
        if (strlen(filename_component) > 0) {
            if (len_base + 1 + 5 + 1 + len_filename_comp + 1 > PATH_MAX) {
                real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/youth/%s", REAL_CHIHO_BASE_PATH, filename_component);
            }
        } else {
             if (len_base + 1 + 5 + 1 > PATH_MAX) {
                 real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
            } else {
                snprintf(real_path_buf, PATH_MAX, "%s/youth", REAL_CHIHO_BASE_PATH);
            }
        }
    } else if (strcmp(chiho_name, "7sref") == 0) {
        type = TYPE_7SREF;
        if (strlen(filename_component) > 0) {
             char *underscore = strchr(filename_component, '_');
             if (underscore) {
                char area_prefix[256]; 
                size_t area_prefix_len = underscore - filename_component;
                if (area_prefix_len >= sizeof(area_prefix)) area_prefix_len = sizeof(area_prefix) -1;
                strncpy(area_prefix, filename_component, area_prefix_len);
                area_prefix[area_prefix_len] = '\0';
                
                char actual_filename_comp[PATH_MAX] = {0};
                strncpy(actual_filename_comp, underscore + 1, sizeof(actual_filename_comp) - 1);

                if (original_filename_buf) {
                    strncpy(original_filename_buf, actual_filename_comp, PATH_MAX - 1);
                    original_filename_buf[PATH_MAX - 1] = '\0';
                }
                
                size_t len_actual_filename_comp = strlen(actual_filename_comp);

                if (strcmp(area_prefix, "starter") == 0) {
                    if (len_base + 1 + 7 + 1 + len_actual_filename_comp + 4 + 1 > PATH_MAX) { 
                        real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
                    } else {
                        snprintf(real_path_buf, PATH_MAX, "%s/starter/%s.mai", REAL_CHIHO_BASE_PATH, actual_filename_comp);
                    }
                } else if (strcmp(area_prefix, "metro") == 0) {
                     if (len_base + 1 + 5 + 1 + len_actual_filename_comp + 1 > PATH_MAX) { 
                        real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
                    } else {
                        snprintf(real_path_buf, PATH_MAX, "%s/metro/%s", REAL_CHIHO_BASE_PATH, actual_filename_comp);
                    }
                } else if (strcmp(area_prefix, "dragon") == 0) {
                     if (len_base + 1 + 6 + 1 + len_actual_filename_comp + 1 > PATH_MAX) { 
                        real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
                    } else {
                        snprintf(real_path_buf, PATH_MAX, "%s/dragon/%s", REAL_CHIHO_BASE_PATH, actual_filename_comp);
                    }
                } else if (strcmp(area_prefix, "blackrose") == 0) {
                    if (len_base + 1 + 9 + 1 + len_actual_filename_comp + 1 > PATH_MAX) { 
                        real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
                    } else {
                        snprintf(real_path_buf, PATH_MAX, "%s/blackrose/%s", REAL_CHIHO_BASE_PATH, actual_filename_comp);
                    }
                } else if (strcmp(area_prefix, "heaven") == 0) {
                     if (len_base + 1 + 6 + 1 + len_actual_filename_comp + 1 > PATH_MAX) { 
                        real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
                    } else {
                        snprintf(real_path_buf, PATH_MAX, "%s/heaven/%s", REAL_CHIHO_BASE_PATH, actual_filename_comp);
                    }
                } else if (strcmp(area_prefix, "youth") == 0) {
                     if (len_base + 1 + 5 + 1 + len_actual_filename_comp + 1 > PATH_MAX) { 
                        real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
                    } else {
                        snprintf(real_path_buf, PATH_MAX, "%s/youth/%s", REAL_CHIHO_BASE_PATH, actual_filename_comp);
                    }
                } else {
                    type = TYPE_UNKNOWN;
                    real_path_buf[0] = '\0';
                }
             } else {
                type = TYPE_UNKNOWN;
                real_path_buf[0] = '\0';
             }
        } else {
            strncpy(real_path_buf, REAL_CHIHO_BASE_PATH, PATH_MAX-1); 
            real_path_buf[PATH_MAX-1] = '\0';
        }
    } else {
        if (len_base + 1 + strlen(chiho_name) + 1 > PATH_MAX) {
            real_path_buf[0] = '\0'; type = TYPE_UNKNOWN;
        } else {
            snprintf(real_path_buf, PATH_MAX, "%s/%s", REAL_CHIHO_BASE_PATH, chiho_name);
        }
    }
    return type;
}
```

Kode tersebut berfungsi untuk mendeteksi tipe area(chiho) berdasarkan path pengguna, menghasilkan path sebenarnya, dan menyimpan nama file asli.

```c
void metro_transform(char *data, size_t len, int direction) { 
    for (size_t i = 0; i < len; i++) {
        data[i] = (unsigned char)data[i] + (direction * (i % 256));
    }
}
```

Kode tersebut berfungsi untuk melakukan transformasi (encoding atau decoding) pada sebuah buffer data.

```c
void rot13_transform(char *str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if ((str[i] >= 'A' && str[i] <= 'M') || (str[i] >= 'a' && str[i] <= 'm')) {
            str[i] += 13;
        } else if ((str[i] >= 'N' && str[i] <= 'Z') || (str[i] >= 'n' && str[i] <= 'z')) {
            str[i] -= 13;
        }
    }
}
```

Kode tersebut digunakan untuk melakukan ROT13 cipher pada string yang diberikan.

```c
int aes_crypt(const unsigned char *input, int input_len,
              unsigned char *output, int *output_len,
              int do_encrypt, unsigned char *iv_in_out) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len = 0; 
    int plaintext_len = 0;  

    if(!(ctx = EVP_CIPHER_CTX_new())) return -1; 

    if(1 != EVP_CipherInit_ex(ctx, EVP_aes_256_cbc(), NULL, g_aes_key, iv_in_out, do_encrypt)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1; 
    }

    if(1 != EVP_CipherUpdate(ctx, output, &len, input, input_len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1; 
    }
    if (do_encrypt) ciphertext_len = len; else plaintext_len = len;

    if(1 != EVP_CipherFinal_ex(ctx, output + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return -1; 
    }
    if (do_encrypt) ciphertext_len += len; else plaintext_len += len;

    *output_len = do_encrypt ? ciphertext_len : plaintext_len;

    EVP_CIPHER_CTX_free(ctx);
    return 0; 
}
```
Fungsi dari kode tersebut adalah implementasi enkripsi dan dekripsi AES-256-CBC menggunakan OpenSSL EVP API.

```c
int gzip_compress_data(const char *src, size_t src_len, char **dst, size_t *dst_len) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        return -1; 
    }
    zs.next_in = (Bytef*)src;
    zs.avail_in = src_len;

    size_t out_buf_size = deflateBound(&zs, src_len);
    *dst = (char*)malloc(out_buf_size);
    if (!*dst) {
        deflateEnd(&zs);
        return -ENOMEM;
    }

    zs.next_out = (Bytef*)*dst;
    zs.avail_out = out_buf_size;

    int ret = deflate(&zs, Z_FINISH);
    deflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        free(*dst);
        *dst = NULL;
        return -1; 
    }
    *dst_len = zs.total_out;
    return 0; 
}
```
Kode tersebut bertujuan untuk mengompresi data yang diberikan menggunakan format gzip dan menyimpan hasilnya di buffer yang dialokasikan secara dinamis.

```c
int gzip_decompress_data(const char *src, size_t src_len, char **dst, size_t *dst_len) {
    z_stream zs;
    memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, 15 + 16) != Z_OK) { 
        return -1; 
    }
    zs.next_in = (Bytef*)src;
    zs.avail_in = src_len;

    size_t out_buf_size_increment = src_len * 2 > 1024 ? src_len * 2 : 1024; 
    size_t current_buf_size = 0;
    *dst = NULL;
    *dst_len = 0;

    int ret;
    do {
        current_buf_size += out_buf_size_increment;
        char *new_dst = (char*)realloc(*dst, current_buf_size);
        if (!new_dst) {
            free(*dst);
            *dst = NULL;
            inflateEnd(&zs);
            return -ENOMEM;
        }
        *dst = new_dst;

        zs.next_out = (Bytef*)(*dst + *dst_len);
        zs.avail_out = current_buf_size - *dst_len;
        
        ret = inflate(&zs, Z_NO_FLUSH);
        *dst_len = zs.total_out;

        if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
            free(*dst);
            *dst = NULL;
            inflateEnd(&zs);
            return -1; 
        }
    } while (ret != Z_STREAM_END && zs.avail_out == 0); 

    inflateEnd(&zs);

    if (ret != Z_STREAM_END) {
        free(*dst);
        *dst = NULL;
        return -1; 
    }
    return 0; 
}
```

Kode tersebut berfungsi untuk mendekompresi data yang sudah dikompresi dengan format gzip dan menyimpan hasil dekompresi di buffer yang dialokasikan secara dinamis.

```c
static int maimai_getattr(const char *path, struct stat *stbuf) {
    char real_path[PATH_MAX];
    ChihoType type = get_chiho_type_and_real_path(path, real_path, NULL);

    memset(stbuf, 0, sizeof(struct stat));

    if (type == TYPE_UNKNOWN && strlen(real_path) == 0) { 
        return -ENOENT;
    }
    if (type == TYPE_ROOT || (type == TYPE_7SREF && strchr(path, '_') == NULL && strcmp(path, "/7sref") == 0 )) { 
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2; 
        if (type == TYPE_ROOT) {
             if (lstat(REAL_CHIHO_BASE_PATH, stbuf) == -1) return -errno;
        }
        return 0;
    }
    
    if (strchr(path + 1, '/') == NULL && type != TYPE_7SREF) { 
        if (lstat(real_path, stbuf) == -1) {
            return -errno;
        }
        return 0;
    }

    int res = lstat(real_path, stbuf);
    if (res == -1) return -errno;

    if (type == TYPE_HEAVEN && S_ISREG(stbuf->st_mode)) {
        if (stbuf->st_size >= AES_BLOCK_SIZE) { 
            stbuf->st_size -= AES_BLOCK_SIZE;
            if (stbuf->st_size < 0) stbuf->st_size = 0;
        } else {
            stbuf->st_size = 0; 
        }
    }
    return 0;
}
```

Fungsi dari kode tersebut adalah implementasi handler untuk operasi getattr di sebuah sistem filesystem (kemungkinan FUSE) yang mengembalikan atribut (stat) file atau direktori berdasarkan path virtual yang diberikan.

```c
static int maimai_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                         off_t offset, struct fuse_file_info *fi) { 
    (void) offset; 
    (void) fi; 

    char real_path[PATH_MAX];
    char original_filename[PATH_MAX]; 
    ChihoType type = get_chiho_type_and_real_path(path, real_path, original_filename);

    filler(buf, ".", NULL, 0); 
    filler(buf, "..", NULL, 0);

    if (type == TYPE_ROOT) {
        const char *chihos[] = {"starter", "metro", "dragon", "blackrose", "heaven", "youth", "7sref"};
        for (int i = 0; i < 7; i++) {
            filler(buf, chihos[i], NULL, 0); 
        }
        return 0;
    }
    
    if (type == TYPE_7SREF && (strcmp(path, "/7sref") == 0 || strcmp(path, "/7sref/") == 0)) {
        const char *source_chihos[] = {"starter", "metro", "dragon", "blackrose", "heaven", "youth"};
        const ChihoType source_types[] = {TYPE_STARTER, TYPE_METRO, TYPE_DRAGON, TYPE_BLACKROSE, TYPE_HEAVEN, TYPE_YOUTH};
        size_t len_base = strlen(REAL_CHIHO_BASE_PATH);

        for (int i = 0; i < 6; i++) {
            char source_chiho_path[PATH_MAX];
            size_t len_source_chiho_name = strlen(source_chihos[i]);
            if (len_base + 1 + len_source_chiho_name + 1 > PATH_MAX) {
                fprintf(stderr, "Warning: Path too long for constructing %s/%s in readdir for 7sref.\n", REAL_CHIHO_BASE_PATH, source_chihos[i]);
                continue; 
            }
            snprintf(source_chiho_path, PATH_MAX, "%s/%s", REAL_CHIHO_BASE_PATH, source_chihos[i]);
            
            DIR *dp = opendir(source_chiho_path);
            if (dp == NULL) continue; 

            struct dirent *de;
            while ((de = readdir(dp)) != NULL) {
                if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;

                char sref_name[PATH_MAX];
                char actual_name_val[NAME_MAX + 1]; 
                
                strncpy(actual_name_val, de->d_name, NAME_MAX);
                actual_name_val[NAME_MAX] = '\0';


                if (source_types[i] == TYPE_STARTER) {
                    char *dot_mai = strstr(actual_name_val, ".mai");
                    if (dot_mai && strlen(dot_mai) == 4) { 
                        *dot_mai = '\0';
                    } else {
                        continue; 
                    }
                }
                snprintf(sref_name, PATH_MAX, "%s_%s", source_chihos[i], actual_name_val);
                filler(buf, sref_name, NULL, 0); 
            }
            closedir(dp);
        }
        return 0;
    }

    DIR *dp = opendir(real_path); 
    if (dp == NULL) return -errno;

    struct dirent *de;
    while ((de = readdir(dp)) != NULL) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) continue;
        
        char name_to_fill[NAME_MAX + 1]; // Use NAME_MAX for safety
        strncpy(name_to_fill, de->d_name, NAME_MAX);
        name_to_fill[NAME_MAX] = '\0';


        if (type == TYPE_STARTER) {
            char *dot_mai = strstr(name_to_fill, ".mai");
            if (dot_mai && strlen(dot_mai) == 4) { 
                *dot_mai = '\0';
            } else {
                 continue; 
            }
        }
        filler(buf, name_to_fill, NULL, 0); 
    }
    closedir(dp);
    return 0;
}
```

Fungsi dari kode ini adalah implementasi handler untuk operasi readdir di filesystem berbasis FUSE, yang bertugas membaca isi direktori virtual dan mengisi daftar isi direktori yang diminta (path), lalu mengirimkan entri-entri tersebut ke kernel/klien melalui callback filler.

```c
static int maimai_open(const char *path, struct fuse_file_info *fi) {
    char real_path[PATH_MAX];
    ChihoType type = get_chiho_type_and_real_path(path, real_path, NULL);

    if (type == TYPE_UNKNOWN && strlen(real_path) == 0) return -ENOENT;
    if (type == TYPE_ROOT || (type == TYPE_7SREF && strchr(path, '_') == NULL)) return -EISDIR;
    if (real_path[0] == '\0') return -ENOENT;


    int res = open(real_path, fi->flags);
    if (res == -1) return -errno;
    close(res); 
    return 0;
}
```

Fungsi dari kode tersebut adalah implementasi handler untuk operasi open di FUSE filesystem yang kamu buat.

```c
static int maimai_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi) {
    (void)fi;
    char real_path[PATH_MAX];
    char original_filename[PATH_MAX]; 
    ChihoType type_of_fuse_path = get_chiho_type_and_real_path(path, real_path, original_filename);
    
    if (real_path[0] == '\0') return -ENOENT;

    ChihoType effective_type = type_of_fuse_path;
    if (type_of_fuse_path == TYPE_7SREF) {
        if (strstr(real_path, "/starter/")) effective_type = TYPE_STARTER;
        else if (strstr(real_path, "/metro/")) effective_type = TYPE_METRO;
        else if (strstr(real_path, "/dragon/")) effective_type = TYPE_DRAGON;
        else if (strstr(real_path, "/blackrose/")) effective_type = TYPE_BLACKROSE;
        else if (strstr(real_path, "/heaven/")) effective_type = TYPE_HEAVEN;
        else if (strstr(real_path, "/youth/")) effective_type = TYPE_YOUTH;
        else return -ENOENT; 
    }

    if (effective_type == TYPE_UNKNOWN || (type_of_fuse_path == TYPE_UNKNOWN && strlen(real_path) == 0)) return -ENOENT;
    if (effective_type == TYPE_ROOT) return -EISDIR;

    int fd = open(real_path, O_RDONLY);
    if (fd == -1) return -errno;

    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return -errno;
    }
    size_t file_size = st.st_size;
    if (file_size == 0 && effective_type != TYPE_HEAVEN) {
         close(fd);
         return 0; 
    }

    char *file_content = (char *)malloc(file_size + 1); 
    if (!file_content) {
        close(fd);
        return -ENOMEM;
    }

    ssize_t read_bytes = pread(fd, file_content, file_size, 0); 
    close(fd);

    if (read_bytes == -1) {
        free(file_content);
        return -errno;
    }
    file_content[read_bytes] = '\0'; 

    char *processed_content = NULL;
    size_t processed_size = 0;
    int res_transform = 0;

    switch (effective_type) {
        case TYPE_STARTER: 
        case TYPE_BLACKROSE:
            processed_content = file_content; 
            processed_size = read_bytes;
            file_content = NULL; 
            break;
        case TYPE_METRO:
            processed_content = file_content; 
            metro_transform(processed_content, read_bytes, -1); 
            processed_size = read_bytes;
            file_content = NULL;
            break;
        case TYPE_DRAGON:
            processed_content = file_content; 
            rot13_transform(processed_content, read_bytes); 
            processed_size = read_bytes;
            file_content = NULL;
            break;
        case TYPE_HEAVEN:
            if (read_bytes < AES_BLOCK_SIZE) { 
                free(file_content);
                return 0; 
            }
            unsigned char iv[AES_BLOCK_SIZE];
            memcpy(iv, file_content, AES_BLOCK_SIZE);
            
            processed_content = (char*)malloc(read_bytes); 
            if (!processed_content) {
                free(file_content);
                return -ENOMEM;
            }
            int decrypted_len;
            res_transform = aes_crypt((unsigned char*)file_content + AES_BLOCK_SIZE, read_bytes - AES_BLOCK_SIZE,
                            (unsigned char*)processed_content, &decrypted_len, 0, iv);
            free(file_content);
            if (res_transform != 0) {
                free(processed_content);
                return -EIO; 
            }
            processed_size = decrypted_len;
            break;
        case TYPE_YOUTH:
            res_transform = gzip_decompress_data(file_content, read_bytes, &processed_content, &processed_size);
            free(file_content);
            if (res_transform != 0) {
                if (processed_content) free(processed_content);
                return -EIO; 
            }
            break;
        default: 
            free(file_content);
            return -EIO;
    }

    if (offset < processed_size) {
        if (offset + size > processed_size) {
            size = processed_size - offset;
        }
        memcpy(buf, processed_content + offset, size);
    } else {
        size = 0;
    }

    if (processed_content) free(processed_content);
    return size;
}
```

Fungsi dari kode ini adalah implementasi handler untuk operasi membaca file pada filesystem FUSE yang kamu buat. Fungsi ini meng-handle pembacaan isi file dengan perlakuan khusus sesuai area "chiho"-nya.

```c
static int maimai_write(const char *path, const char *buf, size_t size,
                       off_t offset, struct fuse_file_info *fi) {
    (void)fi;
    char real_path[PATH_MAX];
    char original_filename[PATH_MAX];
    ChihoType type_of_fuse_path = get_chiho_type_and_real_path(path, real_path, original_filename);

    if (real_path[0] == '\0') return -ENOENT; 

    ChihoType effective_type = type_of_fuse_path;
     if (type_of_fuse_path == TYPE_7SREF) {
        if (strstr(real_path, "/starter/")) effective_type = TYPE_STARTER;
        else if (strstr(real_path, "/metro/")) effective_type = TYPE_METRO;
        else if (strstr(real_path, "/dragon/")) effective_type = TYPE_DRAGON;
        else if (strstr(real_path, "/blackrose/")) effective_type = TYPE_BLACKROSE;
        else if (strstr(real_path, "/heaven/")) effective_type = TYPE_HEAVEN;
        else if (strstr(real_path, "/youth/")) effective_type = TYPE_YOUTH;
        else return -ENOENT;
    }

    if (effective_type == TYPE_UNKNOWN || (type_of_fuse_path == TYPE_UNKNOWN && strlen(real_path) == 0)) return -ENOENT;
    if (effective_type == TYPE_ROOT) return -EISDIR;

    int fd_write; 
    ssize_t res_write; 
    char *data_to_write = NULL;
    size_t data_to_write_len = 0;

    char *mutable_buf = (char*)malloc(size);
    if (!mutable_buf) return -ENOMEM;
    memcpy(mutable_buf, buf, size);

    switch (effective_type) {
        case TYPE_STARTER: 
        case TYPE_BLACKROSE:
            data_to_write = mutable_buf; 
            data_to_write_len = size;
            mutable_buf = NULL; 
            break;
        case TYPE_METRO:
            metro_transform(mutable_buf, size, 1); 
            data_to_write = mutable_buf;
            data_to_write_len = size;
            mutable_buf = NULL;
            break;
        case TYPE_DRAGON:
            rot13_transform(mutable_buf, size); 
            data_to_write = mutable_buf;
            data_to_write_len = size;
            mutable_buf = NULL;
            break;
        case TYPE_HEAVEN: {
            unsigned char iv[AES_BLOCK_SIZE];
            if (!RAND_bytes(iv, sizeof(iv))) { 
                free(mutable_buf);
                return -EIO; 
            }
            data_to_write = (char*)malloc(AES_BLOCK_SIZE + size + AES_BLOCK_SIZE); 
            if (!data_to_write) {
                free(mutable_buf);
                return -ENOMEM;
            }
            memcpy(data_to_write, iv, AES_BLOCK_SIZE); 

            int encrypted_len;
            if (aes_crypt((unsigned char*)mutable_buf, size,
                          (unsigned char*)data_to_write + AES_BLOCK_SIZE, &encrypted_len, 1, iv) != 0) {
                free(mutable_buf);
                free(data_to_write);
                return -EIO; 
            }
            data_to_write_len = AES_BLOCK_SIZE + encrypted_len;
            free(mutable_buf);
            mutable_buf = NULL;
            break;
        }
        case TYPE_YOUTH:
            if (gzip_compress_data(mutable_buf, size, &data_to_write, &data_to_write_len) != 0) {
                free(mutable_buf);
                if(data_to_write) free(data_to_write);
                return -EIO; 
            }
            free(mutable_buf);
            mutable_buf = NULL;
            break;
        default:
            free(mutable_buf);
            return -EIO;
    }

    if (mutable_buf) free(mutable_buf); 

    if (offset == 0 || effective_type == TYPE_STARTER || effective_type == TYPE_BLACKROSE) {
         fd_write = open(real_path, O_WRONLY | O_CREAT | (offset == 0 ? O_TRUNC : 0), 0644);
         if (fd_write == -1) {
            if (data_to_write) free(data_to_write);
            return -errno;
         }
         res_write = pwrite(fd_write, data_to_write, data_to_write_len, offset);
         if (res_write == -1) res_write = -errno;
         close(fd_write);
    } else {
        if (data_to_write) free(data_to_write);
        fprintf(stderr, "Write with offset != 0 not fully supported for transformed chihos in this simple example.\n");
        return -EIO; 
    }

    if (data_to_write) free(data_to_write);                                             
    return (res_write < 0) ? res_write : size;
}
```

Fungsi dari kode ini adalah implementasi handler menulis file (maimai_write) untuk filesystem FUSE-mu, yang menangani penulisan data ke file dengan perlakuan khusus berdasarkan tipe chiho (area) tempat file itu berada.

```c
static int maimai_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    char real_path[PATH_MAX];
    ChihoType type = get_chiho_type_and_real_path(path, real_path, NULL);

    if (real_path[0] == '\0') return -ENOENT; 

    if (type == TYPE_UNKNOWN && strlen(real_path) == 0) return -ENOENT;
    if (type == TYPE_ROOT) return -EPERM; 

    int fd_create = open(real_path, fi->flags | O_CREAT | O_TRUNC, mode); 
    if (fd_create == -1) return -errno;
    close(fd_create);
    return 0;
}
```

Fungsi dari kode ini adalah untuk Implementasi fungsi maimai_create yang kamu berikan sudah cukup baik dan ringkas untuk membuat file baru di path FUSE-mu dengan mode dan flag yang diberikan.

```c
static int maimai_unlink(const char *path) {
    char real_path[PATH_MAX];
    ChihoType type = get_chiho_type_and_real_path(path, real_path, NULL);

    if (real_path[0] == '\0') return -ENOENT; 

    if (type == TYPE_UNKNOWN && strlen(real_path) == 0) return -ENOENT;
    if (type == TYPE_ROOT) return -EPERM;

    int res_unlink = unlink(real_path); 
    if (res_unlink == -1) return -errno;

    return 0;
}
```

Kode tersebut berfungsi untuk menghapus file.

```c
static int maimai_release(const char *path, struct fuse_file_info *fi) {
    (void)path;
    (void)fi; 
    return 0;
}
```

Fungsi dari kode ini adalah implementasi standar kosong untuk FUSE yang artinya saat file ditutup (release), tidak ada aksi khusus yang perlu dilakukan. Ini sudah benar dan cukup untuk kebanyakan kasus, terutama jika kamu tidak menggunakan handle file khusus atau resource tambahan.


```c
static int maimai_truncate(const char *path, off_t size) { 
    char real_path[PATH_MAX];
    char original_filename[PATH_MAX];
    ChihoType type_of_fuse_path = get_chiho_type_and_real_path(path, real_path, original_filename);

    if (real_path[0] == '\0') return -EIO; 

    ChihoType effective_type = type_of_fuse_path;
    if (type_of_fuse_path == TYPE_7SREF) {
        if (strstr(real_path, "/starter/")) effective_type = TYPE_STARTER;
        else if (strstr(real_path, "/metro/")) effective_type = TYPE_METRO;
        else if (strstr(real_path, "/blackrose/")) effective_type = TYPE_BLACKROSE;
        else return -EIO; 
    }


    if (effective_type == TYPE_STARTER || effective_type == TYPE_BLACKROSE) {
        int res_truncate = truncate(real_path, size); 
        if (res_truncate == -1) return -errno;
        return 0;
    } else {
        fprintf(stderr, "Truncate on transformed files not supported in this simple example.\n");
        return -EPERM; 
    }
}
```

Kode terebut berfungsi untuk menangani operasi truncate (memotong atau mengubah ukuran file) pada sistem file virtual yang kamu buat dengan FUSE, khususnya dalam konteks universe maimai yang kamu kelola.

```c
static struct fuse_operations maimai_oper = {
    .getattr    = maimai_getattr,   
    .readdir    = maimai_readdir,   
    .open       = maimai_open,
    .read       = maimai_read,
    .write      = maimai_write,
    .create     = maimai_create,
    .unlink     = maimai_unlink,
    .release    = maimai_release, 
    .truncate   = maimai_truncate,  
};
```

Kode tersebut berfungsi untuk mendefinisikan fungsi-fungsi callback yang akan dipanggil oleh FUSE (Filesystem in Userspace) ketika user atau sistem melakukan operasi tertentu pada filesystem virtual kamu.

```c
int main(int argc, char *argv[]) {
    if (realpath(CHIHO_BASE_DIR, REAL_CHIHO_BASE_PATH) == NULL) {
        perror("Failed to get real path for chiho base directory");
        fprintf(stderr, "Please ensure the '%s' directory exists in the current path or provide an absolute path.\n", CHIHO_BASE_DIR);
        return 1;
    }
    fprintf(stdout, "Using Chiho Base Path: %s\n", REAL_CHIHO_BASE_PATH);

    const char *chihos_to_create[] = {"starter", "metro", "dragon", "blackrose", "heaven", "youth"};
    size_t len_base = strlen(REAL_CHIHO_BASE_PATH);

    for (int i = 0; i < 6; i++) {
        char subdir_path[PATH_MAX];
        size_t len_chiho_name = strlen(chihos_to_create[i]);
        if (len_base + 1 + len_chiho_name + 1 > PATH_MAX) {
            fprintf(stderr, "Error: Path too long to create directory %s/%s. Skipping.\n", REAL_CHIHO_BASE_PATH, chihos_to_create[i]);
            continue;
        }
        snprintf(subdir_path, PATH_MAX, "%s/%s", REAL_CHIHO_BASE_PATH, chihos_to_create[i]);
        mkdir(subdir_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH); 
    }

    umask(0); 
    return fuse_main(argc, argv, &maimai_oper, NULL);
}
```

Kode tersebut berfungsi sebagai entry point program FUSE filesystem yang kamu buat untuk universe "maimai".
