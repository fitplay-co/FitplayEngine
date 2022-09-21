#import <UIKit/UIKit.h>

@class FitplayPoseTracker;

@protocol FitplayPoseTrackerDelegate
-(void)controller:(FitplayPoseTracker *)controller
didOutputIntValue:(int)intValue
    andFloatValue:(float)floatValue;
@end

@interface FitplayPoseTracker: NSObject

@property (weak, nonatomic) id<FitplayPoseTrackerDelegate> delegate;

- (void)start;
+ (int)hello;

@end