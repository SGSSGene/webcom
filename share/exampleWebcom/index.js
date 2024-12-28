// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
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

function connectReadValue(adapter) {
    let tag  = document.getElementById("widget/readValue");

    adapter.methods.init = function(value) {
        tag.innerHTML = value;
    }
    adapter.methods.setValue = function(value) {
        tag.innerHTML = value;
    }
}

function connectReadAndWriteValue(adapter) {
    let tagText  = document.getElementById("widget/readAndWriteValue/Text");
    let tagInput  = document.getElementById("widget/readAndWriteValue/Input");

    tagInput.onkeyup =  function(event) {
        if (event.keyCode === 13) {
            event.preventDefault();
            adapter.call("setValue")(parseInt(tagInput.value));
            tagInput.value = "";
        }
    };

    adapter.methods.init = function(value) {
        tagText.innerHTML = value;
    }
    adapter.methods.setValue = function(value) {
        tagText.innerHTML = value;
    }
}



window.onload = function() {
    let webcom = connectWebcom("ws://" + window.location.host + "/ws");
    connectReadValue(webcom.subscribe("readValue"));
    connectReadAndWriteValue(webcom.subscribe("readAndWriteValue"));

    let chat = initChat(webcom.subscribe("chat"));
}
