#include "Philosophers.h"
#include "Smokers.h"
#include "Barber.h"

int main()
{
	Philosophers::philosopherRun();
	Smokers::smokersRun();
	Barber::barberRun();
	return 0;
}