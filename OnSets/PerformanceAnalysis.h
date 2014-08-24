#ifndef PERFORMANCEANALYSIS_H
#define PERFORMANCEANALYSIS_H

#include "Log.h"

#include <deque>
#include "math.h"

class TargetBeat
{
public:
	double w;//weighting of importance of beat
	double tm1, tm0, t0, tp0, tp1; //thresholds and target in seconds
	int get_c(int n) {return c[n+2];}
	int get_ctotal() {return c[0]+c[1]+c[2]+c[3]+c[4];}
	void add1_c(int n) {c[n+2]++;}
	void reset() {c[0]=c[1]=c[2]=c[3]=c[4]=0;}
private:
	int c[5];//0-missl, 1-early, 2-ontarget, 3-late, 4-missr
};

class PerformanceBeat
{
public:
	double t;//time in seconds
	double e;//relative error
	int c;//class -2,-1,0,1,2 miss/early/ontarget/late/miss
	int tb;//the target beat the performance beat is paired with
};

class PerformanceSummary
{
public:
	double accuracyscore;
	int get_c(int n) {return c[n+2];}
	int get_ctotal() {return c[0]+c[1]+c[2]+c[3]+c[4];}
	double get_timeplayed() {return timeplayed;}
	void add_timeplayed(double t) {timeplayed+=t;}
	void add1_c(int n) {c[n+2]++;}
	void reset() {c[0]=c[1]=c[2]=c[3]=c[4]=0; accuracyscore=timeplayed=0.0;}
private:
	int c[5];//0-missl, 1-early, 2-ontarget, 3-late, 4-missr
	double timeplayed;
};

class Pyramid
{
public:
	int top_x, left_x, right_x, bottom_x;//corner coordinates
	int top_y, left_y, right_y, bottom_y;
	double x, x1;//current and previous coordinates
	double y, y1;
    int status;
};

//*********************************************************************************

class PerformanceAnalysis
{

public:
    //==============================================================================
    PerformanceAnalysis();
    ~PerformanceAnalysis();
	void reset();
	bool update_beat(double t);
	void update_pyramid(PerformanceBeat pb);
	void makeTargetPattern();//default fall back
	bool makeTargetPattern(std::deque<double> pattern, double bpm);//in beats
	bool readTargetPattern(string filename);
    bool makeTargetPatternFromMidi(float BPM,int NumberOfNoteEvents,float * NoteStartTimes);
	void setTargetThresholds(double ontarget, double miss);
	void initialize_pyramid();
    void initialize_iPhone4_pyramid();
    void initialize_iPad2_pyramid();

	double convertBeatsToSeconds(double b);
	
	//GETTERS
	double get_BPM(){return BPM;}
	int get_pyramidx(){return (int)floor( 0.5 + cPyramid.x );}
	int get_pyramidy(){return (int)floor( 0.5 + cPyramid.y );}
    int get_pyramidStatus(){return cPyramid.status; }
    Pyramid getPyramid() { return cPyramid; }
	//PerformanceSummary
	double get_ps_c(int n){return cSummary.get_c(n);}
	double get_ps_accuracyscore(){return cSummary.accuracyscore;}
	double get_ps_timeplayed(){return cSummary.get_timeplayed();}
	//PerformanceBeat
	double get_pb_t();
	double get_pb_e();
	int get_pb_c();
	int get_pb_tb();
	//TargetBeat
	int get_tb_c(int n);

	//SETTERS
	void set_BPM(double bpm){BPM = bpm;}
	void set_LATENCYCORRECTION(double t){LATENCYCORRECTION = t;}
	void set_TIMEPLAYEDMAXINTERVAL(double t){TIMEPLAYEDMAXINTERVAL = t;}
	bool enable_loop_in_beats(double b0, double b1, double bpm);
	bool enable_loop_in_seconds(double t0, double t1, double bpm);
	void disable_loop(){loopmode = false;}

private:
    //==============================================================================
	double BPM;//beats per minute
	int FS;//sampling frequency
	double LATENCYCORRECTION;
	double TIMEPLAYEDMAXINTERVAL;
	double PATTERNLENGTH;//in quarternotes
    double THRESHOLD_ONTARGET;
	double THRESHOLD_MISS;

	bool loopmode;
	double loopstartbeats;
	double loopstartseconds;
	double loopendbeats;
	double loopendseconds;

	std::deque<TargetBeat> tbeats;
	std::deque<PerformanceBeat> pbeats;
	PerformanceSummary cSummary;
	Pyramid cPyramid;
    bool FLATPYRAMID;
    static int instanceCount;
    
	Log *pLog;//FOR DEBUGGING*******************************
    double dabs(double x);
};

#endif