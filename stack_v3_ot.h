#include <stdint.h>

struct defended_stack
{
    STACK_DATA_TYPE*       ptr_on_array;
    size_t                 capacity;
    size_t                 size;

    STACK_DATA_TYPE        canary_left;  
    STACK_DATA_TYPE        canary_right; 

    stack_life_cycle       status_code; 

    uint32_t               struct_hash;
    uint32_t               sdt_pointer_hash;
};

stack_potential_error logprintf(defended_stack* pointer_on_stack,  stack_potential_error error_code, stack_function_called_from func); 