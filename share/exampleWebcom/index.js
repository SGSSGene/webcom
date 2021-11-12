function initChat(adapter) {
    let obj = {};

    let msgsTag  = document.getElementById("msgs");

    let inputTag  = document.getElementById("input");
    inputTag.onkeyup =  function(event) {
        if (event.keyCode === 13) {
            event.preventDefault();
            if (inputTag.value == "exit") {
                adapter.unsubscribe();
            } else {
                adapter.call("addText")(inputTag.value);
            }
            inputTag.value = "";
        }
    };
    obj.onAddMessage = function(msg) {
        let tag = document.createElement("div");
        tag.innerHTML = msg;
        msgsTag.appendChild(tag);
    }

    adapter.methods.init = function(list) {
        if (list) {
            for (let entry of list) {
                obj.onAddMessage(entry);
            }
        }
    }
    adapter.methods.addMsg = function(msg) {
        obj.onAddMessage(msg);
    }
    return obj;
}

window.onload = function() {
    let webcom = connectWebcom("ws://" + window.location.host + "/ws");
    let chat = initChat(webcom.subscribe("chat"));
}
