// 输入数据缓冲，包含传感器输入数据以及客户端发送的消息
var messageBuffer = {
    messageBufferMap: new Map(),

    init: function() {
        this.messageBufferMap.set('pose_landmark', [])
        this.messageBufferMap.set('client', [])
    },

    addNewMessage: function(message) {
        var messageType = ''
        if (message.type === 'pose_landmark') {
            messageType = 'pose_landmark'
        } else if (message.type === 'application_control' || message.type === 'application_client') {
            messageType = 'client'
        }
        if (messageType) {
            this.messageBufferMap.get(messageType).push(message)
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
} 

messageBuffer.init()
module.exports = messageBuffer