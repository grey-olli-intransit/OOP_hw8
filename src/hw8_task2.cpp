#include <iostream>

// Написать класс Ex, хранящий вещественное число x и имеющий конструктор по вещественному числу, 
// инициализирующий x значением параметра. 
// Написать класс Bar, хранящий вещественное число y (конструктор по умолчанию инициализирует его нулем) и
//  имеющий метод set с единственным вещественным параметром a. Если y + a > 100, возбуждается исключение типа Ex с данными a*y,
//  иначе в y заносится значение a.
// В функции main завести переменную класса Bar и в цикле в блоке try вводить с клавиатуры целое n. Использовать его в качестве параметра метода set до тех пор, пока не будет введено 0. В обработчике исключения выводить сообщение об ошибке, содержащее данные объекта исключения.

class Ex {
    double x;
    public:
        Ex(double p) : x{p} {}
	double getX() const {return x;}

};

class Bar {
    double y;
    public:
        Bar(double p = 0.0) : y{p} {}
	void set(double a) {
	    if (y+a > 100)
		 throw Ex(a*y);
	    else
		 y=a;
	}

};

int main(int argc, char ** argv) {
    Bar b;
    int n;
    do {
        std::cout << "Enter integer (0 to exit): ";
        std::cin >> n;
        try{
               b.set(n);
        }
        catch(Ex e) {
            std::cout << "Exception Ex with value " << e.getX() << "\n";
        }
    } while(n != 0);
    return 0;	
}
