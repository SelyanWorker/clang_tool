#include <utility>

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

template<typename T, typename G>
T bar(T t, G g)
{
    return t;
}

template<typename... Args>
void foo2(Args... args)
{
    std::initializer_list<int>{ (bar(args.first, args.second), 0)... };
}

class eba
{
    class shit_place
    {
    public:
        template<typename T>
        void public_template_shit_fun()
        {
        }

        template<typename T>
        struct public_template_shit_struct
        {
        };

        void public_shit_fun() {}

        struct public_shit_struct
        {
        };

    private:
        template<typename T>
        void private_template_shit_fun()
        {
        }

        template<typename T>
        struct private_template_shit_struct
        {
        };

        void private_shit_fun() {}

        struct private_shit_struct
        {
        };
    };
};

int main()
{
    some_template_class<int> govno;
    some_template_class<unsigned> govno_1;
    some_template_class<shit_struct> govno_2;
    some_template_class<shit_class> govno_3;
    some_template_class<float> govno_4;
    some_template_class<float, int, unsigned, shit_class, shit_struct> govno_5;

    foo2(std::make_pair(double{}, int{}));
    foo2(std::make_pair(double{}, int{}));
    foo2(std::make_pair(float{}, unsigned{}));

    return 0;
}