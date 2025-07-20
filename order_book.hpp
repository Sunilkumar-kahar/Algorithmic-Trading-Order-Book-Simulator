#include<map>
#include<list>
#include<unordered_map>

using namespace std;

enum class OrderSide {BUY, SELL};

// Represents a single order in the order book (buy or sell)
class Order{
    private:
        int order_id;
        double price;
        int og_qty;
        int qty;
        OrderSide side;
    public:
        Order(){    }
        Order(int id, OrderSide side, int qty, double price);
        int get_order_id() const;
        double get_price() const;
        int get_qty() const;
        int get_og_qty() const;
        OrderSide get_order_side() const;
        void set_price(double price);
        void set_qty(int qty);
        void set_og_qty(int qty);
        void display_order();
};

// Maintains the buy/sell order book and handles matching, updating, and cancellation
class OrderBook{
    private:
        class Price_Qty{
            public:
                int buy = 0;
                int sell = 0;
        };

        map <double, list<Order>, greater<double>> buy_book;
        map <double, list<Order>> sell_book;
        map <double, Price_Qty> total_qty;
        unordered_map <int, list<Order>::iterator > order_position;

        bool match_order();
        void execute_ord();
    public:
        bool place_order(Order& order, bool update = false);
        void cancel_order(int order_id, bool update = false);
        void view_order_book();
        void update_order(int ord_id);
};
