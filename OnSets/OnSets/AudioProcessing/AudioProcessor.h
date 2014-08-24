//
//  AudioProcessor.h
//  OnSets
//
//  Created by Waruna de Silva on 7/7/13.
//  Copyright (c) 2013 Waruna de Silva. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import "DSPFilter.h"

#define SAMPLE_RATE 11250.0 //Hertz
#define LOWPASSFILTERTIMESLICE .001
#define DBOFFSET -40.0

#define kOutputBus 0
#define kInputBus 1



@interface AudioProcessor : NSObject {
    
    AudioBuffer audioBuffer;
    AudioBuffer bufferList;
    DSPFilter dspFilter;
   
    float gain;
    float _db;
    double fx;
}

@property int graphSampleRate;
@property AudioUnit audioUnit;
@property (readonly) AudioBuffer audioBuffer;

+(id)sharedAudioProcessor;
-(void)initAudio;
-(void)processBuffer:(AudioBufferList*) audioBufferList;

@end
