#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std::chrono_literals;

int num = 0;
std::mutex m;


void opEven()
{
	while (true)// infinite loop
	{
		m.lock();
		if (!(num & 1 ))//if num is not odd
		{
			//++num;
			std::cout << ++num << std::endl;//increase and print out
			m.unlock();
			std::this_thread::sleep_for(2s);
		}
		else { m.unlock(); }
	}
}

void opOdd()
{
	while (true)//infinite loop
	{
		m.lock();
		if (num & 1)//faster way n&1. n&1 means n%2
			//00000001 =1
			//-------- =n
			//0000000- =n&1
			//n&1 only checks the 1st bit to see if its an even or odd number
		{
			//++num;
			std::cout << ++num << std::endl;
			m.unlock();
			std::this_thread::sleep_for(2s);
		}
		else { m.unlock(); }
		
	}
}

void play()
{
	std::thread t0(opEven);
	std::thread t1(opOdd);
	std::thread t2(opEven);
	std::thread t3(opOdd);
	t0.join();
	t1.join();
	t2.join();
	t3.join();

}