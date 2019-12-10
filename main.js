const util = require('util');
const UdpHandler = require("./UdpHandler.js");
const argparse = require("argparse");

const parser = new argparse.ArgumentParser({
  version: "0.0.1",
  addHelp:true,
  description: "webxr server"
});
parser.addArgument(
  [ "-p", "--port" ],
  {
    help: "port to listen on",
    defaultValue: 54321
  }
);
const args = parser.parseArgs();

const udpReceiver = new UdpHandler("172.24.71.214", args.port, null, false);

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
		console.log(latest.readFloatBE(i)/1000.0);
		i+=4;
		udpReceiver.clearQueue();
	}
}

function AndroidIMUStringReceiver(){
	const latest = UdpHandler.udpMsgQueue.pop();
	if(latest){	
		var str = new Buffer(latest).toString('ascii');
		var obj = JSON.parse(str);
		console.log("receive:" + util.inspect(obj, {showHidden: false, depth: null}));
		udpReceiver.clearQueue();
	}
}

setInterval(() => {
	AndroidIMUStringReceiver();
}, 100);
