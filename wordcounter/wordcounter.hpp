//#pragma once
#include <ostream>
#include <iostream>
#include <future>
#include <fstream>
#include <unordered_map>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <exception>
#include <thread>
#include <cstddef>

struct fileDoesntExistException : public std::exception
{
	const char * what() const throw () { return "Text file doesn't exist!"; }
};

struct fileIsNotTxtException : public std::exception {
	const char * what() const throw () { return "File is not .txt format!"; }
};

struct invalidModeException : public std::exception
{
	const char * what() const throw () { return "Invalid writing mode!"; }
};

class wordcounter
{
	private:
		std::vector<std::string> words;
		void push(std::string word);
		void removeNonWords(std::map<std::string, std::size_t> &wordsMap);
		void writeFile(std::map<std::string, std::size_t> wordsMap, std::chrono::duration<double> elapsedTime, std::size_t mode);
	public:
		wordcounter() = default;
		~wordcounter() = default;
	
		void singleFileCountWords(std::string inputFileName);
		void singleFileCountWordsParallel(std::string inputFileName);
		void multiFileCountWords(std::vector<std::string> inputFilesNames);
		void multiFileCountWordsParallel(std::vector<std::string> inputFilesNames);
		void printHelp();
		void closeApp();
		std::vector<std::string> getWords();
};