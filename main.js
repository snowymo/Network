const util = require('util');
const UdpHandler = require("./UdpHandler.js");
const argparse = require("argparse");
const fs = require('fs');

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

const datestr = new Date().toISOString().replace(/:/, '-').replace(/:/, '-');
var filename = datestr + ".csv";
var stream = fs.createWriteStream(filename, {flags:'a'});

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

var msgCount = 0;
function AndroidIMUStringReceiver(){
	if(msgCount != UdpHandler.udpMsgQueue.length){
		msgCount = UdpHandler.udpMsgQueue.length;
	}else if(msgCount>0){
		UdpHandler.udpMsgQueue.forEach( function (item,index) {
			var str = new Buffer(item).toString('ascii');
			var obj = JSON.parse(str);
			//console.log("receive:" + util.inspect(obj, {showHidden: false, depth: null}));
			var newstr = obj["timestamp"]+","+obj["acc"][0] + ","+obj["acc"][1] + ","+obj["acc"][2] + ","+obj["gyro"][0] + ","+obj["gyro"][1] + ","+obj["gyro"][2] + "\n";
			stream.write(newstr);
		});
		udpReceiver.clearQueue();
		console.log("finished.");
		stream.end();
	}	
}

setInterval(() => {
	AndroidIMUStringReceiver();
}, 1000);
