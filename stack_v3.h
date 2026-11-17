#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#define DJB2_ZERNO 5381u

#define RED          "\033[1;31m"
#define GREEN        "\033[1;32m"
#define YELLOW       "\033[1;33m"
#define BLUE         "\033[1;34m"
#define PURPLE       "\033[1;35m"
#define DARK_PURPLE  "\033[38;2;85;26;139m"
#define ORANGE       "\033[38;5;208m"
#define RESET        "\033[0m"

#define PROGON                               9
#define POISON (STACK_DATA_TYPE *) 0x91D01BAEB
#define POISON_VALUE               88005553535
#define POISON_VALUE_2                81337228
#define POISON_HASH_VALUE               696969
#define MAX_CAPACITY_VALUE                8194
#define EPSILON                           1e-8

#define STACK_INIT_CAPACITY    4
#define INCREASE_COEFF         2
#define CANARY_LEFT         1337
#define CANARY_RIGHT        1488

#define STACK_VALIDATE_BIRDS(...)     stack_validate_birds(__VA_ARGS__)
#define STACK_VERIFY(...)             stack_verify(__VA_ARGS__)
#define LOGPRINTF(...)                logprintf(__VA_ARGS__)

#define ON_WEAK_DEBUG

#ifdef  ON_TOTAL_DEBUG
    #define STACK_DUMP(...)           stack_dump(__VA_ARGS__)
    #define STACKPRINTF(...)          printf(__VA_ARGS__)
    #define STACKGETCHAR()            getchar()
#endif 

#ifdef  ON_WEAK_DEBUG
    #define STACK_DUMP(...)           (void(0))
    #define STACKPRINTF(...)          (void(0))
    #define STACKGETCHAR()            (void(0))
#endif

typedef double STACK_DATA_TYPE;

enum stack_life_cycle {
    UNINITALIZED       = 0,
    INITIALIZED        = 200,
    DESTROYED          = 500,
};

enum stack_function_called_from {
    STACK_INIT         = 1,
    STACK_PUSH         = 2,
    STACK_POP          = 3,
    STACK_DESTROY      = 4,
    STACK_DUMP         = 5,
    LOGPRINTF          = 6,
};

enum stack_potential_error {
    OK = 0,

    STACK_UNAVAILABLE_CAPACITY_VALUE      = 1,  
    STACK_NULL_POINTER_ERROR              = 2,
    STACK_ALREADY_INITIALIZED_ERROR       = 3,
    STACK_VIOLATED_RIGHT_CANARY           = 4,
    STACK_VIOLATED_LEFT_CANARY            = 5,
    STACK_ALREADY_DESTROYED_ERROR         = 6,
    STACK_UNITIALIZED_ERROR               = 7,
    STACK_LEFT_END_OF_ARRAY               = 8,

    STACK_INCORRECT_STRUCT_HASH           = 9,
    STACK_INCORRECT_SDT_HASH              = 10,
    
    FILE_WAS_NOT_OPPENED_PROPERLY         = 11,

    UNDEFINED_CPU_STACK_DUMP_MODE         = 12,
    BAD_DEFAULT                           = 13, // означает - отметить как ошибку по умолчанию, чтобы OK выставлялся явно
};

typedef struct defended_stack defended_stack;

defended_stack*       stack_init(size_t                     wished_capacity);                                      // хэш структуры устаналивается, хэш даты устанавливается
stack_potential_error stack_push(defended_stack*            pointer_on_stack,  STACK_DATA_TYPE value_to_push);     // хэш структуры меняется, хэш даты меняется
stack_potential_error stack_pop(defended_stack*             pointer_on_stack,  STACK_DATA_TYPE* variable_to_fill); // хэш структуры меняется, хэш даты меняется

stack_potential_error stack_verify(defended_stack*          pointer_on_stack,  stack_function_called_from func);   // хэш структуры не меняется, хэш даты не меняется   // хэш структуры не меняется, хэш даты не меняется
stack_potential_error stack_destroy(defended_stack*         pointer_on_stack); // оба хэша нулевыми становятся

stack_potential_error stack_validate_birds(defended_stack*  pointer_on_stack); // хэш структуры не меняется, хэш даты не меняется
stack_potential_error stack_dump(defended_stack*            pointer_on_stack); // хэш не менятеся
stack_potential_error get_status_from_init(defended_stack*  pointer_on_stack); // хэш не меняется

stack_potential_error stack_nanosim_uron(defended_stack*    pointer_on_stack, STACK_DATA_TYPE element, int position);