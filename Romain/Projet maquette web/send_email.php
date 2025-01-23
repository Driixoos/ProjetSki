<?php
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $prenom = htmlspecialchars($_POST['prenom']);
    $email = htmlspecialchars($_POST['email']);
    $question = htmlspecialchars($_POST['question']);

    // Adresse e-mail de réception
    $to = "ski.connect.assistance@gmail.com";

    // Sujet de l'e-mail
    $subject = "Nouvelle question de $prenom";

    // Contenu de l'e-mail
    $message = "
    <html>
    <head>
        <title>Nouvelle question</title>
    </head>
    <body>
        <p><strong>Prénom :</strong> $prenom</p>
        <p><strong>Email :</strong> $email</p>
        <p><strong>Question :</strong><br>$question</p>
    </body>
    </html>
    ";

    // En-têtes de l'e-mail
    $headers = "MIME-Version: 1.0" . "\r\n";
    $headers .= "Content-type:text/html;charset=UTF-8" . "\r\n";
    $headers .= "From: <$email>" . "\r\n";

    // Envoi de l'e-mail
    if (mail($to, $subject, $message, $headers)) {
        echo "Votre message a été envoyé avec succès.";
    } else {
        echo "Une erreur s'est produite. Veuillez réessayer.";
    }
} else {
    echo "Méthode non autorisée.";
}
?>
