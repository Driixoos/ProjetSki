package com.example.androidappskiv6

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import org.eclipse.paho.client.mqttv3.*
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence
import com.beust.klaxon.Klaxon
import androidx.navigation.NavController

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
fun MQTTApp(navController: NavController) {
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

        Spacer(modifier = Modifier.height(24.dp))

        // ✅ Bouton Retour
        Button(onClick = { navController.popBackStack() }) {
            Text("Retour")
        }
    }
}

fun parseMQTTMessage(json: String): String {
    return try {
        val root = Klaxon().parse<Map<String, Any?>>(json)
        val uplinkMessage = root?.get("uplink_message") as? Map<*, *>
        val decodedPayload = uplinkMessage?.get("decoded_payload") as? Map<*, *>

        if (decodedPayload != null) {
            // Tri et affichage des champs analog_in_X
            val analogData = decodedPayload.entries
                .filter { it.key.toString().startsWith("analog_in_") }
                .sortedBy { it.key.toString().removePrefix("analog_in_").toIntOrNull() ?: 0 }
                .joinToString("\n") { (key, value) ->
                    val numero = key.toString().removePrefix("analog_in_")
                    "Analogique $numero: ${value ?: "Non disponible"}"
                }

            // Tri et affichage des champs temperature_X
            val temperatureData = decodedPayload.entries
                .filter { it.key.toString().startsWith("temperature_") }
                .sortedBy { it.key.toString().removePrefix("temperature_").toIntOrNull() ?: 0 }
                .joinToString("\n") { (key, value) ->
                    val numero = key.toString().removePrefix("temperature_")
                    "Température $numero: ${value ?: "Non disponible"}"
                }

            (analogData + "\n\n" + temperatureData).trim()
        } else {
            "✅ Message reçu, mais aucun champ trouvé dans decoded_payload."
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
