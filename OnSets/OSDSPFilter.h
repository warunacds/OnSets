#ifndef DSPFILTER_H
#define DSPFILTER_H

#include"BiQuadFilter.h"

#include<deque>

class DSPFilter
{
public:
    //==============================================================================
    DSPFilter();
    ~DSPFilter();
	void updateSample(float x0);
	void updateFrame();
	bool qOnSet();
	BiQuadFilter biquad31;
	BiQuadFilter biquad32;
	BiQuadFilter biquad33;
	BiQuadFilter biquad34;
	int get_timeInSamples(){return timeInSamples;}
	void reset_timeInSamples(){timeInSamples = 0;}
	double get_energy_full(){return energy_full.front();}
	double get_energy_bp3(){return energy_bp3.front();}
	double get_lastonsetsample(){return lastonsetsample;}
	bool get_qframe(){return qframe;}
	bool get_qonset(){return qonset;}
	bool get_qblackout(){return qblackout;}
	double get_NOISEFLOOR(){return NOISEFLOOR;}
	void set_NOISEFLOOR(double nf){NOISEFLOOR = nf;}

private:
	int FRAMELENGTH;
	int STEPSIZE;
	int HISTORYFRAMES;
	double NOISEFLOOR;
	int BLACKOUTSAMPLES;
	int counterstepsize;
	int timeInSamples;
	int lastonsetsample;
	bool qframe;
	bool qonset;
	bool qblackout;
	std::deque<double> dspaudiodata;
	std::deque<double> energy_bp3;
	std::deque<double> energy_full;
};

#endif