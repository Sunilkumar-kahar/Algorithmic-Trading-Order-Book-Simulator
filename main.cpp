#include<iostream>
#include"order_book.hpp"

using namespace std;

// Entry point: Provides a simple CLI for interacting with the order book
int main(){
    OrderBook book;
    OrderSide side;
    int id = 0;

    while (1){
        int choice = 0;
        int ord_id;
        cout<<"Enter Your Choise:"<<endl;
        cout<<"1.Place Your Order"<<endl;
        cout<<"2.Cancel Your Order"<<endl;
        cout<<"3.View Order Book"<<endl;
        cout<<"4.Update Your order."<<endl;
        cout<<"5.Exit"<<endl;
        cin>>choice;
        switch (choice){
            case 1:
                while(1){
                    int option = 0;
                    cout<<"Enter Your Option:"<<endl;
                    cout<<"1.BUY"<<endl;
                    cout<<"2.SELL"<<endl;
                    cout<<"3.Exit"<<endl;
                    cin>>option;
                    if(option == 1)
                        side = OrderSide::BUY;
                    else if(option == 2)
                        side = OrderSide::SELL;
                    else if(option == 3)
                        break;
                    else
                        cout<<">>>>>You entered wrong option.Enter again.<<<<<"<<endl;
                    if(option == 1 || option == 2){
                        int qty;
                        double price;
                        cout<<"Enter Order Quantity:";
                        while(1){
                            cin>>qty;
                            if(qty > 0)
                                break;
                            cout<<"Wrong quantity enter again:";
                        }
                        cout<<"Enter price:";
                        while(1){
                            cin>>price;
                            if(price > 0)
                                break;
                            cout<<"Wrong price enter again:";
                        }
                        id++;
                        Order ord(id, side, qty, price);
                        bool result = book.place_order(ord);
                        if(result == false){
                            cout<<"Order is not placed. Someting went wrong.";
                            id--;
                            exit(0);
                        }
                        break;
                    }
                }
                break;

            case 2:
                    cout<<"Inside cancel order"<<endl;
                    int id;
                    cout<<"Enter order id:";
                    cin>>id;
                    book.cancel_order(id);
                    break;
            case 3:
                    cout<<"Inside view Order"<<endl;
                    book.view_order_book();
                    break;
            case 4:
                    ord_id = 0;
                    cout<<"Enter your order id:";
                    cin>>ord_id;
                    book.update_order(ord_id);
                    break;
            case 5:
                cout<<"Thank You."<<endl;
                exit(0);
            default:
                cout<<"You enterd wrong option.\nPlease Select correct option."<<endl;
        }
    }


    return 0;
}