#include "WiFi.h"
#include "esp_camera.h"
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>


const char* ssid = "dunsin";
const char* password = "dunfash#2";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

boolean takeNewPhoto = false;

// Photo File Name to save in SPIFFS
#define FILE_PHOTO "/photo.jpg"

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hydrophonic Cucumber Plant Disease Monitoring on ESP32-CAM With TensorflowJS</title>
    <style>
    body {
    font-family: 'PT Sans', sans-serif;
    background-color: #dde2e1;
    margin: 0;
    color: #636060;
    line-height: 1.6;
    }
    a, button {
    text-decoration: none;
    color: #ccc;
}
    h2 {
    display: block;
    font-size: 1.17em;
    margin-block-start: 1em;
    margin-block-end: 1em;
    margin-inline-start: 0px;
    margin-inline-end: 0px;
    font-weight: bold;
    }
    .container {
    max-width: 1180px;
    text-align: center;
    margin: 0 auto;
    padding: 0 3rem;
    }
    .btn {
    padding: 1rem;
    color: #fff;
    display: inline-block;
    background: red;
    margin-bottom: 1rem;
    }
    
    </style>
    <script src="https://code.jquery.com/jquery-3.3.1.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs/dist/tf.min.js"> </script>
    <script src="https://requirejs.org/docs/release/2.3.5/minified/require.js"> </script>

    
   </head>
   <body>
     <div class="container">
     <h2>Hydrophonic Cucumber Plant Disease Monitoring on ESP32-CAM With TensorflowJS</h2>    
     <section>
       <img id="img1" width="320" height="200" src='/saved-photo' crossorigin style="border:1px solid red"/>
       <div id="results"/>
      <br>
       <div id="binary"/>
     </section>
     <div id="binary"/>
     <section>
       <button id="classify" >Classify the Photo</button>
       <button id="capture" >CAPTURE PHOTO</button>
       <button id="reload" >REFRESH PAGE</button>
       <p><input type="file"  accept="image/*" name="image" id="file"  onchange="loadFile(event)" style="display: none;"></p>
       <p><label for="file" style="cursor: pointer;">Upload Image</label></p>
       <p><img id="output" width="320" height="200" crossorigin style="border: #636060"/></p>
       <button id="classifyi" >Classify Image</button>
       <div id="result"/>
       <br>
       <div id="bin"/>
       </section>
       <div id="bin"/>
      <section id="i" />
   </div>

   <script>
   $(document).ready(function () {
    var model;
    async function loadModel() {
        model = await tf.loadLayersModel(
        "https://raw.githubusercontent.com/fajemila/cucmber_tfjs/main/model.json"
        );
        console.log("model1 loaded");
        model2 = await tf.loadLayersModel(
        "https://raw.githubusercontent.com/fajemila/plant_leaf_classfication/main/model.json"
        );
        console.log("model2 loaded");
        model3 = await tf.loadLayersModel(
            "https://raw.githubusercontent.com/fajemila/fruit_disease/main/model.json"
        );
        console.log("model3 loaded");
    }

  async function classifyImg() {
        let img = document.getElementById("img1");
        let tensorImg = tf.browser
        .fromPixels(img)
        .resizeNearestNeighbor([224, 224])
        .toFloat()
        .expandDims();
        let tensorIm = tf.browser
        .fromPixels(img)
        .resizeNearestNeighbor([256, 256])
        .toFloat()
        .expandDims();
        const output1  = model2.predict(tensorImg).dataSync();
        const output2 = model3.predict(tensorImg).dataSync();
        const output = model.predict(tensorIm).dataSync();
        
        console.log(output)
        console.log(output1)
        console.log(output2)
//        if (output1[0] + output1[1] >= 1.1) {
//            $("#results").html("None Of Cucumber or Leaf Image");
//        }
//        else if((output1[0] >= 0.5 && output1[1] >= 0.5)){
//            $("#results").html("None Of Cucumber or Leaf Image");
//        }
//        else if (output1[0] >= 0.82) {
//        $("#results").html("Cucumber Image Spotted");
//        if (output2[1] >= 0.8 ) {
//        $("#binary").html("Good Leaf");
//        } else if(output2[0] <= 0.3 ){
//        $("#binary").html("Bad Leaf");
//        }
//        else{
//            $("#binary").html("Can't Estimate");
//        }
//        } else if(output1[1] >= 0.82) {
//        $("#results").html("Leaf Image Spotted");
        if (output[0] >= 0.5) {
        $("#binary").html("Good Leaf");
        } else{
        $("#binary").html("Bad Leaf");
        }
//        }
//        else{
//            $("#results").html("None Of Cucumber or Leaf Image");
//        }
//        
        console.log("good");
        
    }
    async function classifyIm() {
        let img = document.getElementById("output");
        let tensorImg = tf.browser
        .fromPixels(img)
        .resizeNearestNeighbor([224, 224])
        .toFloat()
        .expandDims();
        let tensorIm = tf.browser
        .fromPixels(img)
        .resizeNearestNeighbor([256, 256])
        .toFloat()
        .expandDims();
        const output1  = model2.predict(tensorImg).dataSync();
        const output2 = model3.predict(tensorImg).dataSync();
        const output = model.predict(tensorIm).dataSync();
        
        console.log(output)
        console.log(output1)
        console.log(output2)
        if (output1[0] + output1[1] >= 1.1) {
            $("#result").html("None Of Cucumber or Leaf Image");
        }
        else if((output1[0] >= 0.5 && output1[1] >= 0.5)){
            $("#result").html("None Of Cucumber or Leaf Image");
        }
        else if (output1[0] >= 0.82) {
        $("#result").html("Cucumber Image Spotted");
        if (output2[1] >= 0.8 ) {
        $("#binary").html("Good Leaf");
        } else if(output2[0] <= 0.3 ){
        $("#binary").html("Bad Leaf");
        }
        else{
            $("#binary").html("Can't Estimate");
        }
        } else if(output1[1] >= 0.82) {
        $("#result").html("Leaf Image Spotted");
        if (output[0] >= 0.5 && output1[1] >= 0.82 ) {
        $("#bin").html("Good Leaf");
        } else{
        $("#bin").html("Bad Leaf");
        }
        }
        else{
            $("#result").html("None Of Cucumber or Leaf Image");
        }

        
        
        console.log("good");
        
    }

    $("#classify").click(function () {
        classifyImg();
    });
    $("#capture").click(function () {
        capturePhoto();
    });

    $("#reload").click(function () {
        location.reload()
    });

    $("#classifyi").click(function () {
        classifyIm();
    });

    function capturePhoto() {
        var xhr = new XMLHttpRequest();
//        xhr.open('GET', "/", true);
        xhr.open('GET', '/capture', true)
        xhr.send();
    }
    loadModel();
    });
    var loadFile = function(event) {
           var image = document.getElementById('output');
           image.src = URL.createObjectURL(event.target.files[0]);
       };
    </script> 
   </body>
</html>


)rawliteral";

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    ESP.restart();
  }
  else {
    delay(500);
    Serial.println("SPIFFS mounted successfully");
  }

  // Print ESP32 Local IP Address
  Serial.print("IP Address: http://");
  Serial.println(WiFi.localIP());

  // Turn-off the 'brownout detector'
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  // OV2640 camera module
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/saved-photo", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, FILE_PHOTO, "image/jpg", false);
  });

    server.on("/capture", HTTP_GET, [](AsyncWebServerRequest * request) {
    takeNewPhoto = !takeNewPhoto;
  });

  // Start server
  server.begin();

}

// Check if photo capture was successful
bool checkPhoto( fs::FS &fs ) {
  File f_pic = fs.open( FILE_PHOTO );
  unsigned int pic_sz = f_pic.size();
  return ( pic_sz > 100 );
}

// Capture Photo and Save it to SPIFFS
void capturePhotoSaveSpiffs( void ) {
  camera_fb_t * fb = NULL; // pointer
  bool ok = 0; // Boolean indicating if the picture has been taken correctly

  do {
    // Take a photo with the camera
    Serial.println("Taking a photo...");

    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // Photo file name
    Serial.printf("Picture file name: %s\n", FILE_PHOTO);
    File file = SPIFFS.open(FILE_PHOTO, FILE_WRITE);

    // Insert the data in the photo file
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // payload (image), payload length
      Serial.print("The picture has been saved in ");
      Serial.print(FILE_PHOTO);
      Serial.print(" - Size: ");
      Serial.print(file.size());
      Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);

    // check if file has been correctly saved in SPIFFS
    ok = checkPhoto(SPIFFS);
  } while ( !ok );
}

void loop() {
  if (takeNewPhoto) {
    capturePhotoSaveSpiffs();
    takeNewPhoto = false;
  }
  delay(1);
}
