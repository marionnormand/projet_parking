\# Système de gestion de barrière de parking avec Arduino



\## Description

Ce projet consiste à concevoir et réaliser une carte électronique personnalisée destinée à être connectée à un Arduino Uno R3 afin de piloter une barrière de parking automatique.



Le système utilise deux capteurs infrarouges pour détecter le passage des véhicules et gérer automatiquement l’ouverture et la fermeture de la barrière via une interface de contrôle.



L’objectif est de simuler un système de contrôle d’accès intelligent, similaire à ceux utilisés dans les parkings ou zones privées.



---



\## Objectifs du projet

\- Concevoir une carte électronique (PCB) compatible Arduino

\- Intégrer des capteurs infrarouges de détection

\- Commander une barrière motorisée

\- Développer une interface de supervision

\- Automatiser la logique d’ouverture/fermeture



---



\## Fonctionnement



\### Détection

\- Capteur IR 1 → Détecte l’arrivée d’un véhicule

\- Capteur IR 2 → Détecte le passage complet du véhicule



\### Logique

1\. Véhicule détecté → Ouverture de la barrière

2\. Véhicule passe le second capteur → Fermeture automatique

3\. Sécurité : la barrière ne se ferme pas si un véhicule est encore présent



---



\## Matériel utilisé

\- Arduino Uno R3

\- Carte électronique personnalisée (PCB)

\- 2 capteurs infrarouges

\- Barrière motorisée ou servomoteur

\- Alimentation externe

\- Câbles, résistances, transistors et composants passifs



---

