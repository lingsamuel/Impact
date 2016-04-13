#include <type_traits>

using namespace std;

class Overloaded
{
    bool operator ==(const Overloaded&)
    {
        return false;
    }
};

class NotOverloaded
{
};

template<typename T>
class IsOperatorEqualOverloaded {
    template<typename C> static true_type test(decltype(&T::operator==)); // selected if C is a class type
    template<typename C> static false_type test(...);      // selected otherwise
public:
    using result = decltype(test<T>(nullptr));
};

int main()
{
    IsOperatorEqualOverloaded<Overloaded>::result x;
    IsOperatorEqualOverloaded<NotOverloaded>::result y;
}