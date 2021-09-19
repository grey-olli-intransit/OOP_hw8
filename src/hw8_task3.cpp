/* Делать третье задание было ну очень лениво, так что я взял образец у сокурсника и немного его перепилил, чтобы
 * собиралось и работало на линуксе, ну и чтоб работало чуть иначе.
 */
#include <exception>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <cstring>
#include <vector>

using namespace std;

constexpr int field_size = 10;

class OffTheField : public exception {
   int i_x;
   int i_y;
   string i_error;
   public:
       OffTheField(const int x, const int y);
       const char* what() const noexcept override;
};

const char* OffTheField::what() const noexcept {
   return i_error.c_str();
}


class IllegalCommand : public exception {
   int i_x;
   int i_y;
   string i_error;
   public:
       IllegalCommand(char key);
       const char* what() const noexcept override;
};

const char* IllegalCommand::what() const noexcept {
   return i_error.c_str();
}

IllegalCommand::IllegalCommand(char key) : 
i_error(string("Неверная клавиша управления (можно только hjkl и q): '") + key + string("'"))
{}


class Robot;

class CanGo {
public:
   virtual void go(Robot& r) const = 0;
};

class GoUp final: public CanGo {
public:
   void go(Robot& r) const override;
};

class GoDown final : public CanGo {
public:
   void go(Robot& r) const override;
};

class GoLeft final : public CanGo {
public:
   void go(Robot& r) const override;
};

class GoRight final: public CanGo {
public:
   void go(Robot& r) const override;
};

class Robot {
   int i_x;
   int i_y;
   map<char, unique_ptr<CanGo>> i_dirs_map;
   char i_field[field_size][field_size];
   std::vector<char> i_history;
public:
   Robot(int x, int y);
   void show() const;
   void go(char key);
   void newPlace(int x_dir, int y_dir);
   int getX() const;
   int getY() const;
   string get_history();
};

string Robot::get_history() {
   string s = string("");
   std::vector<char>::const_iterator it; 
   for(it=i_history.begin(); it != i_history.end();++it)
       s.append(sizeof(char), *it);
   return s;
}

OffTheField::OffTheField(const int x, const int y) : i_x{x}, i_y{y},
   i_error{
      string("Координаты робота вне поля ") + to_string(field_size) + 
      string("x") + to_string(field_size) +  string(" (старт был в x=1, y=1)): x=") + 
      to_string(x) + ", y=" + to_string(y)
     }
{}


void GoUp::go(Robot& r) const {
   int x = r.getX();
   int y = r.getY() - 1;
   r.newPlace(x, y);
}

void GoDown::go(Robot& r) const {
   int x = r.getX();
   int y = r.getY() + 1;
   r.newPlace(x, y);
}

void GoLeft::go(Robot& r) const {
   int x = r.getX() - 1;
   int y = r.getY();
   r.newPlace(x, y);
}

void GoRight::go(Robot& r) const {
   int x = r.getX() + 1;
   int y = r.getY();
   r.newPlace(x, y);
}

Robot::Robot(int x, int y) : i_x{x}, i_y{y} {
   if (x < 1 or x > field_size or y < 1 or y > field_size) {
      throw OffTheField(x, y);
   }
   std::memset(i_field, '.', field_size * field_size);
   i_field[y - 1][x - 1] = 'R';
   i_dirs_map.emplace('l', make_unique<GoRight>());
   i_dirs_map.emplace('h', make_unique<GoLeft>());
   i_dirs_map.emplace('k', make_unique<GoUp>());
   i_dirs_map.emplace('j', make_unique<GoDown>());
}

void Robot::go(char key) {
   if (i_dirs_map.find(key) == i_dirs_map.end()) {
      throw IllegalCommand(key);
   }
   i_history.push_back(key);
   i_dirs_map.at(key)->go(*this);
}

void Robot::show() const
{
   system("clear");
   for(size_t i = 0; i < field_size; ++i) {
      for (size_t j = 0; j < field_size; ++j) {
         cout << i_field[i][j] << ' ';
      }
      cout << endl;
   }
}

void Robot::newPlace(int x, int y)
{
   if (x < 1 or x > field_size or y < 1 or y > field_size) {
      throw OffTheField(x, y);
   }
   i_field[i_y - 1][i_x - 1] = '.';
   i_x = x;
   i_y = y;
   i_field[i_y - 1][i_x - 1] = 'R';
}

int Robot::getX() const {
   return i_x;
}

int Robot::getY() const {
   return i_y;
}

int main(int argc, char ** argv) {

try{
      Robot robot(1, 1);
      robot.show();
      char choice;
      int ch;
      while(true) {
         cout << endl << "Робот ходит только по горизонтали и вертикали." << endl;
         cout << "история команд: " << robot.get_history() << " ." << endl;
         cout << "'k' = вверх, 'j' = вниз, 'l' = направо, 'h' = налево, or 'q' = выйти." << endl;
         cout << "Команда завершается <Enter>. Выход за границу = выход из программы." << endl;
         cout << "Ожидаю клавишу управления: " << endl;
         ch = getchar(); getchar();
         choice = tolower(ch);
         if (choice == 'q') {
            break;
         }
         try {
            robot.go(choice);
         }
         catch(const IllegalCommand& ic) {
            cout << endl << ic.what() << endl;
            cout << "Нажмите ввод чтобы продолжить." << endl;
            ch = getchar();
         }
         robot.show();
      }
   }
   catch (const OffTheField& otf) {
      cerr << otf.what() << endl;
      return 1;
   }

   return 0;
}
