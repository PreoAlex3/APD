#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cctype>
#include <map>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
using namespace std::chrono;

// Function to check if a character is alphanumeric
bool isAlphanumeric(char c) {
    return std::isalnum(static_cast<unsigned char>(c));
}

void processText(const string& text, map<string, int>& wordCounts, mutex& mtx) {
    // Use stringstream to split the text into words
    stringstream ss(text);
    string word;

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
            lock_guard<mutex> lock(mtx);
            wordCounts[cleanWord]++;
        }
    }
}

int main() {
    ifstream file("Input.txt");
    string line;
    vector<string> lines;

    if (file.is_open()) {
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    }
    else {
        cerr << "Unable to open file." << endl;
        return 1;
    }

    // Start measuring time
    auto start = high_resolution_clock::now();

    // Store word counts
    map<string, int> wordCounts;
    mutex mtx;

    // Create threads to process each line of text
    vector<thread> threads;
    for (const auto& line : lines) {
        threads.emplace_back(processText, line, ref(wordCounts), ref(mtx));
    }

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

    // Stop measuring time
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    // Write word counts to file
    ofstream outputFile("Output.txt");
    for (const auto& pair : wordCounts) {
        outputFile << pair.first << ": " << pair.second << endl;
    }
    outputFile << "Execution time: " << 0.001 * duration.count() << " seconds" << endl;
    outputFile.close();

    return 0;
}
