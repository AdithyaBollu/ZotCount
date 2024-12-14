// import Chart from 'chart.js';
let ABus1 = document.querySelector(".A1");
// document.addEventListener(ABus1);
console.log("TEST");

// Chart setup
const ctx = document.getElementById('AGraph').getContext('2d');


let bus1Values = [0]
let xValues = [0]
let bus2Values = [0]
let bus3Values = [0]
// chart config
const myChart = new Chart(ctx, {
  type: "line",
  data: {
    labels: xValues,
    datasets: [{
      label: "AE-109",
      fill: false,
      lineTension: 0,
      backgroundColor: "rgba(0,0,255,1.0)",
      borderColor: "rgba(0,0,255,0.3)",
      data: bus1Values
    }, 
    
    {
      label: "AE-110",
      fill: false,
      lineTension: 0,
      backgroundColor: "rgba(0,0,255,1.0)",
      borderColor: "rgba(0,122,255,0.3)",
      data: bus2Values
    },
    
    ]
  },
  options: {
      title: {
      display: true,
      text: "Anteater Express Occupancy A Line",
      fontSize: 18,
      fontColor: "#333", 
      padding: 20
    },
    legend: {display: true},
    scales: {
      yAxes: [
        {
        scaleLabel: {
              display: true,
              labelString: "Bus Occupancy (People)",
              fontSize: 14,
              fontColor: "#333"
            },
        ticks: {min: 0, max:30}
        }],
        
        xAxes: [
        {
        scaleLabel: {
              display: true,
              labelString: "Time (minutes)",
              fontSize: 14,
              fontColor: "#333"
            },
        }],
    }
  }
});

async function getOccupancy () {
    let occ = await fetch("http://18.227.13.52:5000/").then(response => {
        if (!response.ok) {
            console.log("eror");
        }
        return response.json();
    })
    .then (data => {
        return data.Occupancy;
    })
    .catch(err => {
        console.log(err);
    })

    return occ;
}



let counter = 0;
let minuteTracker = 0;
const updateBuses = setInterval(async () => {
    // counter = getOccupancy();

    let occupancy = ABus1.children[1];
    let indicator = ABus1.children[2].children[0];
    occupancy.innerHTML = await getOccupancy();
    counter = occupancy.innerHTML;

    // console.log(occupancy.innerHTML);
    // occupancy.innerHTML = counter++;
    
    if (counter > 10) {
        indicator.style.backgroundColor = "red";
    }
    else if (counter > 5) {
        indicator.style.backgroundColor = "yellow";
    } else {
        indicator.style.backgroundColor = "green";
    }
    xValues.push(minuteTracker);
    bus1Values.push(counter);
    minuteTracker +=0.5;
    myChart.data.labels = xValues;
    myChart.data.datasets[0].data = bus1Values;
    myChart.update()

}, 1000);