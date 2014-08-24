#include "DSPFilter.h"

#include<deque>

DSPFilter::DSPFilter()
{
	FRAMELENGTH = 128;
	STEPSIZE = 64;
	HISTORYFRAMES = 4;
	NOISEFLOOR = 0.01;
	BLACKOUTSAMPLES = 1024;
	
	dspaudiodata.assign(FRAMELENGTH,0.0);
	energy_full.assign(HISTORYFRAMES,0.0);
	energy_bp3.assign(HISTORYFRAMES,0.0);
	counterstepsize = 0;
	timeInSamples = 0;
	lastonsetsample = -BLACKOUTSAMPLES;
	qframe = false;
	qonset = false;
	qblackout = false;
	
	BiQuadFilter biquad31;
	BiQuadFilter biquad32;
	BiQuadFilter biquad33;
	BiQuadFilter biquad34;
}

DSPFilter::~DSPFilter()
{
}

void DSPFilter::updateSample(float x0)
{
	dspaudiodata.pop_back();
	dspaudiodata.push_front(x0);

	counterstepsize++;
	timeInSamples++;
	qframe = (counterstepsize == STEPSIZE);
	qblackout = (timeInSamples-lastonsetsample) < BLACKOUTSAMPLES;
}

bool DSPFilter::qOnSet()
{
	const double KENERGYFACTOR = 2.0;
	
	bool qf = ( energy_full.front() > NOISEFLOOR ) &&
		    ( ( energy_full.front() > KENERGYFACTOR * energy_full.at(1) && energy_full.front() > KENERGYFACTOR * energy_full.at(2) )
		   || ( energy_full.front() > KENERGYFACTOR * energy_full.at(3) && energy_full.at(1)   > KENERGYFACTOR * energy_full.at(3) ) );
		   //|| ( energy_full.front() * KENERGYFACTOR < energy_full.at(1) && energy_full.front() * KENERGYFACTOR < energy_full.at(2) ) );
	
	bool q3 = ( energy_bp3.front() > NOISEFLOOR ) && ( energy_bp3.at(1) > NOISEFLOOR ) && ( energy_bp3.at(2) > NOISEFLOOR ) && 
		    ( ( energy_bp3.front() > KENERGYFACTOR * energy_bp3.at(1) && energy_bp3.front() > KENERGYFACTOR * energy_bp3.at(2) )
		   || ( energy_bp3.front() > KENERGYFACTOR * energy_bp3.at(3) && energy_bp3.at(1)   > KENERGYFACTOR * energy_bp3.at(3) ) );
		   //|| ( energy_bp3.front() * KENERGYFACTOR < energy_bp3.at(1) && energy_bp3.front() * KENERGYFACTOR < energy_bp3.at(2) ) );
	
	if( ( qf || q3 ) && ( ! qblackout ) )
	{
		lastonsetsample = timeInSamples;
		return true;
	}
	else
	{
		return false;
	}
}

void DSPFilter::updateFrame()
{
	const int LENGTHFACTOR = 2;//should be 3 for double biquad filtering
	const double ENERGYSCALINGFULL = 10.0;
	const double ENERGYSCALINGBP3 = 100.0;
	std::deque<double> databp(dspaudiodata);
	databp.resize(LENGTHFACTOR*FRAMELENGTH,0.0);

	double energy;

	//fullband
	energy = 0.0;

	for (int i = 0; i < FRAMELENGTH; ++i)
    {
		energy += ( dspaudiodata.at(i) * dspaudiodata.at(i) );
	}

	energy_full.pop_back();
	energy_full.push_front( ENERGYSCALINGFULL * energy / (double)FRAMELENGTH );

	//bandpass 1
	for (int i = 0; i < LENGTHFACTOR*FRAMELENGTH; ++i) //first filtering
    {
		biquad31.updateSample(databp.at(i));
		biquad32.updateSample(biquad31.get_y0());
		biquad33.updateSample(biquad32.get_y0());
		biquad34.updateSample(biquad33.get_y0());
		databp.at(i) = biquad34.get_y0();
	}

	/*
	for (int i = 0; i < LENGTHFACTOR*FRAMELENGTH; ++i) //second filtering
    {
		biquad31.updateSample(data.at(i));
		biquad32.updateSample(biquad31.get_y0());
		biquad33.updateSample(biquad32.get_y0());
		biquad34.updateSample(biquad33.get_y0());
		data.at(i) = biquad34.get_y0();
	}
	*/

	energy = 0.0;

	for (int i = 0; i < LENGTHFACTOR*FRAMELENGTH; ++i)
    {
		energy += ( databp.at(i) * databp.at(i) );
	}

	energy_bp3.pop_back();
	energy_bp3.push_front( ENERGYSCALINGBP3 * energy / (double)FRAMELENGTH );

	qonset = qOnSet();

	counterstepsize = 0;
}