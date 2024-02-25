var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

$(document).ready(function () {
  $("#send").on("click", event => onSendClick());
  $("#command").on("keydown", event => onCommandChange(event));
  $("#baud").on("change", event => onBaudChange());

  connectWebSocket();
});

function connectWebSocket(){
  console.log("Connecting to: "+gateway);
  $("#console").css("border-color", "orange");
  $("#send").prop("disabled", true);
  $("#command").prop("disabled", true);
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
} 

function onOpen(event) {
  console.log('Socket connected.');
  $("#console").css("border-color", "green");
  $("#send").prop("disabled", false);
  $("#command").prop("disabled", false);
  $("#command").focus();
}

function onClose(event) {
  console.log('Socket disconnected. Will retry connection...');
  setTimeout(connectWebSocket, 100);
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
    $("h1").html(json.device);
  }
}

function onSendClick() {
  if (websocket.readyState == 1) {
    var val = $("#command").val().trim();
    if (val.length > 0) {
      send("passthrough", val)
    }
  } else {
    onClose(null);
  }
  $("#command").val("");
  $("#command").focus();
}

function onCommandChange(event) {
  if (event.which == 13) {
    $("#send").trigger("click");
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
