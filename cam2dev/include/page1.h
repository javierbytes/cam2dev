const char htmlPage1[] PROGMEM = R"=====(
<html>
<head>
        <meta http-equiv="content-type" content="text/html;charset=UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style type="text/css">
            a {
                min-width: 130px;
                height: 30px;
                line-height: 15px;
                color: #fff;
                padding: 3px 3px;
                font-weight: bold;
                font-size:18px;
                text-align: center;
                text-decoration: none;
                cursor: pointer;
                transition: all 0.3s ease;
                position: relative;
                display: inline-block;
                outline: none;
                border-radius: 20px;
                border: 2px solid #4433ff;
                background: #4433ff;
            }
    
            a:hover {
                background: #fff;
                color: #4433ff
            }
    
            html {
                font-family: Arial, Helvetica, sans-serif;
                display: inline-block;
                text-align: center;
            }
    
            h1 {
                font-size: 1.8rem;
                color:red;
            }
    
            p {
                font-size: 1.4rem;
            }
    
            .topnav {
                overflow: hidden;
                background-color: #0A1128;
            }
    
            body {
                margin: 0;
            }
    
            .content {
                padding: 5%;
            }
    
            .card-grid {
                max-width: 800px;
                margin: 0 auto;
                display: grid;
                grid-gap: 2rem;
                grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            }
    
            .card {
                background-color: white;
                box-shadow: 2px 2px 12px 1px rgba(140, 140, 140, .5);
            }
    
            .card-title {
                font-size: 1.2rem;
                font-weight: bold;
                color: #034078
            }
    
            input[type=submit] {
                border: none;
                color: #FEFCFB;
                background-color: #034078;
                padding: 15px 15px;
                text-align: center;
                text-decoration: none;
                display: inline-block;
                font-size: 16px;
                width: 100px;
                margin-right: 10px;
                border-radius: 4px;
                transition-duration: 0.4s;
            }
    
            input[type=submit]:hover {
                background-color: #1282A2;
            }
    
            input[type=text],
            input[type=number],
            select {
                width: 95%;
                padding: 6px 6px;
                margin: 5px;
                display: inline-block;
                border: 1px solid #ccc;
                border-radius: 4px;
                box-sizing: border-box;
            }
    
            label {
                font-size: 1.2rem;
            }
    
            .value {
                font-size: 1.2rem;
                color: #1282A2;
            }
    
            .state {
                font-size: 1.2rem;
                color: #1282A2;
            }
    
            button {
                border: none;
                color: #FEFCFB;
                padding: 15px 32px;
                text-align: center;
                font-size: 16px;
                width: 100px;
                border-radius: 4px;
                transition-duration: 0.4s;
            }
    
            .button-on {
                background-color: #034078;
            }
    
            .button-on:hover {
                background-color: #1282A2;
            }
    
            .button-off {
                background-color: #858585;
            }
    
            .button-off:hover {
                background-color: #252524;
            }
        </style>
        <title> SECUREPASS - Page1</title>
            <script>



                var gateway = `ws://${window.location.hostname}/ws`;
                var websocket;
                window.addEventListener('load', onLoad);
                function initWebSocket() {
                console.log('Trying to open a WebSocket connection...');
                websocket = new WebSocket(gateway);
                websocket.onopen    = onOpen;
                websocket.onclose   = onClose;
                websocket.onmessage = onMessage; // <-- add this line
                }
                function onOpen(event) {
                console.log('Connection opened');
                }
                function onClose(event) {
                console.log('Connection closed');
                setTimeout(initWebSocket, 2000);
                }

                function onMessage(event) {
  
                var obj = JSON.parse(event.data);
                if (event.data == "1"){
                state = "ON";
                document.getElementById('state').innerHTML = state;
                }
                else if(event.data=="0"){

                state = "OFF";
                document.getElementById('state').innerHTML = state;
                }
                else{
                document.getElementById('temp').innerHTML = obj.Temp;
                document.getElementById('pres').innerHTML = obj.Pres;
                document.getElementById('hum').innerHTML = obj.Hum;

                }
                console.log (event.data);


                }
  
                function onLoad(event) {
                initWebSocket();
                initButton();
                }
                function initButton() {
                document.getElementById('button').addEventListener('click', toggle);
                }
                function toggle(){
                websocket.send('toggle');
                }














                function altaRam() {
                const card=document.getElementById("card").value;
                const atr=document.getElementById("atr").value;
                alert("ALTA DE"+":"+card+" PARA "+ atr);

                setTimeout(function(){ document.location.reload(false); }, 500);   
                }
                function grabaFlash() {
                alert("grabaFlash");
                setTimeout(function(){ document.location.reload(false); }, 500);   
                }
            </script>




    </head>
    
    <body>
        <div class="content">
            <div class="card-grid">
                <div class="card">
                    <form action="/" method="get">
                        
                            <p> <h1>SECUREPASS CONFIG</h1></p>
    
    
                            <p> <a href="/">ABM<br>Usuarios</a> </p>
    
                            <p><a href="/page2">Parametros<br>Acceso</a></p>
    
                            <p><a href="/page3">Credenciales<br>Red</a></p>
                        
                     </form>     
                </div>  
            </div>      
        
        </div>
        <div class="content">
            <div class="card-grid">
                <div class="card">
                    <form action="/get" method="GET">
                       
                                      
                            <label for="tag">Tarjeta</label><input type="text"  name="card" id="card">
                            <br>
                            <label for="tag">Atributos</label><input type="text" name="atr" id="atr"><br>
                            
    
                            <input type="submit" value="Alta" onclick="altaRam()" >
                             <p>
                            <input style=background-color:green type="submit" value="Grabacion" onclick=grabaFlash() >
                            </p>
                        
                    </form>
                </div>
            
            <div class="card-grid">
                <div class="card">
                    <form action="/get" method="get"> 
                         
                        <label for="tag">Atributos</label><input type="text" id="tag" name="ssid">
                        <label for="atr">Tarjeta</label><input type="text" id="tag" name="ssid"><br>
    
                        <input type="submit" value="Busqueda"> <input style=background-color:red type="submit" value="Borrar">
                            
                    </form>
                </div>    
                </div>
                
    
            </div>
    
        </div>
    
    </body>


</html>
 )=====";
