#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cctype>
#include <map>
using namespace std;
using namespace std::chrono;

// Function to check if a character is alphanumeric
bool isAlphanumeric(char c) {
    return std::isalnum(static_cast<unsigned char>(c));
}

int main() {
    // Start measuring time
    auto start = high_resolution_clock::now();

    ifstream file("Input.txt");
    string text;
    string line;

    if (file.is_open()) {
        // Remove BOM if present
        char bom[3];
        file.read(bom, 3);
        if (!(bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF'))
            file.seekg(0);

        while (getline(file, line)) {
            text += line + "\n";
        }
        file.close();
    }
    else {
        cerr << "Unable to open file." << endl;
        return 1;
    }

    // Use stringstream to split the text into words
    stringstream ss(text);
    string word;

    // Store word counts
    map<string, int> wordCounts;

    // Store each word and count its appearances
    while (ss >> word) {
        // Remove non-alphanumeric characters from the word
        string cleanWord;
        for (char& c : word) {
            if (isAlphanumeric(c)) {
                cleanWord += c;
            }
        }
        if (!cleanWord.empty()) {
            wordCounts[cleanWord]++;
        }
    }

    // Output the word counts
    for (const auto& pair : wordCounts) {
        cout << pair.first << ": " << pair.second << endl;
    }

    // Stop measuring time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Execution time: " << 0.001 * duration.count() << " seconds" << endl;
    return 0;
}
