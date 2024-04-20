import crypto from "crypto";
class SessionError extends Error {
}
;
/**
 * Data structure to manage sessions.
 */
class SessionManager {
    // default session length - you might want to
    // set this to something small during development
    CookieMaxAgeMs = 60000000;
    // keeping the session data inside a closure to keep them protected
    sessions;
    // SessionError class is available to other modules as "SessionManager.Error"
    Error = SessionError;
    constructor() {
        this.sessions = new Map();
        this.CookieMaxAgeMs = 6000000000;
    }
    // might be worth thinking about why we create these functions
    // as anonymous functions (per each instance) and not as prototype methods
    createSession = (response, username, maxAge = this.CookieMaxAgeMs) => {
        let token = crypto.randomBytes(128).toString('base64url');
        let time = Date.now();
        setTimeout(() => { delete this.sessions[token]; console.log(this.sessions[token]); }, maxAge);
        this.sessions.set(token, {
            "username": username,
            "timestamp": time,
            "expiry": time + maxAge
        });
        response.cookie("platform-session", token, { maxAge: maxAge });
    };
    deleteSession = (request) => {
        this.sessions.delete(request.session);
        delete request.username;
        delete request.session;
        return;
    };
    /**
     *
     * @param request
     * @param response
     * @param next
     * @returns
     */
    middleware = (request, response, next) => {
        request.greatSuccess = true;
        let ck = request.headers.cookie;
        if (ck == null) {
            request.greatSuccess = false;
            next(new SessionError);
            return;
        }
        let cookieObject = parseCookie(ck);
        if (!(this.sessions.has(cookieObject[`platform-session`]))) {
            request.greatSuccess = false;
            next(new SessionError);
            return;
        }
        //assigning usernames and shit
        let cookieID = cookieObject[`platform-session`];
        request.username = this.sessions.get(cookieID).username;
        request.session = cookieID;
        next();
    };
    getUsername = (token) => {
        return (token in this.sessions) ? this.sessions[token].username : null;
    };
}
/**
 * Helper functions for setting password salting and checking
 */
/**
 *
 * @param password string input for the password
 * @param saltedHash salted hash that was saved during initial password generation
 * @returns A boolean, true if password is correct and false otherwise.
 */
function isCorrectPassword(password, saltedHash) {
    // convert password into encrypted password
    var salt = saltedHash.substring(0, 28);
    var saltedPassword = salt + password;
    saltedPassword = crypto.createHash('sha256').update(saltedPassword).digest('base64');
    var encryptedPwd = salt + saltedPassword;
    if (encryptedPwd === saltedHash) {
        return true;
    }
    else {
        return false;
    }
}
/**
 * Creates a salted hash from an input string
 * @param password - input password string
 * @returns  - output hashed and salted password
 */
function createSaltedHash(password) {
    //create salt
    var salt = crypto.randomBytes(20).toString('base64');
    // create salted pwd
    var saltedPwd = salt + password;
    saltedPwd = crypto.createHash('sha256').update(saltedPwd).digest('base64');
    return salt + saltedPwd;
}
/**
 *
 * @param str
 * @returns
 */
const parseCookie = str => str
    .split(';')
    .map(v => v.split('='))
    .reduce((acc, v) => {
    acc[decodeURIComponent(v[0].trim())] = decodeURIComponent(v[1].trim());
    return acc;
}, {});
export { SessionManager, isCorrectPassword, createSaltedHash };
//cookie parsing
//# sourceMappingURL=sessionManager.js.map