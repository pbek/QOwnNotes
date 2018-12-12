import QtQml 2.0
import QOwnNotesTypes 1.0

/**
 * This script shows current weather statistics in a "scripting label"
 * 
 * The Yahoo weather api is used for fetching the weather data
 * https://developer.yahoo.com/weather/
 */

Script {
    // you have to define your registered variables so you can access them later
    property string city;
    property bool useFahrenheit;

    // register your settings variables so the user can set them in the script settings
    property variant settingsVariables: [
        {
            "identifier": "city",
            "name": "City",
            "description": "Please enter your city:",
            "type": "string",
            "default": "Graz",
        },
        {
            "identifier": "useFahrenheit",
            "name": "Unit",
            "description": "Use Fahrenheit instead of Celsius",
            "type": "boolean",
            "default": false,
        },
    ];

    function init() {
        script.registerLabel("weather stats")
        weatherStats();
    }
    
    function weatherStats() {
        script.log(useFahrenheit);
        var unitString = useFahrenheit ? "f" :  "c"
        var json = script.downloadUrlToString("https://query.yahooapis.com/v1/public/yql?q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places(1)%20where%20text%3D%22" + city + "%22)%20and%20u%3D%27" + unitString + "%27&format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys");
        var weatherInfo = JSON.parse(json);
        
        var temp = weatherInfo.query.results.channel.item.condition.temp
        var tempUnit = weatherInfo.query.results.channel.units.temperature;
        var conditionText = weatherInfo.query.results.channel.item.condition.text
        var weatherCity = weatherInfo.query.results.channel.location.city
        var windSpeed = weatherInfo.query.results.channel.wind.speed
        var windUnit = weatherInfo.query.results.channel.units.speed;
        
        if (!useFahrenheit) {
            tempUnit = "°" + tempUnit;
        }
        
        script.setLabelText("weather stats", 
            "<table align='center' width='90%'>
                <tr>
                    <td align='center'>
                        Weather in <b>" + weatherCity + "</b>: " + conditionText + " at <b>" + temp + " " + tempUnit + "</b>
                        (" + windSpeed + " " + windUnit + " wind)
                    </tb>
                </tr>
            </table>")
    }
    
    /**
     * This starts a timer that triggers every 10min
     */
    property QtObject timer: Timer {
        interval: 600000
        repeat: true
        running: true
        
        property int count: 0
        
        onTriggered: {
            weatherStats();
        }
    }
} 
