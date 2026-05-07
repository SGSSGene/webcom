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

    let register__ctor = function(adapter, methodNames, send) {
        adapter.call = {};
        for (let n of methodNames) {
            adapter.call[n] = function() {
                send(n, arguments);
            };
        }
        if (adapter.methods.__ctor) {
            adapter.methods.__ctor();
        }
    };
    rObj.subscribe = function(serviceName) {
        let adapter = {};
        adapter.__ctor = function(methodNames) {
            register__ctor(adapter, methodNames, function(n, params) {
                sendRaw({
                    action: "msgFromClient",
                    params: {"0": adapter.id, "1": {
                        action: n,
                        params: params
                    }}
                });
            });
        };
        adapter.id = rObj.id++;
        adapter.unsubscribe = function() {
            rObj.call.unsubscribe(adapter.id);
        };
        rObj.dispatcher[adapter.id] = adapter;
        rObj.call.subscribe(adapter.id, serviceName);
        return adapter;
    }
    rObj.close = function() {
        rObj.ws.close();
    }

    rObj.__ctor = function(methodNames) {
        register__ctor(rObj, methodNames, function(n, params) {
            sendRaw({
                action: n,
                params: params
            });
        });
    };

    rObj.methods = {
        __ctor: function() {
            let interval = setInterval(function() {
                rObj.call.ping()
            }, 10000)
            if (rObj.onCTor){
                rObj.onCTor();
            }
        },
        msgToClient: function(id, list) {
            if (id in rObj.dispatcher) {
                let adapter = rObj.dispatcher[id];
                for (let e of list) {
                    dispatch(e, adapter);
                }
            } else {
                console.log("unknown dispatcher");
            }
        },
        pong: function() {
            console.log("pong");
        }
    };
    return rObj;
};
