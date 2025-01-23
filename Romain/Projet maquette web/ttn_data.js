// Configuration MQTT pour TTN
const options = {
  clientId: "client_id_unique", // Remplace par un ID unique pour ton client MQTT
  username: "application_id@ttn", // Remplace par l'ID de ton application TTN
  password: "ttn_access_key", // Remplace par la clé d'accès TTN
};

// Connexion au broker MQTT de TTN
const client = mqtt.connect("wss://eu1.cloud.thethings.network:8883", options);

// Fonction exécutée à la connexion
client.on("connect", () => {
  console.log("Connecté à TTN MQTT !");
  // Souscrire au topic pour recevoir les données (remplace le topic par le bon chemin)
  client.subscribe("application_id/devices/device_id/up", (err) => {
    if (err) {
      console.error("Erreur de souscription :", err);
    } else {
      console.log("Souscrit au topic !");
    }
  });
});

// Fonction exécutée lorsqu'un message arrive
client.on("message", (topic, message) => {
  try {
    const payload = JSON.parse(message.toString());
    const temperature = payload.uplink_message.decoded_payload.temperature;
    const windSpeed = payload.uplink_message.decoded_payload.wind_speed;

    // Mise à jour des données dans le site
    document.querySelector("#temperature").textContent = `Temp: ${temperature} °C`;
    document.querySelector("#wind-speed").textContent = `Vent: ${windSpeed} km/h`;
  } catch (error) {
    console.error("Erreur lors du traitement du message MQTT :", error);
  }
});
