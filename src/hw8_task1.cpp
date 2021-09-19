#include <iostream>
#include <fstream>

using namespace std;

// Написать шаблонную функцию div, которая должна вычислять результат деления двух параметров и запускать исключение 
// DivisionByZero, если второй параметр равен 0. 
// В функции main выводить результат вызова функции div в консоль, а также ловить исключения.

class DivisionByZero : public runtime_error {
   public:
   DivisionByZero()
   : runtime_error("Math error: Attempted to divide by Zero.\n") {
   }
   friend ostream & operator<<(ostream & os,DivisionByZero);
};

ostream & operator<<(ostream & os, DivisionByZero) {
  os << "Math error: Attempted to divide by Zero.\n";
  return os;
}

template<typename T>
T div1(const T & a, const T & b) {
  if (b == ((T)0)) 
      throw DivisionByZero();
  else
     return (a/b);
}


int main(int argc, char ** argv) {
    for (auto d : {0.0,1.0,2.0}) {
      try {
        auto result=div1(double(1),d);
        cout << "1/"<< d << "=" << result << endl;
      }
      catch(DivisionByZero & e) {
	cout << "Catched w/ DivisionByZero.\n";
        cout << e << endl;
      }
      catch(...) {
        cout << "Finally catch all. Exception.";
      }
    }
    return 0;	
}
