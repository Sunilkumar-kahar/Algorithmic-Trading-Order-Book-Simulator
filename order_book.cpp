#include<iostream>
#include"order_book.hpp"
#include<iomanip>

using namespace std;

//defining functions(setter and getter) of order-class 
Order::Order(int id, OrderSide side, int qty, double price) : order_id(id), side(side), og_qty(qty), qty(qty), price(price){ }
int Order::get_order_id() const { return order_id; }
double Order::get_price() const { return price; }
int Order::get_qty() const { return qty; }
int Order::get_og_qty() const { return og_qty; }
OrderSide Order::get_order_side() const { return side; }
void Order::set_price(double price){ this->price = price; }
void Order::set_qty(int qty){ this->qty = qty; }
void Order::set_og_qty(int qty){ og_qty = qty; }
void Order::display_order(){
    cout<<"---------------------------------------------------------"<<endl;
    if(get_order_side() == OrderSide::BUY)
        cout<<"Side:"<<"BUY";
    else
        cout<<"Side:"<<"SELL";
    cout<<" Id:"<<get_order_id()<<" Qty:"<<get_qty()<<" Price:"<<get_price()<<endl;
    cout<<"---------------------------------------------------------"<<endl;
}


//Placing order in order-book and if match is found than execute it
//2nd argument is use for check if it is new order or old(coming afetr update_order())
bool OrderBook :: place_order(Order& order, bool update){
    double price = order.get_price();
    int qty = order.get_qty();
    int id = order.get_order_id();
    OrderSide side = order.get_order_side();

    if(qty <= 0 || price <= 0.0){
        cout<<"Your Quantity/Price wrong. qty:"<<qty<<"price:"<<price<<endl;
        return false;
    }

    //Placing order on BUY side
    if(side == OrderSide::BUY){
        list<Order>:: iterator it;
        buy_book[price].push_back(order);
        total_qty[price].buy += qty;
        it = --buy_book[price].end();
        order_position.emplace(id, it);
    }
    else if(side == OrderSide::SELL){ //Placing order on SELL side
        list<Order>:: iterator it;
        sell_book[price].push_back(order);
        total_qty[price].sell += qty;
        it = --sell_book[price].end();
        order_position.emplace(id, it);
    }

    if(update == false){
        cout<<"Your order is placed."<<endl;
    }
    else{
        cout<<"Your order is updated."<<endl;
    }
    order.display_order();

    //if opposite-order match with this order then execute.
    if(match_order()){
        execute_ord();
    }
    return true;
}

//check if order are ready for execution or not
bool OrderBook:: match_order(){

    if(sell_book.empty() || buy_book.empty())
        return false;

    map <double, list<Order>>::iterator s_it = sell_book.begin();
    double sPrice = s_it->first;
    
    map <double, list<Order>, greater<double>>:: iterator b_it = buy_book.begin();
    double bPrice = b_it->first;

    if(sPrice <= bPrice)
        return true;
    return false;
}

//Execute orders and do manipulation on buy_book, sell_book, total_qty, order_position
void OrderBook:: execute_ord(){
    bool all_qty_executed = false;
    while(all_qty_executed == false){
        map <double, list<Order>>::iterator s_it = sell_book.begin();
        int sQty = s_it->second.front().get_qty();
        double sPrice = s_it->second.front().get_price();
        int sId = s_it->second.front().get_order_id();
        
        map <double, list<Order>, greater<double>>:: iterator b_it = buy_book.begin();
        int bQty = b_it->second.front().get_qty();
        double bPrice = b_it->second.front().get_price();
        int bId = b_it->second.front().get_order_id();

        int qty_diff = sQty - bQty;

        b_it->second.front().display_order();
        
        if(qty_diff > 0){
            b_it->second.pop_front();
            if(b_it->second.empty()){
                buy_book.erase(bPrice);
            }
            total_qty[sPrice].sell -= bQty;
            total_qty[bPrice].buy -= bQty;
            s_it->second.front().set_qty(qty_diff);
            order_position.erase(bId);
            cout<<"Id:"<<bId<<" All qty executed successfully."<<endl;
        }
        else if(qty_diff < 0){
            s_it->second.pop_front();
            if(s_it->second.empty()){
                sell_book.erase(sPrice);
            }
            total_qty[sPrice].sell -= sQty;
            total_qty[bPrice].buy -= sQty;
            b_it->second.front().set_qty(-qty_diff);
            order_position.erase(sId);
            cout<<"Id:"<<sId<<" All qty executed successfully."<<endl;
        }
        else{
            s_it->second.pop_front();
            b_it->second.pop_front();
            if(s_it->second.empty()){
                sell_book.erase(sPrice);
            }
            if(b_it->second.empty()){
                buy_book.erase(bPrice);
            }
            total_qty[sPrice].sell -= bQty;
            total_qty[bPrice].buy -= sQty;
            order_position.erase(sId);
            order_position.erase(bId);
            cout<<"Id:"<<sId<<" All qty executed successfully."<<endl;
            cout<<"Id:"<<bId<<" All qty executed successfully."<<endl;
        }
        if(total_qty[bPrice].buy == 0 && total_qty[bPrice].sell == 0)
            total_qty.erase(bPrice);
        if(bPrice != sPrice && total_qty[sPrice].buy == 0 && total_qty[sPrice].sell == 0)
            total_qty.erase(sPrice);

        if(match_order() == false)
            all_qty_executed = true;
    }
}

//use for cancel exesting order
void OrderBook:: cancel_order(int order_id, bool update){
    unordered_map <int, list<Order>::iterator > ::iterator it = order_position.find(order_id);
    if(it == order_position.end()){
        cout<<"Order Id:"<<order_id<<" doesn't exist."<<endl;
    }
    else{
        (*it->second).display_order();
        Order ord = (*it->second);
        OrderSide side = ord.get_order_side();
        double price = ord.get_price();
        if(side == OrderSide::BUY){
            total_qty[price].buy -= ord.get_qty();
            buy_book[price].erase(it->second);
            if(buy_book[price].empty())
                buy_book.erase(price);
        }
        else{
            total_qty[price].sell -= ord.get_qty();
            sell_book[price].erase(it->second);
            if(sell_book[price].empty())
                sell_book.erase(price);
        }
        if(total_qty[price].buy == 0 && total_qty[price].sell == 0){
            total_qty.erase(price);
            cout<<"deleted"<<endl;
        }
        order_position.erase(order_id);
        if(update == false)
            cout<<"Your order id:"<<order_id<<" is deleted successfully."<<endl;
    }
}

//View order book
void OrderBook:: view_order_book(){
    //printf("%-8s%-8s%-8s%-8s\n","Qty", "Bid", "Ask", "Qty");
    cout<<left<<setw(8)<<"Qty"<<setw(8)<<"Bid"<<setw(8)<<"Ask"<<setw(8)<<"Qty"<<endl;

    if(buy_book.empty() && sell_book.empty()){
        cout<<"Order Book is Empty."<<endl;
        return;
    }

    int length = (buy_book.size() > sell_book.size()) ? buy_book.size() : sell_book.size();
    map <double, list<Order>> :: iterator sit = sell_book.begin();
    map <double, list<Order>, greater<double>>:: iterator bit = buy_book.begin();

    for(int i = 0; i < length; i++){
        if(bit != buy_book.end()){
            cout<<left<<setw(8)<<total_qty[bit->first].buy<<setw(8)<<bit->first;
            bit++;
        }
        else{
            cout<<left<<setw(8)<<"--"<<setw(8)<<"--";
        }

        if(sit != sell_book.end()){
            cout<<left<<setw(8)<<sit->first<<setw(8)<<total_qty[sit->first].sell<<endl;
            sit++;
        }
        else{
            cout<<left<<setw(8)<<"--"<<setw(8)<<"--"<<endl;
        }
    }
}

//Update not partially executed order only.
void OrderBook:: update_order(int ord_id){
    unordered_map <int, list<Order>::iterator>:: iterator um_it = order_position.find(ord_id);
    int option = 0;
    int qty;
    double price;
    if(um_it == order_position.end()){
        cout<<"Your order Not found. Either you order not Placed or Fully executed or Canceled."<<endl;
        return;
    }
    list<Order>:: iterator it = um_it->second;
    Order order = *it;
    if((*it).get_og_qty() != (*it).get_qty() ){
        cout<<"Your order is partially executed, so order updation not possible"<<endl;
        cout<<"But you can cancle this order and place new order."<<endl;
        return;
    }
    while(1){
        cout<<"1.Update Quantity."<<endl;
        cout<<"2.Update Price."<<endl;
        cout<<"3.Update Both"<<endl;
        cin>>option;
        if(option == 1 || option == 3){
            cout<<"Enter Update Qty:";
            while(1){
                cin>>qty;
                if(qty <= 0){
                    cout<<"Wrong Qty:"<<qty<<endl;
                    cout<<"Enter Update Qty Again:";
                }
                else
                    break;
            }
        }
        if(option == 1){
            if(qty == order.get_qty()){
                cout<<" prev_qty:"<<order.get_qty()<<" update_qty:"<<qty<<endl;
            }
            else{
                it->set_qty(qty);
                it->set_og_qty(qty);
            }
        }
        else if(option == 2 || option == 3){
            cout<<"Enter Update Price:";
            while(1){
                cin>>price;
                if(price <= 0){
                    cout<<"Wrong Price:"<<price<<endl;
                    cout<<"Enter Update Price Again:";
                }
                else
                    break;
            }
        }
        else{
            cout<<"You enterd wrong option."<<endl;
        }
        if(option == 2 || option == 3){
            cancel_order(ord_id, true);
            order.set_price(price);
            if(option == 3){
                order.set_qty(qty);
                order.set_og_qty(qty);
            }
            place_order(order, true);
            break;
        }

        if(option == 1 || option == 3){
            if(order.get_order_side() == OrderSide::BUY)
                total_qty[order.get_price()].buy += qty - order.get_qty();
            else
                total_qty[order.get_price()].sell += qty - order.get_qty();
            break;
        }
    }

}
