#import "FitplayPoseTracker.h"
#import "integration/hello-time.h"
#import "integration/protocol/actionData_generated.h"

@interface FitplayPoseTracker ()

@end

@implementation FitplayPoseTracker

struct SimpleCStruct {
    int intValue;
    float floatValue;
};

- (void)start {
    SimpleCStruct aStruct = (struct SimpleCStruct){1, 0.5};

    [self.delegate controller:self
            didOutputIntValue:aStruct.intValue
                andFloatValue:aStruct.floatValue];
}

+ (int)hello {
    return callFunction();
}

@end