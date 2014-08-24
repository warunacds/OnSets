//
//  ViewController.h
//  OnSets
//
//  Created by Waruna de Silva on 7/7/13.
//  Copyright (c) 2013 Waruna de Silva. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "OSAudioProcessor.h"

@interface OSViewController : UIViewController <onSetDetectedDelegate>
{
    
    int onsetCount;
}

@property (retain, nonatomic) IBOutlet UIButton *auidoOnOffButton;
@property (retain, nonatomic) IBOutlet UILabel *labelOnsetCount;
@property (nonatomic, retain) OSAudioProcessor *audioProcessor;

- (IBAction)clickOnOffButton:(id)sender;

@end


