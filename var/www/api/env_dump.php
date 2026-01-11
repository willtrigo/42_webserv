#!/usr/bin/php-cgi
<?php

header("Content-Type: text/html; charset=utf-8");
header("Status: 200 OK");
ob_implicit_flush(true);

$request_method = $_SERVER['REQUEST_METHOD'] ?? 'UNKNOWN';
?>
<!DOCTYPE html>
<html>
<head>
    <title>PHP CGI Environment Dump</title>
    <style>
        body { font-family: monospace; margin: 20px; background: #1a1a2e; color: #eee; }
        h1 { color: #00d9ff; }
        h2 { color: #ff6b6b; margin-top: 30px; }
        h3 { color: #ffd93d; margin-top: 20px; }
        table { border-collapse: collapse; width: 100%; margin: 10px 0; }
        th, td { border: 1px solid #444; padding: 8px; text-align: left; }
        th { background: #16213e; color: #00d9ff; }
        td { background: #0f3460; }
        .key { color: #ffd93d; font-weight: bold; }
        .success { color: #6bcb77; }
        .warning { color: #ffd93d; }
        .method { color: #00d9ff; font-weight: bold; }
        .form-section { background: #16213e; padding: 20px; margin: 20px 0; border-radius: 5px; }
        input[type="text"], textarea { 
            background: #0f3460; color: #eee; border: 1px solid #444; 
            padding: 8px; margin: 5px 0; width: 300px; 
        }
        button { 
            background: #00d9ff; color: #1a1a2e; border: none; 
            padding: 10px 20px; cursor: pointer; font-weight: bold; 
        }
        button:hover { background: #00b8d4; }
        .raw-data { 
            background: #0f3460; padding: 10px; border: 1px solid #444; 
            white-space: pre-wrap; word-wrap: break-word;
            max-height: 300px; overflow-y: auto; font-family: monospace;
        }
    </style>
</head>
<body>
    <h1>🚀 PHP CGI Environment Dump</h1>
    <p class="success">✅ PHP-CGI is working!</p>
    <p class="method">Request Method: <?php echo htmlspecialchars($request_method); ?></p>
    
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

    <h2>🔍 Query Parameters ($_GET)</h2>
    <table>
        <tr><th>Key</th><th>Value</th></tr>
<?php
if (empty($_GET)) {
    echo "<tr><td colspan='2'><em>No query parameters</em></td></tr>\n";
} else {
    foreach ($_GET as $key => $value) {
        if (is_array($value)) {
            $value = implode(', ', array_map('htmlspecialchars', $value));
        } else {
            $value = htmlspecialchars($value);
        }
        echo "<tr><td class='key'>" . htmlspecialchars($key) . "</td><td>$value</td></tr>\n";
    }
}
?>
    </table>

<?php if ($request_method === 'POST'): ?>
    <h2>📮 POST Data ($_POST)</h2>
    <table>
        <tr><th>Key</th><th>Value</th></tr>
<?php
    if (empty($_POST)) {
        echo "<tr><td colspan='2'><em>No POST data</em></td></tr>\n";
    } else {
        foreach ($_POST as $key => $value) {
            if (is_array($value)) {
                $value = implode(', ', array_map('htmlspecialchars', $value));
            } else {
                $value = htmlspecialchars($value);
            }
            echo "<tr><td class='key'>" . htmlspecialchars($key) . "</td><td>$value</td></tr>\n";
        }
    }
?>
    </table>

<?php if (!empty($_FILES)): ?>
    <h3>📁 Files Info ($_FILES)</h3>
    <table>
        <tr><th>Field Name</th><th>File Name</th><th>Size</th><th>Type</th></tr>
<?php
    foreach ($_FILES as $field_name => $file_info) {
        $name = htmlspecialchars($file_info['name']);
        $size = number_format($file_info['size']) . ' bytes';
        $type = htmlspecialchars($file_info['type']);
        echo "<tr><td class='key'>$field_name</td><td>$name</td><td>$size</td><td>$type</td></tr>\n";
    }
?>
    </table>
<?php endif; ?>

    <h3>Raw POST Data (php://input)</h3>
    <div class="raw-data">
<?php
    $raw_post = file_get_contents('php://input');
    if (empty($raw_post)) {
        echo "<em>No raw POST data</em>";
    } else {
        $display = substr($raw_post, 0, 1000);
        echo htmlspecialchars($display);
        if (strlen($raw_post) > 1000) {
            echo "\n\n... (truncated, " . number_format(strlen($raw_post)) . " bytes total)";
        }
    }
?>
    </div>
<?php else: ?>
    <p class="warning">⚠️ Not a POST request</p>
<?php endif; ?>

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

    <div class="form-section">
        <h2>🧪 Test Forms</h2>
        
        <h3>GET Request Test</h3>
        <form method="GET">
            <input type="text" name="name" placeholder="Your name" value="Alice">
            <input type="text" name="age" placeholder="Your age" value="30">
            <button type="submit">Send GET Request</button>
        </form>
        
        <h3>POST Request Test</h3>
        <form method="POST">
            <input type="text" name="username" placeholder="Username" value="testuser"><br>
            <input type="text" name="email" placeholder="Email" value="test@example.com"><br>
            <textarea name="message" placeholder="Your message" rows="3">Hello from POST!</textarea><br>
            <button type="submit">Send POST Request</button>
        </form>
    </div>
</body>
</html>
