const util = require('util');

var ListenPort = 44444;
var SendPort = 11111;
var ServerIp = '172.24.71.214';

var dgram = require('dgram');

var client = dgram.createSocket('udp4');

client.on('listening', function() {
	var address = client.address();
	console.log('UDP client listening on ' + address.address + ':' + address.port);
});

client.on('message', function(message, remote) {
	//var data = message.readFloatLE(0);
	//console.log(remote.address + ':' + remote.port +' - ' + message + "\tdate:" + data);
	
	//var datebuf = Buffer.allocUnsafe(4);
	//datebuf.writeFloatLE(Date.now(), 0);
	
	sendmsg = JSON.stringify({ name: "rokoko", arrival_time: Date.now() });
	var data = Buffer.from(sendmsg);
	console.log("client send\t" + util.inspect(sendmsg, {showHidden: false, depth: null}));
	client.send(data, 0, data.length, SendPort, ServerIp, function(err, bytes) {
		if (err) throw err;
		console.log('UDP message sent to ' + ServerIp +':'+ SendPort);
		client.close();
	});
});

client.bind({
  address: ServerIp,
  port: ListenPort,
  exclusive: true
});