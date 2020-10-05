#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef int (*int_arg_fn)(int);
typedef int (*uint_arg_fn)(unsigned int);
typedef int (*void_p_arg_fn)(void *);
typedef void *(*void_p_ret_fn)(int);
typedef int (*float_arg_fn)(float);

// int, int 
int int_arg_1(int arg) {
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int int_arg_2(int arg) {
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int int_arg_3(int arg) {
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int int_arg_4(int arg) {
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}

// int, uint 
int bad_uint_arg_1(unsigned int arg) {
    printf("Evil1: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int bad_uint_arg_2(unsigned int arg) {
    printf("Evil1: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int bad_uint_arg_3(unsigned int arg) {
    printf("Evil1: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int bad_uint_arg_4(unsigned int arg) {
    printf("Evil1: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}

// int, void* 
int bad_void_p_arg_1(void * arg) {
    printf("Evil2: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int bad_void_p_arg_2(void * arg) {
    printf("Evil2: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int bad_void_p_arg_3(void * arg) {
    printf("Evil2: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
int bad_void_p_arg_4(void * arg) {
    printf("Evil2: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}

// void*, int
void * bad_void_p_ret_1(int arg){
    printf("Evil3: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
void * bad_void_p_ret_2(int arg){
    printf("Evil3: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
void * bad_void_p_ret_3(int arg){
    printf("Evil3: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}
void * bad_void_p_ret_4(int arg){
    printf("Evil3: CFI will not protect transfer to here\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    return 0;
}

// static 
int float_arg(float arg) {
    printf("CFI should protect transfer to here\n");
    printf("Original args : %f\n", arg);
    printf("In %s: (%f)\n", __FUNCTION__, (double)arg);
    return 0;
}

// static 
int not_entry_point(int arg) {
    // nop sled for x86 / x86-64
    // these instructions act as a buffer
    // for an indirect control flow transfer to skip
    // a valid function entry point, but continue
    // to execute normal code
    __asm__ volatile (
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
            );
    printf("CFI ensures control flow only transfers to potentially valid destinations\n");
    printf("In %s: (%d)\n", __FUNCTION__, arg);
    // need to exit or the program will segfault anyway,
    // since the indirect call skipped the function preamble
    exit(arg);
}
void print_err(){
    printf("Signature Mismatch!\n");
    exit(-1);
}
void print_one(int poped){
    printf("%d\n", poped);
}
void print_two(int poped1, int poped2){
    printf("%d, %d\n", poped1, poped2);
}
struct foo {
    // Exploit Type Weakness 
    // 여기에 assign 되는 순서대로 table elem에 들어간다.
    int_arg_fn int_funcs[4];
    uint_arg_fn uint_funcs[4];
    void_p_arg_fn void_p_arg_funcs[4];
    void_p_ret_fn void_p_ret_funcs[4];
    float_arg_fn float_funcs[1];  
    int_arg_fn not_entries[1];
};

// the struct aligns the function pointer arrays 
// so indexing past the end will reliably
// call working function pointers

static struct foo f = { 
    // Exploit Type Weakness 
    .int_funcs = {int_arg_1, int_arg_2, int_arg_3, int_arg_4},  
    .uint_funcs = {bad_uint_arg_1, bad_uint_arg_2, bad_uint_arg_3, bad_uint_arg_4 },
    .void_p_arg_funcs = {bad_void_p_arg_1, bad_void_p_arg_2, bad_void_p_arg_3, bad_void_p_arg_4},
    .void_p_ret_funcs = {bad_void_p_ret_1, bad_void_p_ret_2, bad_void_p_ret_3, bad_void_p_ret_4},
    .float_funcs = {float_arg},
    .not_entries = {(int_arg_fn)((uintptr_t)(not_entry_point)+0x20)}
};
int call_int_funcs(int_arg_fn fp, int idx){
    fp(idx);
}
int call_uint_funcs(uint_arg_fn fp, int idx){
    fp(idx);
}
int call_void_p_arg_funcs(void_p_arg_fn fp, int idx){
    fp(idx);
}
int call_void_p_ret_funcs(void_p_ret_fn fp, int idx){
    fp(idx);
}
int main(int argc, const char *argv[]) {
    // if(argc != 2) {
    //     printf("Usage: %s <option>\n", argv[0]);
    //     printf("Option values:\n");
    //     printf("\t0\tCall correct function\n");
    //     printf("\t1\tCall the wrong function but with the same signature\n");
    //     printf("\t2\tCall a float function with an int function signature\n");
    //     printf("\t3\tCall into the middle of a function\n");
    //     printf("\n");
    //     printf("\tAll other options are undefined, but should be caught by CFI :)\n");
    //     printf("\n\n");
    //     printf("Here are some pointers so clang doesn't optimize away members of `struct foo f`:\n");
    //     printf("\tint_funcs: %p\n", (void*)f.int_funcs);
    //     printf("\tu_int_funcs: %p\n", (void*)f.uint_funcs);
    //     printf("\tvoid_p_arg_funcs: %p\n", (void*)f.void_p_arg_funcs);
    //     printf("\tvoid_p_ret_funcs: %p\n", (void*)f.void_p_ret_funcs);
    //     printf("\tfloat_funcs: %p\n", (void*)f.float_funcs);
    //     printf("\tnot_entries: %p\n", (void*)f.not_entries);
    //     return 1;
    // }
    printf("Calling a function:\n");
    int idx = argv[1][0] - '0';
	printf("idx : %d\n", idx);
    call_int_funcs(f.int_funcs[idx], idx);
    call_uint_funcs(f.uint_funcs[idx], idx);
    call_void_p_arg_funcs(f.void_p_arg_funcs[idx], idx);
    call_void_p_ret_funcs(f.void_p_ret_funcs[idx], idx);
    // f.int_funcs[idx](idx);
    // f.uint_funcs[idx](idx);
    // f.void_p_arg_funcs[idx](idx);
    // f.void_p_ret_funcs[idx](idx);
    return 0;
}
