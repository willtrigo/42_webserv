#!/usr/bin/env python3
"""
CGI Environment Dump Script

Displays comprehensive CGI environment information including
server variables, query parameters, and POST data.
"""

import os
import sys
import html
import platform
from typing import List, Optional
from urllib.parse import parse_qs
from io import BytesIO
import cgi


# CGI Environment Variables to Display
CGI_VARS: List[str] = [
    'GATEWAY_INTERFACE', 'SERVER_PROTOCOL', 'SERVER_SOFTWARE',
    'SERVER_NAME', 'SERVER_PORT', 'REQUEST_METHOD', 'REQUEST_URI',
    'SCRIPT_NAME', 'SCRIPT_FILENAME', 'DOCUMENT_ROOT',
    'PATH_INFO', 'QUERY_STRING', 'CONTENT_TYPE', 'CONTENT_LENGTH',
    'REMOTE_ADDR', 'REMOTE_PORT', 'REDIRECT_STATUS',
]

HTML_HEADER: str = """
 <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python CGI Environment Dump</title>
    <style>
        body {{ font-family: monospace; margin: 20px; background: #1a1a2e; color: #eee; }}
        h1 {{ color: #00d9ff; }}
        h2 {{ color: #ff6b6b; margin-top: 30px; }}
        table {{ border-collapse: collapse; width: 100%; margin: 10px 0; }}
        th, td {{ border: 1px solid #444; padding: 8px; text-align: left; }}
        th {{ background: #16213e; color: #00d9ff; }}
        td {{ background: #0f3460; }}
        .key {{ color: #ffd93d; font-weight: bold; }}
        .success {{ color: #6bcb77; }}
        .warning {{ color: #ffd93d; }}
        .method {{ color: #00d9ff; font-weight: bold; }}
        .form-section {{ background: #16213e; padding: 20px; margin: 20px 0; border-radius: 5px; }}
        input[type="text"], textarea {{ 
            background: #0f3460; color: #eee; border: 1px solid #444; 
            padding: 8px; margin: 5px 0; width: 300px; 
        }}
        button {{ 
            background: #00d9ff; color: #1a1a2e; border: none; 
            padding: 10px 20px; cursor: pointer; font-weight: bold; 
        }}
        button:hover {{ background: #00b8d4; }}
        .raw-data {{ 
            background: #0f3460; padding: 10px; border: 1px solid #444; 
            white-space: pre-wrap; word-wrap: break-word; max-height: 300px; overflow-y: auto;
        }}
    </style>
</head>
<body>
    <h1>🚀 Python CGI Environment Dump</h1>
    <p class="success">✅ Python CGI is working!</p>
    <p class="method">Request Method: {method}</p>
"""

HTML_FOOTER: str = """
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
"""


class CGIRenderer:
    """Renders CGI environment information to HTML."""

    @staticmethod
    def send_headers() -> None:
        """Send CGI headers."""
        print("Content-Type: text/html; charset=utf-8")
        print("Status: 200 OK")
        print()
        sys.stdout.flush()

    @staticmethod
    def escape(value: Optional[str]) -> str:
        """Escape HTML and handle None values."""
        return '<em>(not set)</em>' if value is None else html.escape(str(value))

    def render_cgi_vars(self) -> str:
        """Render CGI environment variables table."""
        output = ["    <h2>📋 CGI Environment Variables</h2>\n"]
        output.append("    <table>\n        <tr><th>Variable</th><th>Value</th></tr>\n")
        
        for var in CGI_VARS:
            value = self.escape(os.environ.get(var))
            output.append(f"        <tr><td class='key'>{var}</td><td>{value}</td></tr>\n")
        
        output.append("    </table>\n")
        return ''.join(output)

    def render_query_params(self) -> str:
        """Render GET query parameters table."""
        output = ["    <h2>🔍 Query Parameters (GET)</h2>\n"]
        output.append("    <table>\n        <tr><th>Key</th><th>Value</th></tr>\n")
        
        query_string = os.environ.get('QUERY_STRING', '')
        
        if not query_string:
            output.append("        <tr><td colspan='2'><em>No query parameters</em></td></tr>\n")
        else:
            params = parse_qs(query_string, keep_blank_values=True)
            for key in sorted(params.keys()):
                value = ', '.join(params[key])
                output.append(f"        <tr><td class='key'>{html.escape(key)}</td><td>{html.escape(value)}</td></tr>\n")
        
        output.append("    </table>\n")
        return ''.join(output)

    def render_post_data(self) -> str:
        """Render POST data table."""
        output = ["    <h2>📮 POST Data</h2>\n"]
        
        if os.environ.get('REQUEST_METHOD', '') != 'POST':
            output.append("    <p class='warning'>⚠️ Not a POST request</p>\n")
            return ''.join(output)
        
        raw_post_bytes = b""
        raw_post_text = ""
        content_length = os.environ.get('CONTENT_LENGTH', '0')
        try:
            length = int(content_length)
            if length > 0:
                raw_post_bytes = sys.stdin.buffer.read(length)
                raw_post_text = raw_post_bytes.decode('utf-8', errors='replace')
        except Exception as e:
            output.append(f"    <p class='error'>Error reading POST data: {html.escape(str(e))}</p>\n")
        
        output.append("    <table>\n        <tr><th>Key</th><th>Value</th></tr>\n")
        
        try:
            if raw_post_bytes:
                old_stdin = sys.stdin
                
                class FakeStdin:
                    def __init__(self, data):
                        self.buffer = BytesIO(data)
                
                sys.stdin = FakeStdin(raw_post_bytes)
                
                form = cgi.FieldStorage()
                
                sys.stdin = old_stdin
                
                if not form:
                    output.append("        <tr><td colspan='2'><em>No POST data</em></td></tr>\n")
                else:
                    for key in sorted(form.keys()):
                        field = form[key]
                        if field.filename:
                            value = f"📁 File: {field.filename}"
                        else:
                            value = field.value
                        output.append(f"        <tr><td class='key'>{html.escape(key)}</td><td>{html.escape(value)}</td></tr>\n")
            else:
                output.append("        <tr><td colspan='2'><em>No POST data</em></td></tr>\n")
                
        except Exception as e:
            output.append(f"        <tr><td colspan='2' class='error'>Error parsing: {html.escape(str(e))}</td></tr>\n")
        
        output.append("    </table>\n")
        
        if raw_post_text and len(raw_post_text) < 10000:  # Only show if < 10KB
            output.append("    <h3>Raw POST Data</h3>\n")
            output.append("    <div class='raw-data'>\n")
            display_data = raw_post_text[:1000]
            output.append(f"    {html.escape(display_data)}\n")
            if len(raw_post_text) > 1000:
                output.append(f"    ... (truncated, {len(raw_post_text)} bytes total)\n")
            output.append("    </div>\n")
        
        return ''.join(output)

    def render_all_env_vars(self) -> str:
        """Render all environment variables table."""
        output = ["    <h2>🔧 All Environment Variables</h2>\n"]
        output.append("    <table>\n        <tr><th>Variable</th><th>Value</th></tr>\n")
        
        for key in sorted(os.environ.keys()):
            value = html.escape(os.environ[key])
            output.append(f"        <tr><td class='key'>{html.escape(key)}</td><td>{value}</td></tr>\n")
        
        output.append("    </table>\n")
        return ''.join(output)

    def render_python_info(self) -> str:
        """Render Python runtime information."""
        output = [
            "    <h2>ℹ️ Python Info</h2>\n",
            f"    <p>Python Version: {html.escape(sys.version.split()[0])}</p>\n",
            f"    <p>Platform: {html.escape(platform.platform())}</p>\n",
            f"    <p>Executable: {html.escape(sys.executable)}</p>\n",
        ]
        return ''.join(output)


def main() -> None:
    """Main CGI script execution."""
    renderer = CGIRenderer()
    
    renderer.send_headers()
    
    method = os.environ.get('REQUEST_METHOD', 'UNKNOWN')
    
    print(HTML_HEADER.format(method=html.escape(method)))
    print(renderer.render_cgi_vars())
    print(renderer.render_query_params())
    print(renderer.render_post_data())
    print(renderer.render_all_env_vars())
    print(renderer.render_python_info())
    print(HTML_FOOTER)


if __name__ == '__main__':
    main()
