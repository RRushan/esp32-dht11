from flask import Flask, request, jsonify, render_template
import sqlite3
import datetime

app = Flask(__name__)
DB_FILE = 'data.db'

def init_db():
    conn = sqlite3.connect(DB_FILE)
    conn.execute('''
        CREATE TABLE IF NOT EXISTS sensors (
            id          INTEGER PRIMARY KEY AUTOINCREMENT,
            time        TEXT,
            temperature REAL,
            humidity    REAL
        )
    ''')
    conn.commit()
    conn.close()

@app.route('/data', methods=['POST'])
def receive_data():
    data = request.json
    time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    
    conn = sqlite3.connect(DB_FILE)
    conn.execute('''
        INSERT INTO sensors (time, temperature, humidity)
        VALUES (?, ?, ?)
    ''', (time, data['temperature'], data['humidity']))
    conn.commit()
    conn.close()
    
    print(f"[{time}] T={data['temperature']}°C  H={data['humidity']}%")
    return jsonify({'status': 'ok'})

@app.route('/history', methods=['GET'])
def get_history():
    conn = sqlite3.connect(DB_FILE)
    rows = conn.execute('SELECT * FROM sensors ORDER BY id DESC LIMIT 50').fetchall()
    conn.close()
    return jsonify([{
        'id': r[0], 'time': r[1],
        'temperature': r[2], 'humidity': r[3]
    } for r in rows])

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == '__main__':
    init_db()
    app.run(host='0.0.0.0', port=5000)