
# Wordcounter.cpp
## Introduction
This project is C++ console application for counting words in text files (only `.txt` file extension). The application utilizes both single-threaded and multi-threaded wordcount, the performance/time-to-count difference is only noticeable when counting files with larger amount of words. The application also allows to count multiple files, although resulting word count will be written into a single result file.

### License
**GNU GPLv3**. See `LICENSE` in root directory.

## Compiling
For compiling the source code into executable you can use any C++14 compiler.
On Windows, compiling using [MinGW-w64](https://www.mingw-w64.org/) might be the easiest way to get started.

##### MinGW-w64 example usage:
`g++ -o wordcounter -std=c++14 wordcounter.hpp wordcounter.cpp` 

##### CMake:
The project also contains a very basic `CMakeLists.txt` file, so it should be possible to get it compiled with CMake as well.

## Usage
Once compiled, you should have a `wordcounter.exe` executable (or another type of executable) created.

To count words in `text.txt`, you can issue
`wordcounter -s text.txt` which will start wordcount in single-thread and then create a `singlethread_result.txt` file containing each word and its occurences within the `text.txt`. Final line in the result also displays sum of all words in the wordcounted file.

#### Arguments/Options
`-h | -help | --help` prints usage/help information.
`-s | -single` for single-thread count
`-p | -parralel` for multi-thread count

#### Usage examples
Print information:  
`wordcounter --help`  
Single file:  
`wordcounter [-s | -p] [file_name.txt]`  
Multiple files:  
`wordcounter [-s | -p] [file_name.txt] [file_name.txt] ...`  
Single file with path:  
`wordcounter [-s | -p] [C:\Users\Username\Desktop\file_name.txt]`  
Multiple files with path:  
`wordcounter [-s | -p] [file path] [file path] ...`  

#### Application termination
Application process is terminated either after successful wordcount / usage print, or after issuing an command with a typo/unrecognized/wrong argument.

#### Limitations

 1. **Character set:**
- The program is able to count only words that use English alphabet characters `a-z`, `A-Z` and some special characters if they are part of the word like `-`, `'`.
- If deemed necessary, the character set can be extended by adding more characters into the body of function `isInvalidChar(char& ch)` that is used to recognize a character as a valid word-character.

2. **Two words are actually one:**
- Based on previously mentioned special characters, if there is an English word that uses `'` such as `We'll` it will be counted as one word, even though it is technically two: `We will`.

#### Correctness
Based on comparison of results between this wordcount application and one of the other publicly available web services to count words it was concluded the application works correctly as the number of total words as well as the number of most frequent words were identical.

## Performance
Compiled executable was tested on a generated Lorem Ipsum text included in `data\text.txt`. This text file will be referred to as `short`. The same file was later put into a larger file called `long`, containing 300× the text included in `short` to provide better overview at how performance changes when using parallelism over single-thread operation in large files.

Both files were processed both by single-thread and multi-thread wordcount. The wordcounter result files are provided in the `data` directory, using prefix `short_`or `long_` to distinguish between the performance tests.

Performance was tested on a laptop with i5-6300HQ CPU with minimum background processes.
#### Results and conclusion
Based on the table below and result files in `data` directory it is clear that splitting wordcount operation between multiple threads will not be of noticeable difference while counting words from short text files.
|Single|Parallel|File|
|--|--|--|
|0.013 s|0.00696 s | short (9090 words)|
|2.609 s|0.9946 s  | long (2,727,000 words = 300× short)|

