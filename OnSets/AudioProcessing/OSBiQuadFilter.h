//
//  BiQuadFilter.h
//  OnSets
//
//  Created by Waruna de Silva on 7/7/13.
//  Copyright (c) 2013 Waruna de Silva. All rights reserved.
//

#ifndef BIQUADFILTER_H
#define BIQUADFILTER_H

class OSBiQuadFilter
{
public:
    //==============================================================================
    OSBiQuadFilter();
    ~OSBiQuadFilter();
	void MakeByPass();
	void MakeBP31();
	void MakeBP32();
	void MakeBP33();
	void MakeBP34();
	void MakeNotch();
	void setCoefficients(double g, double b0, double b1, double b2, double a1, double a2);
	void initializeState();
	void updateSample(double x0);
	double get_y0(){return y0;}
	double get_b0(){return b0;}//for debugging
	double get_a1(){return a1;}//for debugging

private:
    double  g;
    double b0;
	double b1;
    double b2;
    double a1;
    double a2;

	double x1;
    double x2;
	double y0;
    double y1;
    double y2;
};

#endif