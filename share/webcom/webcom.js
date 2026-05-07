// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
let connectWebcom = function(url, _onClose) {
    let rObj = {};

    let onClose = function() {
        for (const [key, value] of Object.entries(rObj.dispatcher)) {
            if (value.methods.onclose != null) {
                value.methods.onclose();
            }
        }
        if (_onClose) {
            _onClose();
        }
    }

    rObj.id = 0;
    rObj.ws = new WebSocket(url);
    rObj.dispatcher = {};
    rObj.ws.binaryType = 'arraybuffer';
    rObj.isOpen = false;
    rObj.receiveCB = [];
    rObj.ws.onopen = function(evt) {
        rObj.isOpen = true;
    }

    let dispatch = function(_node, _adapter) {
        let action  = _node.action;
        let params = [];
        for (let i in _node.params) {
            params.push(_node.params[i]);
        }
        if (action == '__ctor') {
            _adapter.__ctor(...params);
        } else if (action in _adapter.methods) {
            _adapter.methods[action](...params);
        } else {
            console.log("illegal method: " + action);
        }
    };

    rObj.ws.onmessage = function(evt) {
        if (evt.data) {
            let text = evt.data;
            let node = JSON.parse(text)[0];
            dispatch(node, rObj);
        }
    }
    rObj.ws.onclose = function(evt) {
        rObj.isOpen = false;
        onClose();
    }
    rObj.ws.onerror = function(evt) {
        console.log("some error happend" +evt);
        onClose();
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
        let params = {};
        for (let i = 2; i < arguments.length; i++) {
            params[(i-2) + ""] = arguments[i];
        }
        sendRaw({
            action: "msgFromClient",
            params: {"0": id, "1": {
                action: action,
                params: params
            }}
        });
    };
    rObj.subscribe = function(serviceName) {
        let adapter = {};
        adapter.__ctor = function(methodNames) {
            adapter.call = {};
            for (let n of methodNames) {
                adapter.call[n] = function() {
                    send(adapter.id, n, ...arguments);
                };
            }
            if (adapter.methods.__ctor) {
                adapter.methods.__ctor();
            }
        };
        adapter.id = rObj.id++;
        adapter.unsubscribe = function() {
            sendRaw({
                action:  "unsubscribe",
                params:  {"0": adapter.id}
            });
        };
        rObj.dispatcher[adapter.id] = adapter;
        sendRaw({
            action:  "subscribe",
            params:  {"0": adapter.id, "1": serviceName}
        });
        return adapter;
    }
    rObj.close = function() {
        rObj.ws.close();
    }

    rObj.__ctor = function(methodNames) {
        rObj.call = {};
        for (let n of methodNames) {
            rObj.call[n] = function() {
                send(adapter.id, n, ...arguments);
            };
        }
        if (rObj.methods.__ctor) {
            rObj.methods.__ctor();
        }
    };

    rObj.methods = {
        msgToClient: function(id, list) {
            if (id in rObj.dispatcher) {
                let adapter = rObj.dispatcher[id];
                for (let e of list) {
                    dispatch(e, adapter);
                }
            } else {
                console.log("unknown dispatcher");
            }
        }
    };
    return rObj;
};
