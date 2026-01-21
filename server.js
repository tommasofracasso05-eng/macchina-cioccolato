const express = require('express');
const bodyParser = require('body-parser');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(bodyParser.json());
app.use(express.static('public')); // serve index.html

// Stato macchina e sensori
let machineData = {
  temperature: 0,
  humidity: 0,
  status: "STOPPED",        // RUNNING, WARNING, STOPPED
  operatorPresent: false
};

// --- Arduino → Server: invio dati ---
app.post('/api/data', (req, res) => {
  const { temperature, humidity, status, operatorPresent } = req.body;
  machineData.temperature = temperature;
  machineData.humidity = humidity;
  machineData.status = status;
  machineData.operatorPresent = operatorPresent;

  console.log("Dati ricevuti:", machineData);
  res.send({ message: "Dati aggiornati" });
});

// --- Pagina web → Server: lettura dati ---
app.get('/api/data', (req, res) => {
  res.json(machineData);
});

// --- Pagina web → Server: invio comandi START/STOP ---
app.post('/api/command', (req, res) => {
  const { command } = req.body;

  if (command === "STOP") machineData.status = "STOPPED";
  if (command === "START") machineData.status = "RUNNING";

  console.log("Comando ricevuto:", command);
  res.send({ message: "Comando eseguito" });
});

// --- Arduino → Server: leggi comando macchina ---
app.get('/api/command', (req, res) => {
  res.send(machineData.status); // Arduino legge RUNNING / STOPPED / WARNING
});

app.listen(PORT, () => {
  console.log(`Server avviato su http://localhost:${PORT}`);
});

