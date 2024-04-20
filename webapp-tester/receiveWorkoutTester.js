const XMLHttpRequest = require("xmlhttprequest").XMLHttpRequest
let xhr = new XMLHttpRequest();

xhr.open("POST", "http://localhost:3000/receiveWorkout", true);
xhr.setRequestHeader("Content-type", "application/json");

let testWorkout = 
{
    id: "1",
    de1Serial:"365",
    workoutDistance: "35",
    workoutTime: "120"
}

xhr.timeout = 1000

console.log(JSON.stringify(testWorkout));
xhr.send(JSON.stringify(testWorkout));

xhr.onload = () =>
{
    console.log(xhr.response);
}

xhr.ontimeout = (err) =>
{
    console.log(err);
}
