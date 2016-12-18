#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <condition_variable>
using namespace std::chrono_literals;
using std::thread;


namespace Smokers
{
	std::mutex m;
	std::condition_variable cv[4];//a cv for each variable
	
	unsigned int smoker1;//paper
	unsigned int smoker2;//tobacco
	unsigned int smoker3;//matches

	unsigned int resource1 = 0;//1 if paper 2 if tobacco 3 if matches 0 if used
	unsigned int resource2 = 0;
	
	bool q = false;


	void smoke(int s)
	{
		std::unique_lock<std::mutex> lck(m);
		//std::cout << "original wait\n";
		cv[s].wait(lck);
		//cv.wait(lck);

		while (!q)
		{
			//std::cout << "smoker lock\n";
			//lck.lock();
			if (s!=resource1 && s!=resource2 && resource1!=0 && resource2!=0)
			{
				std::cout << "Smoker #" << s << " is smoking...\n";
				std::this_thread::sleep_for(3s);
				std::cout << "Smoker #" << s << " is done smoking\n";
				resource1 = 0;
				resource2 = 0;
			}
			//lck.unlock();
			//std::cout << "smoker unlock\n";
			cv[0].notify_all();
			//cv.notify_all();
			cv[s].wait(lck);
			//cv.wait(lck);
		}
	}

	void serve()
	{
		while (!q)
		{
			m.lock();
			//std::cout << "server locked\n";
			do
			{
				//std::cout << "server randomizing\n";
				resource1 = (int)(rand() % 3 + 1);
				resource2 = (int)(rand() % 3 + 1);
			} while (resource1 == 0 || resource2 == 0 || resource1 == resource2);

			if (resource1 == 1 || resource2 == 1)
			{
				std::cout << "The server put out papers\n";
			}
			if (resource1 == 2 || resource2 == 2)
			{
				std::cout << "The server put out tobacco\n";
			}
			if (resource1 == 3 || resource2 == 3)
			{
				std::cout << "The server put out matches\n";
			}
			
			cv[1].notify_all();
			cv[2].notify_all();
			cv[3].notify_all();
			std::this_thread::sleep_for(1s);
			m.unlock();
			std::unique_lock<std::mutex> lck(m);
			cv[0].wait(lck);
			std::this_thread::sleep_for(1s);
		}
	}

	void Quit()
	{
		//when the user hits enter the function quits
		std::cin.get();
		std::cout << "quitting\n";
		q = true;
		for (int i = 0; i < 4; i++)
		{
			cv[i].notify_all();
		}
		std::this_thread::sleep_for(1s);
		for (int i = 0; i < 4; i++)
		{
			cv[i].notify_all();
		}
	}


	void smokersRun()
	{
		srand(time(NULL));

		smoker1 = 1;//papers
		std::cout << "smoker 1 has papers\n";
		smoker2 = 2;//tobacco
		std::cout << "smoker 2 has tobacco\n";
		smoker3 = 3;//matches
		std::cout << "smoker 3 has matches\n";

		resource1 = 0;
		resource2 = 0;
		thread s1(smoke, smoker1);
		thread s2(smoke, smoker2);
		thread s3(smoke, smoker3);
		thread server(serve);
		thread quitter(Quit); //thread that checks for the user to hit enter. When they do it changes quit to true

		server.join();
		s1.join();
		s2.join();
		s3.join();
		quitter.join();
	}
};

