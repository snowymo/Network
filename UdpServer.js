const util = require('util');

var ListenPort = 11111;
var SendPort = 44444;
var ServerIp = '172.24.71.214';

var dgram = require('dgram');
var server4send = dgram.createSocket('udp4');
var server = dgram.createSocket('udp4');

// assume server is the rokoko + floor sensor machine
// the clients should be self, optitrack and oculus quest
var client_ips = ['172.24.71.214'];

/* struct CustomTime{
	string ip;
	string name;
	float roundtrip;
	float depart_time;
	float arrival_time;
}; */

var clients = {};
clients["rokoko"] = {
	"ip": "172.24.71.214",
	"name": "rokoko",
	"roundtrip" : 0,
	"depart_time" : 0,
	"arrival_time" : 0,
	"offset": 0
};

for (var key in clients) {
	var client = clients[key];
	var datebuf = Buffer.allocUnsafe(4);
	var cur_time = Date.now();
	datebuf.writeFloatLE(cur_time, 0);
	
	server4send.send(datebuf, 0, datebuf.length, SendPort, client.ip, function(err, bytes) {
		if (err) throw err;
		console.log('UDP message sent to ' + ServerIp +':'+ SendPort);
		server4send.close();
	});
	
	client.depart_time = Date.now();
}

server.on('listening', function() {
	var address = server.address();
	console.log('UDP Server listening on ' + address.address + ':' + address.port);
});

server.on('message', function(message, remote) {
	var cur_time = Date.now();
	
	var obj = JSON.parse(message.toString());
	console.log(remote.address + ':' + remote.port +' - ' + message + "\tremote data:"
	+ util.inspect(obj, {showHidden: false, depth: null}));
	
	clients[obj.name].arrival_time = obj.arrival_time;
	clients[obj.name].roundtrip = cur_time - clients[obj.name].depart_time; // in ms
	clients[obj.name].offset = obj.arrival_time - clients[obj.name].depart_time - clients[obj.name].roundtrip/2;
	clients[obj.name].end_time = cur_time;
	
	console.log(util.inspect(clients[obj.name], {showHidden: false, depth: null}));
});

server.bind({
  address: ServerIp,
  port: ListenPort,
  exclusive: true
});