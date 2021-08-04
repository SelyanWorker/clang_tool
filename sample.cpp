

template<typename... ArgvT>
class some_template_class
{
};

template<>
class some_template_class<double>
{
};

struct shit_struct
{
};

class shit_class
{
};

template<typename... ArgvT>
void foo(ArgvT... argv)
{
}

int main()
{
    some_template_class<int> govno;
    some_template_class<unsigned> govno_1;
    some_template_class<shit_struct> govno_2;
    some_template_class<shit_class> govno_3;
    some_template_class<float> govno_4;
    some_template_class<float, int, unsigned, shit_class, shit_struct> govno_5;

    foo(double{});
    foo(double{}, int{}, unsigned{});
    foo(double{}, int{}, unsigned{}, shit_struct{}, shit_class{});

    return 0;
}