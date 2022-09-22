const {performance} = require('perf_hooks');
var MessageType = require('../../../common/protocol/js/input/message-type').MessageType

const MAX_BUFFER_SIZE = 3 //缓冲队列容量上限

// 输入数据缓冲，包含传感器输入数据以及客户端发送的消息
var messageBuffer = {
    messageBufferMap: new Map(),
    imuLastFrameTimestamp: 0,
    inputLastFrameTimestamp: 0,
    imuFPS: 60,

    init: function() {
        this.messageBufferMap.set('pose_landmark', []) //摄像头输入缓冲
        this.messageBufferMap.set('imu', []) //IMU输入缓冲
        this.messageBufferMap.set('control', []) //控制消息队列，包括客户端注册及订阅、传感器注册等
        this.messageBufferMap.set('input', []) //input输入缓冲
        this.messageBufferMap.set('device_info', []) //device_info输入缓冲
        this.messageBufferMap.set('vibration', []) //device_info输入缓冲
        this.messageBufferMap.set('heart_control', []) // placeholder for heart control
    },

    addNewMessage: function(message, webSocket, rawData, jsonMessage) {
        var messageType = ''
        if (message.type() === MessageType.Pose) {
            messageType = 'pose_landmark'
        } else if (message.type() === MessageType.ApplicationControl
                   || message.type() === MessageType.ApplicationClient
                   || message.type() === MessageType.SensorClient) {
            messageType = 'control'
        } else if (message.type() === MessageType.SensorFrame) {
            messageType = message.sensorFrame().sensorType()
        } else if (message.type() === MessageType.SensorControl) {
            messageType = message.sensorControl().sensorType()
        }

        if (messageType) {
            if (messageType === 'imu') {// 对imu数据进行抽样以减少帧率
                const now = performance.now()
                if (now - this.imuLastFrameTimestamp < 1000 / this.imuFPS) {
                    return
                }
                this.imuLastFrameTimestamp = now
            } else if (messageType === 'input') {// 对input数据进行抽样以减少帧率
                const now = performance.now()
                if (now - this.inputLastFrameTimestamp < 1000 / this.imuFPS) {
                    return
                }
                this.inputLastFrameTimestamp = now
            }
            if ((messageType === 'pose_landmark' || messageType === 'imu' || messageType === 'input' || messageType === 'device_info' || messageType === 'vibration') && this.messageBufferMap.get(messageType).length >= MAX_BUFFER_SIZE) {
                console.log('drop earliest frame!!!')
                this.messageBufferMap.get(messageType).shift()
            }
            this.messageBufferMap.get(messageType).push({'message': message, 'webSocket': webSocket, 'rawData': rawData, 'jsonMessage': jsonMessage})
           // console.log('add new message:'+message.type+"|"+this.messageBufferMap.get(messageType).length)
        }
    },

    peekMessageWithType: function(type) {
        if (this.messageBufferMap.has(type) && this.messageBufferMap.get(type).length > 0) {
            return this.messageBufferMap.get(type).shift()
        }
        return undefined
    },

    peekMessage: function() {
        for (const type of this.messageBufferMap.keys()) {
            const message = this.peekMessageWithType(type)
            if (message !== undefined) {
                return message
            }
        }
        return undefined
    },

    createSensorBuffer: function(sensorType) {
        if (!this.messageBufferMap.has(sensorType)) {
            this.messageBufferMap.set(sensorType, [])
        }
    },
} 

messageBuffer.init()
module.exports = messageBuffer