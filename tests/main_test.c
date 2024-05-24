
#include "basic_struct_test.c"
#include "basic_operation_test.c"
#include "memory_control_test.c"

int main(void) {
    basic_operation_test();
    memory_control_test();

    basic_struct_test();
}