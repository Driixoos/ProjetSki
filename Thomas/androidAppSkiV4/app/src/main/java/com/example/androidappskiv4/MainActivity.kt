package com.example.androidappskiv4

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.navigation.NavController
import androidx.navigation.compose.rememberNavController
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MyApp()
        }
    }
}

@Composable
fun MyApp() {
    val navController = rememberNavController()

    NavHost(navController, startDestination = "login") {
        composable("login") { LoginScreen(navController) }
        composable("home") { HomeScreen(navController) }
        composable("settings") { SettingsScreen(navController) }
        composable("remontees") { RemonteesScreen(navController) }
        composable("mqtt") { MQTTApp() } // Ajout MQTT
        composable("ttn") { TTNApp() }   // Ajout TTN
    }
}

@Composable
fun LoginScreen(navController: NavController) {
    var email by remember { mutableStateOf("") }
    var password by remember { mutableStateOf("") }
    var errorMessage by remember { mutableStateOf("") }

    Column(
        modifier = Modifier.fillMaxSize().padding(20.dp),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(text = "Connexion", fontSize = 24.sp)

        Spacer(modifier = Modifier.height(16.dp))

        OutlinedTextField(
            value = email,
            onValueChange = { email = it },
            label = { Text("Utilisateur") },
            modifier = Modifier.fillMaxWidth()
        )

        Spacer(modifier = Modifier.height(8.dp))

        OutlinedTextField(
            value = password,
            onValueChange = { password = it },
            label = { Text("Mot de passe") },
            modifier = Modifier.fillMaxWidth(),
            visualTransformation = PasswordVisualTransformation()
        )

        Spacer(modifier = Modifier.height(8.dp))

        if (errorMessage.isNotEmpty()) {
            Text(text = errorMessage, color = androidx.compose.ui.graphics.Color.Red)
            Spacer(modifier = Modifier.height(8.dp))
        }

        Button(
            onClick = {
                if (email == "Administrateur" && password == "Admin123!") {
                    navController.navigate("home")
                } else {
                    errorMessage = "Identifiants incorrects"
                }
            },
            modifier = Modifier.fillMaxWidth().padding(8.dp)
        ) {
            Text("Se connecter")
        }
    }
}

@Composable
fun HomeScreen(navController: NavController) {
    Box(modifier = Modifier.fillMaxSize()) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(20.dp),
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(text = "Bienvenue, Administrateur !", fontSize = 24.sp)

            Spacer(modifier = Modifier.height(16.dp))

            Button(onClick = { navController.navigate("remontees") }) {
                Text("Remontées mécaniques")
            }

            Spacer(modifier = Modifier.height(16.dp))

            Button(onClick = { navController.navigate("mqtt") }) {
                Text("Données MQTT")
            }

            Spacer(modifier = Modifier.height(16.dp))

            Button(onClick = { navController.navigate("ttn") }) {
                Text("Données TTN")
            }

            Spacer(modifier = Modifier.height(16.dp))

            // Bouton Retour
            Button(onClick = { navController.popBackStack() }) {
                Text("Retour")
            }
        }

        // Bouton Paramètres en haut à droite
        Box(
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp),
            contentAlignment = Alignment.TopEnd
        ) {
            IconButton(onClick = { navController.navigate("settings") }) {
                Icon(
                    imageVector = Icons.Filled.Settings,
                    contentDescription = "Paramètres"
                )
            }
        }
    }
}

@Composable
fun RemonteesScreen(navController: NavController) {
    val remontees = listOf(
        Remontee("Télésiège Alpin", "Température: -2°C", "Vitesse du vent: 10 km/h", "Direction du vent: Nord"),
        Remontee("Télécabine Express", "Température: -5°C", "Vitesse du vent: 20 km/h", "Direction du vent: Ouest"),
        Remontee("Tapis Débutant", "Température: 0°C", "Vitesse du vent: 5 km/h", "Direction du vent: Est")
    )

    Column(
        modifier = Modifier.fillMaxSize().padding(20.dp),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(text = "Remontées mécaniques", fontSize = 24.sp)

        Spacer(modifier = Modifier.height(16.dp))

        LazyColumn {
            items(remontees) { remontee ->
                Card(
                    modifier = Modifier.fillMaxWidth().padding(8.dp),
                    elevation = CardDefaults.cardElevation(4.dp)
                ) {
                    Column(modifier = Modifier.padding(16.dp)) {
                        Text(text = remontee.nom, fontSize = 20.sp, color = MaterialTheme.colorScheme.primary)
                        Text(text = remontee.temperature)
                        Text(text = remontee.vitesseVent)
                        Text(text = remontee.directionVent)
                    }
                }
            }
        }

        Spacer(modifier = Modifier.height(16.dp))

        // Bouton Retour
        Button(onClick = { navController.popBackStack() }) {
            Text("Retour")
        }
    }
}

data class Remontee(
    val nom: String,
    val temperature: String,
    val vitesseVent: String,
    val directionVent: String
)

@Composable
fun SettingsScreen(navController: NavController) {
    Column(
        modifier = Modifier.fillMaxSize().padding(20.dp),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(text = "Paramètres", fontSize = 24.sp)
        Spacer(modifier = Modifier.height(16.dp))
        Text(text = "Version de l'application: 1.0.0")
        Text(text = "Développé par: Moi 😎")

        Spacer(modifier = Modifier.height(16.dp))

        // Bouton Retour
        Button(onClick = { navController.popBackStack() }) {
            Text("Retour")
        }
    }
}
