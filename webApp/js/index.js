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
});

