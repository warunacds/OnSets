#include "BiQuadFilter.h"

BiQuadFilter::BiQuadFilter()
{
	MakeByPass();
	initializeState();
}

BiQuadFilter::~BiQuadFilter()
{
}

void BiQuadFilter::initializeState()
{
	x1 = x2 = 0.0;
	y0 = y1 = y2 = 0.0;
}

void BiQuadFilter::MakeByPass()
{
	g  = 1.0;
	b0 = 1.0;
	b1 = 0.0;
	b2 = 0.0;
	a1 = 0.0;
	a2 = 0.0;
}

void BiQuadFilter::MakeBP31()
{
	g  =  0.024406700852205;
	b0 =  1.0; 
	b1 =  2.0;
	b2 =  1.0;
	a1 = -0.624918124604194; 
	a2 =  0.148757869034896;
}

void BiQuadFilter::MakeBP32()
{
	g  =  1.0;
	b0 =  1.0;
	b1 =  2.0;
	b2 =  1.0;
	a1 = -0.670658858820238;
	a2 =  0.546613430030343;
}

void BiQuadFilter::MakeBP33()
{
	g  =  1.0;
	b0 =  1.0;
	b1 = -2.0;
	b2 =  1.0;
	a1 = -1.772332617238884;
	a2 =  0.788448479273348;
}

void BiQuadFilter::MakeBP34()
{
	g  =  1.0;
	b0 =  1.0;
	b1 = -2.0;
	b2 =  1.0;
	a1 = -1.914304839059569;
	a2 =  0.927303029665385;
}

void BiQuadFilter::MakeNotch()
{//notch at 4kHz at fs = 11.025kHz
	       
    g  = 1.0;
    b0 = 0.831839930237566;
    b1 = 1.067696840594297;
	b2 = 0.831839930237566;
	a1 = 1.067696840594297;
	a2 = 0.663679860475132;
}

void BiQuadFilter::setCoefficients(double g_new, double b0_new, double b1_new, double b2_new, double a1_new, double a2_new)
{
	g  = g_new;
    b0 = b0_new;
    b1 = b1_new;
    b2 = b2_new;
    a1 = a1_new;
    a2 = a2_new;
}

void BiQuadFilter::updateSample(double x0)
{
	y0 = b0 * x0 + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
	y0*= g;

    x2 = x1;
    x1 = x0;
    y2 = y1;
    y1 = y0;
}
