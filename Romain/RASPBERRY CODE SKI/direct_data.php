<?php
$host = "192.168.107.61";
$dbname = "projksi";
$username = "nodered";
$password = "ciel";

$conn = new mysqli($host, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Échec de la connexion : " . $conn->connect_error);
}

function getLastMesure($conn, $pylon_id) {
    $sql = "SELECT temperature, vitesse_vent, direction_vent, timestamp
            FROM mesures
            WHERE id_pylone = ?
            ORDER BY timestamp DESC
            LIMIT 1";
    $stmt = $conn->prepare($sql);
    $stmt->bind_param("i", $pylon_id);
    $stmt->execute();
    $res = $stmt->get_result();
    if ($res && $res->num_rows > 0) {
        return $res->fetch_assoc();
    }
    return null;
}

$data1 = getLastMesure($conn, 1);
$data2 = getLastMesure($conn, 2);

$conn->close();

function formatHtml($data) {
    if (!$data) {
        return '<p>Aucune donnée disponible</p>';
    }
    $html = '<p><strong>Température :</strong> ' . htmlspecialchars($data['temperature']) . ' °C</p>';
    $html .= '<p><strong>Vitesse du Vent :</strong> ' . htmlspecialchars($data['vitesse_vent']) . ' km/h</p>';
    $html .= '<p><strong>Direction du Vent :</strong> ' . htmlspecialchars($data['direction_vent']) . ' °</p>';
    $html .= '<p><strong>Horodatage :</strong> ' . htmlspecialchars($data['timestamp']) . '</p>';
    return $html;
}
?>
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8" />
    <meta http-equiv="X-UA-Compatible" content="IE=edge" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Données en direct - Pylônes 1 et 2</title>
    <link rel="stylesheet" href="styles/lastMesure.css" />
    <script>
        function refreshData() {
            fetch('live_data.php')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('pylone1').innerHTML = data.pylone1;
                    document.getElementById('pylone2').innerHTML = data.pylone2;
                })
                .catch(console.error);
        }

        window.onload = () => {
            refreshData();
            setInterval(refreshData, 30000); // Rafraîchissement toutes les 30 secondes
        }
    </script>
</head>
<body>

<header>
    <div class="logo">
        <img src="image/Projet_Ski.png" alt="Logo" />
        <span>Live Data</span>
    </div>
    <nav>
        <ul>
            <li><a href="#">Accueil</a></li>
            <li><a href="#">Données</a></li>
            <li><a href="direct_data.php">Dernière mesure</a></li>
        </ul>
    </nav>
</header>

<section class="hero">
    <h1>Données en Temps Réel</h1>
    <p>Affichage des dernières mesures enregistrées pour les pylônes 1 et 2.</p>
</section>

<div class="container">
    <div class="data-section">
        <h2>Pylône 1</h2>
        <div id="pylone1">
            <p>Chargement en cours...</p>
        </div>
    </div>

    <div class="data-section">
        <h2>Pylône 2</h2>
        <div id="pylone2">
            <p>Chargement en cours...</p>
        </div>
    </div>
</div>

<footer>
    <p>2025 Live Data | <a href="#">Mentions légales</a></p>
</footer>

</body>
</html>
