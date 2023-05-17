#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <utility>

using namespace std;

#define SIZE_CARD 8

string generate_card()                
{   
    stringstream result;
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

            result << num;                  // Add the number to the result string

            if (num < 100)                  // The number is padded with zeros to make it 3 digits
                fout << "0";
            if (num < 10) 
                fout << "0";

            fout << num << " ";             // The numbers are separated by a space
        }

        fout << endl;                       // The numbers are separated by a new line
    }
    fout.close();
    return result.str();
}

string init_card(string client_id){
  return generate_card() + "\n" + client_id;
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

int main(int argc, char *argv[])
{
    // Initialize the card
    string card = "109535885311912491181743135922682134401710319150733356917214211876187302600340872669683610778936336631554594402362025748766034262773246314676481863696747658882767976941639645624967423279\n123";
    
    // Convert string to uint8_t array
    pair<uint8_t*, int> coverted_card = string_to_uint8_t(card);
    uint8_t* card_array = coverted_card.first;
    int card_size = coverted_card.second;

    // Print result
    cout << "Wrong response: " << validate_response(card_array, 1, '0') << endl;
    cout << "Write response: " << validate_response(card_array, 120, '8') << endl; 
    
    return 0;
}

    