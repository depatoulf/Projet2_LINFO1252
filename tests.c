#include "lib_tar.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int test_count = 0;
int test_passed = 0;

void print_test_result(const char *test_name, const char *expected, const char *actual, int passed) {
    test_count++;
    if (passed) {
        printf("PASS" "- %s\n", test_name);
        test_passed++;
    } else {
        printf("FAIL" "- %s\n", test_name);
    }
    printf("expected: %s\nactual: %s\n\n", expected, actual);
}

// Fonction helper pour créer une archive de test simple
int create_test_archive(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }    
    
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
    write(fd, "test", 4);
    
    // Padding
    char pad[501] = {0};
    write(fd, pad, 501);
    
    // EOF blocks
    char zeros[1024] = {0};
    write(fd, zeros, 1024);
    
    close(fd);
    return 0;
}

// Fonction helper pour créer une archive avec répertoires
int create_archive_with_dirs(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }    
    
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
    
    // Ajouter "file.txt" à la racine
    memset(&header, 0, sizeof(tar_header_t));
    strcpy(header.name, "file.txt");
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
    char zeros[1024] = {0};
    write(fd, zeros, 1024);
    
    close(fd);
    return 0;
}

// Créer une archive vide (seulement EOF)
int create_empty_archive(const char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }
    
    char zeros[1024] = {0};
    write(fd, zeros, 1024);
    
    close(fd);
    return 0;
}

// TESTS

void test_check_archive_valid() {
    create_test_archive("test_valid.tar");
    int fd = open("test_valid.tar", O_RDONLY);
    int result = check_archive(fd);
    close(fd);
    unlink("test_valid.tar");
    
    char expected[64]; 
    char actual[64];
    snprintf(expected, sizeof(expected), "%d", 1);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("check_archive (archive valide)", expected, actual, result == 1);
}

void test_check_archive_empty() {
    create_empty_archive("test_empty.tar");
    int fd = open("test_empty.tar", O_RDONLY);
    int result = check_archive(fd);
    close(fd);
    unlink("test_empty.tar");

    char expected[64]; 
    char actual[64];
    snprintf(expected, sizeof(expected), "%d", 0);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("check_archive (archive vide)", expected, actual, result == 0);
}

void test_exists_file() {
    create_test_archive("test_exists.tar");
    int fd = open("test_exists.tar", O_RDONLY);
    int result = exists(fd, "test.txt");
    close(fd);
    unlink("test_exists.tar");

    char expected[64]; 
    char actual[64];
    snprintf(expected, sizeof(expected), "%d", 1);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("exists (test.txt)", expected, actual, result == 1);
}

void test_exists_not_found() {
    create_test_archive("test_not_exists.tar");
    int fd = open("test_not_exists.tar", O_RDONLY);
    int result = exists(fd, "notfound.txt");
    close(fd);
    unlink("test_not_exists.tar");

    char expected[64]; 
    char actual[64];
    snprintf(expected, sizeof(expected), "%d", 0);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("exists (notfound.txt)", expected, actual, result == 0);
}

void test_is_file() {
    create_test_archive("test_isfile.tar");
    int fd = open("test_isfile.tar", O_RDONLY);
    int result = is_file(fd, "test.txt");
    close(fd);
    unlink("test_isfile.tar");

    char expected[64]; 
    char actual[64];
    snprintf(expected, sizeof(expected), "%d", 1);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("is_file (fichier)", expected, actual, result != 0);
}

void test_is_dir() {
    create_archive_with_dirs("test_isdir.tar");
    int fd = open("test_isdir.tar", O_RDONLY);
    int result = is_dir(fd, "dir/");
    close(fd);
    unlink("test_isdir.tar");

    char expected[64]; 
    char actual[64];
    snprintf(expected, sizeof(expected), "%d", 1);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("is_dir (répertoire)", expected, actual, result != 0);
}

void test_is_dir_on_file() {
    create_test_archive("test_isdir_file.tar");
    int fd = open("test_isdir_file.tar", O_RDONLY);
    int result = is_dir(fd, "test.txt");
    close(fd);
    unlink("test_isdir_file.tar");

    char expected[64]; 
    char actual[128];
    snprintf(expected, sizeof(expected), "%d", 0);
    snprintf(actual, sizeof(actual), "%d", result);
    print_test_result("is_dir (fichier)", expected, actual, result == 0);
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
    
    printf("%zu entrées:" "\n", no_entries);
    for (size_t i = 0; i < no_entries; i++) {
        printf("%s\n", entries[i]);
    }
    
    // On devrait avoir "dir/" et "file.txt" à la racine
    int passed = (result == 1 && no_entries == 2);
    
    for (int i = 0; i < 10; i++) {
        free(entries[i]);
    }
    free(entries);
    close(fd);
    unlink("test_list_root.tar");

    char expected[128]; 
    char actual[128];
    snprintf(expected, sizeof(expected), "return = 1, no_entries = 2");
    snprintf(actual, sizeof(actual), "return = %d, no_entries = %zu", result, no_entries);
    print_test_result("list (root)", expected, actual, passed);
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
    
    printf("dir %zu entrées:" "\n", no_entries);
    for (size_t i = 0; i < no_entries; i++) {
        printf("%s\n", entries[i]);
    }
    
    // On devrait avoir "dir/file1.txt", "dir/file2.txt", "dir/subdir/"
    int passed = (result == 1 && no_entries == 3);
    
    for (int i = 0; i < 10; i++) {
        free(entries[i]);
    }
    free(entries);
    close(fd);
    unlink("test_list_dir.tar");
    
    char expected[128]; 
    char actual[128];
    snprintf(expected, sizeof(expected), "return = 1, no_entries = 3");
    snprintf(actual, sizeof(actual), "return = %d, no_entries = %zu", result, no_entries);
    print_test_result("list (répertoire)", expected, actual, passed);
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

    char expected[128]; 
    char actual[128];
    snprintf(expected, sizeof(expected), "return = 1, no_entries = 0");
    snprintf(actual, sizeof(actual), "return = %d, no_entries = %zu", result, no_entries);
    print_test_result("list (archive vide)", expected, actual, passed);
}

void test_add_file() {
    create_empty_archive("test_add.tar");
    int fd = open("test_add.tar", O_RDWR);
    
    uint8_t content[] = "test";
    int result = add_file(fd, "newfile.txt", content, sizeof(content) - 1);
        
    // Vérifier que le fichier existe maintenant
    lseek(fd, 0, SEEK_SET);
    int exists_result = exists(fd, "newfile.txt");
        
    close(fd);
    unlink("test_add.tar");

    char expected[128]; 
    char actual[128];
    snprintf(expected, sizeof(expected), "result = 0, exists = 1");
    snprintf(actual, sizeof(actual), "result = %d, exists = %d", result, exists_result);
    print_test_result("add_file", expected, actual, result == 0 && exists_result == 1);
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

    char expected[128]; 
    char actual[128];
    snprintf(expected, sizeof(expected), "result = 0, exists = 1");
    snprintf(actual, sizeof(actual), "result = %d, exists = %d", result, exists_result);
    print_test_result("add_file (grand fichier)", expected, actual, result == 0 && exists_result == 1);
}

// MAIN 

int main() {

    printf("Tests check_archive\n");
    test_check_archive_valid();
    test_check_archive_empty();
    
    printf("\nTests exists\n");
    test_exists_file();
    test_exists_not_found();
    
    printf("\nTests is_file / is_dir\n");
    test_is_file();
    test_is_dir();
    test_is_dir_on_file();
    
    printf("\nTests list\n");
    test_list_root();
    test_list_directory();
    test_list_empty_archive();
    
    printf("\nTests add_file\n");
    test_add_file();
    test_add_file_large();
    
    printf("Résultat: %d/%d \n", test_passed, test_count);
}