"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.emptyDOM = exports.createDOM = void 0;
// Removes the contents of the given DOM element (equivalent to elem.innerHTML = '' but faster)
function emptyDOM(elem) {
    while (elem.firstChild)
        elem.removeChild(elem.firstChild);
}
exports.emptyDOM = emptyDOM;
// Creates a DOM element from the given HTML string
function createDOM(htmlString) {
    var template = document.createElement('template');
    template.innerHTML = htmlString.trim();
    return template.content.firstChild;
}
exports.createDOM = createDOM;
