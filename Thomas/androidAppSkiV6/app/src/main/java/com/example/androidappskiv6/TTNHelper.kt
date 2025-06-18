package com.example.androidappskiv6

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.launch
import org.eclipse.paho.client.mqttv3.*
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence
import org.json.JSONObject

@Composable
fun TTNApp() {
    val coroutineScope = rememberCoroutineScope()
    var messageTTN by remember { mutableStateOf("En attente des données...") }

    val ttnClient = remember {
        TTNClient(
            brokerUrl = "tcp://eu1.cloud.thethings.network:1883", // Pour connexion non sécurisée
            clientId = "heltec-lora32-cdfski",
            topic = "v3/heltec-lora32-cdfski@ttn/devices/70B3D57E0D06DB0D/up", // À modifier avec ton App ID & Device ID
            username = "heltec-lora32-cdfski@ttn",
            password = "D13F7A714DA5BF7CD84121C047AA6202" // Remplace par ta clé API TTN
        )
    }

    ttnClient.setOnMessageReceived { msg ->
        coroutineScope.launch {
            messageTTN = msg
        }
    }

    Column(modifier = Modifier.padding(16.dp)) {
        Text(
            text = if (ttnClient.isConnected.value) "Connexion TTN réussie" else "Connexion en cours...",
            style = MaterialTheme.typography.headlineSmall
        )

        Spacer(modifier = Modifier.height(8.dp))
        Text(text = "Données TTN :", style = MaterialTheme.typography.headlineSmall)
        Spacer(modifier = Modifier.height(8.dp))
        Text(text = messageTTN, style = MaterialTheme.typography.bodyLarge)
    }
}

class TTNClient(brokerUrl: String, clientId: String, private val topic: String, username: String, password: String) {
    private val client: MqttClient
    var isConnected = mutableStateOf(false)
    private var onMessageReceived: ((String) -> Unit)? = null

    init {
        client = MqttClient(brokerUrl, clientId, MemoryPersistence())
        val options = MqttConnectOptions()
        options.isCleanSession = true
        options.userName = username
        options.password = password.toCharArray()

        try {
            client.connect(options)
            isConnected.value = true
            client.subscribe(topic) { _, message ->
                val payload = String(message.payload)
                val formattedMessage = parseTTNMessage(payload)
                onMessageReceived?.invoke(formattedMessage)
            }
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    fun setOnMessageReceived(listener: (String) -> Unit) {
        onMessageReceived = listener
    }

    private fun parseTTNMessage(payload: String): String {
        return try {
            val json = JSONObject(payload)
            val deviceID = json.getJSONObject("end_device_ids").getString("device_id")
            val uplinkMessage = json.getJSONObject("uplink_message")
            val decodedPayload = uplinkMessage.optJSONObject("decoded_payload")

            if (decodedPayload != null) {
                val temperature = decodedPayload.optDouble("temperature", Double.NaN)
                val humidity = decodedPayload.optDouble("humidity", Double.NaN)
                val pressure = decodedPayload.optDouble("pressure", Double.NaN)

                """
                📡 Device: $deviceID
                🌡️ Température: ${if (!temperature.isNaN()) "$temperature°C" else "N/A"}
                💧 Humidité: ${if (!humidity.isNaN()) "$humidity%" else "N/A"}
                🌬️ Pression: ${if (!pressure.isNaN()) "$pressure hPa" else "N/A"}
                """.trimIndent()
            } else {
                "⚠️ Données non décodées :\n$payload"
            }
        } catch (e: Exception) {
            "❌ Erreur de parsing JSON : ${e.message}"
        }
    }
}
