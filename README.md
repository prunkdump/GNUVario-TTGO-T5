# Gnuvario_for_TTGO-T5
# GNUVARIO-E     Stable version 0.3
# GNUVARIO-E     Developement version 0.4 beta 7
# PCB            version 1.10
# 3D CASE        version 1.05


 v 0.1                              beta 1 version                     
 v 0.2     beta 1      23/06/19     debug VarioScreen                        
 v 0.3     beta 1      25/06/19     correction mesure tension          
                                    correction mesure de vitesse       
 v 0.3     beta 2      26/06/19     correction save IGC                                                 
 v 0.4     beta 1      03/07/19     ajout la coupure du son            
 v 0.4     beta 2      06/07/19     ajout statistique                  
 v 0.4     beta 3      18/07/19     Correction gestion Eeprom          
                                    Correction tache affichage          
                                    Ajout delete task display          
                                    Correction affichage statistique   
 v 0.4      beta 4      22/07/19    Correction effacement screendigit                               
                                    Correction affichage duree du vol  
                                    Correction affichage statistique   
                                    Modification FlightHistory         
                                    Ajout parametre flighthistory dans 
                                    SETTINGS.TXT                       
                                    Enregistrement des statistique     
                                    toutes les 60sec - reduction des   
                                    cycle d'ecriture dans la mémoire   
                                    flash de l'ESP32                   
 v 0.4     beta 5    24/07/19       Ajout TrendDigit                   
                                    Modification TrendLevel            
                                    Modification screendigit           
                                    Correction démarrage du vol    
 v 0.4     beta 6    25/07/19       Ajout NO_RECORD                                                                                          
                                    Correction TWOWIRESCHEDULER        																		
 
 v 0.4     beta 7    05/08/19       Désactivation enregistrement stat  
                                    pour eviter le bug du plantage      
                                    Ajout paramettre wifi              
                                    Modification SETTINGS.TXT v1.2     
 v 0.4     beta 8    06/08/19       Ajout icone Norecord               
                                    Correction bug statistique         
                                    Raffraichissement de l'ensemle de  
                                    l'écran toutes les 30sec           

                   Developpement à venir                               
                                                                                                                                    
 V0.4                                                                                                                                        
 Bug I2C			  																												
 bug affichage finesse  
 
 V0.5                                                                  
 Recupération vol via USB                                              
 Recuperation vol via Wifi                                             
 Calibration MPU																												
 porter best-fit-calibration sur l'ESP32                               
 porter gps-time-analysis sur l'ESP32                                  
                                                                                                                                              
 VX.X                                                                  
 Refaire gestion du son                                                

                    Fonctionalitées   
		
  Version 0		
    Vario précis avec capteur de pression et accéleromètres
    GPS
    Enregistrement des vols sur carte SD
    Affichage sur ecran E-Ink
    Affichage du vario, de l'altitude, de la vitesse sol,
    de l'heure, du temps de vol, ...
                                                                       
  Version 0.3                                                                                                                               
    Parametre utilisateur dans fichier SETTINGS.TXT                    
    Coupure du son via le bouton central (en vol)                      
                                                                       
  Version 0.4                                                          
    Statistiques de Vol                                                
    taux de chute et finesse                                           
    indicateur de monte/descente 
    Désactiver l'enregistrement des vols		
                                                                       
 