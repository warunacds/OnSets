#include"PerformanceAnalysis.h"

//#define NDEBUG//*****uncomment this line to prevent assert statements from having any effect*****
#include<assert.h>

#include <iostream>
#include <fstream>
#include <deque>

#define LOGGING 0

int PerformanceAnalysis::instanceCount = 0;//outside class

PerformanceAnalysis::PerformanceAnalysis()
{
	FS = 11025;
	BPM = 120.0;
	LATENCYCORRECTION  = -0.005; //0.045;//MAGIC NUMBER!
	TIMEPLAYEDMAXINTERVAL = 5.0;
    
    THRESHOLD_ONTARGET = 0.025;
	THRESHOLD_MISS     = 2 * THRESHOLD_ONTARGET;
    
    instanceCount++;
    printf("PerformanceAnalysis instantiated\n");
    if(instanceCount>1) {
        printf("INSTANTIATION NUMBER %i OF PERFORMANCEANALYSIS!\n",instanceCount);
    }

	//double mybeats[]={0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0};//eigth notes
	//double mybeats[]={0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 6.25, 6.5};
	//std::deque<double> mytarget(mybeats, mybeats + sizeof(mybeats) / sizeof(double));
	//makeTargetPattern(mytarget, BPM);
	//setTargetThresholds(0.025, 0.050);
	//enable_loop_in_beats(0.0,4.0,BPM);

	reset();
	//initialize_pyramid();
    initialize_iPad2_pyramid();
    FLATPYRAMID = true;
}

PerformanceAnalysis::~PerformanceAnalysis()
{
    
}

int PerformanceAnalysis::get_tb_c(int n)
{
	if( tbeats.empty() ) return 0;
	else return tbeats.at(n).get_c(n);
}

double PerformanceAnalysis::get_pb_t()
{
	if( pbeats.empty() ) return 0.0;
	else return pbeats.back().t;
}

double PerformanceAnalysis::get_pb_e()
{
	if( pbeats.empty() ) return 0.0;
	else return pbeats.back().e;
}

int PerformanceAnalysis::get_pb_c()
{
	if( pbeats.empty() ) return 0;
	else return pbeats.back().c;
}

int PerformanceAnalysis::get_pb_tb()
{
	if( pbeats.empty() ) return 0;
	else return pbeats.back().tb;
}

double PerformanceAnalysis::convertBeatsToSeconds(double b)
{//assumes beats are quaternotes
	assert ( BPM > 0.0 );
	return b * 60.0 / double(BPM);
}

bool PerformanceAnalysis::enable_loop_in_beats(double b0, double b1, double bpm)
{
	if( b0 < 0.0 || b1 < b0 )
	{
		loopmode = false;
		return false;
	}

	loopstartbeats = b0;
	loopendbeats = b1;
	loopstartseconds = b0 * 60.0 / bpm;
	loopendseconds = b1 * 60.0 / bpm;

	loopmode = true;
	return true;
}

bool PerformanceAnalysis::enable_loop_in_seconds(double t0, double t1, double bpm)
{
	if( t0 < 0.0 || t1 < t0 )
	{
		loopmode = false;
		return false;
	}

	loopstartseconds = t0;
	loopendseconds = t1;
	loopstartbeats = bpm * t0 / 60.0;
	loopendbeats = bpm * t1 / 60.0;

	loopmode = true;
	return true;
}

void PerformanceAnalysis::reset()
{
	if( !pbeats.empty() ) pbeats.clear();
	if( !tbeats.empty() ) for ( deque<int>::size_type n = 0; n < tbeats.size(); n++ ) tbeats.at(n).reset();
    cPyramid.x = cPyramid.x1 = cPyramid.bottom_x;
	cPyramid.y = cPyramid.y1 = cPyramid.left_y;
	cSummary.reset();
}

bool PerformanceAnalysis::makeTargetPatternFromMidi(float bpm, int NumberOfNoteEvents, float * NoteStartTimes)
{
    PATTERNLENGTH = NumberOfNoteEvents;
    BPM = bpm;
	if( !tbeats.empty() ) tbeats.clear();
	for ( deque<int>::size_type n = 0; n < NumberOfNoteEvents; n++ )
    {
		TargetBeat tb;
		tb.w = 1.0;
		tb.t0 = convertBeatsToSeconds(NoteStartTimes[n]);
		tb.tm0 = tb.t0 - THRESHOLD_ONTARGET;
		tb.tp0 = tb.t0 + THRESHOLD_ONTARGET;
		tb.tm1 = tb.t0 - THRESHOLD_MISS;
		tb.tp1 = tb.t0 + THRESHOLD_MISS;
		tbeats.push_back(tb);
	}
    return true;
}

void PerformanceAnalysis::makeTargetPattern()
{
	const int PATTERNLENGTH = 16;
	
	if( !tbeats.empty() ) tbeats.clear();
	for ( deque<int>::size_type n = 0; n < PATTERNLENGTH; n++ )
    {
		TargetBeat tb;
		tb.w = 1.0;
		tb.t0 = n * 0.5;//quarter notes at 120BPM
		tb.tm0 = tb.t0 - THRESHOLD_ONTARGET;
		tb.tp0 = tb.t0 + THRESHOLD_ONTARGET;
		tb.tm1 = tb.t0 - THRESHOLD_MISS;
		tb.tp1 = tb.t0 + THRESHOLD_MISS;
		tbeats.push_back(tb);
	}
}

bool PerformanceAnalysis::makeTargetPattern(std::deque<double> pattern, double bpm)
{
	if( pattern.empty() ) return false;
	if( !tbeats.empty() ) tbeats.clear();
	
	for ( deque<int>::size_type n = 0; n < pattern.size(); n++ )
    {
		TargetBeat tb;
		tb.w = 1.0;
		tb.t0 = pattern.at(n) * 60.0 / bpm;
		tb.tm0 = tb.t0 - THRESHOLD_ONTARGET;
		tb.tp0 = tb.t0 + THRESHOLD_ONTARGET;
		tb.tm1 = tb.t0 - THRESHOLD_MISS;
		tb.tp1 = tb.t0 + THRESHOLD_MISS;
		tbeats.push_back(tb);
	}

	return true;
}

bool PerformanceAnalysis::readTargetPattern(string file)
{
	ifstream infile( file );
	if ( !infile.is_open() )    return false;

	int patternlength = 0;

	if( !tbeats.empty() ) tbeats.clear();
	while ( infile.good() )
    {
		TargetBeat tb;
		infile >> tb.t0;
		tb.w = 1.0;
		tb.tm0 = tb.t0 - THRESHOLD_ONTARGET;
		tb.tp0 = tb.t0 + THRESHOLD_ONTARGET;
		tb.tm1 = tb.t0 - THRESHOLD_MISS;
		tb.tp1 = tb.t0 + THRESHOLD_MISS;
		tbeats.push_back(tb);

		patternlength++;
	}

	infile.close();
	return true;
}

void PerformanceAnalysis::setTargetThresholds(double ontarget, double miss)
{
	THRESHOLD_ONTARGET = ontarget;
	THRESHOLD_MISS     = miss;

	if ( tbeats.empty() )   return;

	for ( deque<int>::size_type n = 0; n < tbeats.size(); n++ )
    {
		tbeats.at(n).w = 1.0;
		tbeats.at(n).tm0 = tbeats.at(n).t0 - ontarget;
		tbeats.at(n).tp0 = tbeats.at(n).t0 + ontarget;
		tbeats.at(n).tm1 = tbeats.at(n).t0 - miss;
		tbeats.at(n).tp1 = tbeats.at(n).t0 + miss;
	}
}

void PerformanceAnalysis::initialize_pyramid()
{
	//values chosen for debugging, change for display
	cPyramid.top_x = cPyramid.bottom_x = 0;
	cPyramid.top_y =  1000;
	cPyramid.bottom_y = -500;
	cPyramid.left_x = -1000;
	cPyramid.right_x = 1000;
	cPyramid.left_y = cPyramid.right_y = 0;
	cPyramid.x = cPyramid.x1 = cPyramid.bottom_x;
	cPyramid.y = cPyramid.y1 = cPyramid.left_y;
}

void PerformanceAnalysis::initialize_iPad2_pyramid()
{
    if(FLATPYRAMID)
    {
        cPyramid.top_x = cPyramid.bottom_x = 512;
        cPyramid.top_y =  580;
        cPyramid.bottom_y = cPyramid.left_y = cPyramid.right_y = 205;
        cPyramid.left_x = 275;
        cPyramid.right_x = 749;
        cPyramid.x = cPyramid.x1 = cPyramid.bottom_x;
        cPyramid.y = cPyramid.y1 = cPyramid.left_y;
    }
    else
    {
        cPyramid.top_x = cPyramid.bottom_x = 512;
        cPyramid.top_y =  580;
        cPyramid.bottom_y = 80;
        cPyramid.left_x = 275;
        cPyramid.right_x = 749;
        cPyramid.left_y = cPyramid.right_y = 205;
        cPyramid.x = cPyramid.x1 = cPyramid.bottom_x;
        cPyramid.y = cPyramid.y1 = cPyramid.left_y;
    }
}


void PerformanceAnalysis::initialize_iPhone4_pyramid()
{
	if(FLATPYRAMID)
    {
        cPyramid.top_x = cPyramid.bottom_x = 165;
        cPyramid.top_y = 450;
        cPyramid.bottom_y = cPyramid.left_y = cPyramid.right_y = 175;
        cPyramid.left_x = 25;
        cPyramid.right_x = 300;
        cPyramid.x = cPyramid.x1 = cPyramid.bottom_x;
        cPyramid.y = cPyramid.y1 = cPyramid.left_y;
    }
	else
    {
        cPyramid.top_x = cPyramid.bottom_x = 165;
        cPyramid.top_y = 450;
        cPyramid.bottom_y = 65;
        cPyramid.left_x = 25;
        cPyramid.right_x = 300;
        cPyramid.left_y = cPyramid.right_y = 175;
        cPyramid.x = cPyramid.x1 = cPyramid.bottom_x;
        cPyramid.y = cPyramid.y1 = cPyramid.left_y;
    }
}


bool PerformanceAnalysis::update_beat(double t)
{
	//assert( !tbeats.empty() );
	if ( tbeats.empty() ) return false;

	double _t = t - LATENCYCORRECTION;

	deque<int>::size_type ncounter;

	PerformanceBeat pb;
	if( pbeats.empty() )//it is the first detected beat
	{
#if LOGGING
		pLog->Write("pbeats empty");
#endif
		ncounter = 0;
	}
	else
	{
		pb = pbeats.back();
		ncounter = pb.tb;
		double dt = _t - pb.t;
		if( dt < TIMEPLAYEDMAXINTERVAL ) cSummary.add_timeplayed( dt );
	}
	pb.t = _t;
#if LOGGING
	pLog->Write("onset: %.3f timeplayed: %.3f",pb.t,cSummary.get_timeplayed());
#endif

	if( !loopmode && _t > tbeats.back().tp1 )
	{
#if LOGGING
		pLog->Write("Performance beat occurs after last target beat");
#endif
		pb.tb = -1;
		pbeats.push_back(pb);
		return false;
	}

	double te = 10.0;//use large value to make sure it gets replaced by better match
	TargetBeat tbn, tb0;
	if( !loopmode )
	{
		do
		{
			tbn = tbeats.at(ncounter);
			double etemp = _t - tbn.t0;
			if( dabs( etemp ) < dabs(te) )
			{
				te = etemp;
				pb.tb = ncounter;
				tb0 = tbn;
#if LOGGING
				pLog->Write("timing error %f, ncounter %d",te, ncounter);
#endif
			}
			ncounter++;
		}
		while( ncounter < tbeats.size() && _t > tbn.tm1 );
	}
	else//loopmode = true
	{
		//determine onset time inside loop
		double looplength = loopendseconds - loopstartseconds;
		while( _t < loopstartseconds ) _t += looplength;
		while( _t > loopendseconds ) _t -= looplength;
#if LOGGING
		pLog->Write("onset time inside loop: %f", _t);
#endif
		//check which target beats are inside the loop
		std::deque<int> loopmembers;
		for ( deque<int>::size_type n = 0; n < tbeats.size(); n++ )
		{
			const double TOLERANCESECONDS = 0.050;
			if( tbeats.at(n).t0 > ( loopstartseconds - TOLERANCESECONDS ) && tbeats.at(n).t0 < ( loopendseconds - TOLERANCESECONDS ) )
			{
				loopmembers.push_back( n );
			}
		}

		if( loopmembers.empty() ) return false;//no target beats in loop segment
		
		for (std::deque<int>::iterator it = loopmembers.begin(); it!=loopmembers.end(); ++it)		
		{
			tbn = tbeats.at(*it);
			double etemp = _t - tbn.t0;
			if( 2 * etemp >  looplength ) etemp -= looplength;
			if( 2 * etemp < -looplength ) etemp += looplength;
#if LOGGING
		//pLog->Write("etemp: %f", etemp);
#endif

			if( dabs( etemp ) < dabs( te ) )
			{
				te = etemp;
				pb.tb = *it;
				tb0 = tbn;
#if LOGGING
				//pLog->Write("timing error %f, ncounter %d",te, ncounter);
#endif
			}
		}
	}

#if LOGGING
	//pLog->Write("tb %.3f %.3f %.3f %.3f %.3f", tb0.tm1, tb0.tm0, tb0.t0, tb0.tp0, tb0.tp1);
	pLog->Write("te: %f", te);
#endif
    
    
	//classify miss/early/ontarget/late/miss and calculate error
	int cn;
	if( te < ( tb0.tm1 - tb0.t0 ) )//missl
	{
		cn = -2;
		pb.e = 1.0;
	}
	else if( te > ( tb0.tp1 - tb0.t0 ) )//missr
	{ 
		cn = 2;
		pb.e = 1.0;
	}
	else if( te > ( tb0.tm0 - tb0.t0 ) && te < ( tb0.tp0 - tb0.t0 ) ) {//ontarget
		cn = 0;
		pb.e = 0.0;
	}
	else if( te < 0.0 )//early
	{
		cn = -1;
		pb.e = te / ( tb0.tm1 - tb0.t0 );
	}
	else//late
	{
		cn = 1;
		pb.e = te / ( tb0.tp1 - tb0.t0 );
	}
	cSummary.add1_c(cn);
	tbeats[pb.tb].add1_c(cn);
	pb.c = cn;

#if LOGGING
	pLog->Write("Matched target beat: %d Classification %d ",pb.tb,cn);
	pLog->Write("bSummary: %d %d %d %d %d ... %d",tbeats[pb.tb].get_c(-2),tbeats[pb.tb].get_c(-1),tbeats[pb.tb].get_c(0),tbeats[pb.tb].get_c(1),tbeats[pb.tb].get_c(2),tbeats[pb.tb].get_ctotal());
	pLog->Write("cSummary: %d %d %d %d %d ... %d",cSummary.get_c(-2),cSummary.get_c(-1),cSummary.get_c(0),cSummary.get_c(1),cSummary.get_c(2),cSummary.get_ctotal());
#endif
    printf("onset: %.3f timeplayed: %.3f \n",pb.t,cSummary.get_timeplayed());
    printf("onset time inside loop: %f \n", _t);
    printf("tb %.3f %.3f %.3f %.3f %.3f \n", tb0.tm1, tb0.tm0, tb0.t0, tb0.tp0, tb0.tp1);
    printf("Matched target beat: %d Classification %d \n",pb.tb,cn);
    printf("bSummary: %d %d %d %d %d ... %d \n",tbeats[pb.tb].get_c(-2),tbeats[pb.tb].get_c(-1),tbeats[pb.tb].get_c(0),tbeats[pb.tb].get_c(1),tbeats[pb.tb].get_c(2),tbeats[pb.tb].get_ctotal());
    
	const double POLEACCURACYSCORE = 0.8;
	//sSummary.dAccuracyscore = POLEACCURACYSCORE * sSummary.dAccuracyscore + ( 1.0 - POLEACCURACYSCORE ) * ( 1.0 - pb.e );
	double pole = 1.0 - tb0.w + tb0.w * POLEACCURACYSCORE;//compensate for importance of target beat
	cSummary.accuracyscore = pole * cSummary.accuracyscore + ( 1.0 - pole ) * ( 1.0 - pb.e );

#if LOGGING
	pLog->Write("error %f",pb.e);
#endif

	pbeats.push_back(pb);

	update_pyramid(pb);

	return true;
}

double PerformanceAnalysis::dabs(double x)
{
	if( x < 0.0 ) return -x;
	else return x;
}


void PerformanceAnalysis::update_pyramid(PerformanceBeat pb)
{
	const double POLEPYRAMIDSLOW = 0.7;
	const double POLEPYRAMIDFAST = 0.5;
    
	double w = tbeats.at(pb.tb).w;
	double poleslow = 1.0 - w + w * POLEPYRAMIDSLOW;
	double polefast = 1.0 - w + w * POLEPYRAMIDFAST;
    
	cPyramid.x1 = cPyramid.x;
	cPyramid.y1 = cPyramid.y;
    
	switch( pb.c ) {
        case -2:
            if(FLATPYRAMID)
            {
                cPyramid.x = ( 1.0 - poleslow ) * cPyramid.left_x + poleslow * cPyramid.x;
                cPyramid.y = ( 1.0 - poleslow ) * cPyramid.left_y + poleslow * cPyramid.y;
            }
            else
            {
                cPyramid.x = ( 1.0 - poleslow ) * cPyramid.bottom_x + poleslow * cPyramid.x;
                cPyramid.y = ( 1.0 - poleslow ) * cPyramid.bottom_y + poleslow * cPyramid.y;
            }
            cPyramid.status = -2;
            break;
        case -1:
            if( cPyramid.x > cPyramid.top_x ) cPyramid.x = cPyramid.top_x;
            cPyramid.x += pb.e * ( 1.0 - poleslow ) * ( cPyramid.left_x - cPyramid.x );
            cPyramid.y += pb.e * ( 1.0 - poleslow ) * ( cPyramid.left_y - cPyramid.y );
            cPyramid.status = -1;
            break;
        case  0:
            cPyramid.x = ( 1.0 - polefast ) * cPyramid.top_x + polefast * cPyramid.x;
            cPyramid.y = ( 1.0 - poleslow ) * cPyramid.top_y + poleslow * cPyramid.y;
            cPyramid.status = 0;
            break;
        case  1:
            if( cPyramid.x < cPyramid.top_x ) cPyramid.x = cPyramid.top_x;
            cPyramid.x += pb.e * ( 1.0 - poleslow ) * ( cPyramid.right_x - cPyramid.x );
            cPyramid.y += pb.e * ( 1.0 - poleslow ) * ( cPyramid.right_y - cPyramid.y );
            cPyramid.status = 1;
            break;
        case  2:
            if(FLATPYRAMID)
            {
                cPyramid.x = ( 1.0 - poleslow ) * cPyramid.right_x + poleslow * cPyramid.x;
                cPyramid.y = ( 1.0 - poleslow ) * cPyramid.right_y + poleslow * cPyramid.y;
            }
            else
            {
                cPyramid.x = ( 1.0 - poleslow ) * cPyramid.bottom_x + poleslow * cPyramid.x;
                cPyramid.y = ( 1.0 - poleslow ) * cPyramid.bottom_y + poleslow * cPyramid.y;
            }
            cPyramid.status = 2;
            break;
        default:
            assert(false);//performance beat has been assigned an invalid class
	}
}