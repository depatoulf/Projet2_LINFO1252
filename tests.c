#include "lib_tar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

// Couleurs pour l'affichage
#define GREEN "\033[0;32m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

int test_count = 0;
int test_passed = 0;

void print_test_result(const char *test_name, int passed) {
    test_count++;
    if (passed) {
        printf(GREEN "✓ PASS" RESET " - %s\n", test_name);
        test_passed++;
    } else {
        printf(RED "✗ FAIL" RESET " - %s\n", test_name);
    }
}

// Fonction helper pour créer une archive de test simple
int create_test_archive(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    
    tar_header_t header;
    memset(&header, 0, sizeof(tar_header_t));
    
    // Ajouter un fichier simple "test.txt"
    strcpy(header.name, "test.txt");
    snprintf(header.size, sizeof(header.size), "%011o", 11);
    header.typeflag = REGTYPE;
    memcpy(header.magic, TMAGIC, 6);
    memcpy(header.version, TVERSION, 2);
    
    // Calculer checksum
    memset(header.chksum, ' ', 8);
    unsigned int sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    
    write(fd, &header, 512);
    write(fd, "hello world", 11);
    
    // Padding
    char pad[501] = {0};
    write(fd, pad, 501);
    
    // EOF blocks
    char eof[1024] = {0};
    write(fd, eof, 1024);
    
    close(fd);
    return 0;
}

// Fonction helper pour créer une archive avec répertoires
int create_archive_with_dirs(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    
    tar_header_t header;
    
    // Ajouter "dir/"
    memset(&header, 0, sizeof(tar_header_t));
    strcpy(header.name, "dir/");
    snprintf(header.size, sizeof(header.size), "%011o", 0);
    header.typeflag = DIRTYPE;
    memcpy(header.magic, TMAGIC, 6);
    memcpy(header.version, TVERSION, 2);
    memset(header.chksum, ' ', 8);
    unsigned int sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    write(fd, &header, 512);
    
    // Ajouter "dir/file1.txt"
    memset(&header, 0, sizeof(tar_header_t));
    strcpy(header.name, "dir/file1.txt");
    snprintf(header.size, sizeof(header.size), "%011o", 6);
    header.typeflag = REGTYPE;
    memcpy(header.magic, TMAGIC, 6);
    memcpy(header.version, TVERSION, 2);
    memset(header.chksum, ' ', 8);
    sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    write(fd, &header, 512);
    write(fd, "file1\n", 6);
    char pad[506] = {0};
    write(fd, pad, 506);
    
    // Ajouter "dir/file2.txt"
    memset(&header, 0, sizeof(tar_header_t));
    strcpy(header.name, "dir/file2.txt");
    snprintf(header.size, sizeof(header.size), "%011o", 6);
    header.typeflag = REGTYPE;
    memcpy(header.magic, TMAGIC, 6);
    memcpy(header.version, TVERSION, 2);
    memset(header.chksum, ' ', 8);
    sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    write(fd, &header, 512);
    write(fd, "file2\n", 6);
    write(fd, pad, 506);
    
    // Ajouter "dir/subdir/"
    memset(&header, 0, sizeof(tar_header_t));
    strcpy(header.name, "dir/subdir/");
    snprintf(header.size, sizeof(header.size), "%011o", 0);
    header.typeflag = DIRTYPE;
    memcpy(header.magic, TMAGIC, 6);
    memcpy(header.version, TVERSION, 2);
    memset(header.chksum, ' ', 8);
    sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    write(fd, &header, 512);
    
    // Ajouter "file_root.txt" à la racine
    memset(&header, 0, sizeof(tar_header_t));
    strcpy(header.name, "file_root.txt");
    snprintf(header.size, sizeof(header.size), "%011o", 5);
    header.typeflag = REGTYPE;
    memcpy(header.magic, TMAGIC, 6);
    memcpy(header.version, TVERSION, 2);
    memset(header.chksum, ' ', 8);
    sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += ((unsigned char *)&header)[i];
    }
    snprintf(header.chksum, sizeof(header.chksum), "%06o", sum);
    header.chksum[6] = '\0';
    header.chksum[7] = ' ';
    write(fd, &header, 512);
    write(fd, "root\n", 5);
    char pad2[507] = {0};
    write(fd, pad2, 507);
    
    // EOF blocks
    char eof[1024] = {0};
    write(fd, eof, 1024);
    
    close(fd);
    return 0;
}

// Créer une archive vide (seulement EOF)
int create_empty_archive(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    
    char eof[1024] = {0};
    write(fd, eof, 1024);
    
    close(fd);
    return 0;
}

// ========== TESTS ==========

void test_check_archive_valid() {
    create_test_archive("test_valid.tar");
    int fd = open("test_valid.tar", O_RDONLY);
    int result = check_archive(fd);
    close(fd);
    unlink("test_valid.tar");
    
    print_test_result("check_archive - archive valide", result == 1);
}

void test_check_archive_empty() {
    create_empty_archive("test_empty.tar");
    int fd = open("test_empty.tar", O_RDONLY);
    int result = check_archive(fd);
    close(fd);
    unlink("test_empty.tar");
    
    print_test_result("check_archive - archive vide", result == 0);
}

void test_exists_file() {
    create_test_archive("test_exists.tar");
    int fd = open("test_exists.tar", O_RDONLY);
    int result = exists(fd, "test.txt");
    close(fd);
    unlink("test_exists.tar");
    
    print_test_result("exists - fichier existant", result == 1);
}

void test_exists_not_found() {
    create_test_archive("test_not_exists.tar");
    int fd = open("test_not_exists.tar", O_RDONLY);
    int result = exists(fd, "notfound.txt");
    close(fd);
    unlink("test_not_exists.tar");
    
    print_test_result("exists - fichier non existant", result == 0);
}

void test_is_file() {
    create_test_archive("test_isfile.tar");
    int fd = open("test_isfile.tar", O_RDONLY);
    int result = is_file(fd, "test.txt");
    close(fd);
    unlink("test_isfile.tar");
    
    print_test_result("is_file - fichier régulier", result != 0);
}

void test_is_dir() {
    create_archive_with_dirs("test_isdir.tar");
    int fd = open("test_isdir.tar", O_RDONLY);
    int result = is_dir(fd, "dir/");
    close(fd);
    unlink("test_isdir.tar");
    
    print_test_result("is_dir - répertoire", result != 0);
}

void test_is_dir_on_file() {
    create_test_archive("test_isdir_file.tar");
    int fd = open("test_isdir_file.tar", O_RDONLY);
    int result = is_dir(fd, "test.txt");
    close(fd);
    unlink("test_isdir_file.tar");
    
    print_test_result("is_dir - fichier (pas un dir)", result == 0);
}

void test_list_root() {
    create_archive_with_dirs("test_list_root.tar");
    int fd = open("test_list_root.tar", O_RDONLY);
    
    char **entries = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++) {
        entries[i] = malloc(256);
    }
    size_t no_entries = 10;
    
    int result = list(fd, NULL, entries, &no_entries);
    
    printf(YELLOW "  Liste racine (%zu entrées):" RESET "\n", no_entries);
    for (size_t i = 0; i < no_entries; i++) {
        printf("    - %s\n", entries[i]);
    }
    
    // On devrait avoir "dir/" et "file_root.txt" à la racine
    int passed = (result == 1 && no_entries == 2);
    
    for (int i = 0; i < 10; i++) {
        free(entries[i]);
    }
    free(entries);
    close(fd);
    unlink("test_list_root.tar");
    
    print_test_result("list - racine de l'archive", passed);
}

void test_list_directory() {
    create_archive_with_dirs("test_list_dir.tar");
    int fd = open("test_list_dir.tar", O_RDONLY);
    
    char **entries = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++) {
        entries[i] = malloc(256);
    }
    size_t no_entries = 10;
    
    int result = list(fd, "dir/", entries, &no_entries);
    
    printf(YELLOW "  Liste dir/ (%zu entrées):" RESET "\n", no_entries);
    for (size_t i = 0; i < no_entries; i++) {
        printf("    - %s\n", entries[i]);
    }
    
    // On devrait avoir "dir/file1.txt", "dir/file2.txt", "dir/subdir/"
    int passed = (result == 1 && no_entries == 3);
    
    for (int i = 0; i < 10; i++) {
        free(entries[i]);
    }
    free(entries);
    close(fd);
    unlink("test_list_dir.tar");
    
    print_test_result("list - contenu d'un répertoire", passed);
}

void test_list_empty_archive() {
    create_empty_archive("test_list_empty.tar");
    int fd = open("test_list_empty.tar", O_RDONLY);
    
    char **entries = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++) {
        entries[i] = malloc(256);
    }
    size_t no_entries = 10;
    
    int result = list(fd, NULL, entries, &no_entries);
    
    int passed = (result == 1 && no_entries == 0);
    
    for (int i = 0; i < 10; i++) {
        free(entries[i]);
    }
    free(entries);
    close(fd);
    unlink("test_list_empty.tar");
    
    print_test_result("list - archive vide", passed);
}

void test_add_file() {
    create_empty_archive("test_add.tar");
    int fd = open("test_add.tar", O_RDWR);
    
    uint8_t content[] = "Hello TAR!";
    int result = add_file(fd, "newfile.txt", content, sizeof(content) - 1);
    
    printf(YELLOW "  add_file result: %d\n" RESET, result);
    
    // Vérifier que le fichier existe maintenant
    lseek(fd, 0, SEEK_SET);
    int exists_result = exists(fd, "newfile.txt");
    
    printf(YELLOW "  exists result: %d\n" RESET, exists_result);
    
    close(fd);
    unlink("test_add.tar");
    
    print_test_result("add_file - ajouter un nouveau fichier", 
                      result == 0 && exists_result == 1);
}

void test_add_file_duplicate() {
    create_test_archive("test_add_dup.tar");
    int fd = open("test_add_dup.tar", O_RDWR);
    
    uint8_t content[] = "Duplicate";
    int result = add_file(fd, "test.txt", content, sizeof(content) - 1);
    
    close(fd);
    unlink("test_add_dup.tar");
    
    print_test_result("add_file - fichier déjà existant", result == -1);
}

void test_add_file_large() {
    create_empty_archive("test_add_large.tar");
    int fd = open("test_add_large.tar", O_RDWR);
    
    // Créer un fichier de 1000 octets
    uint8_t *content = malloc(1000);
    for (int i = 0; i < 1000; i++) {
        content[i] = 'A' + (i % 26);
    }
    
    int result = add_file(fd, "largefile.txt", content, 1000);
    
    lseek(fd, 0, SEEK_SET);
    int exists_result = exists(fd, "largefile.txt");
    
    free(content);
    close(fd);
    unlink("test_add_large.tar");
    
    print_test_result("add_file - fichier volumineux", 
                      result == 0 && exists_result == 1);
}

// ========== MAIN ==========

int main() {
    printf("\n" YELLOW "========================================\n");
    printf("  Suite de tests pour lib_tar\n");
    printf("========================================\n" RESET "\n");
    
    printf(YELLOW "--- Tests check_archive ---\n" RESET);
    test_check_archive_valid();
    test_check_archive_empty();
    
    printf(YELLOW "\n--- Tests exists ---\n" RESET);
    test_exists_file();
    test_exists_not_found();
    
    printf(YELLOW "\n--- Tests is_file / is_dir ---\n" RESET);
    test_is_file();
    test_is_dir();
    test_is_dir_on_file();
    
    printf(YELLOW "\n--- Tests list ---\n" RESET);
    test_list_root();
    test_list_directory();
    test_list_empty_archive();
    
    printf(YELLOW "\n--- Tests add_file ---\n" RESET);
    test_add_file();
    test_add_file_duplicate();
    test_add_file_large();
    
    printf("\n" YELLOW "========================================\n");
    printf("  Résultat: %d/%d tests réussis ", test_passed, test_count);
    if (test_passed == test_count) {
        printf(GREEN "✓\n" RESET);
    } else {
        printf(RED "✗\n" RESET);
    }
    printf("========================================\n" RESET "\n");
    
    return (test_passed == test_count) ? 0 : 1;
}