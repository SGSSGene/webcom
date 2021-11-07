function initChat(ws, methods) {
    let msgsTag  = document.getElementById("msgs");

    let inputTag  = document.getElementById("input");
    inputTag.onkeyup =  function(event) {
        if (event.keyCode === 13) {
            event.preventDefault();
            ws.call("addText")(inputTag.value);
            inputTag.value = "";
        }
    };


    let addMessage = function(msg) {
        let tag = document.createElement("div");
        tag.innerHTML = msg
        msgsTag.appendChild(tag);
    };

    methods.init = function(list) {
        for (let entry of list) {
            addMessage(entry);
        }
    }
    methods.addMsg = function(msg) {
        addMessage(msg);
    }
}

window.onload = function() {
    let ws = createWebSocket("ws");

    ws.subscribe("chat", initChat);
}
