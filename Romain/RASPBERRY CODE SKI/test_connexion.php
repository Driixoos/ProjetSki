<?php
$host = "192.168.107.61";  // IP de la Raspberry
$dbname = "Projksi";
$username = "nodered";
$password = "ciel";

// Connexion
$conn = new mysqli($host, $username, $password, $dbname);

// Vérifier la connexion
if ($conn->connect_error) {
    die("Échec de la connexion : " . $conn->connect_error);
} else {
    echo "Connexion réussie à la base de données !";
}

$conn->close();
?>
