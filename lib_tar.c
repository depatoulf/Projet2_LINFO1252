#include "lib_tar.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd) {
    if (tar_fd < 0) {
        fprintf(stderr, "Description de fichier invalide\n");
        return -4;
    }
    if(lseek(tar_fd, 0, SEEK_SET) < 0) {
        fprintf(stderr, "lseek\n");
        return -4;
    }

    tar_header_t header;
    int header_count = 0;
    ssize_t bytes_read;
    bytes_read = read(tar_fd, &header, 512);
    if (bytes_read != 512) {
        fprintf(stderr, "read\n");
        return -4;
    }

    while (bytes_read == 512){
        if (isEOFBlock(&header) == 0){
            retrun header_count;
        }  
        
        //magic verification
        if (strncmp(header.magic, TMAGIC, 5) != 0 || header.magic[5] != '\0') {
            return -1;
        }
        // version verification
        if (strncmp(header.version, TVERSION, 2) != 0) {
            return -2;
        }
        // checksum verification
        unsigned int expected = TAR_INT(header.chksum);
        unsigned int actual = calculate_checksum(&header);
        if (expected != actual) {
            return -3;
        }
        // continue to next header
        size_t file_size = TAR_INT(header.size);
        size_t blocks_to_skip = (file_size + 511) / 512; 
        if (lseek(tar_fd, blocks_to_skip * 512, SEEK_CUR) < 0) {
            fprintf(stderr, "lseek\n");
            return -4;
        }
        bytes_read = read(tar_fd, &header, 512);
        header_count++;

    }
    return header_count;
}
/**
 * Checks whether an entry exists in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path) {
    return find_header(tar_fd, path, NULL);
}

/**
 * Checks whether an entry exists in the archive and is a directory.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path) {
    tar_header_t out;
    if (find_header(tar_fd, path, &out) <= 0) {
        return 0;
    }
    return out.typeflag == DIR_TYPE;
}

/**
 * Checks whether an entry exists in the archive and is a file.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path) {
    tar_header_t out;
    if (find_header(tar_fd, path, &out) <= 0) {
        return 0;
    }
    return out.typeflag == REG_TYPE || out.typeflag == AREG_TYPE;
}

/**
 * Checks whether an entry exists in the archive and is a symlink.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path) {
    tar_header_t out;
    if (find_header(tar_fd, path, &out) <= 0) {
        return 0;
    }
    return out.typeflag == SYM_TYPE;
}

/**
 * Lists the entries at a given path in the archive.
 * list() does *not* recurse into the directories listed at the given path.
 * If the path is NULL, it lists the entries at the root of the archive.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         1 in case of success,
 *         -1 in case of error.
 */
int list(int tar_fd, char *path, char **entries, size_t *no_entries) {
    // TODO
    return 0;
}

/**
 * Adds a file at the end of the archive, at the archive's root level.
 * The archive's metadata must be updated accordingly.
 * For the file header, only the name, size, typeflag, magic value (to "ustar"), version value (to "00") and checksum fields need to be correctly set.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param filename The name of the file to add. If an entry already exists with the same name, the file is not written, and the function returns -1.
 * @param src A source buffer containing the file content to add.
 * @param len The length of the source buffer.
 *
 * @return 0 if the file was added successfully,
 *         -1 if the archive already contains an entry at the given path,
 *         -2 if an error occurred
 */
int add_file(int tar_fd, char *filename, uint8_t *src, size_t len) {
    // TODO
    return 0;
}

// helper functions

int isEOFBlock(tar_header_t *header) {
    for (int i = 0; i < 512; i++) {
        if (((unsigned char *) header)[i] != 0) {
            return 1;
        }
    }
    return 0;
}

int calculate_checksum(tar_header_t *header) {
    char saved [8];
    memcpy(saved, header->chksum, 8);
    memset(header->chksum, ' ', 8);

    unsigned int sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)header)[i];
    }
    memcpy(header->chksum, saved, 8);

    return sum;
}

int find_header(int tar_fd, char *path, tar_header_t *out) {
    if (lseek(tar_fd, 0, SEEK_SET) < 0) {
        fprintf(stderr, "lseek\n");
        return -1;
    }

    tar_header_t header;
    int header_count = 0;
    ssize_t bytes_read;
    bytes_read = read(tar_fd, &header, 512);
    if (bytes_read != 512) {
        fprintf(stderr, "read\n");
        return -1;
    }

    while (bytes_read == 512){
        if (isEOFBlock(&header) == 0){
            return 0;
        }  
        if (strcmp(header.name, path) == 0) {
            memcpy(out, &header, sizeof(tar_header_t));
            return 1;
        }
        // continue to next header
        size_t file_size = TAR_INT(header.size);
        size_t blocks_to_skip = (file_size + 511) / 512; 
        if (lseek(tar_fd, blocks_to_skip * 512, SEEK_CUR) < 0) {
            fprintf(stderr, "lseek\n");
            return -1;
        }
        bytes_read = read(tar_fd, &header, 512);
        header_count++;

    }
    return 0;
}
