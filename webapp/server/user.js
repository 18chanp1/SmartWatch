"use strict";
var __classPrivateFieldSet = (this && this.__classPrivateFieldSet) || function (receiver, state, value, kind, f) {
    if (kind === "m") throw new TypeError("Private method is not writable");
    if (kind === "a" && !f) throw new TypeError("Private accessor was defined without a setter");
    if (typeof state === "function" ? receiver !== state || !f : !state.has(receiver)) throw new TypeError("Cannot write private member to an object whose class did not declare it");
    return (kind === "a" ? f.call(receiver, value) : f ? f.value = value : state.set(receiver, value)), value;
};
var __classPrivateFieldGet = (this && this.__classPrivateFieldGet) || function (receiver, state, kind, f) {
    if (kind === "a" && !f) throw new TypeError("Private accessor was defined without a getter");
    if (typeof state === "function" ? receiver !== state || !f : !state.has(receiver)) throw new TypeError("Cannot read private member from an object whose class did not declare it");
    return kind === "m" ? f : kind === "a" ? f.call(receiver) : f ? f.value : state.get(receiver);
};
var _fitnessUser_password;
Object.defineProperty(exports, "__esModule", { value: true });
var fitnessUser = /** @class */ (function () {
    function fitnessUser(user, pass, wkouts) {
        _fitnessUser_password.set(this, void 0); //TODO salting and hashing
        this.username = user;
        __classPrivateFieldSet(this, _fitnessUser_password, pass, "f");
        this.workouts = wkouts === null ? [] : wkouts;
    }
    fitnessUser.prototype.comparePassword = function (inp) {
        return inp === __classPrivateFieldGet(this, _fitnessUser_password, "f");
    };
    fitnessUser.prototype.addWorkout = function (newWK) {
        this.workouts.push(newWK);
    };
    return fitnessUser;
}());
exports.default = fitnessUser;
_fitnessUser_password = new WeakMap();
