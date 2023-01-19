# Asservisssement_MCC_STM32
Control d'une MCC par asservissement via STM32
PWMs fonctionnelles

Mise a jour du Jeudi 15 Décembre 2022:
Génération des PWM:
  > commande complémentaire décalée et fréquence à 16 kHz: avec ARR = 5311 et PSC = 0
  > réglage du rapport cyclique: pulseCH1 = alpha*ARR et pulseCH2 = ARR - pulseCH1
  > réglage temps mort à 2µs : DTG=[11001011]=203 à mettre dans dead time de TIM1 settings
  
Mise a jour du Jeudi 5 Janvier 2023:
  >capteur de courant opérationnel
  >début de l'encodeur 
  
Mise a jour du Jeudi 12 Janvier 2023:
   >Encodeur Ok avec affichage de la vitesse
   >Asservissement en courant fonctionnel

Mise à jour du 19/01/2023:   
	>Fin du TP
	>Asservissement en vitesse OK
	>Revoir le paramétres Kp et Ki de la boucle de vitesse pour un meilleur asservissement 