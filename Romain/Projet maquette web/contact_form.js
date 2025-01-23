document.getElementById("contactForm").addEventListener("submit", function(event) {
    event.preventDefault();

    const email = document.getElementById("email").value;
    const validEmailDomains = ["gmail.com", "outlook.com", "yahoo.com"];
    const emailDomain = email.split("@")[1];

    if (!validEmailDomains.includes(emailDomain)) {
        alert("Veuillez entrer une adresse email valide (ex : @gmail.com).");
        return;
    }

    const formData = new FormData(this);
    formData.append("to", "ski.connect.assistance@gmail.com");

    fetch("send_email.php", {
        method: "POST",
        body: formData
    })
    .then(response => response.text())
    .then(data => {
        alert("Votre message a été envoyé avec succès !");
    })
    .catch(error => {
        alert("Une erreur s'est produite lors de l'envoi. Veuillez réessayer.");
    });
});
