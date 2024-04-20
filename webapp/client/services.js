"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Service = void 0;
var Service = {
    getLeaderboard: function () {
        var request = new Promise(function (resolve, reject) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/leaderboard");
            xhr.send(null);
            xhr.timeout = 2000;
            xhr.ontimeout = function () {
                reject(new Error("timed out"));
            };
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(JSON.parse(xhr.response));
                }
                else {
                    reject(new Error(xhr.response));
                }
            };
            xhr.onerror = function (err) {
                reject(err);
            };
        });
        return request;
    },
    getUploads: function () {
        var request = new Promise(function (resolve, reject) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/uploads");
            xhr.send(null);
            xhr.timeout = 2000;
            xhr.ontimeout = function () {
                reject(new Error("timed out"));
            };
            xhr.onload = function () {
                if (xhr.status == 200) {
                    resolve(JSON.parse(xhr.response));
                }
                else {
                    reject(new Error(xhr.response));
                }
            };
            xhr.onerror = function (err) {
                reject(err);
            };
        });
        return request;
    }
};
exports.Service = Service;
