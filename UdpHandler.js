const util = require('util');
const dgram = require('dgram');
const fs = require('fs');
// file is included here:
//eval(fs.readFileSync('./server.js')+'');

const udpMsgQueue = [];

class UdpHandler{
	constructor(ip, listenPort, sendPort, react){
		this.listenPort = listenPort ? listenPort : 44444;
		this.sendPort = sendPort ? sendPort : 11111;		
		this.client = dgram.createSocket('udp4');
		this.ip = ip ? ip : '127.0.0.1';
		this.setupClient(this.client, this.sendPort, react);
		console.log("constructor: listen " + listenPort + " send to " + sendPort);
	}
	setupClient(client, sendPort, react){
		client.on('listening', function() {
			const address = client.address();
			console.log('UDP client listening on ' + address.address + ':' + address.port);
		});

		client.on('message', function(message, remote) {
			//console.log("receive:" + util.inspect(message.toString(), {showHidden: false, depth: null}) + " from " + remote.address);
			
			if(react){
				const msg = JSON.stringify({ name: "rokoko", arrival_time: Date.now() });
				console.log("send\t" + util.inspect(msg, {showHidden: false, depth: null}) + "to " + remote.address + ":" + sendPort);
				
				const data = Buffer.from(msg);
				client.send(data, 0, data.length, sendPort, remote.address, function(err, bytes) {
					if (err) 
						throw err;
				});
			}

			// send to websocket with sync port
			/*if(udpMsgQueue){
				const newMsg = {
					"src": -1,
					"dst": "*", 
					"message": JSON.parse(message)
				};
				udpMsgQueue.push(newMsg);
				console.log("newMsg:" + util.inspect(newMsg, {showHidden: false, depth: 2}) );
			}*/
			udpMsgQueue.push(message);
		});

		client.bind({
			address: this.ip,
			port: this.listenPort,
			exclusive: true
		});
	}
	
	sendMsg(msg, ip){
		const data = Buffer.from(msg);
		const port = this.sendPort;
		this.client.send(data, 0, data.length, this.sendPort, ip, function(err, bytes) {
			if (err) 
				throw err;
			console.log('UDP message sent to ' + ip +':'+ port);
		});
	}

	clearQueue(){
		while(udpMsgQueue.length > 0)
			udpMsgQueue.pop();
	}
}

module.exports = UdpHandler;
module.exports.udpMsgQueue = udpMsgQueue;