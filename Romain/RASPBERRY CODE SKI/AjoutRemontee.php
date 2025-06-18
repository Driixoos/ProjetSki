<?php
// Affichage des erreurs pour le débogage
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// Connexion à la base de données
$host = "192.168.107.61";
$dbname = "projksi";
$username = "nodered";
$password = "ciel";

$conn = new mysqli($host, $username, $password, $dbname);
if ($conn->connect_error) {
    die("Erreur de connexion : " . $conn->connect_error);
}

$message = "";

if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $nom_remontee = trim($_POST["nom_remontee"]);
    $nb_pylones = intval($_POST["nb_pylones"]);

    if (!empty($nom_remontee) && $nb_pylones > 0) {
        // Insertion de la remontée
        $stmt = $conn->prepare("INSERT INTO remontee (nom) VALUES (?)");
        $stmt->bind_param("s", $nom_remontee);

        if ($stmt->execute()) {
            $id_remontee = $stmt->insert_id;

            $insert_ok = true;
            $pstmt = $conn->prepare("INSERT INTO pylone (remontee_id, latitude, longitude) VALUES (?, ?, ?)");

            for ($i = 1; $i <= $nb_pylones; $i++) {
                if (!isset($_POST["latitude_$i"], $_POST["longitude_$i"])) {
                    $insert_ok = false;
                    $message = "❌ Coordonnées manquantes pour le pylône $i.";
                    break;
                }

                $lat = floatval($_POST["latitude_$i"]);
                $lon = floatval($_POST["longitude_$i"]);

                $pstmt->bind_param("idd", $id_remontee, $lat, $lon);
                if (!$pstmt->execute()) {
                    $insert_ok = false;
                    $message = "❌ Erreur lors de l'insertion du pylône $i.";
                    break;
                }
            }

            $pstmt->close();

            if ($insert_ok) {
                $message = "✅ Remontée et pylônes ajoutés avec succès.";
            }
        } else {
            $message = "❌ Erreur lors de l'ajout de la remontée.";
        }

        $stmt->close();
    } else {
        $message = "❌ Tous les champs sont obligatoires et le nombre de pylônes doit être supérieur à zéro.";
    }
}

$conn->close();
?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Ajouter une Remontée</title>
    <link rel="stylesheet" href="styles/lastMesure.css">
    
    <script>
        function afficherChampsPylones() {
            const nb = parseInt(document.getElementById("nb_pylones").value) || 0;
            const container = document.getElementById("pylone-fields");
            container.innerHTML = "";

            for (let i = 1; i <= nb; i++) {
                container.innerHTML += `
                    <div class="data-section">
                        <h3>Pylône ${i}</h3>
                        <label for="latitude_${i}">Latitude :</label>
                        <input type="number" step="any" name="latitude_${i}" required>

                        <label for="longitude_${i}">Longitude :</label>
                        <input type="number" step="any" name="longitude_${i}" required>
                    </div>
                `;
            }
        }
    </script>
</head>
<body>

<header>
    <div class="logo">
        <img src="image/Projet_Ski.png" alt="Logo">
        <span>Gestion Remontées</span>
    </div>
    <nav>
        <ul>
            <li><a href="accueil.html">Accueil</a></li>
            <li><a href="lastMesurePyloneAlerte.php">Dernière mesure</a></li>
            <li><a href="AjoutRemontee.php">Nouvelles Remontées</a></li>
            
        </ul>
    </nav>
</header>

<section class="hero">
    <h1>Ajout d'une Remontée</h1>
    <p>Créez une nouvelle remontée avec ses pylônes associés.</p>
</section>

<div class="container">
    <form method="POST">
        <div class="data-section">
            <h2>Informations Remontée</h2>

            <label for="nom_remontee">Nom de la remontée :</label>
            <input type="text" name="nom_remontee" required>

            <label for="nb_pylones">Nombre de pylônes :</label>
            <input type="number" name="nb_pylones" id="nb_pylones" min="1" required onchange="afficherChampsPylones()">
        </div>

        <div id="pylone-fields"></div>

        <div style="width: 100%; text-align: center; margin-top: 20px;">
            <button type="submit" class="add">Ajouter</button>
        </div>
    </form>
</div>

<?php if ($message): ?>
<div class="container">
    <div class="data-section">
        <p><?= htmlspecialchars($message) ?></p>
    </div>
</div>
<?php endif; ?>

<footer>
    <p>© 2025 Live Data | <a href="legal.html">Mentions légales</a></p>
</footer>

</body>
</html>
