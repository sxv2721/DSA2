#pragma once
#include <thread>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <chrono>
using namespace std::chrono_literals;


namespace Barber
{
	int numWaiting;//number in the queue
	int numServed;//how many have been served
	std::mutex m;
	std::condition_variable cv;
	bool q = false;//true if the user wants to quit

	void client()
	{
		while (!q)//while the user doesn't want to quit
		{
			//sleep for 1 or 2 seconds
			std::this_thread::sleep_for(1s*(rand() % 2 + 1));
			m.lock();
			std::cout << "Client #" << numServed + 1 << " entered with " << numWaiting << " ahead of him\n";
			m.unlock();
			
			//new client walks in
			++numWaiting;
			if (numWaiting)
			{
				cv.notify_all();
			}
		}
	}
	void server()
	{
		std::unique_lock<std::mutex> lck(m);
		while (!q)
		{
			if (numWaiting == 0)
			{
				std::cout << "Nobody to serve.\n";
				cv.wait(lck);
			}
			if (numWaiting > 0)
			{
				numWaiting--;
				numServed++;
				std::cout << "Serving client #" << numServed << std::endl;
				std::this_thread::sleep_for(2s);
			}
		}
	}
	
	void Quit()
	{
		//when the user hits enter the function quits
		std::cin.get();
		std::cout << "quitting\n";
		q = true;
	}
	void barberRun()
	{
		numWaiting = 0;
		numServed = 0;

		srand(time(NULL));

		std::thread t0(server);
		std::thread t1(client);
		//std::thread t2(client);
		std::thread quitter(Quit);

		t0.join();
		t1.join();
		quitter.join();
	}
};

