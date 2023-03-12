#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

int main(int argc, char *argv[])
{
    string path = "card.txt";                                                           // The default path is "card.txt"

    bool pty = false;                                                                   // By default, the file is not printed in a pretty format      

    for (int i = 1; i < argc; i++)                                                      // Get the arguments from the user
    {
        if (string(argv[i]) == "-P")                                                    // The path of the file is given by the user as argument -P 
            path = argv[i+1];
        if (string(argv[i]) == "--pty")                                                 // If the flag "--pty" is given, the file is printed in a pretty format
            pty = true;
        if (string(argv[i]) == "-h")                                                    // If the flag "-h" is given, the program prints the help
        {
            cout << "Usage: " << argv[0] << " [-P <path of the file>] [--pty]" << endl;
            return 0;
        }
    }

    ifstream fin(path.c_str());                                                        // The file is opened in read mode

    if (!fin)                                                                          // If the file does not exist, print an error message and exit
    {
        cout << "Error: File does not exist" << endl;
        return 0;
    }

    int n = 0, m = 0;                                                                  // The number of rows and columns

    string line;
    while (getline(fin, line))                                                         // Get the number of rows
        n++;

    fin.clear();
    fin.seekg(0, ios::beg);
    getline(fin, line);

    for (int i = 0; i < line.length(); i++)                                             // Get the number of columns
        if (line[i] == ' ')
            m++;

    int **arr = new int*[n];                                                            // Create the 2D array dynamically
    for (int i = 0; i < n; i++)
        arr[i] = new int[m];


    fin.clear();

    fin.seekg(0, ios::beg);
    for (int i = 0; i < n; i++)                                                        // Read the numbers from the file and store them in the array
        for (int j = 0; j < m; j++)
            fin >> arr[i][j];

    fin.close();

    if (!pty)                                                                          // Print the array with simple format if the flag "--pty" is not given
    {
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                if (arr[i][j] < 100)                                                   // The numbers are padded with zeros to make them 3 digits
                    cout << "0";
                if (arr[i][j] < 10)
                    cout << "0";

                cout << arr[i][j] << " ";
            }
            cout << endl;
        }
    }
    else                                                                               // Print the array with pretty format if the flag "--pty" is given
    {
        cout << "    ";                                                                // Identation for the first column
        for (int i = 1; i < m + 1; i++)
        {
            if (i < 100)                                                               // The numbers are padded with zeros to make them 3 digits
                cout << "0";
            if (i < 10)
                cout << "0";

            cout << i << " ";
        }

        cout << endl;
        for (int i = 0; i < n; i++)                                                   // Print the first column and the rest of the array
        {
            if (i < 26)                                                               // The first 26 rows are of the form "  A", "  B", ..., "  Z"
                cout << "  " << char(i + 'A') << " ";
            else if (i < 702)                                                         // The next 676 rows are of the form " AA", " AB", ..., " AZ", " BA", " BB", ..., " ZZ"
                cout << " " << char(i/26 + 'A' - 1) << char(i%26 + 'A') << " ";
            else                                                                      // The rest of the rows are of the form "AAA", "AAB", ..., "ZZZ"
                cout << char(i/676 + 'A' - 1) << char(i/26 + 'A' - 1) << char(i%26 + 'A') << " ";

            for (int j = 0; j < m; j++)                                               // Print the rest of the array
            {
                if (arr[i][j] < 100)                                                  // The numbers are padded with zeros to make them 3 digits
                    cout << "0";
                if (arr[i][j] < 10)
                    cout << "0";

                cout << arr[i][j] << " ";
            }
            cout << endl;
        }

    }

    for (int i = 0; i < n; i++)                                                      // Delete the array
        delete[] arr[i];
    delete[] arr;

    return 0;
}

    


