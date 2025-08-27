#!/usr/bin/env python3
import http.server
import socketserver
import json
import subprocess
import tempfile
import os

class LoxHandler(http.server.SimpleHTTPRequestHandler):
    def do_POST(self):
        if self.path == '/run':
            content_length = int(self.headers['Content-Length'])
            post_data = self.rfile.read(content_length)
            data = json.loads(post_data.decode('utf-8'))
            
            try:
                with tempfile.NamedTemporaryFile(mode='w', suffix='.lox', delete=False) as f:
                    f.write(data['code'])
                    temp_file = f.name
                
                result = subprocess.run(['../lox', temp_file], 
                                      capture_output=True, text=True, cwd='.')
                os.unlink(temp_file)
                
                self.send_response(200)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                
                if result.returncode == 0:
                    response = {'success': True, 'output': result.stdout}
                else:
                    response = {'success': False, 'error': result.stderr}
                
                self.wfile.write(json.dumps(response).encode())
                
            except Exception as e:
                self.send_response(500)
                self.send_header('Content-type', 'application/json')
                self.end_headers()
                response = {'success': False, 'error': str(e)}
                self.wfile.write(json.dumps(response).encode())

PORT = 8080
os.chdir('/Users/rudxkush/lox-interpreter/web')
with socketserver.TCPServer(("", PORT), LoxHandler) as httpd:
    print(f"Server running at http://localhost:{PORT}")
    httpd.serve_forever()