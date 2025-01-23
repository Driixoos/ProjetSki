<?php
require_once("phpMQTT.php"); // Inclure le fichier phpMQTT.php avec le bon chemin

use Bluerhinos\phpMQTT; // Importer l'espace de noms pour utiliser la classe phpMQTT

// Configuration de la connexion TTN MQTT
$server = "eu1.cloud.thethings.network"; // Serveur MQTT de TTN
$port = 1883; // Port MQTT
$username = "application-id@ttn"; // Remplacez par l'ID de votre application TTN
$password = "NNSXS.XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; // Clé d'accès TTN
$client_id = "SkiConnectClient"; // ID client unique

// Instanciation de phpMQTT
$mqtt = new phpMQTT($server, $port, $client_id);

if ($mqtt->connect(true, NULL, $username, $password)) {
    echo "Connexion réussie à TTN.\n";

    // Abonnement au sujet MQTT (remplacez `application-id` et `device-id` par vos valeurs)
    $topic = "v3/application-id/devices/device-id/up";
    $mqtt->subscribe([$topic => ["qos" => 0]], function ($topic, $message) {
        echo "Message reçu sur le sujet : $topic\n";

        // Décodage des données JSON
        $data = json_decode($message, true);

        if (isset($data['uplink_message']['decoded_payload'])) {
            $temperature = $data['uplink_message']['decoded_payload']['temperature'];
            $wind_speed = $data['uplink_message']['decoded_payload']['wind_speed'];
            $wind_direction = $data['uplink_message']['decoded_payload']['wind_direction'];

            // Sauvegarder les données dans un fichier JSON
            $outputData = [
                "temperature" => $temperature,
                "wind_speed" => $wind_speed,
                "wind_direction" => $wind_direction
            ];

            file_put_contents("data.json", json_encode($outputData));
            echo "Données sauvegardées : " . json_encode($outputData) . "\n";
        } else {
            echo "Aucun payload décodé disponible.\n";
        }
    });

    // Déconnecter
    $mqtt->close();
} else {
    echo "Impossible de se connecter au serveur TTN.\n";
}
?>
