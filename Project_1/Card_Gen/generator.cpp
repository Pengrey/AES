#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;


int main(int argc, char *argv[])            // The program generates an array of nxm random numbers
{
    int n = 8, m = 8;                       // The default values are n=8 and m=8

    for (int i = 1; i < argc; i++)          // The n and m are given by the user as arguments -n and -m
    {
        if (string(argv[i]) == "-n")
            n = atoi(argv[i+1]);
        if (string(argv[i]) == "-m")
            m = atoi(argv[i+1]);
        if (string(argv[i]) == "-h")        // If the flag "-h" is given, the program prints the help
        {
            cout << "Usage: " << argv[0] << " [-n <number of rows>] [-m <number of columns>]" << endl;
            return 0;
        }
    }
 
    
    ofstream fout("card.txt");              // The numbers are stored in a file named "card.txt"
    srand(time(NULL));

    for (int i = 0; i < n; i++)             // The program also prints the numbers on the screen
    {   
        for (int j = 0; j < m; j++)
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