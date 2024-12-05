
let ABus1 = document.querySelector(".A1");
// document.addEventListener(ABus1);
console.log("TEST");

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
}, 1000);