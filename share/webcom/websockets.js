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
    let path = "ws://" + window.location.host + "/" + url;
    console.log("opening new ws " + path);

    let rObj = {};

    rObj.ws = new WebSocket(path);
    rObj.dispatcher = {};
    rObj.ws.binaryType = 'arraybuffer';
    rObj.isOpen = false;
    rObj.receiveCB = [];
    rObj.ws.onopen = function(evt) {
        rObj.isOpen = true;
    }

    rObj.ws.onmessage = function(evt) {
        if (evt.data) {
            let text = evt.data
            let node = YAML.parse(text).params[0];
            let service = node.service;
            let action  = node.action;
            console.log(node.params);
            let params = [];
            for (let i in node.params) {
                params.push(node.params[i]);
            }
            if (service in rObj.dispatcher) {
                let methods = rObj.dispatcher[service].methods;
                if (action in methods) {
                    methods[action](...params)
                }
            }
        }
    }
    rObj.ws.onclose = function(evt) {
        rObj.isOpen = false;
    }
    rObj.ws.onerror = function(evt) {
        console.log("some error happend" +evt)
    }
    let sendRaw = function(msg) {
        if (rObj.isOpen) {
            rObj.ws.send(JSON.stringify(msg));
        } else {
            let prevFunc = rObj.ws.onopen;
            rObj.ws.onopen = function(evt) {
                prevFunc(evt);
                rObj.ws.send(JSON.stringify(msg));
            }
        }
    }
    let send = function(service, action) {
        let params = []
        for (let i = 2; i < arguments.length; i++) {
            params.push(arguments[i]);
        }
        sendRaw({
            service: "services",
            action: "message",
            params: [{
                service: service,
                action: action,
                params: params
            }]
        })
    };
    rObj.subscribe = function(serviceName, adapterCTor) {
        let adapter = new function() {
            this.methods = {};
            this.serviceName = serviceName;
            this.call = function(actionName) {
                return function() {
                    send(serviceName, actionName, ...arguments)
                }
            }
            return this
        }()

        rObj.dispatcher[serviceName] = adapter;
        sendRaw({
            service: "services",
            action:  "subscribe",
            params:  [serviceName]
        })
        return adapter;
    }
    return rObj;
};
