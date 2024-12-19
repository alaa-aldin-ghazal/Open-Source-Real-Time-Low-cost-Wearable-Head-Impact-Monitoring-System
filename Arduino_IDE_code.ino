#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "SparkFun_LIS331.h"
#include "MPU9250.h"
#include <Wire.h>
LIS331 xl;
MPU9250 mpu;
const char *ssid =  "aaa";     // replace with your wifi ssid and wpa2 key
const char *password =  "12345678";
// Running a webserver

ESP8266WebServer server(80);

#define LED 2       //On board LED


float X,Y,Z,TA,Rotx, Roty, Rotz, RotT;
const char MAIN_page[] PROGMEM = R"=====(
<!doctype html>
<html>
<head>
  <title>Data Logger</title>
  <h1 style="text-align:center; color:red;">Linear and Angular Acceleration</h1>
  <h3 style="text-align:center;">Data on X , Y , Z</h3>
  <script src = 
  "https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.7.3/Chart.min.js">
  </script>  
  <style>
  canvas{
    -moz-user-select: none;
    -webkit-user-select: none;
    -ms-user-select: none;
  }
  /* Data Table Styling*/ 
  #dataTable {
    font-family: "Trebuchet MS", Arial, Helvetica, sans-serif;
    border-collapse: collapse;
    width: 100%;
    text-align: center;
  }
  #dataTable td, #dataTable th {
    border: 1px solid #ddd;
    padding: 8px;
  }
  #dataTable tr:nth-child(even){background-color: #f2f2f2;}
  #dataTable tr:hover {background-color: #ddd;}
  #dataTable th {
    padding-top: 12px;
    padding-bottom: 12px;
    text-align: center;
    background-color: #050505;
    color: white;
  }
  </style>
</head>
<body>
Threshold: <input type="number" value="5" id="NUMBER1" min="5" max="75">

<button onclick="exportTableToExcel('dataTable')">Export Table Data To Excel File</button>
<div class="chart-container" position: relative; height:250px; width:100%">
<canvas id="Chart" width="400" height="100"></canvas>
    </div>
    <div class="chart-container" position: relative; height:250px; width:100%">
<canvas id="Chart2" width="400" height="100"></canvas>
    </div>
<div>
  <table id="dataTable">
    <tr><th>Time</th><th>X (g)</th><th>Y (g)</th><th>Z (g)</th><th>TA (g)</th><th>Rotx (dps)</th><th>Roty (dps)</th><th>RotZ (dps)</th><th>RotT (dps)</th></tr>
  </table>
</div>
<br>
<br>  
<script>
var Xvalues = [];
var Yvalues = [];
var Zvalues = [];
var timeStamp = [];
var tavalues = [];
var RotXval=[];
var RotYval=[];
var RotZval=[];
var RotTval=[];
function showGraph()
{
    var ctx = document.getElementById("Chart").getContext('2d');
    var Chart2 = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: timeStamp,  //Bottom Labeling
            datasets: [
            {
                label: "TA",

                fill: false,  //Try with true
                backgroundColor: 'rgba(34,139,34 , 1)', //Dot marker color
               
                data: tavalues,
            }],
        },
        options: {
          
            title: {
                    display: true,
                    text: "Acceleration"
                },
            scales: {
                    yAxes: [{
                        ticks: {
                            beginAtZero:true,
                             min: 0, // minimum value
                max: 100 // maximum value (Based on the selectable range of the accelermeter)
                        }
                    }]
            }
        }
    });
   var ctx = document.getElementById("Chart2").getContext('2d');
    var Chart3 = new Chart(ctx, {
        type: 'bar',
        data: {
            labels: timeStamp,  //Bottom Labeling
            datasets: [
            {
                label: "RotT",

                fill: false,  //Try with true
                backgroundColor: 'rgba(40,220,220 , 1)', //Dot marker color
               
                data: RotTval,
            }],
        },
        options: {
          
            title: {
                    display: true,
                    text: "Gyroscope"
                },
            scales: {
                    yAxes: [{
                        ticks: {
                            beginAtZero:true,
                             min: 0, // minimum value
                max: 2000 // maximum value
                        }
                    }]
            }
        }
    })
}

//On Page load show graphs
window.onload = function() {
  console.log(new Date().toLocaleTimeString([], { hour12: false, hour: '2-digit', minute: '2-digit', fractionalSecondDigits: 3 }));
};

setInterval(function() {
  // Call a function repetatively with interval
  getData();
}, 0); //mSeconds update rate
 function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //Push the data in array
  var time = new Date().toLocaleTimeString([], { hour12: false, hour: '2-digit', minute: '2-digit', fractionalSecondDigits: 3 });
  var txt = this.responseText;
  var obj = JSON.parse(txt);

   if ( (+obj.TA) >getValue()){
      Xvalues.push(obj.X);
      Yvalues.push(obj.Y);
      Zvalues.push(obj.Z);
      tavalues.push(obj.TA);
     RotXval.push(obj.Rotx);
     RotYval.push(obj.Roty);
     RotZval.push(obj.Rotz);
     RotTval.push(obj.RotT);
      timeStamp.push(time);
      showGraph();
  //Update Data Table
    var table = document.getElementById("dataTable");
    
    var row = table.insertRow(1); //Add after headings
    
    var cell1 = row.insertCell(0);
    var cell2 = row.insertCell(1);
    var cell3 = row.insertCell(2);
    var cell4 = row.insertCell(3);
    var cell5 = row.insertCell(4);
    var cell6 = row.insertCell(5);
    var cell7 = row.insertCell(6);
    var cell8 = row.insertCell(7);
    var cell9 = row.insertCell(8);
    cell1.innerHTML = time;
    cell2.innerHTML = obj.X;
    cell3.innerHTML = obj.Y;
    cell4.innerHTML = obj.Z;
    cell5.innerHTML = obj.TA;
    cell6.innerHTML = obj.Rotx;
    cell7.innerHTML = obj.Roty;
    cell8.innerHTML = obj.Rotz;
    cell9.innerHTML = obj.RotT;
    
   }
    }
  };
  xhttp.open("GET", "readData", true); //Handle readData server on ESP8266
  xhttp.send();
}
   function getValue() {
      let t = document.getElementById("NUMBER1").value;
       return t;
   }
function exportTableToExcel(dataTable, filename = 'data'){
    var downloadLink;
    var dataType = 'application/vnd.ms-excel';
    var tableSelect = document.getElementById(dataTable);
    var tableHTML = tableSelect.outerHTML.replace(/ /g, '%20');
    
    // Specify file name
    filename = filename?filename+'.xls':'excel_data.xls';
    
    // Create download link element
    downloadLink = document.createElement("a");
    
    document.body.appendChild(downloadLink);
    
    if(navigator.msSaveOrOpenBlob){
        var blob = new Blob(['\ufeff', tableHTML], {
            type: dataType
        });
        navigator.msSaveOrOpenBlob( blob, filename);
    }else{
        // Create a link to the file
        downloadLink.href = 'data:' + dataType + ', ' + tableHTML;
    
        // Setting the file name
        downloadLink.download = filename;
        
        //triggering the function
        downloadLink.click();
    }
}



</script>
 </script>
</body>
</html>
)=====";
void handleRoot()
{
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
void readData()
{
  int16_t x, y, z;
 mpu.update();
    xl.readAxes(x, y, z); 
  // xl.setHighPassCoeff(LIS331::HPC_64);
  // xl.enableHPF(true);
String data = "{\"X\":\""+ String(X) +"\", \"Y\":\""+ String(Y) +"\", \"Z\":\""+ String(Z) +"\",\"TA\":\""+ String(TA) +"\", \"Rotx\":\""+ String(Rotx) +"\", \"Roty\":\""+ String(Roty) +"\", \"Rotz\":\""+ String(Rotz) +"\", \"RotT\":\""+ String(RotT) +"\"}";
digitalWrite(LED,!digitalRead(LED)); // Toggle LED on data request ajax
server.send(200, "text/plane", data); 
 X = xl.convertToG(100,x)- 0.0; // Adjust 0.0 to the zeroing offset if needed;
 Y = xl.convertToG(100,y)- 0.0; // Adjust 0.0 to the zeroing offset if needed; 
 Z = xl.convertToG(100,z)- 0.0; // Adjust 0.0 to the zeroing offset if needed;
TA=sqrt(X*X+Y*Y+Z*Z);
Rotx=mpu.getGyroX()- 0.0; // Adjust 0.0 to the zeroing offset if needed;
Roty=mpu.getGyroY()- 0.0; // Adjust 0.0 to the zeroing offset if needed;
Rotz=mpu.getGyroZ()- 0.0; // Adjust 0.0 to the zeroing offset if needed;
RotT=sqrt(Rotx*Rotx+Roty*Roty+Rotz*Rotz);
}
void setup(void) {
  Serial.begin(115200);

Wire.begin();
  xl.setI2CAddr(0x19);    // This MUST be called BEFORE .begin() so       
    //  .begin() can communicate with the chip
    xl.setODR(LIS331::DR_1000HZ );
  xl.begin(LIS331::USE_I2C); // Selects the bus to be used and sets
    mpu.setup(0x68);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
pinMode(LED,OUTPUT);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(LED,HIGH);
    delay(1000);
    digitalWrite(LED,LOW);
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);     
server.on("/readData", readData); 
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
