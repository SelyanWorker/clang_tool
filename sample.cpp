

template<typename T>
class test_class{};

template<typename T>
struct test_struct{};

template<typename T>
void test_fun(){}


template<>
class test_class<double>{};

struct shit_struct {};

class shit_class {};

void foo()
{
  test_class<int> govno;
  test_struct<unsigned> govno_1;
  test_struct<shit_struct> govno_2;
  test_struct<shit_class> govno_3;
}