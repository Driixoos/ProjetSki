<?php
// Connexion base de données
$host = "192.168.107.61";
$dbname = "projksi";
$username = "nodered";
$password = "ciel";

try {
    $pdo = new PDO("mysql:host=$host;dbname=$dbname;charset=utf8", $username, $password, [
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION
    ]);
} catch (PDOException $e) {
    die("Erreur connexion BDD : " . $e->getMessage());
}

// Récupérer liste remontées pour le select
$stmt = $pdo->query("SELECT id, nom FROM remontee ORDER BY nom");
$remontees = $stmt->fetchAll(PDO::FETCH_ASSOC);

// Variable pour stocker la remontée sélectionnée
$remontee_id = $_GET['remontee_id'] ?? null;

$pylones = [];
$mesures = [];

if ($remontee_id && is_numeric($remontee_id)) {
    // Récupérer pylônes liés
    $stmt = $pdo->prepare("SELECT * FROM pylone WHERE remontee_id = ?");
    $stmt->execute([$remontee_id]);
    $pylones = $stmt->fetchAll(PDO::FETCH_ASSOC);

    if ($pylones) {
        // IDs des pylônes pour la requête mesures
        $ids = array_column($pylones, 'id');
        $in  = str_repeat('?,', count($ids) - 1) . '?';

        // Récupérer dernière mesure par pylône
        $sqlMesures = "
            SELECT m.*
            FROM mesures m
            INNER JOIN (
                SELECT id_pylone, MAX(timestamp) AS max_ts
                FROM mesures
                WHERE id_pylone IN ($in)
                GROUP BY id_pylone
            ) lm ON m.id_pylone = lm.id_pylone AND m.timestamp = lm.max_ts
        ";
        $stmtMesures = $pdo->prepare($sqlMesures);
        $stmtMesures->execute($ids);
        $mesuresRaw = $stmtMesures->fetchAll(PDO::FETCH_ASSOC);

        foreach ($mesuresRaw as $m) {
            $mesures[$m['id_pylone']] = $m;
        }
    }
}

?>

<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8" />
    <title>Pylônes par Remontée</title>
  
    <link rel="stylesheet" href="styles/Direct.css">
    <link rel="stylesheet" href="styles/lastMesure.css">
  
</head>
<body>
<header>
    <div class="logo">
        <img src="image/Projet_Ski.png" alt="Logo">
        <span>Données en Direct</span>
    </div>
    <nav>
        <ul>
            <li><a href="accueil.html">Accueil</a></li>
            <li><a href="lastMesurePyloneAlerte.php">Dernière mesure</a></li>
            <li><a href="AjoutRemontee.php">Nouvelles Remontées</a></li>
        </ul>
    </nav>
</header>

    <form method="get" action="">
        <label for="remontee_id">Choisir une remontée :</label>
        <select id="remontee_id" name="remontee_id" onchange="this.form.submit()">
            <option value="">-- Sélectionner --</option>
            <?php foreach ($remontees as $r): ?>
                <option value="<?= htmlspecialchars($r['id']) ?>" <?= ($r['id'] == $remontee_id) ? 'selected' : '' ?>>
                    <?= htmlspecialchars($r['nom']) ?>
                </option>
            <?php endforeach; ?>
        </select>
    </form>

    <div id="resultats">
    <?php if ($remontee_id === null || $remontee_id === ''): ?>
        <p>Sélectionnez une remontée pour afficher les pylônes.</p>
    <?php elseif (empty($pylones)): ?>
        <p>Aucun pylône trouvé pour cette remontée.</p>
    <?php else: ?>
        <table>
            <thead>
                <tr>
                    <th>ID Pylône</th>
                    <th>Latitude</th>
                    <th>Longitude</th>
                    <th>Température (°C)</th>
                    <th>Vitesse Vent (km/h)</th>
                    <th>Direction (°)</th>
                    <th>Horodatage</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($pylones as $p): 
                    $m = $mesures[$p['id']] ?? null;
                    $vitesse_vent = $m['vitesse_vent'] ?? null;
                ?>
                <tr>
                    <td><?= htmlspecialchars($p['id']) ?></td>
                    <td><?= htmlspecialchars($p['latitude']) ?></td>
                    <td><?= htmlspecialchars($p['longitude']) ?></td>
                    <td><?= $m['temperature'] ?? '-' ?></td>
                    <td class="<?= ($vitesse_vent !== null && $vitesse_vent > 50) ? 'alert' : '' ?>">
                        <?= $vitesse_vent ?? '-' ?>
                    </td>
                    <td><?= $m['direction_vent'] ?? '-' ?></td>
                    <td><?= $m['timestamp'] ?? '-' ?></td>
                </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
    <?php endif; ?>
    </div>

 <footer>
    <p>© 2025 Live Data | <a href="legal.html">Mentions légales</a></p>
</footer>

</body>
</html>
