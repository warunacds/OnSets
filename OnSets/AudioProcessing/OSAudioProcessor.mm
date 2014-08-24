//
//  AudioProcessor.m
//  OnSets
//
//  Created by Waruna de Silva on 7/7/13.
//  Copyright (c) 2013 Waruna de Silva. All rights reserved.
//

#import "OSAudioProcessor.h"

static OSStatus recordingCallback(void *inRefCon,
                                  AudioUnitRenderActionFlags *ioActionFlags,
                                  const AudioTimeStamp *inTimeStamp,
                                  UInt32 inBusNumber,
                                  UInt32 inNumberFrames,
                                  AudioBufferList *ioData)
{
	
    
    OSAudioProcessor *audioProcessor = (OSAudioProcessor*)inRefCon;
    
    AudioBuffer buffer;
    OSStatus status;
    
    buffer.mDataByteSize = inNumberFrames * 2;
    buffer.mNumberChannels = 1;
	buffer.mData = malloc( inNumberFrames * 2 );
	
	AudioBufferList bufferList;
	bufferList.mNumberBuffers = 1;
	bufferList.mBuffers[0] = buffer;
    
    
    status = AudioUnitRender([audioProcessor audioUnit], ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, &bufferList);
    [audioProcessor hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
    [audioProcessor processBuffer:&bufferList];
    
	free(bufferList.mBuffers[0].mData);
    
    return noErr;

}


@implementation OSAudioProcessor

@synthesize delegate;
@synthesize audioBuffer;
@synthesize audioUnit;


+ (id)sharedInstance
{
    static dispatch_once_t onceQueue;
    static OSAudioProcessor *audioProcessor = nil;
    
    dispatch_once(&onceQueue, ^{ audioProcessor = [[self alloc] init]; });
    return audioProcessor;
}


-(OSAudioProcessor*)init
{
    
    self = [super init];
    if (self)
    {
        
        dspFilter.set_NOISEFLOOR(0.001);
    }
    return self;
}

-(void)initAudio
{
    
    self.sampleRate = SAMPLE_RATE;
    
    //Configure Auido Session
    NSError *audioSessionError = nil;
    
    AVAudioSession *audioSession = [AVAudioSession sharedInstance];
    [audioSession setPreferredSampleRate:self.sampleRate error:&audioSessionError];
    [audioSession setCategory:AVAudioSessionCategoryRecord error:&audioSessionError];
    [audioSession setActive:YES error:&audioSessionError];
    self.sampleRate = [audioSession sampleRate];
    
    
    OSStatus status;
	
	//Audio component definition
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output; 
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple; 
    
	
	
	AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
	status = AudioComponentInstanceNew(inputComponent, &audioUnit);
	[self hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
	
    //to access mic date on input bus
    UInt32 flag = 1;
	status = AudioUnitSetProperty(audioUnit,
								  kAudioOutputUnitProperty_EnableIO,
								  kAudioUnitScope_Input,
								  kInputBus,
								  &flag,
								  sizeof(flag));
	[self hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
	
    
	AudioStreamBasicDescription audioFormat;
	audioFormat.mSampleRate			= self.sampleRate;
	audioFormat.mFormatID			= kAudioFormatLinearPCM;
	audioFormat.mFormatFlags		= kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
	audioFormat.mFramesPerPacket	= 1;
	audioFormat.mChannelsPerFrame	= 1;
	audioFormat.mBitsPerChannel		= 16;
	audioFormat.mBytesPerPacket		= 2;
	audioFormat.mBytesPerFrame		= 2;
    
    
    
	//format on the output stream
	status = AudioUnitSetProperty(audioUnit,
								  kAudioUnitProperty_StreamFormat,
								  kAudioUnitScope_Output,
								  kInputBus,
								  &audioFormat,
								  sizeof(audioFormat));
    
	[self hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
    
   
	
	AURenderCallbackStruct callbackStruct;
	callbackStruct.inputProc = recordingCallback;
	callbackStruct.inputProcRefCon = ( void *)(self);
    
	status = AudioUnitSetProperty(audioUnit,
                                  kAudioOutputUnitProperty_SetInputCallback,
								  kAudioUnitScope_Global,
								  kInputBus,
								  &callbackStruct,
								  sizeof(callbackStruct));
    
	flag = 0;
	status = AudioUnitSetProperty(audioUnit,
								  kAudioUnitProperty_ShouldAllocateBuffer,
								  kAudioUnitScope_Input,
								  kInputBus,
								  &flag,
								  sizeof(flag));
	
	audioBuffer.mNumberChannels = 1;
	audioBuffer.mDataByteSize = 512 * 2;
	audioBuffer.mData = malloc( 512 * 2 );
	
	status = AudioUnitInitialize(audioUnit);
	[self hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
    
}

-(void)startAudio
{
    //start auidounit
    OSStatus status = AudioOutputUnitStart(audioUnit);
    [self hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
    _audioON = YES;
}

-(void)stopAudio
{
    OSStatus status = AudioOutputUnitStop(audioUnit);
    [self hasError:status File:(char*)__FILE__ Line:(int)__LINE__];
    _audioON= NO;
}

-(void)processBuffer: (AudioBufferList*) audioBufferList
{
    
    SInt16 *buffer0 = static_cast<SInt16 *>(audioBufferList->mBuffers[0].mData);
    
    //run through all packets
    for (int nb = 0; nb < (audioBufferList->mBuffers[0].mDataByteSize / 2); nb++)
    {
        double sample = buffer0[nb] / 32768.0; // convert SInt16 to double 
        
        for (int i = 0; i < sample; ++i)
        {
            dspFilter.updateSample(sample);
            if(dspFilter.get_qframe())
            {
                dspFilter.updateFrame();
                if(dspFilter.get_qonset())//On Set detected
                {
                    [delegate onSetDetected];
                }
            }
        }//end of for 'i'
        
    }//end of for 'nb'
}


-(void)hasError:(int)statusCode File:(char*)file Line:(int)lineNum
{
	if (statusCode)
    {
		printf("AU Error: %d in file %s on line %d\n", statusCode, file, lineNum);
        exit(-1);
	}
}

@end
