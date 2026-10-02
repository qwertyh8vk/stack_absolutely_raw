#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PROGON 8
#define POISON (STACK_DATA_TYPE *)0x91D01BAEB
#define POISON_VALUE 88005553535
#define MAX_CAPACITY_VALUE 8192
#define EPSILON 1e-5

#define ON_DEBUG

#ifdef ON_DEBUG
    #define STACK_VERIFY(...) stack_verify(__VA_ARGS__)
    #define STACK_HANDLE_ERROR(...) stack_handle_error(__VA_ARGS__)
#endif 
#ifndef ON_DEBUG
    #define STACK_VERIFY(...) ((void)0)
    #define STACK_HANDLE_ERROR(...) ((void)0)
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
    STACK_HANDLE_ERROR = 6,
};

enum stack_potential_error {
    OK = 0,

    STACK_UNAVAILABLE_CAPACITY_VALUE      = -1,
    STACK_NULL_POINTER_ERROR              = -2,
    STACK_UNLUCKY_CALLOC_ERROR            = -3,
    STACK_ALREADY_INITIALIZED_ERROR       = -4,
    
    STACK_BAD_PUSH_ATTEMPT_ERROR_CODE     = -5,
    STACK_BAD_PUSH_ATTEMPT_ERROR_STATUS   = -6,

    STACK_BAD_PUSH_VIOLATED_RIGHT_CANARY  = -8,
    STACK_BAD_PUSH_VIOLATED_LEFT_CANARY   = -9,

    STACK_ALREADY_DESTROYED_ERROR         = -10,
    STACK_POP_VARIABLE_NULL_POINTER       = -11,
    STACK_POP_UNITIALIZED_ERROR           = -12,

};

typedef struct
{
    STACK_DATA_TYPE*      ptr_on_array;
    ssize_t                capacity;
    ssize_t                size;

    STACK_DATA_TYPE       canary_left;
    STACK_DATA_TYPE       canary_right;

    stack_potential_error  error_code;
    stack_life_cycle       status_code;

} defended_stack;


stack_potential_error stack_init(defended_stack*     pointer_on_stack,  ssize_t wished_capacity);
stack_potential_error stack_push(defended_stack*     pointer_on_stack,  STACK_DATA_TYPE value_to_push);
stack_potential_error stack_pop(defended_stack*      pointer_on_stack,  STACK_DATA_TYPE* variable_to_fill);
stack_potential_error stack_verify(defended_stack*   pointer_on_stack,  stack_function_called_from func);
stack_potential_error stack_destroy(defended_stack*  pointer_on_stack);
stack_potential_error stack_validate_birds(defended_stack* pointer_on_stack);

void stack_handle_error(defended_stack pointer_on_stack, 
                        stack_potential_error error_code, stack_function_called_from func);
void stack_dump(defended_stack);

const ssize_t stack_init_capacity   = 4;
const unsigned int increase_coeff   = 2;
const STACK_DATA_TYPE CANARY_LEFT  = 1337;
const STACK_DATA_TYPE CANARY_RIGHT = 1488;

int main() {
    defended_stack test_stack = {0};

    /*
    считаю правильным явно указать что я подразумеваю под начальными условиями
        NULL;
        0;
        0;

        0.0;
        0.0;

        OK;
        UNITIALIZED;
    */

    ssize_t test_stack_capacity = stack_init_capacity;
    stack_potential_error test_stack_check = stack_init(&test_stack, test_stack_capacity);
    printf("\n\n\nINIT STACK HANDLE!!!!!!\nINIT STACK HANDLE!!!!!!\n");
    stack_handle_error(test_stack, test_stack_check, STACK_INIT);
    printf("ЗАВЕРШИЛИ!!!!\nЗАВЕРШИЛИ!!!!\n\n");

    
    for (double i = 0; i < PROGON; i++) {
        stack_potential_error test_stack_while_push = stack_push(&test_stack, 2*(i+1));
        stack_handle_error(test_stack, test_stack_while_push, STACK_PUSH);
    }

    double x = 0;
    
    stack_pop(&test_stack, &x);
    printf("что хранится в иксе: <%lf>\n", x);
    getchar();

    return OK;
}

stack_potential_error stack_verify(defended_stack* pointer_on_stack, stack_function_called_from func) {

    switch (func) {
        case STACK_INIT:
            if (pointer_on_stack == NULL) {
                return STACK_NULL_POINTER_ERROR;
            }

            if (pointer_on_stack->status_code == INITIALIZED) {
                return STACK_ALREADY_INITIALIZED_ERROR;
            }

            break;
        
        case STACK_PUSH:
            if (pointer_on_stack == NULL) {
                return STACK_NULL_POINTER_ERROR;
            }

            if (pointer_on_stack->status_code != INITIALIZED) {
                return STACK_BAD_PUSH_ATTEMPT_ERROR_STATUS;
            }

            if (pointer_on_stack->error_code != OK) {
                return STACK_BAD_PUSH_ATTEMPT_ERROR_CODE;
            }

            break;

        case STACK_POP:
            if (pointer_on_stack == NULL) {
                return STACK_NULL_POINTER_ERROR;
            }

            if (pointer_on_stack->status_code != INITIALIZED) {
                return STACK_POP_UNITIALIZED_ERROR;
            }

            if (pointer_on_stack->error_code != OK) {
                return STACK_BAD_PUSH_ATTEMPT_ERROR_CODE;
            }

            break;

        case STACK_DESTROY:
            if (pointer_on_stack == NULL) {
                return STACK_NULL_POINTER_ERROR;
            }

            if (pointer_on_stack->status_code == DESTROYED) {
                return STACK_ALREADY_DESTROYED_ERROR;
            }

            break;

        case STACK_DUMP:
            
            break;

        case STACK_HANDLE_ERROR:

            break;
    }

    return OK;
}

stack_potential_error stack_init(defended_stack* pointer_on_stack, ssize_t wished_capacity) {
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_INIT);
    
    if (basic_stack_verify == OK) {
        if (wished_capacity <= 0 || (wished_capacity + 2 > MAX_CAPACITY_VALUE)) {
            pointer_on_stack->error_code = STACK_UNAVAILABLE_CAPACITY_VALUE;

            return STACK_UNAVAILABLE_CAPACITY_VALUE;
        }
        
        else {
            STACK_DATA_TYPE* temp_pointer_for_safety = (STACK_DATA_TYPE* )calloc(wished_capacity + 2, sizeof(STACK_DATA_TYPE));
            if (temp_pointer_for_safety != NULL) {

                pointer_on_stack->ptr_on_array = temp_pointer_for_safety;
                pointer_on_stack->capacity     = wished_capacity;
                
                STACK_DATA_TYPE* left_canary_pointer  = temp_pointer_for_safety;
                STACK_DATA_TYPE* right_canary_pointer = temp_pointer_for_safety + wished_capacity + 1;

                *(left_canary_pointer)  = CANARY_LEFT;
                *(right_canary_pointer) = CANARY_RIGHT;

                pointer_on_stack->size         = 1; // пропустили канарейку слева.
                pointer_on_stack->error_code   = OK;
                pointer_on_stack->status_code  = INITIALIZED;
                pointer_on_stack->canary_left  = CANARY_LEFT;
                pointer_on_stack->canary_right = CANARY_RIGHT;

                return OK;
            }

            else {
                pointer_on_stack->error_code = STACK_UNLUCKY_CALLOC_ERROR;
                
                return STACK_UNLUCKY_CALLOC_ERROR;
            }
        }
    }

    return basic_stack_verify;
}

stack_potential_error stack_push(defended_stack* pointer_on_stack, STACK_DATA_TYPE push_value) { // todo переделать стэк пуш
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_PUSH);

    // проверка на нулл указателя на стэк
    // проверка инициализирован ли стэк
    // проверка в порядке ли наш стэк

    STACK_DATA_TYPE* pointer_on_array = NULL;

    printf("===========================================================\n");
    printf("присвоил pointer_on_array'ю переменной значения поля стэка\n"
           "<%p> значение адреса сохранённое\n", pointer_on_array);

    printf("проверим значение поля стэка\n"
           "<%p> значение адреса из ПОЛЯ СТЭКА\n", pointer_on_stack->ptr_on_array);
    printf("===========================================================\n");
    getchar();

    if (basic_stack_verify == OK) {
        if (pointer_on_stack->size > pointer_on_stack->capacity - 1) { // check
            printf("СЕЙЧАС я попал на последний НЕ канареешный элемент массива!\n");
            getchar();
            STACK_DATA_TYPE* temp_pointer_for_safety = (STACK_DATA_TYPE* )realloc(pointer_on_stack->ptr_on_array, 
                                                                                   ((pointer_on_stack->capacity + 2) * sizeof(STACK_DATA_TYPE) * increase_coeff));
            if (temp_pointer_for_safety != NULL) {
                pointer_on_stack->ptr_on_array = temp_pointer_for_safety;

                printf("===========================================================\n");
                printf("проверим переменную pointer_on_array\n"
                       "<%p> значение адреса сохранённое\n", pointer_on_array);

                printf("проверим значение поля стэка\n"
                           "<%p> значение адреса из ПОЛЯ СТЭКА\n", pointer_on_stack->ptr_on_array);
                printf("===========================================================\n");
                getchar();

                // Эти принтфы я убирать не буду, они увековечат в камне меня, как долбаеба

                // pointer_on_array = pointer_on_stack->ptr_on_array;
                
                pointer_on_array = pointer_on_stack->ptr_on_array;

                printf("===========================================================\n");
                printf("ПОВТОРНО ПРОВЕРИМ переменную pointer_on_array\n"
                       "<%p> значение адреса сохранённое\n", pointer_on_array);
                printf("===========================================================\n");

                pointer_on_array[pointer_on_stack->size + 1] = POISON_VALUE; // Там, где была канарейка, теперь отравленное значение.

                pointer_on_stack->capacity *= 2;
                pointer_on_array[(pointer_on_stack->capacity) + 1] = CANARY_RIGHT;

                pointer_on_array[pointer_on_stack->size] = push_value;
                pointer_on_stack->size += 1;
                
            }

            else {

                return STACK_NULL_POINTER_ERROR;
            }
        }

        else {
            pointer_on_array = pointer_on_stack->ptr_on_array;
            pointer_on_array[pointer_on_stack->size] = push_value;
            pointer_on_stack->size += 1;
        }

        // проверка канареек

        if (fabs(pointer_on_array[(pointer_on_stack->capacity) + 1] - pointer_on_stack->canary_right) > EPSILON) { // check
                
            return STACK_BAD_PUSH_VIOLATED_RIGHT_CANARY;
        }

        if (fabs(pointer_on_array[0] - pointer_on_stack->canary_left) > EPSILON) { // check
                
            return STACK_BAD_PUSH_VIOLATED_LEFT_CANARY;
        } 
        
        stack_potential_error check_birdies = stack_validate_birds(pointer_on_stack);
        // проверка канареек

        return check_birdies;
    }

    return basic_stack_verify;
}

/*
    canary 0 1 2 5 canary 
*/

stack_potential_error stack_pop(defended_stack* pointer_on_stack, STACK_DATA_TYPE* variable) {

    // проверка на нулл переменной куда записываем
    // проверка указателя на стэк на нулл
    // проверка на то инициализирован ли стэк
    // проверка в порядке ли наш стэк

    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_POP);

    if (basic_stack_verify == OK) {
        if (variable == NULL) {

            return STACK_NULL_POINTER_ERROR;
        }

        else {
            STACK_DATA_TYPE* pointer_on_array = pointer_on_stack->ptr_on_array;
            STACK_DATA_TYPE  pop_value = pointer_on_array[pointer_on_stack->size];
            pointer_on_array[pointer_on_stack->size] = POISON_VALUE;
            pointer_on_stack->size -= 1;
            *variable = pop_value;

            return OK;
        }
    }

    // realloc вниз не реализован

    return basic_stack_verify;
}

stack_potential_error stack_validate_birds(defended_stack* pointer_on_stack) 
{
    STACK_DATA_TYPE* pointer_on_array = pointer_on_stack->ptr_on_array;

    if (fabs(pointer_on_array[(pointer_on_stack->capacity) + 1] - pointer_on_stack->canary_right) > EPSILON) { // check

        return STACK_BAD_PUSH_VIOLATED_RIGHT_CANARY;
    }

    if (fabs(pointer_on_array[0] - pointer_on_stack->canary_left) > EPSILON) { // check
                
        return STACK_BAD_PUSH_VIOLATED_LEFT_CANARY;
    } 

    return OK;
}

void stack_handle_error(defended_stack pointer_on_stack, 
                        stack_potential_error error_code, stack_function_called_from func) 
{
    if (error_code != OK) {
        switch (func) {
            case STACK_INIT:
                printf("Ошибка прокнулась в функции stack_init.\n");
                break;

            case STACK_PUSH:
                printf("Ошибка прокнулась в функции stack_push.\n");
                break;

            case STACK_POP:
                printf("Ошибка прокнулась в функции stack_pop.\n");
                break;

            case STACK_DESTROY:
                printf("Ошибка прокнулась в функции stack_destroy.\n");
                break;

            case STACK_DUMP:
                printf("Ошибка прокнулась в функции stack_dump.\n");
                break;
        }

        getchar();
        printf("Код ошибки: <%d>\n", error_code);
        printf("--------------------------------------\n");
        getchar();
    }

    stack_dump(pointer_on_stack);
    getchar();
}

stack_potential_error stack_destroy(defended_stack* pointer_on_stack) {
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_DESTROY);

    if (basic_stack_verify == OK) {
        STACK_DATA_TYPE* ptr_on_array_to_poison = pointer_on_stack->ptr_on_array;

        for (ssize_t i = 0; i < (pointer_on_stack->capacity) + 2; i++) {
            ptr_on_array_to_poison[i]  = POISON_VALUE;
        }

        free(ptr_on_array_to_poison);

        ptr_on_array_to_poison         = POISON;
        pointer_on_stack->capacity     = 0;
        pointer_on_stack->size         = 0;
        pointer_on_stack->error_code   = OK;
        pointer_on_stack->status_code  = DESTROYED;

        return OK;
    }
    
    return basic_stack_verify;
}

void stack_dump(defended_stack stack_copy) {
    // проблема с переданным аргументом. решим её 
    // но сначала нужно привести в здравый смысл все другие функции

    printf("-----------------------------------\n");
    printf("указатель дабловского массива: <%p>\n", stack_copy.ptr_on_array);
    printf("размер массива без учёта канареек: <%zd>\n", stack_copy.capacity);

    STACK_DATA_TYPE* ptr_on_array = stack_copy.ptr_on_array;

    for (ssize_t i = 0; i < stack_copy.capacity + 2; i++) {
        printf("число: [%lf], индекс: [%zd]\n", ptr_on_array[i], i);
    }

    printf("\n");
    printf("индекс самого первого свободного элемента: <%zd>\n", stack_copy.size);
    printf("значение канарейки слева: <%lf>\n", stack_copy.canary_left);
    printf("значение канарейки справа: <%lf> \n", stack_copy.canary_right);
    printf("статус стэка: <%d>\n", stack_copy.status_code);
    printf("код ошибки стэка (статус ошибки стэка): <%d>\n", stack_copy.error_code);
    printf("-----------------------------------\n");
}

/*
canary 0 1 2 canary 
*/