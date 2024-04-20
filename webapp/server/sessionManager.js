"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (Object.prototype.hasOwnProperty.call(b, p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        if (typeof b !== "function" && b !== null)
            throw new TypeError("Class extends value " + String(b) + " is not a constructor or null");
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
exports.SessionManager = void 0;
var crypto_1 = require("crypto");
var SessionError = /** @class */ (function (_super) {
    __extends(SessionError, _super);
    function SessionError() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return SessionError;
}(Error));
;
var SessionManager = /** @class */ (function () {
    function SessionManager() {
        var _this = this;
        // default session length - you might want to
        // set this to something small during development
        this.CookieMaxAgeMs = 60000000;
        // SessionError class is available to other modules as "SessionManager.Error"
        this.Error = SessionError;
        // might be worth thinking about why we create these functions
        // as anonymous functions (per each instance) and not as prototype methods
        this.createSession = function (response, username, maxAge) {
            if (maxAge === void 0) { maxAge = _this.CookieMaxAgeMs; }
            console.log("creating sesion");
            var token = crypto_1.default.randomBytes(128).toString('base64url');
            var time = Date.now();
            setTimeout(function () { delete _this.sessions[token]; console.log(_this.sessions[token]); }, maxAge);
            _this.sessions.set(token, {
                "username": username,
                "timestamp": time,
                "expiry": time + maxAge
            });
            response.cookie("platform-session", token, { maxAge: maxAge });
        };
        this.deleteSession = function (request) {
            _this.sessions.delete(request.session);
            delete request.username;
            delete request.session;
            console.log("deletion done");
            return;
        };
        this.middleware = function (request, response, next) {
            console.log("in middleware");
            request.greatSuccess = true;
            var ck = request.headers.cookie;
            if (ck == null) {
                request.greatSuccess = false;
                next(new SessionError);
                return;
            }
            var cookieObject = parseCookie(ck);
            console.log(_this.CookieMaxAgeMs);
            if (!(_this.sessions.has(cookieObject["platform-session"]))) {
                request.greatSuccess = false;
                next(new SessionError);
                return;
            }
            //assigning usernames and shit
            var cookieID = cookieObject["platform-session"];
            request.username = _this.sessions.get(cookieID).username;
            request.session = cookieID;
            next();
        };
        // this function is used by the test script.
        // you can use it if you want.
        this.getUsername = function (token) {
            return (token in _this.sessions) ? _this.sessions[token].username : null;
        };
        this.sessions = new Map();
        this.CookieMaxAgeMs = 6000000000;
        console.log(this.sessions);
    }
    return SessionManager;
}());
exports.SessionManager = SessionManager;
//cookie parsing
var parseCookie = function (str) {
    return str
        .split(';')
        .map(function (v) { return v.split('='); })
        .reduce(function (acc, v) {
        acc[decodeURIComponent(v[0].trim())] = decodeURIComponent(v[1].trim());
        return acc;
    }, {});
};
