import { InputMessage, Control, Pose} from "../protocol/ts/inputMessage"
import { OutputMessage} from "../protocol/ts/outputMessage"
import { Client } from "../protocol/ts/inputMessage"
import { ByteBuffer } from '../protocol/ts/flatbuffers/flatbuffers'; // located in node_modules
import { Builder } from '../protocol/ts/flatbuffers/builder'
import { MessageType } from '../protocol/ts/inputMessage'
import { Buffer } from 'buffer';

class MonitorClient {
    private ws : WebSocket;
    monitorValueQueue = [1,1,1,1];

    constructor() {
        this.ws = new WebSocket('ws://192.168.50.106:8181')
        this.ws.binaryType = "arraybuffer"
        this.ws.onmessage = this.messageProcessor
        this.ws.onopen = this.sendSubsribtion
    }
    
    async messageProcessor(evt: MessageEvent){
        const z = new Uint8Array(evt.data);
        var outputMessage = OutputMessage.getRootAsOutputMessage(new ByteBuffer(z))
        var detectionResult = outputMessage.detectionResult()!;
        var groundLocation = detectionResult.ground()!
        console.log(groundLocation.x())
    }

    sendSubsribtion(this: WebSocket, evt: Event) {
        var appClientMessage =  {
            "type" : "application_client",
            "id": "vue_client"
            // "useJson": true
        }
        var actionDetectionSubscribe = {
            "type" : "application_control",
            "feature_id" : "action_detection",
            "action" : "subscribe" 
        }
        var actionDetectionRelease = {
            "type" : "application_control",
            "feature_id" : "action_detection",
            "action" : "release" 
        }
        var groundLocationSubscribe = {
            "type" : "application_control",
            "feature_id" : "ground_location",
            "action" : "subscribe" 
        }
        var gazeSubscribe = {
            "type" : "application_control",
            "feature_id" : "gaze_tracking",
            "action" : "subscribe" 
        }
        var fittingSubscribe = {
            "type" : "application_control",
            "feature_id" : "fitting",
            "action" : "subscribe" 
        }
        var groundLocationRelease = {
            "type" : "application_control",
            "feature_id" : "ground_location",
            "action" : "release" 
        }
        var groundLocationReset = {
            "type" : "application_control",
            "feature_id" : "ground_location",
            "action" : "reset" 
        }
        this.send(flatbuffersMessage(appClientMessage))
        this.send(flatbuffersMessage(actionDetectionSubscribe))
        this.send(flatbuffersMessage(groundLocationSubscribe))
        this.send(flatbuffersMessage(gazeSubscribe))
        this.send(flatbuffersMessage(fittingSubscribe))
        // this.send(JSON.stringify(appClientMessage))
        // this.send(JSON.stringify(actionDetectionSubscribe))
        // this.send(JSON.stringify(actionDetectionRelease))
        // this.send(JSON.stringify(groundLocationSubscribe))
        // this.send(JSON.stringify(gazeSubscribe))
        // this.send(JSON.stringify(fittingSubscribe))
    }

    sayHello() : number  {
        return 1;
    }
    
    tryConnect() {
        
    }

}

var ClientInstance = new MonitorClient();

function flatbuffersMessage(message: any) {
    var type = message.type;
    var builder = new Builder(1024);
    //var builder = new flatbuffers.Builder(1024)
    if(type === "application_client") {
        var version = builder.createString('0.1.0')
        var id = builder.createString(message.id)
        Client.startClient(builder)
        Client.addId(builder, id)
        Client.addUseJson(builder, false)
        var sensorControlOffset = Client.endClient(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.ApplicationClient)
        InputMessage.addClient(builder, sensorControlOffset)
    }
    if(type === "application_control") {
        var version = builder.createString('0.1.0')
        var featureId = builder.createString(message.feature_id)
        var action = builder.createString(message.action)
        var controlDataOffset
        Control.startControl(builder)
        Control.addAction(builder, action)
        Control.addFeatureId(builder, featureId)
        if (controlDataOffset) {
            Control.addData(builder, controlDataOffset)
        }
        var sensorControlOffset = Control.endControl(builder)

        InputMessage.startInputMessage(builder)
        InputMessage.addVersion(builder, version)
        InputMessage.addType(builder, MessageType.ApplicationControl)
        InputMessage.addControl(builder, sensorControlOffset)
    }
    builder.finish(InputMessage.endInputMessage(builder))
    return Buffer.from(builder.asUint8Array())
}

export { ClientInstance } 
export { flatbuffersMessage }