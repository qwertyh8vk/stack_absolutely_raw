/* NOTE

    struct Stack;

    Stack* Init (int capacity);
    void Push (Stack* st, int value);
    void Destroy (Stack* st);
    // ...

    void UserFunc (Stack* st)
    {
        st->capacity = 1000;
    }

    int main()
    {
    //    Stack sss = {}; Will NOT be compiled

        Stack* st = Init (10); // calloc
        Push (st, 100);

        st->size = -1;

        UserFunc (st);

        Destroy (st);          // free
    }

    //-------------------------------------------------

    struct Stack
    {
        int size, capacity;
        //.....
    };

    void Push (Stack* st)
    {
        st->size++;
        //......
    }

    #define main FUCK-idiot

*/