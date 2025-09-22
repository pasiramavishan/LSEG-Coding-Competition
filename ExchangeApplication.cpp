#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <ctime>

using namespace std;

class Order {
public:
    string cl_ord_id;
    string instrument;
    string side;
    int quantity;
    string price;
    string order_id;
    string timeStr;

    static int orderCounter;

    Order(string clOrdId, string inst, string s, int qty, string prc, string timeStr)
        : cl_ord_id(clOrdId), instrument(inst), side(s), quantity(qty), price(prc), timeStr(timeStr) {
        order_id = "ord" + to_string(orderCounter++);
    }

    double getPriceAsDouble() const {
        return stod(price);
    }
};

int Order::orderCounter = 1;

bool compareBuyOrders(const Order& a, const Order& b) {
    return a.getPriceAsDouble() > b.getPriceAsDouble();
}

bool compareSellOrders(const Order& a, const Order& b) {
    return a.getPriceAsDouble() < b.getPriceAsDouble();
}

class OrderBook {

private:
    vector<Order> buyOrders;
    vector<Order> sellOrders;

    bool validOrder(const Order& order, ofstream& csvFile) {
        vector<string> validProducts = { "Rose", "Lavender", "Lotus", "Tulip", "Orchid" };
        if (find(validProducts.begin(), validProducts.end(), order.instrument) == validProducts.end()) {
            writeOrderToCSV(csvFile, order, order.instrument, order.quantity, "Rejected", order.price, "Invalid instrument", order.timeStr);
            return false;
        }
        if (order.side != "1" && order.side != "2") {
            writeOrderToCSV(csvFile, order, order.instrument, order.quantity, "Rejected", order.price, "Invalid side", order.timeStr);
            return false;
        }
        if (order.quantity < 10 || order.quantity >= 1000 || order.quantity % 10 != 0) {
            writeOrderToCSV(csvFile, order, order.instrument, order.quantity, "Rejected", order.price, "Invalid size", order.timeStr);
            return false;
        }
        if (stoi(order.price) <= 0) {
            writeOrderToCSV(csvFile, order, order.instrument, order.quantity, "Rejected", order.price, "Invalid price", order.timeStr);
            return false;
        }
        return true;
    }

    void processOrder(Order& incomingOrder, vector<Order>& opposingOrders, bool isBuy, ofstream& csvFile) {
        int remainingQty = incomingOrder.quantity;
        int flag = 0;  //To track the previously executed orders

        //Check if the incoming order satify the required conditions
        while (!opposingOrders.empty() && remainingQty > 0 &&
            ((isBuy && incomingOrder.getPriceAsDouble() >= opposingOrders.front().getPriceAsDouble()) ||
                (!isBuy && incomingOrder.getPriceAsDouble() <= opposingOrders.front().getPriceAsDouble()))) {
            flag = 1;
            Order matchedOrder = opposingOrders.front();

            int tradeQty = min(remainingQty, matchedOrder.quantity);

            if (remainingQty >= matchedOrder.quantity)
            {
                opposingOrders.erase(opposingOrders.begin()); //Erase entry if its quantity is less than incoming order quantity
            }
            else
            {
                opposingOrders.front().quantity -= tradeQty; //Otherwise decrease the first entry quantyty
            }
            //Decrease all quantities
            remainingQty -= tradeQty;
            incomingOrder.quantity -= tradeQty;
            matchedOrder.quantity -= tradeQty;
            executeTrade(incomingOrder, matchedOrder, csvFile, tradeQty);
        }

        //If the incoming order quantity remains, add it to the orderbook
        if (remainingQty > 0) {
            Order newOrder = incomingOrder;
            newOrder.quantity = remainingQty;
            if (isBuy) {
                buyOrders.push_back(newOrder);
            }
            else {
                sellOrders.push_back(newOrder);
            }
            if (flag == 0) {
                writeOrderToCSV(csvFile, newOrder, newOrder.instrument, remainingQty, "New", newOrder.price, "Accepted", newOrder.timeStr);
            }
        }
    }

public:
    //To add orders to orderbook
    void addOrder(Order& order, ofstream& csvFile) {
        bool isValidOrder = validOrder(order, csvFile);
        if (isValidOrder) {
            if (order.side == "1") {
                processOrder(order, sellOrders, true, csvFile);
            }
            else if (order.side == "2") {
                processOrder(order, buyOrders, false, csvFile);
            }
        }
    }

    void executeTrade(Order& order1, Order& order2, ofstream& csvFile, int tradeQty) {
        string status1 = (order1.quantity == 0) ? "Fill" : "PFill";
        string status2 = (order2.quantity == 0) ? "Fill" : "PFill";

        writeOrderToCSV(csvFile, order1, order1.instrument, tradeQty, status1, order2.price, "Accepted", order2.timeStr);
        writeOrderToCSV(csvFile, order2, order2.instrument, tradeQty, status2, order2.price, "Accepted", order2.timeStr);
    }

    void sortOrders() {
        sort(buyOrders.begin(), buyOrders.end(), compareBuyOrders);
        sort(sellOrders.begin(), sellOrders.end(), compareSellOrders);
    }

    void writeOrderToCSV(ofstream& csvFile, const Order& order, const string& instrument, int quantity, const string& status, const string& price, const string& rejectionReason = "Accepted", const string& timeStr = " ") const {
        csvFile << order.order_id << ","
            << order.cl_ord_id << ","
            << instrument << ","
            << order.side << ","
            << status << ","
            << quantity << ","
            << price << ","
            << rejectionReason << ","
            << timeStr << "\n";
    }

    void displayOrderBook() const {
        cout << "Orderbook:" << endl;
        cout << setw(10) << "Buy Side (Blue)" << endl;
        cout << setw(10) << "Price" << setw(10) << "Qty" << setw(10) << "Order ID" << endl;
        for (const auto& order : buyOrders) {
            cout << setw(10) << order.price << setw(10) << order.quantity << setw(10) << order.order_id << endl;
        }

        cout << "\nSell Side (Pink)" << endl;
        cout << setw(10) << "Price" << setw(10) << "Qty" << setw(10) << "Order ID" << endl;
        for (const auto& order : sellOrders) {
            cout << setw(10) << order.price << setw(10) << order.quantity << setw(10) << order.order_id << endl;
        }
    }
};

OrderBook roseOrderBook;
OrderBook lavenderOrderBook;
OrderBook lotusOrderBook;
OrderBook tulipOrderBook;
OrderBook orchidOrderBook;

OrderBook* getOrderBook(const string& instrument) {
    if (instrument == "Rose") {
        return &roseOrderBook;
    }
    else if (instrument == "Lavender") {
        return &lavenderOrderBook;
    }
    else if (instrument == "Lotus") {
        return &lotusOrderBook;
    }
    else if (instrument == "Tulip") {
        return &tulipOrderBook;
    }
    else if (instrument == "Orchid") {
        return &orchidOrderBook;
    }
    return nullptr;
}

int main() {
    ifstream file("orders_final.csv");
    if (!file) {
        cout << "Error: File could not be opened!" << endl;
        return 1;
    }

    ofstream csvFile("execution_rep_final.csv");
    if (!csvFile) {
        cout << "Error: Could not open the CSV file for writing!" << endl;
        return 1;
    }

    csvFile << "execution_rep.csv\n";
    csvFile << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction time\n";

    string line;
    getline(file, line);
    getline(file, line);
    while (getline(file, line)) {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm localTime;
        localtime_s(&localTime, &currentTime);

        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        char timeStr[30];
        sprintf_s(timeStr, "%04d.%02d.%02d-%02d.%02d.%02d-%03lld",
            1900 + localTime.tm_year,
            1 + localTime.tm_mon,
            localTime.tm_mday,
            localTime.tm_hour,
            localTime.tm_min,
            localTime.tm_sec,
            static_cast<long long>(milliseconds.count()));

        stringstream ss(line);
        string cl_ord_id, instrument, side, price, quantityStr;
        int quantity;

        if (!getline(ss, cl_ord_id, ',')) break;
        if (!getline(ss, instrument, ',')) break;
        if (!getline(ss, side, ',')) break;
        if (!getline(ss, quantityStr, ',')) break;
        if (!getline(ss, price, ',')) break;

        try {
            quantity = stoi(quantityStr);
        }
        catch (const exception& e) {
            quantity = 0;
            Order order(cl_ord_id, instrument, side, quantity, price, timeStr);
            csvFile << order.order_id << ","
                << cl_ord_id << ","
                << instrument << ","
                << side << ","
                << "Rejected" << ","
                << quantityStr << ","
                << price << ","
                << "Invalid quantity" << ","
                << timeStr << "\n";
            continue;
        }

        Order order(cl_ord_id, instrument, side, quantity, price, timeStr);

        OrderBook* orderBook = getOrderBook(instrument);
        if (orderBook != nullptr) {
            orderBook->addOrder(order, csvFile);
            orderBook->sortOrders();
            //orderBook->displayOrderBook();
        }
        else {
            csvFile << order.order_id << ","
                << order.cl_ord_id << ","
                << order.instrument << ","
                << order.side << ","
                << "Rejected" << ","
                << order.quantity << ","
                << order.price << ","
                << "Invalid instrument" << ","
                << timeStr << "\n";;
        }
    }

    file.close();
    csvFile.close();

    return 0;
}
