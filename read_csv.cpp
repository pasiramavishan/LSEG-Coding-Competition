#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class Order {
public:
    int orderID;
    std::string productName;
    int quantity;
    double price;

    // Constructor to initialize from CSV data
    Order(int id, const std::string& name, int qty, double pr)
        : orderID(id), productName(name), quantity(qty), price(pr) {}

    // Function to print details of the order
    void print() const {
        std::cout << "OrderID: " << orderID << ", Product: " << productName
                  << ", Quantity: " << quantity << ", Price: " << price << std::endl;
    }
};

int main() {
    // Open the CSV file
    std::ifstream file("Orders.csv");

    // Check if the file is opened successfully
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        return 1;
    }

    // // To store each row of the CSV file
    // std::string line;

    // // Skip the first line (header)
    // getline(file, line);

    // Reading each line from the CSV, starting from the second line
    // while (getline(file, line)) {
    //     std::vector<std::string> row;
    //     std::stringstream sstream(line);
    //     std::string value;

    //     // Splitting each line by comma
    //     while (getline(sstream, value, ',')) {
    //         row.push_back(value);
    //     }

    //     // Process each row (For demonstration, printing the values)
    //     for (const auto& v : row) {
    //         std::cout << v << " ";
    //     }
    //     std::cout << std::endl;
    // }
    // To store each row of the CSV file
    std::string line;

    // Skip the first line (header)
    getline(file, line);

    // Vector to store objects of the Order class
    std::vector<Order> orders;

    while (getline(file, line)) {
        std::stringstream sstream(line);
        std::string value;

        // Variables to hold the values from each row
        int orderID;
        std::string productName;
        int quantity;
        double price;

        // Read and assign values to the variables
        getline(sstream, value, ',');
        orderID = std::stoi(value);  // Convert to int

        getline(sstream, productName, ',');

        getline(sstream, value, ',');
        quantity = std::stoi(value); // Convert to int

        getline(sstream, value, ',');
        price = std::stod(value);    // Convert to double

        // Create an Order object and add it to the orders vector
        orders.emplace_back(orderID, productName, quantity, price);
    }

    // Close the file
    file.close();
    return 0;
}
