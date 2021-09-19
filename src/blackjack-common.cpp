//#include <ostream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// Правила Black Jack: https://add-hobby.ru/blackjack.html
// Уточнения под которые сделан код:
// игра одной колодой,
// ставок нет - только статус выигрыша,
// нет даблов и триплов,
// За дилера будет играть компьютер.

enum Suit {HEARTS, SPADES, CROSS, DIAMONDS}; // Черви, Пики, Крести, Буби (в этой игре не нужно)
enum rank {
        ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
        JACK, QUEEN, KING
    };

// карта
class Card {
       rank m_Rank;
       Suit m_Suit;
       bool m_IsFaceUp=true;  // рубашкой вверх = false.
    public:
       Card(rank r = ACE, Suit s = SPADES, bool ifu = true) : m_Rank(r), m_Suit(s), m_IsFaceUp(ifu) {}
       void Flip() { m_IsFaceUp = ! m_IsFaceUp; }
       int getValue() const { 
           if (m_IsFaceUp) {
               if (m_Rank > 10)
                   return 10;
               else
                   return m_Rank;
	   }
           return 0;
       }
       void setValue(int val) { m_Rank=static_cast<::rank>(val); }
       void setValue(rank val) { m_Rank=val; }
       void setSuit(Suit s) { m_Suit=s; }
       Suit getSuit() const { return m_Suit; }
//       Card & operator=(const Card & other) {
//           cardValue=other.cardValue;
//           picture=other.picture;
//	  .setName(other.cardName);
//	  .setSuit(other.suit);
//	   return *this; 
//       }

       Card& operator=(const Card&) = default;
       friend std::ostream& operator<<(std::ostream& os, const Card& aCard);
};

// перегружает оператор <<, чтобы получить возможность отправить
// объект типа Card в поток cout
std::ostream& operator<<(std::ostream& os, const Card& aCard)
{
    const std::string RANKS[] = { "0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    const std::string SUITS[] = { "h", "s", "c", "d" };
    
    if (aCard.m_IsFaceUp)
    {
        os << RANKS[aCard.m_Rank] << SUITS[aCard.m_Suit];
    }
    else
    {
        os << "XX";
    }
    
    return os;
}


class Hand {
    protected:
        std::vector<Card*> m_Cards; // коллекция указателей карт
    public:
        Hand() { m_Cards.reserve(9); }
        virtual ~Hand() {Clear();}
        void Add(Card * pCard); // добавляет указатель на карту в m_Cards
	void Clear();   // очищает руку от карт удаляя все указатели из m_Cards
	int GetTotal() const; // возвращает сумму очков карт в руке
};

int Hand::GetTotal() const {

    // если карт в руке нет, возвращает значение 0
    if (m_Cards.empty())
    {
        return 0;
    }
    
    //если первая карта имеет значение 0, то она лежит рубашкой вверх:
    // вернуть значение 0
    if (m_Cards[0]->getValue() == 0)
    {
        return 0;
    }
    
    // находит сумму очков всех карт, каждый туз дает 1 очко
    int total = 0;
    std::vector<Card*>::const_iterator iter;
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
    {
        total += (*iter)->getValue();
    }
    
    // определяет, держит ли рука туз
    bool containsAce = false;
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
    {
        if ((*iter)->getValue() == ACE)
        {
            containsAce = true;
        }
    }
    
    // если рука держит туз и сумма довольно маленькая, туз дает 11 очков
    if (containsAce && total <= 11)
    {
        // добавляем только 10 очков, поскольку мы уже добавили
        // за каждый туз по одному очку
        total += 10;
    }
    
    return total;

}

void Hand::Add(Card * pCard) {
  m_Cards.push_back(pCard);
}

void Hand::Clear() {
    // проходит по вектору, освобождая всю память в куче
    std::vector<Card*>::iterator iter = m_Cards.begin();
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter)
    {
        delete *iter;
        *iter = 0;
    }
    m_Cards.clear();
}

class GenericPlayer : public Hand {

    protected:
    std::string m_Name; // имя игрока
    
    public:
    friend std::ostream& operator<<(std::ostream& os, const GenericPlayer& aGenericPlayer);
    GenericPlayer(const std::string & name=(std::string)"") : m_Name(name) {}
    virtual ~GenericPlayer() {}
    virtual bool isHitting() const = 0; // указывает нужна ли игроку ещё карта
    bool isBusted() const { return (Hand::GetTotal() > 21);}; // указывает что у игрока перебор
    void Bust() const { std::cout << m_Name << " получил больше 21.\n";} // объявляет что у игрока перебор
};

std::ostream& operator<<(std::ostream& os, const GenericPlayer& aGenericPlayer)
{
    os << aGenericPlayer.m_Name << ":\t";
    
    std::vector<Card*>::const_iterator pCard;
    if (!aGenericPlayer.m_Cards.empty())
    {
        for (pCard = aGenericPlayer.m_Cards.begin();
             pCard != aGenericPlayer.m_Cards.end();
             ++pCard)
        {
            os << *(*pCard) << "\t";
        }
        
        
        if (aGenericPlayer.GetTotal() != 0)
        {
            std::cout << "(" << aGenericPlayer.GetTotal() << ")";
        }
    }
    else
    {
        os << "<empty>";
    }
    
    return os;
}


// колода карт
class Deck : public Hand {
    public:
	Deck() {m_Cards.reserve(52); Populate();}
        ~Deck() {}
        void Populate(); // инициализация колоды
	void Shuffle();
	void Deal(Hand & aHand); // раздача одной карты в руку
	void AdditionalCards(GenericPlayer & aGenericPlayer); // раздаёт игроку дополнительные карты пока он может и хочет их получить
};

void Deck::AdditionalCards(GenericPlayer& aGenericPlayer)
{
    std::cout << std::endl;
    // продолжает раздавать карты до тех пор, пока у игрока не случается
    // перебор или пока он хочет взять еще одну карту
    while (!(aGenericPlayer.isBusted()) && aGenericPlayer.isHitting())
    {
        Deal(aGenericPlayer);
	std::cout << aGenericPlayer << std::endl;

        if (aGenericPlayer.isBusted())
        {
            aGenericPlayer.Bust();
        }
    }
}


void Deck::Shuffle()
{
    random_shuffle(m_Cards.begin(), m_Cards.end());
}


void Deck::Deal(Hand& aHand)
{
    if (!m_Cards.empty())
    {
        aHand.Add(m_Cards.back());
        m_Cards.pop_back();
    }
    else
    {
        std::cout << "Out of cards. Unable to deal.";
    }
}

void Deck::Populate()
{
    Clear();
    // создает стандартную колоду
    for (int s = HEARTS; s <= DIAMONDS; ++s)
    {
        for (int r = ACE; r <= KING; ++r)
        {
            Add(new Card(static_cast<::rank>(r),
                         static_cast<::Suit>(s)));
        }
    }
}


class Player : public GenericPlayer {
    public:
	Player(const std::string & name=(std::string)"") : GenericPlayer(name) {};
	virtual ~Player() {}
        virtual bool isHitting() const; // показывает нужна ли игроку ещё карта
	void Win() const; // объявляет что игрок выиграл
	void Lose() const; // объявляет что игрок проиграл
	void Push() const; // объявляет что игрок сыграл вничью
};

bool Player::isHitting() const
{
    std::cout << m_Name << ", хотите ещё карту? (Y/N): ";
    char response;
    std::cin >> response;
    return (response == 'y' || response == 'Y');
}

void Player::Win() const
{
   std::cout << m_Name << " выиграл.\n";
}

void Player::Lose() const
{
    std::cout << m_Name << " проиграл.\n";
}

void Player::Push() const
{
    std::cout << m_Name << " сыграл в ничью.\n";
}


class House :public GenericPlayer {
    public:
	House(const std::string name=(std::string)"Дилер") : GenericPlayer(name) {}
	virtual ~House() {};
        virtual bool isHitting() const; // показывает нужна ли игроку ещё карта
	void FlipFirstCard(); // переворачивает первую карту
};

bool House::isHitting() const
{
    return (GetTotal() <= 16);
}

void House::FlipFirstCard()
{
    if (!(m_Cards.empty()))
    {
        m_Cards[0]->Flip();
    }
    else
    {
        std::cout << "No card to flip!\n";
    }
}



class Game {
  Deck m_Deck;    // колода карт
  House m_House;  // рука дилера
  std::vector<Player> m_Players; // группа игроков людей
  public:
      Game(const std::vector<std::string> & n);
      ~Game() {}
      void Play();
};

Game::Game(const std::vector<std::string>& names)
{
    // создает вектор игроков из вектора с именами
    std::vector<std::string>::const_iterator pName;
    for (pName = names.begin(); pName != names.end(); ++pName)
    {
        m_Players.push_back(Player(*pName));
    }

    // запускает генератор случайных чисел
    srand(static_cast<unsigned int>(time(0)));
    m_Deck.Populate();
    m_Deck.Shuffle();
}

void Game::Play()
{
    // раздает каждому по две стартовые карты
    std::vector<Player>::iterator pPlayer;
    for (int i = 0; i < 2; ++i)
    {
        for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
        {
            m_Deck.Deal(*pPlayer);
        }
        m_Deck.Deal(m_House);
    }

    // прячет первую карту дилера
    m_House.FlipFirstCard();

    // открывает руки всех игроков
    for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
    {
        std::cout << *pPlayer << std::endl;
    }
    std::cout << m_House << std::endl;

    // раздает игрокам дополнительные карты
    for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
    {
        m_Deck.AdditionalCards(*pPlayer);
    }

    // показывает первую карту дилера
    m_House.FlipFirstCard();
    std::cout << std::endl << m_House;

    // раздает дилеру дополнительные карты
    m_Deck.AdditionalCards(m_House);

    if (m_House.isBusted())
    {
        // все, кто остался в игре, побеждают
        for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
        {
            if (!(pPlayer->isBusted()))
            {
                pPlayer->Win();
            }
        }
    }
    else
    {
        // сравнивает суммы очков всех оставшихся игроков с суммой очков дилера
        for (pPlayer = m_Players.begin(); pPlayer != m_Players.end();
             ++pPlayer)
        {
            if (!(pPlayer->isBusted()))
            {
                if (pPlayer->GetTotal() > m_House.GetTotal())
                {
                    pPlayer->Win();
                }
                else if (pPlayer->GetTotal() < m_House.GetTotal())
                {
                    pPlayer->Lose();
                }
                else
                {
                    pPlayer->Push();
                }
            }
        }

    }

    // очищает руки всех игроков
    for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer)
    {
        pPlayer->Clear();
    }
    m_House.Clear();
}


int main(int argc, char ** argv)
{
    std::cout << "\t\tWelcome to Blackjack!\n\n";

    int numPlayers = 0;
    while (numPlayers < 1 || numPlayers > 7)
    {
	std::cout << "How many players? (1 - 7): ";
	std::cin >> numPlayers;
    }

    std::vector<std::string> names;
    std::string name;
    for (int i = 0; i < numPlayers; ++i)
    {
	std::cout << "Enter player name: ";
	std::cin >> name;
        names.push_back(name);
    }
    std::cout << std::endl;

    // игровой цикл
    Game aGame(names);
    char again = 'y';
    while (again != 'n' && again != 'N')
    {
        aGame.Play();
	std::cout << "\nDo you want to play again? (Y/N): ";
	std::cin >> again;
    }

    return 0;
}
