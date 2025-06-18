<?php
header('Content-Type: application/json');

$host = "192.168.107.61";
$dbname = "projksi";
$username = "nodered";
$password = "ciel";

$conn = new mysqli($host, $username, $password, $dbname);
if ($conn->connect_error) {
    echo json_encode([
        'pylone1' => '<p>Erreur de connexion à la base de données</p>',
        'pylone2' => '<p>Erreur de connexion à la base de données</p>',
    ]);
    exit;
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

echo json_encode([
    'pylone1' => formatHtml($data1),
    'pylone2' => formatHtml($data2),
]);
