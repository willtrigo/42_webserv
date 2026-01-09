#!/usr/bin/php-cgi
<?php
// CGI Headers - MUST come first, followed by blank line
header("Content-Type: text/html; charset=utf-8");
header("Status: 200 OK");

// Force output
ob_implicit_flush(true);
?>
<!DOCTYPE html>
<html>
<head>
    <title>PHP CGI Environment Dump</title>
    <style>
        body { font-family: monospace; margin: 20px; background: #1a1a2e; color: #eee; }
        h1 { color: #00d9ff; }
        h2 { color: #ff6b6b; margin-top: 30px; }
        table { border-collapse: collapse; width: 100%; margin: 10px 0; }
        th, td { border: 1px solid #444; padding: 8px; text-align: left; }
        th { background: #16213e; color: #00d9ff; }
        td { background: #0f3460; }
        .key { color: #ffd93d; font-weight: bold; }
        .success { color: #6bcb77; }
        .warning { color: #ffd93d; }
    </style>
</head>
<body>
    <h1>🚀 PHP CGI Environment Dump</h1>
    <p class="success">✅ If you see this, PHP-CGI is working!</p>
    
    <h2>📋 CGI Environment Variables</h2>
    <table>
        <tr><th>Variable</th><th>Value</th></tr>
<?php
$cgi_vars = array(
    'GATEWAY_INTERFACE', 'SERVER_PROTOCOL', 'SERVER_SOFTWARE',
    'SERVER_NAME', 'SERVER_PORT', 'REQUEST_METHOD', 'REQUEST_URI',
    'SCRIPT_NAME', 'SCRIPT_FILENAME', 'DOCUMENT_ROOT', 'PATH_INFO',
    'PATH_TRANSLATED', 'QUERY_STRING', 'CONTENT_TYPE', 'CONTENT_LENGTH',
    'REMOTE_ADDR', 'REMOTE_PORT', 'REDIRECT_STATUS'
);

foreach ($cgi_vars as $var) {
    $value = isset($_SERVER[$var]) ? htmlspecialchars($_SERVER[$var]) : '<em>(not set)</em>';
    echo "<tr><td class='key'>$var</td><td>$value</td></tr>\n";
}
?>
    </table>

    <h2>📝 Query Parameters ($_GET)</h2>
    <table>
        <tr><th>Key</th><th>Value</th></tr>
<?php
if (empty($_GET)) {
    echo "<tr><td colspan='2'><em>No query parameters. Try ?name=test</em></td></tr>\n";
} else {
    foreach ($_GET as $key => $value) {
        echo "<tr><td class='key'>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars($value) . "</td></tr>\n";
    }
}
?>
    </table>

    <h2>🔧 All $_SERVER Variables</h2>
    <table>
        <tr><th>Variable</th><th>Value</th></tr>
<?php
foreach ($_SERVER as $key => $value) {
    if (is_array($value)) {
        $value = print_r($value, true);
    }
    echo "<tr><td class='key'>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars($value) . "</td></tr>\n";
}
?>
    </table>

    <h2>ℹ️ PHP Info</h2>
    <p>PHP Version: <?php echo phpversion(); ?></p>
    <p>SAPI: <?php echo php_sapi_name(); ?></p>
</body>
</html>
