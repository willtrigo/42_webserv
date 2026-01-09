#!/usr/bin/env python3
"""
CGI Environment Dump Script

This script displays comprehensive CGI environment information including
server variables, query parameters, and Python runtime details.
"""

import os
import sys
import html
import platform
from typing import Dict, List, Optional, Tuple
from urllib.parse import parse_qs


# Constants
CGI_ENVIRONMENT_VARIABLES: List[str] = [
    'GATEWAY_INTERFACE',
    'SERVER_PROTOCOL',
    'SERVER_SOFTWARE',
    'SERVER_NAME',
    'SERVER_PORT',
    'REQUEST_METHOD',
    'REQUEST_URI',
    'SCRIPT_NAME',
    'SCRIPT_FILENAME',
    'DOCUMENT_ROOT',
    'PATH_INFO',
    'PATH_TRANSLATED',
    'QUERY_STRING',
    'CONTENT_TYPE',
    'CONTENT_LENGTH',
    'REMOTE_ADDR',
    'REMOTE_PORT',
    'REDIRECT_STATUS',
]

HTML_TEMPLATE_HEADER: str = """
 <!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Python CGI Environment Dump</title>
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
    <h1>🚀 Python CGI Environment Dump</h1>
    <p class="success">✅ If you see this, Python CGI is working!</p>
"""

HTML_TEMPLATE_FOOTER: str = """</body>
</html>
"""


class CGIEnvironmentRenderer:
    """Handles rendering of CGI environment information to HTML."""

    @staticmethod
    def send_cgi_headers() -> None:
        """Send required CGI headers."""
        print("Content-Type: text/html; charset=utf-8")
        print("Status: 200 OK")
        print()
        sys.stdout.flush()

    @staticmethod
    def escape_html(value: Optional[str]) -> str:
        """
        Safely escape HTML content.
        
        Args:
            value: String to escape or None
            
        Returns:
            HTML-escaped string or placeholder for None
        """
        if value is None:
            return '<em>(not set)</em>'
        return html.escape(str(value))

    @staticmethod
    def render_table_header(columns: List[str]) -> str:
        """
        Generate HTML table header.
        
        Args:
            columns: List of column names
            
        Returns:
            HTML string for table header
        """
        header_cells = ''.join(f'<th>{html.escape(col)}</th>' for col in columns)
        return f"    <table>\n        <tr>{header_cells}</tr>\n"

    @staticmethod
    def render_table_row(key: str, value: str, key_class: str = 'key') -> str:
        """
        Generate HTML table row.
        
        Args:
            key: Row key/label
            value: Row value (should already be HTML-escaped)
            key_class: CSS class for key cell
            
        Returns:
            HTML string for table row
        """
        escaped_key = html.escape(key)
        return f"        <tr><td class='{key_class}'>{escaped_key}</td><td>{value}</td></tr>\n"

    @staticmethod
    def render_empty_row(message: str, colspan: int = 2) -> str:
        """
        Generate HTML table row for empty state.
        
        Args:
            message: Message to display
            colspan: Number of columns to span
            
        Returns:
            HTML string for empty row
        """
        return f"        <tr><td colspan='{colspan}'><em>{html.escape(message)}</em></td></tr>\n"

    @classmethod
    def render_cgi_variables(cls) -> str:
        """
        Render CGI environment variables section.
        
        Returns:
            HTML string containing CGI variables table
        """
        output = ["    <h2>📋 CGI Environment Variables</h2>\n"]
        output.append(cls.render_table_header(['Variable', 'Value']))
        
        for variable_name in CGI_ENVIRONMENT_VARIABLES:
            value = os.environ.get(variable_name)
            escaped_value = cls.escape_html(value)
            output.append(cls.render_table_row(variable_name, escaped_value))
        
        output.append("    </table>\n")
        return ''.join(output)

    @classmethod
    def render_query_parameters(cls) -> str:
        """
        Render query parameters section.
        
        Returns:
            HTML string containing query parameters table
        """
        output = ["    <h2>🔍 Query Parameters</h2>\n"]
        output.append(cls.render_table_header(['Key', 'Value']))
        
        query_string = os.environ.get('QUERY_STRING', '')
        
        if not query_string:
            output.append(cls.render_empty_row('No query parameters. Try ?name=test'))
        else:
            parsed_params = parse_qs(query_string, keep_blank_values=True)
            for key in sorted(parsed_params.keys()):
                values = parsed_params[key]
                # Join multiple values with commas
                combined_value = ', '.join(values)
                escaped_value = html.escape(combined_value)
                output.append(cls.render_table_row(key, escaped_value))
        
        output.append("    </table>\n")
        return ''.join(output)

    @classmethod
    def render_all_environment_variables(cls) -> str:
        """
        Render all environment variables section.
        
        Returns:
            HTML string containing all environment variables table
        """
        output = ["    <h2>🔧 All Environment Variables (os.environ)</h2>\n"]
        output.append(cls.render_table_header(['Variable', 'Value']))
        
        for key in sorted(os.environ.keys()):
            value = os.environ[key]
            escaped_value = html.escape(value)
            output.append(cls.render_table_row(key, escaped_value))
        
        output.append("    </table>\n")
        return ''.join(output)

    @staticmethod
    def get_python_version() -> str:
        """
        Get Python version string.
        
        Returns:
            Python version number
        """
        return sys.version.split()[0]

    @staticmethod
    def get_python_sapi() -> str:
        """
        Get Python SAPI equivalent (execution mode).
        
        Returns:
            String describing how Python is being executed
        """
        if os.environ.get('GATEWAY_INTERFACE'):
            return 'CGI'
        return 'CLI'

    @classmethod
    def render_python_info(cls) -> str:
        """
        Render Python runtime information section.
        
        Returns:
            HTML string containing Python info
        """
        version = cls.get_python_version()
        sapi = cls.get_python_sapi()
        platform_info = platform.platform()
        
        output = [
            "    <h2>ℹ️ Python Info</h2>\n",
            f"    <p>Python Version: {html.escape(version)}</p>\n",
            f"    <p>SAPI: {html.escape(sapi)}</p>\n",
            f"    <p>Platform: {html.escape(platform_info)}</p>\n",
        ]
        return ''.join(output)


def main() -> None:
    """Main execution function for CGI script."""
    renderer = CGIEnvironmentRenderer()
    
    # Send CGI headers first
    renderer.send_cgi_headers()
    
    # Render HTML content
    print(HTML_TEMPLATE_HEADER)
    print(renderer.render_cgi_variables())
    print(renderer.render_query_parameters())
    print(renderer.render_all_environment_variables())
    print(renderer.render_python_info())
    print(HTML_TEMPLATE_FOOTER)


if __name__ == '__main__':
    main()
