var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

$(document).ready(function () {
  var send = $("#send");
  send.on("click", event => onSendClick());
  send.prop("disabled", true);
  $("#command").on("keydown", event => onCommandChange(event));
  $("#baud").on("change", event => onBaudChange());

  connectWebSocket();
});

function connectWebSocket(){
  console.log("Connecting to: "+gateway);
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
} 

function onOpen(event) {
  console.log('Socket connected.');
  $("#console").css("border-color", "green");
}

function onClose(event) {
  console.log('Socker disconnected. Will retry connection...');
  $("#console").css("border-color", "orange");
  setTimeout(connectWebSocket, 500);
}

function onMessage(event) {
  try {
    console.log("Recieved: "+event.data);
    var json = JSON.parse(event.data);
  } catch (e) {
    console.log("JSON parse error: "+e.message);
    return;
  }

  if (json.type == "data") {
    $("#console").append(json.content);
    $("#console").append("\n");
    $("#console").scrollTop($("#console")[0].scrollHeight);
  } else if (json.type == "info") {
    $("#baud").val(json.baud);
  }
}

function onSendClick() {
  var command = $("#command").val();
  if (websocket.readyState == 1) {
    send("passthrough", command)
    $("#command").val("");
    $("#send").prop("disabled", true);
  } else {
    console.log("Send failed: Socket is not connected.");
  }
}

function onCommandChange(event) {
  if (websocket.readyState == 1) {
    var command = $("#command").val();
    var send = $("#send");
    if (command.length > 0) {
      if (event.which == 13) {
        send.trigger("click");
      }
    }
    send.prop("disabled", command.length == 0);
  } else {
    send.prop("disabled", true);
  }
}

function onBaudChange() {
  send("baud", parseInt($("#baud").val()));
}

function send(type, value) {
  var json = JSON.stringify({ type: type, value: value});
  console.log("Sending: "+json);
  websocket.send(json);
}
