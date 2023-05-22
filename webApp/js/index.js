// ----------- CONSTANT & VARIABLE ------------
const labels_temp = [];
const data_temp = {
    labels: labels_temp,
    datasets: [{
        label: 'Temperature',
        pointBackgroundColor: 'rgb(0,0,0)',
        borderWidth: 1,
        fill: false,
        tension: 0.3,
        data: [],
    }]  
}

const data_hum = {
    labels: labels_temp,
    datasets: [{
        label: 'Humidity',
        borderColor: [
            'rgba(255, 99, 132, 1)',
            'rgba(54, 162, 235, 1)',
            'rgba(255, 206, 86, 1)',
            'rgba(75, 192, 192, 1)',
            'rgba(153, 102, 255, 1)',
            'rgba(255, 159, 64, 1)'
        ],
        pointBackgroundColor: 'rgb(0,0,0)',
        borderWidth: 1,
        fill: false,
        tension: 0.3,
        data: [],
    }]
}
var tempChart,humChart;

// ----------- INITIALIZATION ------------
function chart_layout(){

  // New graph with Char js
  tempChart = new Chart(document.getElementById("chartTemp"), {
      type: 'line',
      data: data_temp
  });

  // New graph with Char js
  humChart = new Chart(document.getElementById("chartHum"), {
      type: 'line',
      data: data_hum
  });

}

// ----------- HELPER FUNCTION ------------
function handleTimeStamp(timestamp){
  // Generate timestamp from $(timestamp()) of aws.
  var seconds = Math.floor((timestamp / 1000) % 60);
  var minutes = Math.floor((timestamp / (1000 * 60)) % 60);
  var hours = Math.floor((timestamp / (1000 * 60 * 60)) % 24);
  return hours.toString().padStart(2, '0') + ":" +
          minutes.toString().padStart(2, '0') + ":" +
          seconds.toString().padStart(2, '0');
}
// Lambda Function : read_temperature
async function read_valueLambdaF(){
  try {
    const lambdaFunctionURL = 'https://peu4sutlyzjphwafmkif5slj5y0tchlz.lambda-url.eu-west-1.on.aws/'; // Replace with the actual Lambda Function URL
  
    const response = await fetch(lambdaFunctionURL, {
      method: 'GET'
    });
  
    if (response.ok) {
      const data = await response.json();

      console.log('Lambda function, read_value from DynnamoDB, invocation successful!');
      data.forEach((elem)=>{
        // Transform the response in a pretty way (This change based on the specific of AWS IoT Core Rule used to write on DynamoDB )
        const transformedPayload = {
          id: elem.id.N,
          ...Object.fromEntries(
            Object.entries(elem.payload.M).map(([key, value]) => [key, value.S])
          )
        };
        var timestamp = transformedPayload.id
        var formattedTime = handleTimeStamp(timestamp);
        
        
        if(!(data_temp.labels.includes(formattedTime))){
          var temp = parseFloat(transformedPayload["temperature"])
          var hum = parseFloat(transformedPayload["humidity"])
          
          labels_temp.push(formattedTime);
          data_temp.datasets[0].data.push(temp);
          data_hum.datasets[0].data.push(hum);
        }


      });

      labels_temp.sort((a, b) => {
        if (a > b) {
          return -1;
        }
        if (a < b) {
          return 1;
        }
        return 0;
      });
      tempChart.update();
      humChart.update();
      exec_measurements();
      
   
    } else {
      console.error('Lambda function invocation failed with status:', response.status);
    }
  } catch (error) {
    console.error('Error invoking Lambda function:', error.message);
  }

}

function exec_measurements(){

  const values = data_temp.datasets[0].data;

  const max_temp = Math.max(...values);

  const min_temp = Math.min(...values);

  const sum_temp = values.reduce((acc, val) => acc + val, 0);
  const avg_temp = sum_temp / values.length;

  const variation = max_temp - min_temp;

  //display to html
  document.getElementById("AVG_temp").innerHTML = "AVG :" + avg_temp + "&degC "
  document.getElementById("MAX_temp").innerHTML = "MAX :" + max_temp + "&degC " 
  document.getElementById("MIN_temp").innerHTML = "MIN :" + min_temp + "&degC " 
  document.getElementById("delta_temp").innerHTML = "Variation in last Hour:" + variation + "&degC"

  const values2 = data_hum.datasets[0].data;

  const max_hum = Math.max(...values);

  const min_hum = Math.min(...values);

  const sum_hum = values.reduce((acc, val) => acc + val, 0);
  const avg_hum = sum_hum / values2.length;

  const variation2 = max_hum - min_hum;


  //display to html
  document.getElementById("AVG_hum").innerHTML = "AVG :" + avg_hum + "%"
  document.getElementById("MAX_hum").innerHTML = "MAX :" + max_hum + "%" 
  document.getElementById("MIN_hum").innerHTML = "MIN :" + min_hum + "%" 
  document.getElementById("delta_hum").innerHTML = "Variation in last Hour:" + variation2 + "%"

}

// ----------- MAIN FUNCTION ------------
function main(){

  console.log("Start - Create chart layout")
  chart_layout();
  console.log("Start - Retrieve info for the graph")
  read_valueLambdaF();
  
  /*
  // Starting campling data - ogni 10 secondi.
  setInterval(read_valueLambdaF, 10000)
  */

}

document.addEventListener('DOMContentLoaded', function() {
  main()
});

// ----------- FUNCTION TRIGGERED FROM WEBSITE ------------

// Lambda Function : publish_to_iotcore	
async function postLambdaFunction(content) {

  await fetch("https://mzmk5uzj2fjrah5cqlcynm3ome0swley.lambda-url.eu-west-1.on.aws/", {
    method: 'POST',
    body: content,
  })
  .then(data => {
    // Gestisci la risposta dalla funzione Lambda
    console.log("publish_to_iotcore successfully - see on bridge");
  })
  .catch(error => {
    console.error('Error calling Lambda function:', error);
  });

}


// ----------- TEST FUNCTION ------------
var intervalId;
function test_interval(){

  intervalId = setInterval(read_valueLambdaF, 10000);
  console.log("Loop Iniziato");

}

function test_closeloop(){
  clearInterval(intervalId);
  console.log("Loop terminato");
}

  

  

