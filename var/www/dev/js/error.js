/**
 * Initialize error page functionality
 */
function initErrorPage() {
    // Set timestamp if element exists
    const timestampElement = document.getElementById('timestamp');
    if (timestampElement) {
        timestampElement.textContent = new Date().toLocaleString();
    }
    
    // Generate error ID if element exists
    const errorIdElement = document.getElementById('errorId');
    if (errorIdElement && !errorIdElement.textContent.trim()) {
        errorIdElement.textContent = generateErrorId();
    }
    
    // Add click effects to buttons
    setupButtonEffects();
    
    // Setup specific error page handlers
    setupErrorSpecificHandlers();
}

/**
 * Generate a unique error ID
 */
function generateErrorId() {
    const timestamp = Date.now().toString(36);
    const random = Math.random().toString(36).substr(2, 5).toUpperCase();
    return `ERR-${timestamp}-${random}`;
}

/**
 * Add visual feedback to buttons
 */
function setupButtonEffects() {
    const buttons = document.querySelectorAll('.btn');
    
    buttons.forEach(btn => {
        btn.addEventListener('click', function() {
            // Add click animation
            this.style.transform = 'scale(0.95)';
            this.style.transition = 'transform 150ms ease';
            
            setTimeout(() => {
                this.style.transform = '';
            }, 150);
        });
        
        // For buttons that trigger page reload
        if (btn.textContent.includes('Refresh') || btn.textContent.includes('Try Again')) {
            btn.addEventListener('click', function(e) {
                if (!confirm('Are you sure you want to refresh the page?')) {
                    e.preventDefault();
                }
            });
        }
    });
}

/**
 * Setup handlers for specific error pages
 */
function setupErrorSpecificHandlers() {
    // For 405 page - Try GET button
    const tryGetBtn = document.querySelector('[onclick*="tryGetMethod"]');
    if (tryGetBtn) {
        tryGetBtn.addEventListener('click', function() {
            window.location.href = window.location.pathname;
        });
        tryGetBtn.removeAttribute('onclick');
    }
    
    // For 500 page - Clear Cache button
    const clearCacheBtn = document.querySelector('[onclick*="clearCache"]');
    if (clearCacheBtn) {
        clearCacheBtn.addEventListener('click', function() {
            if (confirm('Clear browser cache?\n\nYou will need to log in again to any sites you\'re logged into.')) {
                alert('Cache clearing simulated. In a real browser, this would clear cached data.');
                location.reload();
            }
        });
        clearCacheBtn.removeAttribute('onclick');
    }
}

/**
 * Simulate server recovery timer (for 500 errors)
 */
function setupRecoveryTimer() {
    const errorCode = document.querySelector('.error-code');
    if (errorCode && errorCode.textContent === '500') {
        let recoveryTime = 30;
        const recoveryTimer = document.createElement('div');
        recoveryTimer.className = 'error-footer__muted';
        recoveryTimer.innerHTML = `Estimated recovery: <span id="recoveryTime">${recoveryTime}</span> seconds`;
        
        const errorFooter = document.querySelector('.error-footer');
        if (errorFooter) {
            errorFooter.appendChild(recoveryTimer);
            
            const recoveryInterval = setInterval(() => {
                recoveryTime--;
                const timeElement = document.getElementById('recoveryTime');
                if (timeElement) {
                    timeElement.textContent = recoveryTime;
                }
                
                if (recoveryTime <= 0) {
                    clearInterval(recoveryInterval);
                    recoveryTimer.innerHTML = '✅ Server should be recovered. Please try again.';
                    recoveryTimer.style.color = 'var(--color-success)';
                }
            }, 1000);
        }
    }
}

/**
 * Add method card interactions for 405 page
 */
function setupMethodCards() {
    const methodCards = document.querySelectorAll('.error-content__item');
    
    methodCards.forEach(card => {
        if (card.textContent.includes('✅') || card.textContent.includes('❌')) {
            card.style.cursor = 'pointer';
            card.addEventListener('click', function() {
                const isAllowed = this.textContent.includes('✅');
                const method = this.textContent.split(' - ')[0].replace('✅', '').replace('❌', '').trim();
                
                if (isAllowed) {
                    alert(`Method ${method} is allowed for this resource.`);
                } else {
                    alert(`Method ${method} is NOT allowed for this resource.\n\nTry using GET or HEAD instead.`);
                }
            });
        }
    });
}

// Initialize when DOM is loaded
if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initErrorPage);
} else {
    initErrorPage();
}

// Export for potential module usage
if (typeof module !== 'undefined' && module.exports) {
    module.exports = {
        initErrorPage,
        generateErrorId,
        setupButtonEffects
    };
}
