const UdpHandler = require("./UdpHandler.js");

const udpReceiver = new UdpHandler("172.24.71.214", 12345, null, false);

function AndroidIMUReceiver(){
	const latest = UdpHandler.udpMsgQueue.pop();
	if(latest){		
		var i = 0;
		console.log("gyro:");
		console.log(latest.readFloatBE(i));
		i+=4;
		console.log(latest.readFloatBE(i));
		i+=4;
		console.log(latest.readFloatBE(i));
		i+=4;
		
		console.log("rotation vec:");
		console.log(latest.readFloatBE(i));
		i+=4;
		console.log(latest.readFloatBE(i));
		i+=4;
		console.log(latest.readFloatBE(i));
		i+=4;
		console.log(latest.readFloatBE(i));
		i+=4;
		
		console.log("time:");
		console.log(latest.readFloatBE(i));
		i+=4;
		udpReceiver.clearQueue();
	}
}

setInterval(() => {
	AndroidIMUReceiver();
}, 100);
