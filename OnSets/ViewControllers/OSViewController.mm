//
//  ViewController.m
//  OnSets
//
//  Created by Waruna de Silva on 7/7/13.
//  Copyright (c) 2013 Waruna de Silva. All rights reserved.
//

#import "OSViewController.h"

@interface OSViewController ()

@end

@implementation OSViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    
    onsetCount = 0;
    
    _audioProcessor = [OSAudioProcessor sharedInstance];
    [_audioProcessor setDelegate:self];
    [_audioProcessor initAudio];
    
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSError *error;
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    
    NSString *txtPath = [documentsDirectory stringByAppendingPathComponent:@"txtFile.txt"];
    
    if ([fileManager fileExistsAtPath:txtPath] == NO) {
        NSString *resourcePath = [[NSBundle mainBundle] pathForResource:@"txtFile" ofType:@"txt"];
        [fileManager copyItemAtPath:resourcePath toPath:txtPath error:&error];
    }
    
}

- (void)didReceiveMemoryWarning
{
    _audioProcessor = nil;
    [_audioProcessor release];
    [super didReceiveMemoryWarning];
}

- (void)dealloc
{
    
    [_labelOnsetCount release];
    [_auidoOnOffButton release];
    [super dealloc];
}

- (IBAction)clickOnOffButton:(id)sender
{
    
    if([_audioProcessor isAudioON])
    {
        
        [_audioProcessor stopAudio];
        [_auidoOnOffButton setTitle:@"Start" forState:UIControlStateNormal];
        
    }
    else
    {
        
        [_audioProcessor startAudio];
        [_auidoOnOffButton setTitle:@"Stop" forState:UIControlStateNormal];
    }
}

-(void)onSetDetected
{

    //GCD to update UI thread
    dispatch_async(dispatch_get_main_queue(), ^{
        onsetCount++;
        [_labelOnsetCount setText:[NSString stringWithFormat:@"%d",onsetCount]];

    });
    
}
@end
