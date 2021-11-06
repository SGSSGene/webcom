/*var unsubscribe = function(handler) {
	delete dispatcher[handler.service];

	sendRaw({
		service: "services",
		action:  "unsubscribe",
		unsubscribeFrom: service
	})
	handler.service = null;
	console.log("unsubscribe");
};*/

let createWebSocket = function(url) {
	let path = "ws://" + window.location.host + "/" + url
	console.log("opening new ws " + path)

	let ws = new WebSocket(path)
	ws.dispatcher = {}
	this.ws = ws
	ws.binaryType = 'arraybuffer';
	ws.isOpen = false;
	ws.receiveCB = [];
	ws.onopen = function(evt) {
		ws.isOpen = true;
	}

	ws.onmessage = function(evt) {
		if (evt.data) {
			let text = evt.data
			node = YAML.parse(text);
			let service = node.service;
			let action  = node.action;
			let params  = node.params;
			if (params === null) {
				params = [];
			}
			if (service in ws.dispatcher) {
				let methods = ws.dispatcher[service].methods;
				if (action in methods) {
					methods[action](...params)
				}
			}
		}
	}
	ws.onclose = function(evt) {
		ws.isOpen = false;
	}
	ws.onerror = function(evt) {
		console.log("some error happend" +evt)
	}
	let sendRaw = function(msg) {
		if (ws.isOpen) {
			ws.send(JSON.stringify(msg));
		} else {
			let prevFunc = ws.onopen;
			ws.onopen = function(evt) {
				prevFunc(evt);
				ws.send(JSON.stringify(msg));
			}
		}
	}
	let send = function(service, action) {
		let params = []
		for (let i = 2; i < arguments.length; i++) {
			params.push(arguments[i]);
		}
		sendRaw({
			service: service,
			action: action,
			params: params
		})
	};
	return {
		subscribe: function(serviceName, adapterCTor) {
			let adapter = new function() {
				this.methods = {}
				this.serviceName = serviceName
				this.call = function(actionName) {
					return function() {
						send(serviceName, actionName, ...arguments)
					}
				}
				return this
			}()
			adapterCTor(adapter, adapter.methods)

			ws.dispatcher[serviceName] = adapter;
			sendRaw({
				service: "services",
				action:  "subscribe",
				subscribeTo:   serviceName,
				params:  []
			})
		}
	};
};
