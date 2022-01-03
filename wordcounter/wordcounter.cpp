#include "wordcounter.hpp"

unsigned int nthreads = std::thread::hardware_concurrency(); // Set to number of threads utilized by machine

/* ==== Helper functions ==== */

std::string parseArgToString(char *arg) {
	return (std::string)arg;
}

bool isInvalidChar(char& ch)
{
	if ((ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z') ||
		(ch == '-') || (ch == '\'')) {
		return false;
	}
	else { return true; }
}

bool isTxtFile(std::string fileName) {
	return (fileName.substr(fileName.size() - 4, 4) == ".txt");
}

std::map<std::string, std::size_t> add(const std::vector<std::string> chunk) {
	std::map<std::string, std::size_t> map;
	for (std::string word : chunk) {
		map[word];
		if (map.count(word) > 0) {
			++map.at(word);
		}
	}
	return map;
}





/* ==== Wordcounter functions ==== */

// Adds word to list
void wordcounter::push(std::string word) {
	wordcounter::words.push_back(word);
}

// Removes characters that are not words
void wordcounter::removeNonWords(std::map<std::string, std::size_t> &wordsMap) {
	if (wordsMap.count("") > 0) {
		wordsMap.erase("");
	}
	if (wordsMap.count("-") > 0) {
		wordsMap.erase("-");
	}
}

void wordcounter::writeFile(std::map<std::string, std::size_t> wordsMap, std::chrono::duration<double> elapsedTime, std::size_t mode) {

	std::ofstream outputFile;
	if (mode == 0)
	{
		outputFile.open("singlethread_result.txt");
		std::cout << "Writing to singlethread_result.txt ..." << std::endl;
	}
	else if (mode == 1)
	{
		outputFile.open("multithread_result.txt");
		std::cout << "Writing to multithread_result.txt ..." << std::endl;
	}
	else
	{
		throw invalidModeException();
	}
	outputFile << "WordCount completed.\nElapsed time: " << elapsedTime.count() << " s\n\n";
	outputFile << "WORD\tOCCURENCES\n====\t==========\n";
	long total = 0;
	for (const auto& word : wordsMap)
	{
		total += word.second;
		outputFile << word.first << "\t" << word.second << "\n";
	}
	outputFile << std::endl << "Total words count: " << total << std::endl;
	outputFile.close();
	std::cout << "Writing to result file completed!" << std::endl;
}

// Singlethread wordcount in single file
void wordcounter::singleFileCountWords(std::string inputFileName)
{
	std::cout << "Loading text file... " << inputFileName << std::endl;
	if (!isTxtFile(inputFileName)) {
		std::cout << "Error at: " << inputFileName << std::endl;
		throw fileIsNotTxtException();
	}
	std::ifstream inputFile(inputFileName);
	if (inputFile)
	{
		std::cout << "Text file " << inputFileName << " loaded.\n" << std::endl;
	}
	else
	{
		std::cout << "Error at: " << inputFileName << std::endl;
		throw fileDoesntExistException();
	}
	
	std::cout << "Reading words from file..." << std::endl;
	for (std::string word; inputFile >> word;) {
		word.erase(remove_if(word.begin(), word.end(), isInvalidChar), word.end());
		push(word);
	}
	std::cout << "Read complete.\n" << std::endl;

	std::map<std::string, std::size_t> map;
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	startTime = std::chrono::high_resolution_clock::now();

	std::cout << "Starting word count..." << std::endl;
	for (std::string word : words)
	{
		map[word];
		if (map.count(word) > 0)
		{
			++map.at(word);
		}
	}
	std::cout << "Word count complete.\n" << std::endl;
	endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = endTime - startTime;
	std::cout << "Single file single thread processing took: " << elapsedTime.count() << " s" << std::endl;
	
	removeNonWords(map);
	writeFile(map, elapsedTime, 0);
}

// Multithread wordcount in single file
void wordcounter::singleFileCountWordsParallel(std::string inputFileName)
{
	std::cout << "Loading text file... " << inputFileName << std::endl;
	if(!isTxtFile(inputFileName))
	{
		std::cout << "Error at: " << inputFileName << std::endl;
		throw fileIsNotTxtException();
	}
	std::ifstream inputFile(inputFileName);
	if (inputFile)
	{
		std::cout << "Text file " << inputFileName << " loaded.\n" << std::endl;
	}
	else
	{
		std::cout << "Error at: " << inputFileName << std::endl;
		throw fileDoesntExistException();
	}

	std::cout << "Reading words from file..." << std::endl;
	for (std::string word; inputFile >> word;)
	{
		word.erase(remove_if(word.begin(), word.end(), isInvalidChar), word.end());
		push(word);
	}
	std::cout << "Read complete.\n" << std::endl;

	std::vector<std::vector<std::string>> chunks;
	std::size_t chunks_size = words.size() / nthreads;
	for (std::size_t i = 0; i < words.size(); i += chunks_size)
	{
		auto last = std::min(words.size(), i + chunks_size);
		chunks.emplace_back(words.begin()+i, words.begin() + last);
	}

	std::map<std::string, std::size_t> map;
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	startTime = std::chrono::high_resolution_clock::now();

	std::cout << "Starting parallel word count..." << std::endl;
	std::vector<std::future<std::map<std::string, std::size_t>>> futures;
	for (auto chunk : chunks) {
		futures.push_back(async(add, chunk));
	}

	for (auto& future : futures)
	{
		std::map<std::string, std::size_t> result;
		result = future.get();
		for (const auto& word : result)
		{
			map[word.first];
			if (map.count(word.first) > 0)
			{
				map.at(word.first) += word.second;
			}
		}
	}
	std::cout << "Parallel single file word count complete." << std::endl;
	endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = endTime - startTime;
	std::cout << "Parallel processing took: " << elapsedTime.count() << " s" << std::endl;

	removeNonWords(map);
	writeFile(map, elapsedTime, 1);
}

// Singlethread wordcount in multiple files
void wordcounter::multiFileCountWords(std::vector<std::string> inputFilesNames)
{
	std::vector<std::ifstream> inputFiles;
	for (std::size_t i = 0; i < inputFilesNames.size(); i++)
	{
		std::cout << "Loading text file... " << inputFilesNames.at(i) << std::endl;
		if (!isTxtFile(inputFilesNames.at(i)))
		{
			std::cout << "Error at: " << inputFilesNames.at(i) << std::endl;
			throw fileIsNotTxtException();
		}
		std::ifstream inputFile(inputFilesNames.at(i));
		if (inputFile)
		{
			std::cout << "Text file " << inputFilesNames.at(i) << " loaded." << std::endl;
			inputFiles.push_back(std::move(inputFile));
		}
		else
		{
			std::cout << "Error at: " << inputFilesNames.at(i) << std::endl;
			throw fileDoesntExistException();
		}

	}
	std::cout << std::endl;

	std::cout << "Reading words from files..." << std::endl;
	for (std::size_t i = 0; i < inputFiles.size(); i++)
	{
		for (std::string word; inputFiles.at(i) >> word;)
		{
			word.erase(remove_if(word.begin(), word.end(), isInvalidChar), word.end());
			push(word);
		}
	}
	std::cout << "Read complete.\n" << std::endl;

	std::map<std::string, std::size_t> map;
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	startTime = std::chrono::high_resolution_clock::now();

	std::cout << "Starting word count..." << std::endl;
	for (std::string word : words) {
		map[word];
		if (map.count(word) > 0) {
			++map.at(word);
		}
	}
	std::cout << "Word count complete.\n" << std::endl;
	endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = endTime - startTime;
	std::cout << "Multiple file single thread processing took: " << elapsedTime.count() << " s" << std::endl;

	removeNonWords(map);
	writeFile(map, elapsedTime, 0);
}

// Multithread wordcount in multiple files
void wordcounter::multiFileCountWordsParallel(std::vector<std::string> inputFilesNames)
{
	std::vector<std::ifstream> inputFiles;
	for (std::size_t i = 0; i < inputFilesNames.size(); i++)
	{
		std::cout << "Loading text file... " << inputFilesNames.at(i) << std::endl;
		if (!isTxtFile(inputFilesNames.at(i)))
		{
			std::cout << "Error at: " << inputFilesNames.at(i) << std::endl;
			throw fileIsNotTxtException();
		}
		std::ifstream inputFile(inputFilesNames.at(i));
		if (inputFile)
		{
			std::cout << "Text file " << inputFilesNames.at(i) << " loaded." << std::endl;
			inputFiles.push_back(std::move(inputFile));
		}
		else
		{
			std::cout << "Error at: " << inputFilesNames.at(i) << std::endl;
			throw fileDoesntExistException();
		}
	}
	std::cout << std::endl;

	std::cout << "Reading words from files..." << std::endl;
	for (std::size_t i = 0; i < inputFiles.size(); i++)
	{
		for (std::string word; inputFiles.at(i) >> word;)
		{
			word.erase(remove_if(word.begin(), word.end(), isInvalidChar), word.end());
			push(word);
		}
	}
	std::cout << "Read complete.\n" << std::endl;

	std::vector<std::vector<std::string>> chunks;
	std::size_t chunks_size = words.size() / nthreads;
	for (std::size_t i = 0; i < words.size(); i += chunks_size)
	{
		auto last = std::min(words.size(), i + chunks_size);
		chunks.emplace_back(words.begin() + i, words.begin() + last);
	}

	std::map<std::string, std::size_t> map;
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	startTime = std::chrono::high_resolution_clock::now();

	std::cout << "Starting parallel word count..." << std::endl;

	std::vector<std::future<std::map<std::string, std::size_t>>> futures;
	for (auto chunk : chunks)
	{
		futures.push_back(async(add, chunk));
	}

	for (auto& future : futures)
	{
		std::map<std::string, std::size_t> result;
		result = future.get();
		for (const auto& word : result)
		{
			map[word.first];
			if (map.count(word.first) > 0)
			{
				map.at(word.first) += word.second;
			}
		}
	}
	std::cout << "Parallel multifile word count complete." << std::endl;
	endTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsedTime = endTime - startTime;
	std::cout << "Parallel processing took: " << elapsedTime.count() << " s" << std::endl;

	removeNonWords(map);
	writeFile(map, elapsedTime, 1);
}

// Prints program usage and information.
void wordcounter::printHelp()
{
	std::cout << "=======================" << std::endl;
	std::cout << "WordCounter application" << std::endl << std::endl;

	std::cout << "This application's main purpose is to count occurence of words.\n"
		<< "Program can process both single and multiple .txt files.\n"
		<< "Processing of text files is possible in single thread setup or\nparallel setup based on fixed amount of your CPU threads." << std::endl << std::endl;

	std::cout << "USAGE" << std::endl;
	std::cout << "wordcounter [-s | -p] [file name]" << std::endl;
	std::cout << "wordcounter [-s | -p] [file name] [file name] ..." << std::endl;
	std::cout << "wordcounter [-s | -p] [file path with file name]" << std::endl;
	std::cout << "wordcounter [-s | -p] [file path with file name] [file path with file name] ..." << std::endl << std::endl;

	std::cout << "OPTIONS" << std::endl;
	std::cout << "-h -help --help" << "\tShows help screen." << std::endl;
	std::cout << "-s -single" << "\tLaunches program in single thread setup." << std::endl;
	std::cout << "-p -parallel" << "\tLaunches program in multiple threads setup." << std::endl;

	std::cout << std::endl << "Created by Jan Hlavac for B6B36PJC course at FEE CTU in 2019." << std::endl;
	std::cout << "=======================" << std::endl << std::endl;
}

// Closes app with error message
void wordcounter::closeApp()
{
	std::cout << "WordCounter Application" << std::endl;
	std::cout << "Error: unrecognized or incomplete command. Closing application." << std::endl << std::endl;
}

std::vector<std::string> wordcounter::getWords()
{
	return wordcounter::words;
}

int main(int argc, char *argv[])
{
	wordcounter wc;
	if (argc <= 1) { wc.closeApp(); }
	else if (argc == 2)
	{
		if (parseArgToString(argv[1]) == "-h" || parseArgToString(argv[1]) == "-help" || parseArgToString(argv[1]) == "--help")
		{
			wc.printHelp();
		}
		else { wc.closeApp(); }
	}
	else if (argc == 3)
	{
		if (parseArgToString(argv[1]) == "-s" || parseArgToString(argv[1]) == "-single")
		{
			try
			{
				std::string fileName = argv[2];
				std::cout << "Starting single file single thread processing.\n" << std::endl;
				wc.singleFileCountWords(fileName);
			}
			catch (fileDoesntExistException e) { std::cout << e.what() << std::endl; }
			catch (fileIsNotTxtException t) { std::cout << t.what() << std::endl; }
			catch (invalidModeException m) {	std::cout << m.what() << std::endl;	}
		}
		else if (parseArgToString(argv[1]) == "-p" || parseArgToString(argv[1]) == "-parallel")
		{
			try
			{
				std::string fileName = argv[2];
				std::cout << "Starting single file parallel thread processing.\n" << std::endl;
				wc.singleFileCountWordsParallel(fileName);
			}
			catch (fileDoesntExistException e) { std::cout << e.what() << std::endl; }
			catch (fileIsNotTxtException t) { std::cout << t.what() << std::endl; }
			catch (invalidModeException m) { std::cout << m.what() << std::endl; }
		}
		else { wc.closeApp(); }
	}
	// More than 2 params
	else
	{
		if (parseArgToString(argv[1]) == "-s" || parseArgToString(argv[1]) == "-single")
		{
			try
			{
				std::vector<std::string> files;
				for (std::size_t i = 2; i < argc; i++)
				{
					files.push_back(argv[i]);
				}
				std::cout << "Starting multifile single thread processing.\n" << std::endl;
				wc.multiFileCountWords(files);
			}
			catch (fileDoesntExistException e) { std::cout << e.what() << std::endl; }
			catch (fileIsNotTxtException t) { std::cout << t.what() << std::endl; }
			catch (invalidModeException m) {	std::cout << m.what() << std::endl;	}
		}
		else if (parseArgToString(argv[1]) == "-p" || parseArgToString(argv[1]) == "-parallel")
		{
			try
			{
				std::vector<std::string> files;
				for (std::size_t i = 2; i < argc; i++)
				{
					files.push_back(argv[i]);
				}
				std::cout << "Starting multifile parallel thread processing.\n" << std::endl;
				wc.multiFileCountWordsParallel(files);
			}
			catch (fileDoesntExistException e) { std::cout << e.what() << std::endl; }
			catch (fileIsNotTxtException t) { std::cout << t.what() << std::endl; }
			catch (invalidModeException m) { std::cout << m.what() << std::endl; }
		}
		else { wc.closeApp(); }
	}
	return 0;
}