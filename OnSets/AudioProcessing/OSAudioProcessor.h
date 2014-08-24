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
#import "OSDSPFilter.h"

#define SAMPLE_RATE 11250.0 //Hertz , 11250 is sufficient for onset detection
#define kInputBus 1

@protocol onSetDetectedDelegate;


@interface OSAudioProcessor : NSObject
{
    
    AudioBuffer audioBuffer;
    AudioBuffer bufferList;
    OSDSPFilter dspFilter;
    
    __weak id<onSetDetectedDelegate>delegate;
}

@property int sampleRate;
@property AudioUnit audioUnit;
@property (readonly) AudioBuffer audioBuffer;
@property (nonatomic, weak) id<onSetDetectedDelegate> delegate;
@property (atomic, getter = isAudioON) BOOL audioON;

/*
 *  AudioProcessor singleton initiazlier
 */
+(id)sharedInstance;
-(void)initAudio;
-(void)startAudio;
-(void)stopAudio;
/*
 *  Analyze audio data from input buffer
 */
-(void)processBuffer:(AudioBufferList*) audioBufferList;
/*
 *  to print error messages from audiounit
 */
-(void)hasError:(int)statusCode File:(char*)file Line:(int)lineNum;


@end

@protocol onSetDetectedDelegate <NSObject>

-(void)onSetDetected;

@end
