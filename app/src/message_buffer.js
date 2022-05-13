// 输入数据缓冲，包含传感器输入数据以及客户端发送的消息
var messageBuffer = {
    messageBufferMap: new Map(),

    init: function() {
        this.messageBufferMap.set('pose_landmark', []) //摄像头输入缓冲
        // this.messageBufferMap.set('imu', []) //IMU输入缓冲
        this.messageBufferMap.set('control', []) //控制消息队列，包括客户端注册及订阅、传感器注册等
    },

    addNewMessage: function(message, webSocket) {
        var messageType = ''
        if (message.type === 'pose_landmark') {
            messageType = 'pose_landmark'
        } else if (message.type === 'application_control' 
                   || message.type === 'application_client' 
                   || message.type === 'sensor_client') {
            messageType = 'control'
        } else if (message.type === 'sensor_frame') {
            messageType = message.sensor_type
        }
        if (messageType) {
            this.messageBufferMap.get(messageType).push({'message': message, 'webSocket': webSocket})
            console.log('new message:'+message.type+"|"+this.messageBufferMap.get(messageType).length)
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