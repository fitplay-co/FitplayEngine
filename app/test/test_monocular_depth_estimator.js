var depthEstimator = require('../src/depth_estimator')
var mockData = require('../test/mock_pose_data')
depthEstimator.estimateDepth(mockData[0])