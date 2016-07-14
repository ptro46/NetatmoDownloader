# NetatmoDownloader
Qt utilities to download netatmo measures (Rain, Wind, CO2, Noise, Temperature ...)

open MeasuresDownloader.pro with Qt Creator, follow wizard to create project, compile it.

edit config.json, complete login, password, clientId, clientSecret, for the first launch leave mainDevice blank

    "account" : {
        "login" : "HERE_YOUR_NETATMO_LOGIN",
        "password" : "YOUR_NETATMO_PASSWORD",
        "clientId" : "YOUR_NETATMO_CLIENT_ID",
        "clientSecret" : "YOUR_NETATMO_CLIENT_SECRET",
        "dbName" : "netatmo.sqlite",
        "mainDevice" : "YOUR_MAIN_DEVICE_ID"
    }

copy config.json file to build folder.

execute first with showConfig argument, this will display your netatmo config :
$ ./MeasuresDownloader showConfig

The first device is your main netatmo device (indoor module) 
MainDevice 
FR-46340-Salviac Salon 70:ee:50:00:XX:XX firmware(102)

copy mac address (70:ee:50:00:XX:XX) to config.json :
    "account" : {
        "login" : "HERE_YOUR_NETATMO_LOGIN",
        "password" : "YOUR_NETATMO_PASSWORD",
        "clientId" : "YOUR_NETATMO_CLIENT_ID",
        "clientSecret" : "YOUR_NETATMO_CLIENT_SECRET",
        "dbName" : "netatmo.sqlite",
        "mainDevice" : "70:ee:50:00:XX:XX"
    }

now you can execute ./MeasuresDownloader without any argument, this will download all
your datas into netatmo.sqlite database.

To rename the database change name in config.json.
