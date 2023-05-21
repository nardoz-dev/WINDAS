document.addEventListener('DOMContentLoaded', function() {
    const data_temp = {
        labels: ['Red', 'Blue', 'Yellow', 'Green', 'Purple', 'Orange'],
        datasets: [{
            label: 'Temperature',

            pointBackgroundColor: 'rgb(0,0,0)',
            borderWidth: 1,
            fill: false,
            tension: 0.3,
            data: [23, 20, 21, 23, 21, 19],
        }]  
    }
    
    /*
    const data_hum = {
        labels: ['Red', 'Blue', 'Yellow', 'Green', 'Purple', 'Orange'],
        datasets: [{
            label: 'Humiddity',
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
            data: [23, 20, 21, 23, 21, 19],
        }]
    }
    */

    // Crea un nuovo grafico a barre utilizzando Chart.js
    var tempChart = new Chart(document.getElementById("chartTemp"), {
        type: 'line',
        data: data_temp
    });

    /*
    // Crea un nuovo grafico a barre utilizzando Chart.js
    var humChart = new Chart(document.getElementById("chartHum"), {
        type: 'line',
        data: data_hum
    });
    */
    //const lambdaUrl = 'https://twjlutt2xbqhlnviply6nzgtjy0ccmny.lambda-url.eu-west-1.on.aws';
    
    async function invokeLambdaFunction() {
        try {
          const lambdaFunctionURL = 'https://peu4sutlyzjphwafmkif5slj5y0tchlz.lambda-url.eu-west-1.on.aws/'; // Replace with the actual Lambda Function URL
        
          const response = await fetch(lambdaFunctionURL, {
            method: 'GET'
          });
        
          if (response.ok) {
            const data = await response.json();
            console.log('Lambda function invocation successful!');
            console.log('Response:', data);
          } else {
            console.error('Lambda function invocation failed with status:', response.status);
          }
        } catch (error) {
          console.error('Error invoking Lambda function:', error.message);
        }
      }
      
    //invokeLambdaFunction();

});


// Send to Board
async function postLambdaFunction() {
    console.log("PuercosLosDios")
    var requestOptions = {
        method: 'POST'
    };
    fetch("https://mzmk5uzj2fjrah5cqlcynm3ome0swley.lambda-url.eu-west-1.on.aws/", requestOptions)
    .then(response => {
        if (!response.ok) {
            throw new Error("Errore nella richiesta: " + response.status);
        }
        return response.text();
        })
        .then(data => {
        console.log("Risposta:", data);
        })
        .catch(error => {
        console.error("Errore durante la chiamata fetch:", error);
        });
}

  
  
  
  
  
  

