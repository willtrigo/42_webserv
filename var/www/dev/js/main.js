/**
 * 42 Webserv - Main JavaScript Module
 * ES6+ with strict mode, modular design for scalability
 * Handles: Uptime tracking, DELETE test utility
 * Conventions: ES Modules ready (if bundler added), JSDoc, private methods
 */

'use strict';

/**
 * UptimeTracker Class
 * @description Tracks and displays server uptime in HH:MM:SS format
 * @class
 */
class UptimeTracker {
    /**
     * @constructor
     * @param {string} displayElementId - ID of the element to update
     */
    constructor(displayElementId) {
        this.startTime = Date.now();
        this.displayElement = document.getElementById(displayElementId);
        this.intervalId = null;
    }

    /**
     * @private
     * @returns {string} Formatted time string (HH:MM:SS)
     */
    _formatElapsedTime() {
        const elapsed = Date.now() - this.startTime;
        const hours = Math.floor(elapsed / 3600000);
        const minutes = Math.floor((elapsed % 3600000) / 60000);
        const seconds = Math.floor((elapsed % 60000) / 1000);

        return [hours, minutes, seconds]
            .map(unit => String(unit).padStart(2, '0'))
            .join(':');
    }

    /**
     * @private
     * @description Updates the display element with current uptime
     */
    _updateDisplay() {
        if (this.displayElement) {
            this.displayElement.textContent = this._formatElapsedTime();
        }
    }

    /**
     * @public
     * @description Starts the uptime tracking interval
     */
    start() {
        this._updateDisplay();
        this.intervalId = setInterval(() => this._updateDisplay(), 1000);
    }

    /**
     * @public
     * @description Stops the uptime tracking interval
     */
    stop() {
        if (this.intervalId) {
            clearInterval(this.intervalId);
            this.intervalId = null;
        }
    }
}

/**
 * @global
 * @description Tests the DELETE HTTP method on a target URL
 * @function testDelete
 * @returns {Promise<void>}
 */
function testDelete() {
    const targetUrl = '/uploads/test.txt';

    fetch(targetUrl, { method: 'DELETE' })
        .then(response => {
            const message = `DELETE request sent to ${targetUrl}.\nStatus: ${response.status} ${response.statusText}\nCheck console for details.`;
            console.log('DELETE Response:', response);
            alert(message);
            return response.text();
        })
        .then(body => console.log('Response Body:', body))
        .catch(error => {
            console.error('DELETE Request Error:', error);
            alert(`Error: ${error.message}`);
        });
}

/**
 * @description Module initialization: DOMContentLoaded event handler
 * @module main
 */
document.addEventListener('DOMContentLoaded', () => {
    const uptimeTracker = new UptimeTracker('uptime');
    uptimeTracker.start();

    window.testDelete = testDelete;
});

