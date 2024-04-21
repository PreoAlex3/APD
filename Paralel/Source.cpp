#define _CRT_SECURE_NO_WARNINGS
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <cctype>
#include <map>
#include <vector>

using namespace std;
using namespace std::chrono;

// Function to check if a character is alphanumeric
bool isAlphanumeric(char c) {
    return std::isalnum(static_cast<unsigned char>(c));
}

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Start measuring time
    auto start = high_resolution_clock::now();

    ifstream file("Input.txt");
    string line;
    vector<string> lines(world_size);

    if (file.is_open()) {
        int line_number = 0;
        while (getline(file, line)) {
            lines[line_number % world_size] += line + "\n";
            line_number++;
        }
        file.close();
    }
    else {
        cerr << "Unable to open file." << endl;
        MPI_Finalize();
        return 1;
    }

    // Each process handles its portion of the text
    string text = lines[world_rank];

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

    // Gather all word counts to root process
    if (world_rank == 0) {
        ofstream outputFile("Output.txt");
        // Root process receives word counts from all processes
        for (int i = 1; i < world_size; i++) {
            MPI_Status status;
            int count;
            MPI_Probe(i, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &count);

            // Receive data
            vector<char> buffer(count);
            MPI_Recv(buffer.data(), count, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Deserialize data
            stringstream ss(string(buffer.begin(), buffer.end()));
            string word;
            int freq;
            while (ss >> word >> freq) {
                wordCounts[word] += freq;
            }
        }

        // Write word counts to file
        for (const auto& pair : wordCounts) {
            outputFile << pair.first << ": " << pair.second << endl;
        }

        // Stop measuring time
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        outputFile << "Execution time: " << 0.001 * duration.count() << " seconds" << endl;
        outputFile.close();
    }
    else {
        // Non-root processes send word counts to root process
        stringstream ss;
        for (const auto& pair : wordCounts) {
            ss << pair.first << ' ' << pair.second << ' ';
        }
        string data = ss.str();
        MPI_Send(data.data(), data.size(), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
