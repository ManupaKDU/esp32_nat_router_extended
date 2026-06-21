#pragma once
struct arg_str {
    const char **sval;
    int count;
};
struct arg_end {
    int count;
};
struct arg_int {
    int *ival;
    int count;
};
struct arg_str* arg_str1(const char *shortopts, const char *longopts, const char *datatype, const char *glossary);
struct arg_str* arg_str0(const char *shortopts, const char *longopts, const char *datatype, const char *glossary);
struct arg_end* arg_end(int maxcount);
int arg_parse(int argc, char **argv, void **argtable);
void arg_print_errors(void *fp, struct arg_end *end, const char *progname);
