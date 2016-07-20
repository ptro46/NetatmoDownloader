#
# NetatmoDownloader
# ptrognon@gmail.com
#
Utilitaire écrit en QT permettant de télécharger toutes les données de vos équipements Netatmo (Pluviométrie, Vent, CO2, Bruit, Températures, ...)

Pour le compiler sur MacOS Windows ou Linux (Si vous avez X) ouvrir le fichier MeasuresDownloader.pro avec Qt Creator, suivre l'aide pour la création du projet, et le compiler.

Sur linux si vous n'avez que la ligne de commande, après avoir installé Qt (suivre leur procédure pour cela), vérifier que votre Qt est bien dans le PATH :
export PATH=/usr/local/Qt-5.7.0/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
Créer un répertoire pour le build, s'y placer, lancer qmake et ensuite la construction :
mkdir build
cd build
qmake -o Makefile ../NetatmoDownloader/MeasuresDownloader.pro
make

Copier le fichier config.json dans le répertoire ou le projet a été construit, l'éditer et completer avec
votre login, password, clientId, clientSecret, pour un premier lancement laissez le mainDevice vide

    "account" : {
        "login" : "HERE_YOUR_NETATMO_LOGIN",
        "password" : "YOUR_NETATMO_PASSWORD",
        "clientId" : "YOUR_NETATMO_CLIENT_ID",
        "clientSecret" : "YOUR_NETATMO_CLIENT_SECRET",
        "dbName" : "netatmo.sqlite",
        "mainDevice" : "YOUR_MAIN_DEVICE_ID"
    }

Executer le projet avec l'option showConfig, cela va vous afficher la liste de vos devices
ainsi que celles sur lesquelles vous avez la vue parce que ce sont des stations amies.

$ ./MeasuresDownloader showConfig

La première device est votre device principale (le module indoor)
MainDevice 
FR-46340-Salviac Salon 70:ee:50:00:XX:XX firmware(102)

Recopier l'adresse mac de votre device dans le fichier config.json
    "account" : {
        "login" : "HERE_YOUR_NETATMO_LOGIN",
        "password" : "YOUR_NETATMO_PASSWORD",
        "clientId" : "YOUR_NETATMO_CLIENT_ID",
        "clientSecret" : "YOUR_NETATMO_CLIENT_SECRET",
        "dbName" : "netatmo.sqlite",
        "mainDevice" : "70:ee:50:00:XX:XX"
    }

Maintenant vous pouvez lancer sans argument, le logiciel va télécharger toutes vos mesures dans
une base de données SQLite.

Le logiciel va aller chercher la date d'installation de votre devices et des modules qui lui
sont ratachés, il va ensuite effectuer tous les téléchargement, Netatmo a mis en place des limites
sur les appels a leurs webservices, ces limites sont génées dans la section downloader :
    "downloader" : {
        "netatmoPerUserLimitEvery10Seconds" : 48,
        "netatmoPerUserLimitEveryHour" : 490,
        "limit" : 0
    },

netatmoPerUserLimitEvery10Seconds : nombre d'appels maximum en 10 secondes, ce nombre est normalement
de 50 mais je l'ai positionné a 48 pour ne pas risquer de dépasser la limite.

netatmoPerUserLimitEveryHour : nombre d'appels maximum en une heure, ce nombre est normalement de 500
mais je l'ai positionné a 490 pour ne pas risque de depasser la limite.

Si jamais une de ces limites est depassé le logiciel va se mettre en pause le temps que ces indicateurs
de ces quotas redescendent.

limit : nombre de telechargements par module, 0 il n'y a pas de limite il va alors tout télécharger
jusqu'a la date du jour.

Si vous lancez l'utilitaire une fois par semaine il reprendra automatiquement a la date du dernier telechargement
jusqu'a la date du jour et ajoutera les nouvelles valeurs dans la base de données sqlite.


Patrice Trognon
ptrognon@gmail.com

