const char htmlPage2[ ] PROGMEM = R"=====(
<html>

    <head>
        <meta http-equiv="content-type" content="text/html;charset=UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style type="text/css">
            a{
                min-width: 130px;
                height: 30px;
                line-height:15px;
                color: #fff;
                padding: 3px 3px;
                font-weight: bold;
                text-align :center;
                text-decoration :none;
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
              color: white;
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
              box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
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

            input[type=text], input[type=number], select {
              width: 50%;
              padding: 12px 20px;
              margin: 18px;
              display: inline-block;
              border: 1px solid #ccc;
              border-radius: 4px;
              box-sizing: border-box;
            }

            label {
              font-size: 1.2rem; 
            }
            .value{
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
    </head>
   
    <body>
        <div class="container">
            <h1>SECUREPASS CONFIG</h1>
            <p></p>
            <p>
             <p> <a  href="/" href="/" >ABM<br>Usuarios</a> </p>
               
            </p> <a  href="/page2">Parametros<br>Acceso</a> </p>
                
            </p> <a  href="/page3" >Credenciales<br>Red</a> </p>
            </p>
   
            <p></p>
        </div>
    </body>
   
    </html>
)=====";