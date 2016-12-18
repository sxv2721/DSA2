#include <thread>
#include <iostream>
#include <chrono>
using namespace std::chrono_literals;


void input(char* word)
{
	while (0 != strcmp(word, "quit"))
	{
		std::cin.getline(word, 99);
	}
}

void output(char* word)
{
	while (0 != strcmp(word, "quit"))
	{
		std::cout << std::endl << word;
		std::this_thread::sleep_for(2s);
	}
}

int main()
{
	char word[100] = "Hello, Threads ";
	std::thread t0(input, word);
	std::thread t1(output, word);

	t0.join();
	t1.join();
	return 0;
}