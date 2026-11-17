#include "stack_v3.h"
#include "stack_v3_ot.h"

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
// // // // // // // // // // MAIN БОЛЬШЕ НЕДЕЙСТВИТЕЛЕН, БИЛДИМ PROCESSOR // // // // // // // // // // //
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //
// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

stack_potential_error stack_nanosim_uron(defended_stack* pointer_on_stack, STACK_DATA_TYPE element, int position) {
    STACK_DATA_TYPE* pointer_on_array = pointer_on_stack->ptr_on_array;
    pointer_on_array[position]        = element;

    return OK;
}

/* вычисление хэша для stack data type указателя, ака МАССИВА!!! */

uint32_t djb2_calculate_hash(const void *data, size_t len) {
    const unsigned char* pointer_on_data = (const unsigned char *)data;
    uint32_t hash = DJB2_ZERNO;

    for (size_t i = 0; i < len; i++)
        hash = ((hash << 5) + hash) + *(pointer_on_data + i); 

    return hash;
}

/* считает хэш всего массива не учитывая непосредственно указатель на stack data type */

uint32_t djb2_calculate_for_check_struct_hash(defended_stack* pointer_on_stack) {
    uint32_t struct_hash_temp          = pointer_on_stack->struct_hash;
    uint32_t sdt_pointer_hash_temp     = pointer_on_stack->sdt_pointer_hash;

    pointer_on_stack->struct_hash      = 0;
    pointer_on_stack->sdt_pointer_hash = 0;

    uint32_t hash_calculated           = djb2_calculate_hash(pointer_on_stack, sizeof(defended_stack));

    pointer_on_stack->struct_hash      = struct_hash_temp;
    pointer_on_stack->sdt_pointer_hash = sdt_pointer_hash_temp;

    return hash_calculated;
}

uint32_t djb2_calculate_for_rewrite_struct_hash(defended_stack* pointer_on_stack) {
    pointer_on_stack->struct_hash      = 0;
    pointer_on_stack->sdt_pointer_hash = 0;

    uint32_t hash_calculated           = djb2_calculate_hash(pointer_on_stack, sizeof(defended_stack));

    return hash_calculated;
}

/* пересчитывает хэш после каждой функции */

void recalculate_hash_after_func(defended_stack* pointer_on_stack) {
    uint32_t hash_sdt_recounted        = djb2_calculate_hash(pointer_on_stack->ptr_on_array, ((pointer_on_stack->capacity + 2) * sizeof(STACK_DATA_TYPE)));
    uint32_t hash_struct_recounted     = djb2_calculate_for_rewrite_struct_hash(pointer_on_stack);

    STACKPRINTF("Новый " GREEN "пересчитанный " RESET "хэш структуры: [%u]\n", hash_struct_recounted);
    STACKGETCHAR();

    pointer_on_stack->struct_hash      = hash_struct_recounted;
    pointer_on_stack->sdt_pointer_hash = hash_sdt_recounted;
}

stack_potential_error stack_verify(defended_stack* pointer_on_stack, stack_function_called_from func) {
    if (pointer_on_stack == NULL) {

        return STACK_NULL_POINTER_ERROR; // зачем усложнять себе жизнь??? ЕСЛИ NULL - то НАХУЙ Отсюда!
    }

    else {
        if (pointer_on_stack->ptr_on_array != POISON) {
            uint32_t struct_hash = djb2_calculate_for_check_struct_hash(pointer_on_stack);
            uint32_t array_hash  = djb2_calculate_hash(pointer_on_stack->ptr_on_array, ((pointer_on_stack->capacity + 2) * sizeof(STACK_DATA_TYPE)));
            
            // STACKPRINTF(RED "=================================================================\n" RESET);
            // STACKPRINTF("Вызван из функции: [%d]\n", func);
            // STACKPRINTF("Хэш структуры, который посчитала функция djb2_calculate_for_check_struct_hash: [%u]\n", struct_hash);
            // STACKPRINTF("Хэш структуры, который сохранил стэк: [%u]\n", pointer_on_stack->struct_hash);
            // STACKPRINTF("---------------------------------------------------------------------\n");
            // STACKPRINTF("Хэш массива, который посчитала функция djb2_calculate_hash: [%u]\n", array_hash);
            // STACKPRINTF("Хэш массива, который сохранил стэк: [%u]\n", pointer_on_stack->sdt_pointer_hash);
            // STACKPRINTF(RED "=================================================================\n" RESET);
            // STACKGETCHAR();

            if (struct_hash != (pointer_on_stack->struct_hash)) {
                return STACK_INCORRECT_STRUCT_HASH;
            }

            if (array_hash != (pointer_on_stack->sdt_pointer_hash)) {
                return STACK_INCORRECT_SDT_HASH;
            }

            stack_potential_error check_birdies_in_beginning = stack_validate_birds(pointer_on_stack);

            if (check_birdies_in_beginning != OK) {
                return check_birdies_in_beginning;
            }
        }  // проверяем и хэш, и канарейки если не отравленный указатель!!!
        
        switch (func) {
            case STACK_INIT:
                if (pointer_on_stack->status_code == INITIALIZED) {
                    return STACK_ALREADY_INITIALIZED_ERROR;
                }

                break;
            
            case STACK_PUSH:
            case STACK_POP:
                if (pointer_on_stack->status_code != INITIALIZED) {
                    return STACK_UNITIALIZED_ERROR;
                }

                break;

            case STACK_DESTROY:
                if (pointer_on_stack->status_code == DESTROYED) {
                    return STACK_ALREADY_DESTROYED_ERROR;
                }

                break;

            case LOGPRINTF:
            case STACK_DUMP:
                // нифигашеньки
                break;

            default:
                STACKPRINTF(RED  "АЛАРМ!!!!! НЕОБРАБОТАННЫЙ СЛУЧАЙ\n"
                            "АЛАРМ!!!!! НЕОБРАБОТАННЫЙ СЛУЧАЙ\n"
                            "АЛАРМ!!!!! НЕОБРАБОТАННЫЙ СЛУЧАЙ\n" RESET);
                break;
        }

        return OK;
    }
}

defended_stack* stack_init(size_t wished_capacity) {
    defended_stack* stack_waits_to_be_initialized = (defended_stack *)calloc(1, sizeof(defended_stack));

    if (wished_capacity <= 0 || (wished_capacity + 2 > MAX_CAPACITY_VALUE)) {
        LOGPRINTF(NULL, STACK_UNAVAILABLE_CAPACITY_VALUE, STACK_INIT);

        return NULL;
    } 
    
    else {
        STACK_DATA_TYPE* temp_pointer_for_safety = (STACK_DATA_TYPE* )calloc(wished_capacity + 2, sizeof(STACK_DATA_TYPE));
        if (temp_pointer_for_safety != NULL) {

            stack_waits_to_be_initialized->ptr_on_array = temp_pointer_for_safety;
            stack_waits_to_be_initialized->capacity     = wished_capacity;
            
            STACK_DATA_TYPE* left_canary_pointer  = temp_pointer_for_safety;
            STACK_DATA_TYPE* right_canary_pointer = temp_pointer_for_safety + wished_capacity + 1;

            *(left_canary_pointer)  = CANARY_LEFT;
            *(right_canary_pointer) = CANARY_RIGHT;

            stack_waits_to_be_initialized->size               =  1; // пропустили канарейку слева.
            stack_waits_to_be_initialized->status_code        =  INITIALIZED;
            stack_waits_to_be_initialized->canary_left        =  CANARY_LEFT;
            stack_waits_to_be_initialized->canary_right       =  CANARY_RIGHT;

            stack_waits_to_be_initialized->struct_hash        =  djb2_calculate_for_rewrite_struct_hash(stack_waits_to_be_initialized); // переставить вниз ещё раз. вниз = после calculate_hash.
            stack_waits_to_be_initialized->sdt_pointer_hash   =  djb2_calculate_hash(stack_waits_to_be_initialized->ptr_on_array, (wished_capacity + 2) * sizeof(STACK_DATA_TYPE));

            STACKPRINTF(BLUE "что там у нас по хэшу массива? [%u] \n" RESET, stack_waits_to_be_initialized->sdt_pointer_hash);
            STACKPRINTF(BLUE "а что насчёт хэша структуры? [%u] \n" RESET, stack_waits_to_be_initialized->struct_hash);
            STACKGETCHAR();         

        }

        else {
            LOGPRINTF(NULL, STACK_NULL_POINTER_ERROR, STACK_INIT);

            return NULL;
        }
    }

    return stack_waits_to_be_initialized;
}

stack_potential_error stack_push(defended_stack* pointer_on_stack, STACK_DATA_TYPE push_value) { // todo переделать стэк пуш
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_PUSH);
    STACK_DATA_TYPE*      pointer_on_array = NULL;

    if (basic_stack_verify == OK) {

        STACKPRINTF("===========================================================\n");
        STACKPRINTF(RED "ЗАНУЛИЛ " RESET "pointer_on_array'скую переменную значения поля стэка\n"
            "<%p> значение адреса сохранённое\n", pointer_on_array);

        STACKPRINTF(GREEN "ПРОВЕРИМ " RESET "значение поля стэка\n"
            "<%p> значение адреса из ПОЛЯ СТЭКА\n", pointer_on_stack->ptr_on_array);
        STACKPRINTF("===========================================================\n");
        STACKGETCHAR();

        if (pointer_on_stack->size > pointer_on_stack->capacity - 1) { // check
            STACKPRINTF(RED "СЕЙЧАС я попал на последний НЕ канареешный элемент массива!\n" RESET);
            STACKGETCHAR();
            STACK_DATA_TYPE* temp_pointer_for_safety = (STACK_DATA_TYPE* )realloc(pointer_on_stack->ptr_on_array, 
                                                                                   ((pointer_on_stack->capacity + 2) * sizeof(STACK_DATA_TYPE) * INCREASE_COEFF));
            if (temp_pointer_for_safety != NULL) {
                pointer_on_stack->ptr_on_array = temp_pointer_for_safety;

                STACKPRINTF("===========================================================\n");
                STACKPRINTF(GREEN "ПРОВЕРИМ " RESET "переменную pointer_on_array\n"
                       "<%p> значение адреса сохранённое\n", pointer_on_array);

                STACKPRINTF(GREEN "ПРОВЕРИМ " RESET "значение поля стэка\n"
                           "<%p> значение адреса из ПОЛЯ СТЭКА\n", pointer_on_stack->ptr_on_array);
                STACKPRINTF("===========================================================\n");
                STACKGETCHAR();
            
                pointer_on_array = pointer_on_stack->ptr_on_array;

                STACKPRINTF("===========================================================\n");
                STACKPRINTF(RED "ПОВТОРНО ПРОВЕРИМ " RESET "переменную pointer_on_array\n"
                       "<%p> значение адреса сохранённое\n", pointer_on_array);
                STACKPRINTF("===========================================================\n");

                pointer_on_array[pointer_on_stack->size]           = push_value;   // записали дуру
                pointer_on_array[pointer_on_stack->size + 1]       = POISON_VALUE; // Там, где была канарейка, теперь отравленное значение.
                pointer_on_stack->capacity                        *= 2;
                pointer_on_array[(pointer_on_stack->capacity) + 1] = CANARY_RIGHT;
                pointer_on_stack->size                            += 1;

                // так называемое нововведение, если что все проблемы будут тут!!! 

                for (size_t i = (pointer_on_stack->size + 1); i < (pointer_on_stack->capacity + 1); i++) {
                    pointer_on_array[i] = 0;
                }
            }

            else {
                LOGPRINTF(pointer_on_stack, STACK_NULL_POINTER_ERROR, STACK_PUSH);

                return STACK_NULL_POINTER_ERROR;
            }
        }

        else {
            pointer_on_array = pointer_on_stack->ptr_on_array;
            STACKPRINTF("===========================================================\n");
            STACKPRINTF(RED "ПОВТОРНО ПРОВЕРИМ " RESET "переменную pointer_on_array\n"
                "<%p> значение адреса сохранённое\n", pointer_on_array);
            STACKPRINTF("===========================================================\n");
            pointer_on_array[pointer_on_stack->size] = push_value;
            pointer_on_stack->size += 1;
        }

        stack_potential_error check_birdies_in_end = STACK_VALIDATE_BIRDS(pointer_on_stack);

        if (check_birdies_in_end != OK) {
            LOGPRINTF(pointer_on_stack, check_birdies_in_end, STACK_PUSH);
        }

        recalculate_hash_after_func(pointer_on_stack);
        STACKPRINTF("recalculating hash from stack_push...\n");
        STACKGETCHAR();

        return check_birdies_in_end;
    }

    LOGPRINTF(pointer_on_stack, basic_stack_verify, STACK_PUSH);

    return basic_stack_verify;
}

stack_potential_error stack_pop(defended_stack* pointer_on_stack, STACK_DATA_TYPE* variable) {
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_POP);

    if (basic_stack_verify == OK) {
        if (pointer_on_stack->size <= 1) {
            LOGPRINTF(pointer_on_stack, STACK_LEFT_END_OF_ARRAY, STACK_POP);

            return STACK_LEFT_END_OF_ARRAY;
        }

        else {
            STACK_DATA_TYPE* pointer_on_array = pointer_on_stack->ptr_on_array;
            STACK_DATA_TYPE  pop_value = pointer_on_array[pointer_on_stack->size - 1];
            pointer_on_array[pointer_on_stack->size - 1] = POISON_VALUE_2;
            pointer_on_stack->size -= 1;

            if (variable != NULL) {
                *variable = pop_value;
            }

            else {
                STACKPRINTF(PURPLE "Отладочный принтф, запись не осуществляется.\n" RESET);
                STACKGETCHAR();
            }
            
            if ((pointer_on_stack->size <= (pointer_on_stack->capacity / 4) + 1) && pointer_on_stack->capacity > 4) {
                pointer_on_array[pointer_on_stack->capacity + 1] = POISON_VALUE;
                STACK_DATA_TYPE* temp_pointer_for_safety = (STACK_DATA_TYPE* )realloc(pointer_on_stack->ptr_on_array, 
                                                                                     (((pointer_on_stack->capacity / INCREASE_COEFF) + 2) * sizeof(STACK_DATA_TYPE)));
                if (temp_pointer_for_safety != NULL) {

                    pointer_on_stack->ptr_on_array = temp_pointer_for_safety;
                    // залить нулями всю правую часть вручную (лучше мемсетом) , можно и циклом
                    pointer_on_array = pointer_on_stack->ptr_on_array;
                    pointer_on_stack->capacity /= 2;

                    pointer_on_array[pointer_on_stack->capacity + 1] = CANARY_RIGHT;
                }

                else {
                    LOGPRINTF(pointer_on_stack, STACK_NULL_POINTER_ERROR, STACK_POP);
                    
                    return STACK_NULL_POINTER_ERROR;
                }

            }   

            stack_potential_error check_birdies_in_end = STACK_VALIDATE_BIRDS(pointer_on_stack);

            if (check_birdies_in_end != OK) {
                LOGPRINTF(pointer_on_stack, check_birdies_in_end, STACK_POP);
            }

            recalculate_hash_after_func(pointer_on_stack);
            STACKPRINTF("recalculating hash from stack_pop...\n");
            STACKGETCHAR();

            return check_birdies_in_end;
        }
    }

    LOGPRINTF(pointer_on_stack, basic_stack_verify, STACK_POP);

    return basic_stack_verify;
}

stack_potential_error stack_validate_birds(defended_stack* pointer_on_stack) {
    STACK_DATA_TYPE* pointer_on_array = pointer_on_stack->ptr_on_array;

    if (fabs(pointer_on_array[(pointer_on_stack->capacity) + 1] - pointer_on_stack->canary_right) > EPSILON) { // check

        return STACK_VIOLATED_RIGHT_CANARY;
    }

    if (fabs(pointer_on_array[0] - pointer_on_stack->canary_left) > EPSILON) { // check
                
        return STACK_VIOLATED_LEFT_CANARY;
    } 

    return OK;
}

stack_potential_error logprintf(defended_stack* pointer_on_stack, stack_potential_error error_code, stack_function_called_from func) {
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, LOGPRINTF);

    if (basic_stack_verify == OK) {
        if (error_code != OK) {
            switch (func) {
                case STACK_INIT:
                    STACKPRINTF(RED "Ошибка прокнулась в функции stack_init.\n"    RESET);
                    break;

                case STACK_PUSH:
                    STACKPRINTF(RED "Ошибка прокнулась в функции stack_push.\n"    RESET);
                    break;

                case STACK_POP:
                    STACKPRINTF(RED "Ошибка прокнулась в функции stack_pop.\n"     RESET);
                    break;

                case STACK_DESTROY:
                    STACKPRINTF(RED "Ошибка прокнулась в функции stack_destroy.\n" RESET);
                    break;

                case STACK_DUMP:
                    STACKPRINTF(RED "Ошибка прокнулась в функции stack_dump.\n"    RESET);
                    break;

                case LOGPRINTF: // рекурсия пошла уже какая-то страшная
                default:
                    STACKPRINTF(RED "АЛАРМ!!!!! НЕОБРАБОТАННЫЙ СЛУЧАЙ\n"
                                "АЛАРМ!!!!! НЕОБРАБОТАННЫЙ СЛУЧАЙ\n"
                                "АЛАРМ!!!!! НЕОБРАБОТАННЫЙ СЛУЧАЙ\n" RESET);
                    break;
            }

            STACKPRINTF(RED "Код ошибки: [%d]\n" RESET, error_code);
            STACKGETCHAR();
        }
    
        stack_dump(pointer_on_stack);

        return OK;
    }

    STACKPRINTF(RED "\nОшибка произошла в САМОЙ функции logprintf.\n" RESET);
    STACKPRINTF(RED "Код ошибки: [%d]\n\n" RESET, basic_stack_verify);

    return basic_stack_verify;
}

stack_potential_error stack_destroy(defended_stack* pointer_on_stack) {
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_DESTROY);

    if (basic_stack_verify == OK) {

        for (size_t i = 0; i < (pointer_on_stack->capacity) + 2; i++) {
            (pointer_on_stack->ptr_on_array)[i]  = POISON_VALUE;
        }

        free(pointer_on_stack->ptr_on_array);

        pointer_on_stack->ptr_on_array     = POISON;
        pointer_on_stack->canary_left      = POISON_VALUE;
        pointer_on_stack->canary_right     = POISON_VALUE;
        pointer_on_stack->capacity         = 0;
        pointer_on_stack->size             = 0;
        pointer_on_stack->status_code      = DESTROYED;

        pointer_on_stack->struct_hash      = POISON_HASH_VALUE; // если у меня сломался стэк , первым делом анализирую эти вещи.
        pointer_on_stack->sdt_pointer_hash = POISON_HASH_VALUE; // если у меня сломался стэк , первым делом анализирую эти вещи.

        return OK;
    }
    
    LOGPRINTF(pointer_on_stack, basic_stack_verify, STACK_DESTROY);

    return basic_stack_verify;
}

stack_potential_error stack_dump(defended_stack* pointer_on_stack) {
    stack_potential_error basic_stack_verify = STACK_VERIFY(pointer_on_stack, STACK_DUMP);

    if (basic_stack_verify == OK) {
        STACKPRINTF("-----------------------------------------------------------\n");
        STACKPRINTF(RED "значение хэша СТРУКТУРЫ: [%u]\n" RESET, pointer_on_stack->struct_hash);
        STACKPRINTF(RED "значение хэша МАССИВА: [%u]\n" RESET, pointer_on_stack->sdt_pointer_hash);
        STACKPRINTF("указатель дабловского массива: <%p>\n", pointer_on_stack->ptr_on_array);
        STACKPRINTF(GREEN "размер массива без учёта канареек: <%zu>\n\n" RESET, pointer_on_stack->capacity);

        STACK_DATA_TYPE* ptr_on_array = pointer_on_stack->ptr_on_array;

        if (ptr_on_array != POISON) {
            for (long long i = (long long)(pointer_on_stack->capacity + 1); i >= 0; i--) {
                if (i >= 0) { STACKPRINTF(BLUE "число: [%lf], индекс: [%lld]\n" RESET, ptr_on_array[i], i); }
                else        { return STACK_LEFT_END_OF_ARRAY; }
            }
        }

        else {
            STACKPRINTF(RED "не могу распечатать элементы по отравленному указателю.\n" RESET);
        }

        STACKPRINTF("\n");
        STACKPRINTF(GREEN "индекс самого первого свободного элемента: <%zu>\n" RESET, pointer_on_stack->size);
        STACKPRINTF("ожидаемое значение канарейки слева: <%lf>\n", pointer_on_stack->canary_left);
        STACKPRINTF("ожидаемое значение канарейки справа: <%lf> \n", pointer_on_stack->canary_right);
        STACKPRINTF(YELLOW "статус стэка: <%d>\n" RESET, pointer_on_stack->status_code);
        STACKPRINTF("-----------------------------------------------------------\n");
        STACKGETCHAR();

        if (pointer_on_stack->status_code != DESTROYED) {
            stack_potential_error check_birdies_in_end = stack_validate_birds(pointer_on_stack);

            return check_birdies_in_end;
        }

        return OK;
    }

    STACKPRINTF(YELLOW "\nОшибка произошла непосредственно в stack_dump\n" RESET);
    STACKPRINTF(YELLOW "Код ошибки: [%d]\n\n" RESET, basic_stack_verify);
    STACKGETCHAR();

    return basic_stack_verify;
}