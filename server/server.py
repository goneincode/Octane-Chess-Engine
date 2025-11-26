import subprocess
import threading
import time
from flask import Flask, request, jsonify, send_from_directory

app = Flask(__name__, static_folder='../web', static_url_path='')

class Engine:
    def __init__(self):
        self.process = subprocess.Popen(
            ['../build/Octant', '--uci'],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=1
        )
        self.lock = threading.Lock()

    def send_command(self, cmd):
        with self.lock:
            self.process.stdin.write(cmd + '\n')
            self.process.stdin.flush()

    def read_output(self, timeout=1.0):
        # Simple non-blocking read with timeout
        # In a real engine, we'd have a dedicated reader thread
        start = time.time()
        lines = []
        while time.time() - start < timeout:
            line = self.process.stdout.readline()
            if line:
                lines.append(line.strip())
                if line.startswith('bestmove'):
                    break
            else:
                time.sleep(0.01)
        return lines

engine = Engine()
engine.send_command('uci')

@app.route('/')
def index():
    return send_from_directory('../web', 'index.html')

@app.route('/move', methods=['POST'])
def move():
    data = request.json
    fen = data.get('fen') # Not used yet by engine, but good for stateless
    moves = data.get('moves') # List of moves from startpos
    
    # Construct UCI command
    cmd = "position startpos moves " + " ".join(moves)
    engine.send_command(cmd)
    
    # Ask engine to search
    engine.send_command("go")
    
    # Wait for bestmove
    output = engine.read_output(timeout=5.0)
    best_move = None
    for line in output:
        if line.startswith('bestmove'):
            best_move = line.split()[1]
            break
            
    return jsonify({'bestmove': best_move, 'logs': output})

if __name__ == '__main__':
    app.run(port=5000)
