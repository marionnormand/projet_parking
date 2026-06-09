\# Description

Tout le programme a été codé en C++, avec une base de projet proposée par M. Guinand. 
Il s'agit du code qui simule entièrement le comportement d'une entrée d'un parking. 


---


\# Librairies utilisées 

Le programme utilise la librairie lvglDrivers.h pour utiliser l'écran qui est l'élement central du projet. 
Pour contrôler le servomoteur, il s'agit de la librairie Servo.h. 

---



\# Logique

- Mode automatique :

    1\. Véhicule détecté → Affichage du bouton "Prendre un ticket"

    2\. Véhicule detecté + ticket prit → Ouverture barrière 

    2\. Véhicule passe le second capteur → Fermeture barrière

    3\. S'il n'y a plus de détection au niveau des capteurs, la barrière se ferme au bout de 5 secondes  

- Mode manuel : 

    1\. L'ouverture et la fermeture de la barrière sont contrôlés avec l'interface 
    
    2\. Si l'utilisateur ne ferme pas la barrière, elle se ferme automatiquement au bout de 5 secondes 


--- 


\# Interface 

Sur l'interface, on retrouve le titre centré en haut, un switch qui contrôle le mode du parking (automatique ou manuel), 2 boutons en dessous 
du switch avec l'ouverture et la fermeture barrière. A droite se trouve le bouton pour prendre un ticket (virtuel) et en dessous l'image de 
la configuration de la barrière (ouverte, fermée et avec ou sans voiture présente). 
