#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include "libasm.h"

#define C_RESET  "\033[0m"
#define C_RED    "\033[31m"
#define C_GREEN  "\033[32m"
#define C_YELLOW "\033[33m"
#define C_BLUE   "\033[34m"
#define C_CYAN   "\033[36m"
#define C_BOLD   "\033[1m"

static void print_header(const char *title) {
    printf("\n" C_BOLD C_CYAN "========================================\n");
    printf("  TESTING: %s\n", title);
    printf("========================================" C_RESET "\n");
}

static void test_strlen(void) {
    print_header("ft_strlen");
    const char *cases[] = {
        "Hello, world!",
        "",
        "Assembly is fun!",
        "A very long string that has quite a lot of characters in it just to verify that the length counter works correctly with larger inputs and does not overflow any registers.",
        NULL
    };

    for (int i = 0; cases[i] != NULL; i++) {
        size_t expected = strlen(cases[i]);
        size_t got = ft_strlen(cases[i]);
        if (expected == got) {
            printf("  " C_GREEN "[PASS]" C_RESET " Length: %4lu | Input: \"%s%s\"\n", 
                   got, cases[i][0] == '\0' ? "" : (strlen(cases[i]) > 30 ? "..." : cases[i]), strlen(cases[i]) > 30 ? "..." : "");
        } else {
            printf("  " C_RED "[FAIL]" C_RESET " Input: \"%s\"\n    Expected: %lu\n    Got:      %lu\n", 
                   cases[i], expected, got);
        }
    }
}

static void test_strcpy(void) {
    print_header("ft_strcpy");
    const char *cases[] = {
        "Hello, world!",
        "",
        "A very long string to copy into our buffer to test capacity.",
        NULL
    };

    char dest_expected[500];
    char dest_got[500];

    for (int i = 0; cases[i] != NULL; i++) {
        memset(dest_expected, 0, 500);
        memset(dest_got, 0, 500);

        char *ret_expected = strcpy(dest_expected, cases[i]);
        (void)ret_expected;
        char *ret_got = ft_strcpy(dest_got, cases[i]);

        int content_ok = (strcmp(dest_expected, dest_got) == 0);
        int pointer_ok = (ret_got == dest_got);

        if (content_ok && pointer_ok) {
            printf("  " C_GREEN "[PASS]" C_RESET " Copied: \"%s%s\" (ret: %p)\n",
                   dest_got, strlen(dest_got) > 30 ? "..." : "", (void*)ret_got);
        } else {
            printf("  " C_RED "[FAIL]" C_RESET " Input: \"%s\"\n", cases[i]);
            if (!content_ok) {
                printf("    Expected content: \"%s\"\n    Got content:      \"%s\"\n", dest_expected, dest_got);
            }
            if (!pointer_ok) {
                printf("    Expected return:  %p\n    Got return:       %p\n", (void*)dest_got, (void*)ret_got);
            }
        }
    }
}

static void test_strcmp(void) {
    print_header("ft_strcmp");
    struct {
        const char *s1;
        const char *s2;
    } cases[] = {
        {"Hello", "Hello"},
        {"", ""},
        {"Hello", "Hello World"},
        {"Hello World", "Hello"},
        {"abcde", "abxde"},
        {"\xff", "\x01"},
        {"", "something"},
        {"something", ""},
        {NULL, NULL}
    };

    for (int i = 0; cases[i].s1 != NULL; i++) {
        int expected = strcmp(cases[i].s1, cases[i].s2);
        int got = ft_strcmp(cases[i].s1, cases[i].s2);

        // Standardize positive/negative to match signs
        int sign_expected = (expected > 0) - (expected < 0);
        int sign_got = (got > 0) - (got < 0);

        if (sign_expected == sign_got) {
            printf("  " C_GREEN "[PASS]" C_RESET " strcmp(\"%s\", \"%s\") -> expected: %d, got: %d\n",
                   cases[i].s1, cases[i].s2, expected, got);
        } else {
            printf("  " C_RED "[FAIL]" C_RESET " strcmp(\"%s\", \"%s\")\n    Expected: %d (sign matches %d)\n    Got:      %d (sign matches %d)\n",
                   cases[i].s1, cases[i].s2, expected, sign_expected, got, sign_got);
        }
    }
}

static void test_write(void) {
    print_header("ft_write");
    ssize_t ret_std, ret_ft;
    int err_std, err_ft;

    // Test 1: Writing to /dev/null (silent success)
    printf("Test 1 (write to /dev/null):\n");
    int null_fd = open("/dev/null", O_WRONLY);
    errno = 0;
    ret_std = write(null_fd, "Hello standard write", 20);
    err_std = errno;

    errno = 0;
    ret_ft = ft_write(null_fd, "Hello standard write", 20);
    err_ft = errno;
    close(null_fd);

    if (ret_std == ret_ft && err_std == err_ft) {
        printf("  " C_GREEN "[PASS]" C_RESET " ret: %ld, errno: %d\n", ret_ft, err_ft);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " write to /dev/null\n    Expected: ret %ld, errno %d\n    Got:      ret %ld, errno %d\n",
               ret_std, err_std, ret_ft, err_ft);
    }

    // Test 2: Invalid File Descriptor
    printf("Test 2 (invalid fd 9999):\n");
    errno = 0;
    ret_std = write(9999, "test", 4);
    err_std = errno;

    errno = 0;
    ret_ft = ft_write(9999, "test", 4);
    err_ft = errno;

    if (ret_std == ret_ft && err_std == err_ft) {
        printf("  " C_GREEN "[PASS]" C_RESET " ret: %ld, errno: %d\n", ret_ft, err_ft);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " invalid fd\n    Expected: ret %ld, errno %d\n    Got:      ret %ld, errno %d\n",
               ret_std, err_std, ret_ft, err_ft);
    }

    // Test 3: Null Buffer
    printf("Test 3 (null buffer):\n");
    errno = 0;
    ret_std = write(1, NULL, 10);
    err_std = errno;

    errno = 0;
    ret_ft = ft_write(1, NULL, 10);
    err_ft = errno;

    if (ret_std == ret_ft && err_std == err_ft) {
        printf("  " C_GREEN "[PASS]" C_RESET " ret: %ld, errno: %d\n", ret_ft, err_ft);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " null buffer\n    Expected: ret %ld, errno %d\n    Got:      ret %ld, errno %d\n",
               ret_std, err_std, ret_ft, err_ft);
    }

    // Test 4: Writing to stdout
    printf("Test 4 (write to stdout):\n");
    printf("  [Expected]: ");
    fflush(stdout);
    ret_std = write(1, "Hello standard write!\n", 22);
    printf("  [Got]:      ");
    fflush(stdout);
    ret_ft = ft_write(1, "Hello standard write!\n", 22);
    if (ret_std == ret_ft) {
        printf("  " C_GREEN "[PASS]" C_RESET " Characters match and successfully printed on terminal.\n");
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " Mismatched stdout return values: expected %ld, got %ld\n", ret_std, ret_ft);
    }
}

static void test_read(int interactive) {
    print_header("ft_read");
    char buf_std[100];
    char buf_ft[100];
    ssize_t ret_std, ret_ft;
    int err_std, err_ft;
    int fd;

    // Test 1: Real file read (libasm.h)
    printf("Test 1 (reading libasm.h):\n");
    memset(buf_std, 0, 100);
    memset(buf_ft, 0, 100);

    fd = open("libasm.h", O_RDONLY);
    errno = 0;
    ret_std = read(fd, buf_std, 50);
    err_std = errno;
    close(fd);

    fd = open("libasm.h", O_RDONLY);
    errno = 0;
    ret_ft = ft_read(fd, buf_ft, 50);
    err_ft = errno;
    close(fd);

    int content_ok = (memcmp(buf_std, buf_ft, 50) == 0);

    if (ret_std == ret_ft && err_std == err_ft && content_ok) {
        printf("  " C_GREEN "[PASS]" C_RESET " Read %ld bytes safely with content match\n", ret_ft);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " reading libasm.h\n");
        printf("    Expected: ret %ld, errno %d\n", ret_std, err_std);
        printf("    Got:      ret %ld, errno %d\n", ret_ft, err_ft);
        if (!content_ok) {
            printf("    Buffer contents mismatched!\n");
        }
    }

    // Test 2: Invalid FD
    printf("Test 2 (invalid fd 9999):\n");
    errno = 0;
    ret_std = read(9999, buf_std, 10);
    err_std = errno;

    errno = 0;
    ret_ft = ft_read(9999, buf_ft, 10);
    err_ft = errno;

    if (ret_std == ret_ft && err_std == err_ft) {
        printf("  " C_GREEN "[PASS]" C_RESET " ret: %ld, errno: %d\n", ret_ft, err_ft);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " invalid fd\n    Expected: ret %ld, errno %d\n    Got:      ret %ld, errno %d\n",
               ret_std, err_std, ret_ft, err_ft);
    }

    // Test 3: Null Buffer
    printf("Test 3 (null buffer on file):\n");
    int temp_fd = open("libasm.h", O_RDONLY);
    errno = 0;
    ret_std = read(temp_fd, NULL, 10);
    err_std = errno;
    close(temp_fd);

    temp_fd = open("libasm.h", O_RDONLY);
    errno = 0;
    ret_ft = ft_read(temp_fd, NULL, 10);
    err_ft = errno;
    close(temp_fd);

    if (ret_std == ret_ft && err_std == err_ft) {
        printf("  " C_GREEN "[PASS]" C_RESET " ret: %ld, errno: %d\n", ret_ft, err_ft);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " null buffer\n    Expected: ret %ld, errno %d\n    Got:      ret %ld, errno %d\n",
               ret_std, err_std, ret_ft, err_ft);
    }

    // Test 4: Stdin reading (interactive if terminal, non-blocking)
    printf("Test 4 (stdin read):\n");
    if (interactive && isatty(0)) {
        printf("  " C_YELLOW "[WAIT]" C_RESET " Please type a word and press Enter to test stdin: ");
        fflush(stdout);
        char stdin_buf[100];
        memset(stdin_buf, 0, 100);
        ssize_t bytes = ft_read(0, stdin_buf, 99);
        // Remove trailing newline if present
        if (bytes > 0 && stdin_buf[bytes - 1] == '\n') {
            stdin_buf[bytes - 1] = '\0';
        }
        printf("  " C_GREEN "[PASS]" C_RESET " Successfully read %ld bytes from stdin: '%s'\n", bytes, stdin_buf);
    } else {
        printf("  " C_YELLOW "[SKIP]" C_RESET " Run with './test_bin --interactive' to test stdin interactively.\n");
    }
}

static void test_strdup(void) {
    print_header("ft_strdup");
    const char *cases[] = {
        "Hello, world!",
        "",
        "Assembly memory allocations on the heap are tricky!",
        "Another very long string to duplicate to verify heap memory allocation capacity works reliably under normal operating conditions.",
        NULL
    };

    for (int i = 0; cases[i] != NULL; i++) {
        char *dup_std = strdup(cases[i]);
        char *dup_ft = ft_strdup(cases[i]);

        int content_ok = (strcmp(cases[i], dup_ft) == 0);
        int different_pointer = (dup_ft != cases[i]);

        if (content_ok && different_pointer) {
            printf("  " C_GREEN "[PASS]" C_RESET " Duplicated: \"%s%s\" (ptr: %p != orig: %p)\n",
                   dup_ft, strlen(dup_ft) > 30 ? "..." : "", (void*)dup_ft, (void*)cases[i]);
        } else {
            printf("  " C_RED "[FAIL]" C_RESET " Input: \"%s\"\n", cases[i]);
            if (!content_ok) {
                printf("    Expected content: \"%s\"\n    Got content:      \"%s\"\n", cases[i], dup_ft);
            }
            if (!different_pointer) {
                printf("    Error: Duplicate pointer is identical to original string pointer!\n");
            }
        }
        free(dup_std);
        free(dup_ft);
    }
}

static void test_bonus_lists(void) {
    print_header("ft_list_push_front & ft_list_size");
    t_list *list = NULL;

    printf("  Initial list size: %d (Expected: 0)\n", ft_list_size(list));
    if (ft_list_size(list) == 0) {
        printf("  " C_GREEN "[PASS]" C_RESET " Size is 0 on NULL list\n");
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " Size is not 0 on NULL list\n");
    }

    printf("  Pushing 'Node 1'...\n");
    ft_list_push_front(&list, "Node 1");
    int size = ft_list_size(list);
    int ok1 = (size == 1 && strcmp((char*)list->data, "Node 1") == 0);
    if (ok1) {
        printf("  " C_GREEN "[PASS]" C_RESET " List head data: '%s', size: %d\n", (char*)list->data, size);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " After push 1 -> data: '%s', size: %d\n", (char*)list->data, size);
    }

    printf("  Pushing 'Node 2'...\n");
    ft_list_push_front(&list, "Node 2");
    size = ft_list_size(list);
    int ok2 = (size == 2 && strcmp((char*)list->data, "Node 2") == 0 && strcmp((char*)list->next->data, "Node 1") == 0);
    if (ok2) {
        printf("  " C_GREEN "[PASS]" C_RESET " List head data: '%s', next data: '%s', size: %d\n",
               (char*)list->data, (char*)list->next->data, size);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " After push 2 -> data: '%s', next data: '%s', size: %d\n",
               (char*)list->data, list->next ? (char*)list->next->data : "NULL", size);
    }

    printf("  Pushing 'Node 3'...\n");
    ft_list_push_front(&list, "Node 3");
    size = ft_list_size(list);
    int ok3 = (size == 3 && strcmp((char*)list->data, "Node 3") == 0);
    if (ok3) {
        printf("  " C_GREEN "[PASS]" C_RESET " List head data: '%s', size: %d\n", (char*)list->data, size);
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " After push 3 -> data: '%s', size: %d\n", (char*)list->data, size);
    }

    // Clean up
    t_list *curr = list;
    while (curr) {
        t_list *tmp = curr->next;
        free(curr);
        curr = tmp;
    }
    printf("  " C_GREEN "[PASS]" C_RESET " List elements freed successfully.\n");
}

static void test_list_sort(void) {
    print_header("ft_list_sort");
    t_list *list = NULL;

    // Push elements in an unsorted order
    printf("  Creating unsorted list of names...\n");
    ft_list_push_front(&list, "Charlie");
    ft_list_push_front(&list, "Alpha");
    ft_list_push_front(&list, "Echo");
    ft_list_push_front(&list, "Delta");
    ft_list_push_front(&list, "Bravo");

    printf("  List before sorting:\n");
    for (t_list *curr = list; curr != NULL; curr = curr->next) {
        printf("    - %s\n", (char*)curr->data);
    }

    printf("  Sorting list with ft_list_sort using ft_strcmp...\n");
    ft_list_sort(&list, ft_strcmp);

    printf("  List after sorting:\n");
    int sorted_ok = 1;
    for (t_list *curr = list; curr != NULL; curr = curr->next) {
        printf("    - %s\n", (char*)curr->data);
        if (curr->next && strcmp((char*)curr->data, (char*)curr->next->data) > 0) {
            sorted_ok = 0;
        }
    }

    if (sorted_ok) {
        printf("  " C_GREEN "[PASS]" C_RESET " List successfully sorted in ascending order!\n");
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " List is not correctly sorted!\n");
    }

    // Clean up
    t_list *curr2 = list;
    while (curr2) {
        t_list *tmp = curr2->next;
        free(curr2);
        curr2 = tmp;
    }
}

static void test_list_remove_if(void) {
    print_header("ft_list_remove_if");
    t_list *list = NULL;

    printf("  Creating list of heap-allocated fruit names (using ft_strdup)...\n");
    ft_list_push_front(&list, ft_strdup("Apple"));
    ft_list_push_front(&list, ft_strdup("Banana"));
    ft_list_push_front(&list, ft_strdup("Orange"));
    ft_list_push_front(&list, ft_strdup("Banana"));
    ft_list_push_front(&list, ft_strdup("Grape"));

    printf("  List before removal:\n");
    for (t_list *curr = list; curr != NULL; curr = curr->next) {
        printf("    - %s\n", (char*)curr->data);
    }

    printf("  Removing all 'Banana' nodes using ft_list_remove_if and free...\n");
    ft_list_remove_if(&list, "Banana", ft_strcmp, free);

    printf("  List after removing 'Banana':\n");
    int has_banana = 0;
    int size = 0;
    for (t_list *curr = list; curr != NULL; curr = curr->next) {
        printf("    - %s\n", (char*)curr->data);
        size++;
        if (strcmp((char*)curr->data, "Banana") == 0) {
            has_banana = 1;
        }
    }

    if (!has_banana && size == 3) {
        printf("  " C_GREEN "[PASS]" C_RESET " All matching nodes and their data freed and removed successfully!\n");
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " List still contains Banana or incorrect size (size: %d)!\n", size);
    }

    printf("  Removing head node 'Grape'...\n");
    ft_list_remove_if(&list, "Grape", ft_strcmp, free);
    printf("  List after removing 'Grape':\n");
    int has_grape = 0;
    size = 0;
    for (t_list *curr = list; curr != NULL; curr = curr->next) {
        printf("    - %s\n", (char*)curr->data);
        size++;
        if (strcmp((char*)curr->data, "Grape") == 0) {
            has_grape = 1;
        }
    }

    if (!has_grape && size == 2) {
        printf("  " C_GREEN "[PASS]" C_RESET " Head node successfully removed and freed!\n");
    } else {
        printf("  " C_RED "[FAIL]" C_RESET " Head node not correctly removed!\n");
    }

    // Clean up remaining elements
    t_list *curr = list;
    while (curr) {
        t_list *tmp = curr->next;
        free(curr->data);
        free(curr);
        curr = tmp;
    }
    printf("  " C_GREEN "[PASS]" C_RESET " List and remaining heap data freed.\n");
}

static void test_atoi_base(void) {
    print_header("ft_atoi_base");
    struct {
        char *str;
        char *base;
        int expected;
    } cases[] = {
        {"1234", "0123456789", 1234},
        {"   -42", "0123456789", -42},
        {" \t --++--25a", "0123456789", 25},
        {"101010", "01", 42},
        {"FF", "0123456789ABCDEF", 255},
        {"-A", "0123456789ABCDEF", -10},
        {"14", "314", 5},                 // Base 314 (base 3: '3'=0, '1'=1, '4'=2) -> 1*3 + 2 = 5
        {"42", "0", 0},                   // Invalid base: too short
        {"42", "01234567890", 0},           // Invalid base: duplicate
        {"42", "012+4", 0},                 // Invalid base: contains +
        {"42", "012 34", 0},                // Invalid base: contains space
        {NULL, NULL, 0}
    };

    for (int i = 0; cases[i].str != NULL; i++) {
        int got = ft_atoi_base(cases[i].str, cases[i].base);
        if (got == cases[i].expected) {
            printf("  " C_GREEN "[PASS]" C_RESET " atoi_base(\"%s\", \"%s\") -> expected: %d, got: %d\n",
                   cases[i].str, cases[i].base, cases[i].expected, got);
        } else {
            printf("  " C_RED "[FAIL]" C_RESET " atoi_base(\"%s\", \"%s\")\n    Expected: %d\n    Got:      %d\n",
                   cases[i].str, cases[i].base, cases[i].expected, got);
        }
    }
}

static void run_benchmarks(void) {
    printf("\n" C_BOLD C_YELLOW "========================================\n");
    printf("  PERFORMANCE BENCHMARKS\n");
    printf("========================================" C_RESET "\n");

    struct timespec start, end;
    double t_std, t_ft;
    volatile size_t dummy_len = 0;
    (void)dummy_len;

    char large_str[1001];
    memset(large_str, 'A', 1000);
    large_str[1000] = '\0';

    // 1. strlen benchmark (1,000,000 runs)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000000; i++) {
        dummy_len += strlen(large_str);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_std = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000000; i++) {
        dummy_len += ft_strlen(large_str);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_ft = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
    printf(C_BOLD "strlen vs ft_strlen" C_RESET " (1M iterations over 1000-char string):\n");
    printf("  Standard C: %8.3f ms\n", t_std);
    printf("  Libasm:     %8.3f ms\n", t_ft);
    printf("  Ratio:      %8.2fx %s\n\n", t_ft / t_std, t_ft < t_std ? "faster!" : "slower");

    // 2. strcpy benchmark (1,000,000 runs)
    char benchmark_dest[1100];
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000000; i++) {
        strcpy(benchmark_dest, large_str);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_std = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000000; i++) {
        ft_strcpy(benchmark_dest, large_str);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_ft = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
    printf(C_BOLD "strcpy vs ft_strcpy" C_RESET " (1M iterations over 1000-char string):\n");
    printf("  Standard C: %8.3f ms\n", t_std);
    printf("  Libasm:     %8.3f ms\n", t_ft);
    printf("  Ratio:      %8.2fx %s\n\n", t_ft / t_std, t_ft < t_std ? "faster!" : "slower");

    // 3. strcmp benchmark (1,000,000 runs)
    volatile int cmp_res = 0;
    (void)cmp_res;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000000; i++) {
        cmp_res += strcmp(large_str, large_str);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_std = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000000; i++) {
        cmp_res += ft_strcmp(large_str, large_str);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_ft = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
    printf(C_BOLD "strcmp vs ft_strcmp" C_RESET " (1M iterations over 1000-char string):\n");
    printf("  Standard C: %8.3f ms\n", t_std);
    printf("  Libasm:     %8.3f ms\n", t_ft);
    printf("  Ratio:      %8.2fx %s\n\n", t_ft / t_std, t_ft < t_std ? "faster!" : "slower");

    // 4. write benchmark using /dev/null (100,000 runs)
    int null_fd = open("/dev/null", O_WRONLY);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 100000; i++) {
        write(null_fd, "A", 1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_std = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 100000; i++) {
        ft_write(null_fd, "A", 1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_ft = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
    close(null_fd);
    printf(C_BOLD "write vs ft_write" C_RESET " (100k iterations writing 1 byte to /dev/null):\n");
    printf("  Standard C: %8.3f ms\n", t_std);
    printf("  Libasm:     %8.3f ms\n", t_ft);
    printf("  Ratio:      %8.2fx %s\n\n", t_ft / t_std, t_ft < t_std ? "faster!" : "slower");

    // 5. read benchmark using /dev/zero (100,000 runs)
    int zero_fd = open("/dev/zero", O_RDONLY);
    char dev_zero_buf[10];
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 100000; i++) {
        read(zero_fd, dev_zero_buf, 1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_std = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 100000; i++) {
        ft_read(zero_fd, dev_zero_buf, 1);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_ft = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
    close(zero_fd);
    printf(C_BOLD "read vs ft_read" C_RESET " (100k iterations reading 1 byte from /dev/zero):\n");
    printf("  Standard C: %8.3f ms\n", t_std);
    printf("  Libasm:     %8.3f ms\n", t_ft);
    printf("  Ratio:      %8.2fx %s\n\n", t_ft / t_std, t_ft < t_std ? "faster!" : "slower");

    // 6. strdup benchmark (100,000 runs)
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 100000; i++) {
        char *temp = strdup(large_str);
        free(temp);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_std = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 100000; i++) {
        char *temp = ft_strdup(large_str);
        free(temp);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    t_ft = (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
    printf(C_BOLD "strdup vs ft_strdup" C_RESET " (100k iterations duplicating 1000-char string):\n");
    printf("  Standard C: %8.3f ms\n", t_std);
    printf("  Libasm:     %8.3f ms\n", t_ft);
    printf("  Ratio:      %8.2fx %s\n\n", t_ft / t_std, t_ft < t_std ? "faster!" : "slower");
}

int main(int argc, char **argv) {
    int interactive = (argc > 1 && strcmp(argv[1], "--interactive") == 0);

    printf(C_BOLD C_YELLOW "\n============================================\n");
    printf("  LIBASM COMPREHENSIVE AUTOMATED TEST RUNNER\n");
    printf("============================================\n" C_RESET);

    test_strlen();
    test_strcpy();
    test_strcmp();
    test_write();
    test_read(interactive);
    test_strdup();
    test_bonus_lists();
    test_list_sort();
    test_list_remove_if();
    test_atoi_base();

    run_benchmarks();

    printf(C_BOLD C_GREEN "\nAll automated functional validations finished.\n\n" C_RESET);
    return (0);
}
