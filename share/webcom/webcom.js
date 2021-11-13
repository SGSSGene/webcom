let connectWebcom = function(url) {
    console.log("opening new ws " + url);

    let rObj = {};

    rObj.id = 0;
    rObj.ws = new WebSocket(url);
    rObj.dispatcher = {};
    rObj.ws.binaryType = 'arraybuffer';
    rObj.isOpen = false;
    rObj.receiveCB = [];
    rObj.ws.onopen = function(evt) {
        rObj.isOpen = true;
    }

    rObj.ws.onmessage = function(evt) {
        if (evt.data) {
            let text = evt.data;
            let node = YAML.parse(text).params[0];
            let id = node.id;
            let action  = node.action;
            console.log(node.params);
            let params = [];
            for (let i in node.params) {
                params.push(node.params[i]);
            }
            if (id in rObj.dispatcher) {
                let methods = rObj.dispatcher[id].methods;
                if (action in methods) {
                    methods[action](...params);
                }
            }
        }
    }
    rObj.ws.onclose = function(evt) {
        rObj.isOpen = false;
    }
    rObj.ws.onerror = function(evt) {
        console.log("some error happend" +evt);
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
    let send = function(id, action) {
        let params = [];
        for (let i = 2; i < arguments.length; i++) {
            params.push(arguments[i]);
        }
        sendRaw({
            action: "message",
            params: [id, {
                action: action,
                params: params
            }]
        });
    };
    rObj.subscribe = function(serviceName) {

        let adapter = {};
        adapter.methods = {};
        adapter.id = rObj.id++;
        adapter.call = function(actionName) {
            return function() {
                send(adapter.id, actionName, ...arguments);
            }
        };
        adapter.unsubscribe = function() {
            sendRaw({
                action:  "unsubscribe",
                params:  [adapter.id]
            });
        };

        rObj.dispatcher[adapter.id] = adapter;
        sendRaw({
            action:  "subscribe",
            params:  [adapter.id, serviceName]
        });
        return adapter;
    }
    return rObj;
};