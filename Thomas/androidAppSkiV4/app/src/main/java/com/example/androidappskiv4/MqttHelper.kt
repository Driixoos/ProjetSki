package com.example.androidappskiv4

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import org.eclipse.paho.client.mqttv3.*
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence
import com.beust.klaxon.Klaxon

// Modèle de données ajusté
data class DecodedPayload(
    val analog_in_3: Double?,
    val analog_in_4: Double?,
    val analog_in_5: Double?,
    val analog_in_6: Double?,
    val analog_in_7: Double?,
    val analog_in_8: Double?,
    val analog_in_9: Double?,
    val temperature_1: Double,
    val temperature_2: Double?,
)

data class UplinkMessage(
    val decoded_payload: DecodedPayload?
)

data class MQTTData(
    val uplink_message: UplinkMessage?
)

@Composable
fun MQTTApp() {
    var message by remember { mutableStateOf("En attente du message...") }
    val mqttClient = remember {
        MQTTClient(
            "tcp://eu1.cloud.thethings.network:1883",
            "heltec-lora32-cdfski",
            "v3/skiconnect@ttn/devices/heltec-lora32-cdfski/up",
            "skiconnect@ttn",
            "NNSXS.S4OORCZZED4GKLHMBXELB2YNLFY2GYERC7BQRYY.BA5A4EP76Z6EBC4MEESCOLGSMQZI6Y7SLHYQQTNLVR33Q2E4O3MQ"
        )
    }
    mqttClient.setOnMessageReceived { msg ->
        println("📡 Message MQTT reçu: $msg")
        message = parseMQTTMessage(msg)
    }

    Column(modifier = Modifier.padding(16.dp)) {
        Text(
            text = if (mqttClient.isConnected.value) "Connexion réussie" else "Connexion en cours...",
            style = MaterialTheme.typography.headlineSmall
        )

        Spacer(modifier = Modifier.height(8.dp))
        Text(text = "Données MQTT :", style = MaterialTheme.typography.headlineSmall)
        Spacer(modifier = Modifier.height(8.dp))
        Text(text = message, style = MaterialTheme.typography.bodyLarge)
    }
}

fun parseMQTTMessage(json: String): String {
    return try {
        val parsedJson = Klaxon().parse<MQTTData>(json)
        if (parsedJson != null) {
            val analogIn3 = parsedJson.uplink_message?.decoded_payload?.analog_in_3 ?: "Non disponible"
            val analogIn4 = parsedJson.uplink_message?.decoded_payload?.analog_in_4 ?: "Non disponible"
            val analogIn5 = parsedJson.uplink_message?.decoded_payload?.analog_in_5 ?: "Non disponible"
            val analogIn6 = parsedJson.uplink_message?.decoded_payload?.analog_in_6 ?: "Non disponible"
            val analogIn7 = parsedJson.uplink_message?.decoded_payload?.analog_in_7 ?: "Non disponible"
            val analogIn8 = parsedJson.uplink_message?.decoded_payload?.analog_in_8 ?: "Non disponible"
            val analogIn9 = parsedJson.uplink_message?.decoded_payload?.analog_in_9 ?: "Non disponible"
            val temperature1 = parsedJson.uplink_message?.decoded_payload?.temperature_1 ?: "Non disponible"
            val temperature2 = parsedJson.uplink_message?.decoded_payload?.temperature_2 ?: "Non disponible"

            "Vitesse vent 1: $analogIn3\nVitesse vent 2: $analogIn4\nDirection 1: $analogIn5\nDirection 2: $analogIn6\nId pylone 1: $analogIn7\nId pylone 2: $analogIn8\nRemontée id : $analogIn9\nTempérature 1: $temperature1\nTempérature 2:$temperature2"
        } else {
            "❌ Erreur lors du parsing : JSON invalide"
        }
    } catch (e: Exception) {
        "❌ Erreur lors du parsing : ${e.message}"
    }
}

class MQTTClient(brokerUrl: String, clientId: String, private val topic: String, username: String, password: String) {
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
                println("📡 Données brutes MQTT reçues: $payload")
                onMessageReceived?.invoke(payload)
            }
        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    fun setOnMessageReceived(listener: (String) -> Unit) {
        onMessageReceived = listener
    }
}
