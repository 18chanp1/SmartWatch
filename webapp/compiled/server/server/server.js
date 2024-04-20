const path = require("path");
const express = require('express');
const app = express();
const port = 3000;
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(logRequest);
let LeaderboardObject = [];
//initialize with some values
LeaderboardObject.push({ user: "testUser", avgspd: 1, dist: 1, time: 1, gain: 1 });
//serve static files to client
const clientApp = path.join(__dirname, "..", "client");
app.use('/', express.static(clientApp, { extensions: ['html'] }));
//open ports for getting the leaderboard
app.get("/leaderboard", (req, res) => {
    res.status(200).send(JSON.stringify(LeaderboardObject));
});
//listen to incoming requests
app.listen(port, () => {
    console.log(`Example app listening on port ${port}`);
});
//function to log incoming requests
function logRequest(req, res, next) {
    console.log(`${new Date()}  ${req.ip} : ${req.method} ${req.path}`);
    next();
}
export {};
//# sourceMappingURL=server.js.map