#include <deque>
#include <iostream>

class Integer {
public:
    class IntegerProxy {
    public:
        std::deque<int> valuesToCompare_;
    public:
        IntegerProxy( Integer &integer, Integer anotherInteger ) : valuesToCompare_({ integer.value_, anotherInteger.value_ }){

        }

        operator bool(){
            for(std::size_t i = 0; i < valuesToCompare_.size() - 1; ++i) {
                if(valuesToCompare_.at(i) <= valuesToCompare_.at(i + 1)) {
                    return false;
                }
            }
            return true;
        };

        IntegerProxy &operator>( Integer &integer ){
            valuesToCompare_.push_back(integer.value_);
            return *this;
        }

        IntegerProxy &operator>( int integer ){
            valuesToCompare_.push_back(integer);
            return *this;
        }
    };

private:
    int value_;
public:
    Integer( int x ) : value_(x){

    }

    explicit operator int(){
        return value_;
    }

    IntegerProxy operator>( Integer &integer ){
        return IntegerProxy(*this, integer);
    }

    IntegerProxy operator>( int integer ){
        return IntegerProxy(*this, Integer(integer));
    }
};

int main(){
    Integer a = 5;
    if(a > 4 > 3) {
        std::cout << ( a > 4 ) << std::endl;
    }
    if(a > 4 < 5) {
        std::cout << "yoo!" << std::endl;
    }
    if(Integer(4) > a < 4 < 5) {
        std::cout << "goo!" << std::endl;
    }
}
