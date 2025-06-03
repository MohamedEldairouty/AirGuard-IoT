const express = require("express");
const app = express();
const http = require("http").createServer(app);
const mqtt = require("mqtt");
const path = require("path");
const io = require("socket.io")(http);
const cors = require("cors");

app.use(cors());
app.use(express.static(path.join(__dirname, "../frontend"))); // serve frontend

const mqttUrl = "mqtts://te20dd0d.ala.eu-central-1.emqxsl.com:8883";
const options = {
  username: "Habiba",
  password: "123456789",
  rejectUnauthorized: false
};
const mqttClient = mqtt.connect(mqttUrl, options);

let sensorData = {
  temperature: 0,
  humidity: 0,
  pressure: 0,
  gas: 0,
  altitude: 0,
  iaq: 0,
  iaq_label: "Unknown",
  fan: "OFF",
  buzzer: "OFF"
};

mqttClient.on("connect", () => {
  console.log("✅ MQTT connected");
  mqttClient.subscribe("bme680/#");
  mqttClient.subscribe("airguard/iaq_label");
  mqttClient.subscribe("airguard/fan");
  mqttClient.subscribe("airguard/buzzer");
});

mqttClient.on("message", (topic, message) => {
  const value = message.toString();
  const key = topic.split("/")[1];

  if (sensorData.hasOwnProperty(key)) {
    sensorData[key] = parseFloat(value);
  } else if (key === "iaq_label") {
    sensorData.iaq_label = value;
  } else if (key === "fan" || key === "buzzer") {
    sensorData[key] = value;
  }

  io.emit("data", sensorData); // update frontend
});

io.on("connection", (socket) => {
  console.log("🟢 Web client connected");
  socket.emit("data", sensorData); // send latest values on connect

  socket.on("control", ({ device, state }) => {
    if (device === "power") {
      console.log(`⚡ POWER ${state} triggered`);
      mqttClient.publish("airguard/power", state); 
    } else {
      console.warn(`Unknown device control: ${device}`);
    }
  });
  

  // ✅ Properly placed disconnect handler
  socket.on("disconnect", () => {
    console.log("🔴 Web client disconnected");
  });
});


const PORT = process.env.PORT || 3000;
http.listen(PORT, () => {
  console.log(`🚀 Server running at http://localhost:${PORT}`);
});
