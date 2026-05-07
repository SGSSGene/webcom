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
                adapter.call.addText(inputTag.value);
            }
            inputTag.value = "";
        }
    };
    obj.onAddMessage = function(msg) {
        let tag = document.createElement("div");
        tag.innerHTML = msg;
        msgsTag.appendChild(tag);
    }

    // methods that are called by the server
    adapter.methods = {
        init: function(list) {
            if (list) {
                for (let entry of list) {
                    obj.onAddMessage(entry);
                }
            }
        },
        addMsg: function(msg) {
            obj.onAddMessage(msg);
        }
    };

    return obj;
}

function connectReadValue(adapter) {
    let tag  = document.getElementById("widget/readValue");

    // methods that are called by the server
    adapter.methods = {
        init: function(value) {
            tag.innerHTML = value;
        },
        setValue: function(value) {
            tag.innerHTML = value;
        }
    };
}

function connectReadAndWriteValue(adapter) {
    let tagText  = document.getElementById("widget/readAndWriteValue/Text");
    let tagInput  = document.getElementById("widget/readAndWriteValue/Input");

    tagInput.onkeyup =  function(event) {
        if (event.keyCode === 13) {
            event.preventDefault();
            adapter.call.setValue(parseInt(tagInput.value));
            tagInput.value = "";
        }
    };

    // methods that are called by the server
    adapter.methods = {
        init: function(value) {
            tagText.innerHTML = value;
        },
        setValue: function(value) {
            tagText.innerHTML = value;
        }
    };
}



window.onload = function() {
    let webcom = connectWebcom("ws://" + window.location.host + "/ws");
    webcom.onCTor = () => {
        connectReadValue(webcom.subscribe("readValue"));
        connectReadAndWriteValue(webcom.subscribe("readAndWriteValue"));

        let chat = initChat(webcom.subscribe("chat"));
    };
}
