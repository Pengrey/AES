#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <utility>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;

#define SIZE_CARD 8

int generate_card()                
{
    ofstream fout("card.txt");              // The numbers are stored in a file named "card.txt"
    srand(time(NULL));

    // first row
    fout << "     ";                          
    for (int k = 1; k <= SIZE_CARD; k++) {
        if (k < 100)                        // The number is padded with zeros to make it 3 digits
            fout << "0";
        if (k < 10) 
            fout << "0";

        fout << k << " ";                   // The numbers are separated by a space
    }
    fout << endl;                           // The numbers are separated by a new line

    for (int i = 0; i < SIZE_CARD; i++)     // The program also prints the numbers on the screen
    { 
        if (i < 26) {                       // The first column is labeled with letters from A to Z
            fout << "  " << (char)(i + 65) << "  ";
        } else if (i < 702) {
            fout << " " << (char)(i / 26 + 64) << (char)(i % 26 + 65) << "  ";
        } else {
            fout << (char)(i / 676 + 64) << (char)(i / 26 % 26 + 65) << (char)(i % 26 + 65) << "  ";
        }

        for (int j = 0; j < SIZE_CARD; j++)
        {
            int num = rand() % 999 + 1;     // The range of the numbers are from the range 001 to 999

            if (num < 100)                  // The number is padded with zeros to make it 3 digits
                fout << "0";
            if (num < 10) 
                fout << "0";

            fout << num << " ";             // The numbers are separated by a space
        }

        fout << endl;                       // The numbers are separated by a new line
    }
    fout.close();
    return 0;
}

string init_card(string client_id){
  return generate_card() + "\n" + client_id;
}

string parse_card() {
    ifstream fin("card.txt");
    // Ignore first line
    string line;
    getline(fin, line);

    // Read the rest of the file and ignore first 3 characters of each line
    string result;
    while (getline(fin, line)) {
        // Remove the first 3 characters
        line.erase(0, 3);
        // Remove white spaces from the line
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        // Add the line to the result string
        result += line;
    }
    fin.close();
    return result;
}

pair<uint8_t*, int> string_to_uint8_t(string str){
    int size = str.size();
    uint8_t* result = new uint8_t[size];
    for (int i = 0; i < size; i++) {
        result[i] = str[i];
    }
    return make_pair(result, size);
}

int validate_response(uint8_t* card, int expected, char response){
    return(card[expected - 1] == response);
}

int request_validation(pair<uint8_t*, int> card){
    // Generate a secure random number between 0 and card_size
    random_device rd;
    default_random_engine engine(rd());

    // Define the range of the random number
    int min = 0;
    int max = SIZE_CARD * SIZE_CARD * 3;

    // Generate the random number between min and max
    uniform_int_distribution<int> dist(min, max);
    int position = dist(engine);

    // Get the human readable position
    int row = position / (SIZE_CARD * 3);
    // The row label is of the type "A", ... "ZZZ", depending on the row number
    // Given the row, its calculated the row label
    // If the row is 1 then the row_label is "A"
    // If the row is 2 then the row_label is "B"
    // If the row is 27 then the row_label is "AA"
    // If the row is 28 then the row_label is "AB"

    string row_label = "";
    while (row >= 0) {
        row_label = (char)(row % 26 + 65) + row_label;
        row /= 26;
        row--;
    }

    int col = (position % (SIZE_CARD * 3)) / 3;
    int letter = (position % (SIZE_CARD * 3)) % 3;

    // Ask the user to write the number for the given position
    cout << "Write the number for the position " << row_label << "-" << col + 1 << "-" << letter + 1 << ": ";
    char number;
    cin >> number;

    // Validate the response
    if (validate_response(card.first, position + 1, number)) {
        cout << "Valid response" << endl;
    } else {
        cout << "Wrong response" << endl;
    }

    return 0;
}

int menu(){
    int option;
    cout << "1. Generate card" << endl;
    cout << "2. Validate" << endl;
    cout << "3. Exit" << endl;
    cout << "Option: ";
    cin >> option;

    // Check if the option is valid
    while (option < 1 || option > 3) {
        cout << "Invalid option. Try again: ";
        cin >> option;
    }

    // For each option, call the corresponding function
    switch (option) {
        case 1:
            cout << "Generating card..." << endl;
            init_card("123");
            break;
        case 2:
            cout << "Parsing card..." << endl;
            request_validation(string_to_uint8_t(parse_card()));
            break;
        case 3:
            cout << "Exiting..." << endl;
            break;
        default:
            cout << "Invalid option" << endl;
            break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    // Run app
    menu();   
    return 0;
}