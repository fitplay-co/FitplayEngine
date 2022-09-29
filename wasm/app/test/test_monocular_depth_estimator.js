var depthEstimator = require('../src/midware_ground_location')
var mockData = require('../test/mock_pose_data')
depthEstimator.process(mockData[0])